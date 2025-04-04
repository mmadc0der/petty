#include "../include/pet_state.h"
#include "../include/game_config.h"
#include <fstream>
#include <iostream>
#include <chrono>
#include <cstring>
#include <stdexcept>
#include <array>
#include <algorithm>

#ifdef _WIN32
#include <windows.h>
#include <shlobj.h>
#else
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#endif

PetState::PetState() noexcept
    : m_name("Unnamed Pet")
    , m_evolutionLevel(EvolutionLevel::Egg)
    , m_xp(0)
    , m_hunger(GameConfig::InitialStats::INITIAL_HUNGER)
    , m_happiness(GameConfig::InitialStats::INITIAL_HAPPINESS)
    , m_energy(GameConfig::InitialStats::INITIAL_ENERGY)
    , m_lastInteractionTime(std::chrono::system_clock::now())
    , m_birthDate(std::chrono::system_clock::now())
{
}

void PetState::initialize() noexcept {
    initialize("Unnamed Pet");
}

void PetState::initialize(std::string_view name) noexcept {
    m_name = name;
    m_evolutionLevel = EvolutionLevel::Egg;
    m_xp = 0;
    m_hunger = GameConfig::InitialStats::INITIAL_HUNGER;
    m_happiness = GameConfig::InitialStats::INITIAL_HAPPINESS;
    m_energy = GameConfig::InitialStats::INITIAL_ENERGY;
    m_lastInteractionTime = std::chrono::system_clock::now();
    m_birthDate = std::chrono::system_clock::now();
    
    // Reset achievements system when creating a new pet
    m_achievementSystem.reset();
}

std::filesystem::path PetState::getStateFilePath() const noexcept {
    std::filesystem::path statePath;
    
#ifdef _WIN32
    // Windows: %APPDATA%\pet\state.dat
    char appDataPath[MAX_PATH];
    if (SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_APPDATA, NULL, 0, appDataPath))) {
        statePath = std::filesystem::path(appDataPath) / "pet";
    } else {
        // Fallback to current directory if APPDATA is not available
        statePath = std::filesystem::current_path() / "pet";
    }
    return statePath / "state.dat";
#else
    // Linux: ~/.pet_state
    const char* homeDir = getenv("HOME");
    if (!homeDir) {
        struct passwd* pwd = getpwuid(getuid());
        if (pwd) {
            homeDir = pwd->pw_dir;
        } else {
            // Fallback to current directory if HOME is not available
            char currentDir[PATH_MAX];
            getcwd(currentDir, sizeof(currentDir));
            return std::string(currentDir) + "/.pet_state";
        }
    }
    return std::string(homeDir) + "/.pet_state";
#endif
}

bool PetState::saveFileExists() const noexcept {
    try {
        auto statePath = getStateFilePath();
        return std::filesystem::exists(statePath);
    } catch (const std::exception& e) {
        std::cerr << "Error checking save file: " << e.what() << std::endl;
        return false;
    }
}

bool PetState::load() noexcept {
    try {
        auto statePath = getStateFilePath();
        
        if (!std::filesystem::exists(statePath)) {
            return false;
        }
        
        std::ifstream file(statePath, std::ios::binary);
        if (!file) {
            std::cerr << "Failed to open state file: " << statePath.string() << std::endl;
            return false;
        }
        
        // Read file format version
        uint8_t version = 0;
        file.read(reinterpret_cast<char*>(&version), sizeof(version));
        
        if (version > 4) {
            std::cerr << "Unsupported state file version: " << static_cast<int>(version) << std::endl;
            return false;
        }
        
        // Read name
        uint16_t nameLength = 0;
        file.read(reinterpret_cast<char*>(&nameLength), sizeof(nameLength));
        
        m_name.resize(nameLength);
        file.read(&m_name[0], nameLength);
        
        // Read basic stats
        uint8_t evolutionLevel = 0;
        file.read(reinterpret_cast<char*>(&evolutionLevel), sizeof(evolutionLevel));
        m_evolutionLevel = static_cast<EvolutionLevel>(evolutionLevel);
        
        file.read(reinterpret_cast<char*>(&m_xp), sizeof(m_xp));
        
        // For version 1 and 2, read stats as uint8_t and convert to float
        if (version <= 2) {
            uint8_t hunger = 0, happiness = 0, energy = 0;
            file.read(reinterpret_cast<char*>(&hunger), sizeof(hunger));
            file.read(reinterpret_cast<char*>(&happiness), sizeof(happiness));
            file.read(reinterpret_cast<char*>(&energy), sizeof(energy));
            
            // Convert from percentage (0-100) to actual values based on max
            float maxStat = getMaxStatValue();
            m_hunger = (static_cast<float>(hunger) / 100.0f) * maxStat;
            m_happiness = (static_cast<float>(happiness) / 100.0f) * maxStat;
            m_energy = (static_cast<float>(energy) / 100.0f) * maxStat;
        } else {
            // For future versions, read stats as float directly
            file.read(reinterpret_cast<char*>(&m_hunger), sizeof(m_hunger));
            file.read(reinterpret_cast<char*>(&m_happiness), sizeof(m_happiness));
            file.read(reinterpret_cast<char*>(&m_energy), sizeof(m_energy));
        }
        
        // Read last interaction time
        uint64_t lastInteractionSeconds = 0;
        file.read(reinterpret_cast<char*>(&lastInteractionSeconds), sizeof(lastInteractionSeconds));
        m_lastInteractionTime = std::chrono::system_clock::time_point(
            std::chrono::seconds(lastInteractionSeconds));
        
        // Read birth date if version >= 2
        if (version >= 2) {
            uint64_t birthDateSeconds = 0;
            file.read(reinterpret_cast<char*>(&birthDateSeconds), sizeof(birthDateSeconds));
            m_birthDate = std::chrono::system_clock::time_point(
                std::chrono::seconds(birthDateSeconds));
        } else {
            // For old versions, set to current time
            m_birthDate = std::chrono::system_clock::now();
        }
        
        // Read achievement progress if version >= 2
        if (version >= 2) {
            if (!m_achievementSystem.load(file, version)) {
                std::cerr << "Failed to load achievement progress" << std::endl;
                return false;
            }
        }
        
        if (!file) {
            std::cerr << "Error reading state file: " << statePath.string() << std::endl;
            return false;
        }
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Exception while loading state: " << e.what() << std::endl;
        return false;
    }
}

bool PetState::save() const noexcept {
    try {
        auto statePath = getStateFilePath();
        
        // Create parent directory if it doesn't exist
        std::filesystem::create_directories(statePath.parent_path());
        
        std::ofstream file(statePath, std::ios::binary | std::ios::trunc);
        if (!file) {
            std::cerr << "Failed to open state file for writing: " << statePath.string() << std::endl;
            return false;
        }
        
        // File format version
        // Version 1: Basic pet state
        // Version 2: Added birth date and achievements
        // Version 3: Changed stats from uint8_t to float
        // Version 4: Changed stats from percentage to actual values
        const uint8_t version = 4;
        file.write(reinterpret_cast<const char*>(&version), sizeof(version));
        
        // Write name
        uint16_t nameLength = static_cast<uint16_t>(m_name.size());
        file.write(reinterpret_cast<const char*>(&nameLength), sizeof(nameLength));
        file.write(m_name.c_str(), nameLength);
        
        // Write basic stats
        uint8_t evolutionLevel = static_cast<uint8_t>(m_evolutionLevel);
        file.write(reinterpret_cast<const char*>(&evolutionLevel), sizeof(evolutionLevel));
        file.write(reinterpret_cast<const char*>(&m_xp), sizeof(m_xp));
        
        // Write stats as float
        file.write(reinterpret_cast<const char*>(&m_hunger), sizeof(m_hunger));
        file.write(reinterpret_cast<const char*>(&m_happiness), sizeof(m_happiness));
        file.write(reinterpret_cast<const char*>(&m_energy), sizeof(m_energy));
        
        // Write last interaction time
        auto lastInteractionSeconds = std::chrono::duration_cast<std::chrono::seconds>(
                m_lastInteractionTime.time_since_epoch()).count();
        file.write(reinterpret_cast<const char*>(&lastInteractionSeconds), sizeof(lastInteractionSeconds));
        
        // Write birth date
        auto birthDateSeconds = std::chrono::duration_cast<std::chrono::seconds>(
                m_birthDate.time_since_epoch()).count();
        file.write(reinterpret_cast<const char*>(&birthDateSeconds), sizeof(birthDateSeconds));
        
        // Write achievement progress
        if (!m_achievementSystem.save(file)) {
            std::cerr << "Failed to save achievement progress" << std::endl;
            return false;
        }
        
        if (!file) {
            std::cerr << "Error writing state file: " << statePath.string() << std::endl;
            return false;
        }
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Exception while saving state: " << e.what() << std::endl;
        return false;
    }
}

bool PetState::addXP(uint32_t amount) noexcept {
    m_xp += amount;
    
    // Check if we should evolve
    uint32_t xpForNextLevel = getXPForNextLevel();
    
    if (xpForNextLevel > 0 && m_xp >= xpForNextLevel) {
        // Evolve to the next level
        m_evolutionLevel = static_cast<EvolutionLevel>(static_cast<uint8_t>(m_evolutionLevel) + 1);
        
        // Unlock achievement for evolution
        m_achievementSystem.unlock(AchievementType::Evolution);
        
        // Special achievements for reaching Master and Ancient levels
        if (m_evolutionLevel == EvolutionLevel::Master) {
            m_achievementSystem.unlock(AchievementType::Master);
        } else if (m_evolutionLevel == EvolutionLevel::Ancient) {
            m_achievementSystem.unlock(AchievementType::Eternal);
        }
        
        return true;
    }
    
    return false;
}

uint32_t PetState::getXPForNextLevel() const noexcept {
    // Get XP requirements from GameConfig
    return GameConfig::getEvolutionXPRequirement(static_cast<uint8_t>(m_evolutionLevel));
}

void PetState::increaseHunger(float amount) noexcept {
    m_hunger += amount;
    
    // Cap at max
    if (m_hunger > getMaxStatValue()) {
        m_hunger = getMaxStatValue();
    }
    
    // Update progress for achievement - based on percentage (0-100 scale)
    float percentageNow = (m_hunger / getMaxStatValue()) * 100.0f;
    m_achievementSystem.setProgress(AchievementType::WellFed, static_cast<uint32_t>(percentageNow));
}

void PetState::decreaseHunger(float amount) noexcept {
    m_hunger = (m_hunger > amount) ? (m_hunger - amount) : 0.0f;
}

void PetState::increaseHappiness(float amount) noexcept {
    m_happiness += amount;
    
    // Cap at max
    if (m_happiness > getMaxStatValue()) {
        m_happiness = getMaxStatValue();
    }
    
    // Update progress for achievement - based on percentage (0-100 scale)
    float percentageNow = (m_happiness / getMaxStatValue()) * 100.0f;
    m_achievementSystem.setProgress(AchievementType::HappyDays, static_cast<uint32_t>(percentageNow));
}

void PetState::decreaseHappiness(float amount) noexcept {
    m_happiness = (m_happiness > amount) ? (m_happiness - amount) : 0.0f;
}

void PetState::increaseEnergy(float amount) noexcept {
    m_energy += amount;
    
    // Cap at max
    if (m_energy > getMaxStatValue()) {
        m_energy = getMaxStatValue();
    }
    
    // Update progress for achievement - based on percentage (0-100 scale)
    float percentageNow = (m_energy / getMaxStatValue()) * 100.0f;
    m_achievementSystem.setProgress(AchievementType::FullyRested, static_cast<uint32_t>(percentageNow));
}

void PetState::decreaseEnergy(float amount) noexcept {
    m_energy = (m_energy > amount) ? (m_energy - amount) : 0.0f;
}

void PetState::updateInteractionTime() noexcept {
    m_lastInteractionTime = std::chrono::system_clock::now();
}

float PetState::getMaxStatValue() const noexcept {
    return GameConfig::getMaxStatForEvolutionLevel(static_cast<uint8_t>(m_evolutionLevel));
}

std::string_view PetState::getAsciiArt() const noexcept {
    // Using string_view for better performance with string literals
    static const std::string eggArt = R"(
  .-.
 /   \
 \   /
  '-'
)";
    static const std::string babyArt = R"(
 |\_/|
 `o.o'
 =(_)=
)";
    static const std::string childArt = R"(
  ^__^
 (o.o)
 (___) 
)";
    static const std::string teenArt = R"(
  /\_/\
 ( o.o )
  > ^ <
)";
    static const std::string adultArt = R"(
  /\_/\
 ( ^.^ )
 (>   <)
   ---
)";
    static const std::string masterArt = R"(
  .       .         
  \`-"'"-'/
   } 6 6 {    
  =.  Y  ,=   
    /^^^\  .
   /     \  )           
  (  )-(  )/ 
   ""   ""
)";
    static const std::string ancientArt = R"(
        .     .
        |\-=-/|
     /| |O _ O| |\
   /' \ \_^-^_/ / `\
 /'    \-/ ~ \-/    `\
 |      /\\ //\      |
  \|\|\/-""-""-\/|/|/
)";

    switch (m_evolutionLevel) {
        case EvolutionLevel::Egg:
            return eggArt;
        case EvolutionLevel::Baby:
            return babyArt;
        case EvolutionLevel::Child:
            return childArt;
        case EvolutionLevel::Teen:
            return teenArt;
        case EvolutionLevel::Adult:
            return adultArt;
        case EvolutionLevel::Master:
            return masterArt;
        case EvolutionLevel::Ancient:
            return ancientArt;
        default:
            static const std::string unknown = "Unknown evolution level";
            return unknown;
    }
}

std::string_view PetState::getDescription() const noexcept {
    // Using string_view for better performance with string literals
    static const std::string eggDesc = "A mysterious egg. It seems to be moving slightly...";
    static const std::string babyDesc = "A tiny, adorable creature has hatched! It looks at you with curious eyes.";
    static const std::string childDesc = "Your pet has grown a bit. It's playful and full of energy!";
    static const std::string teenDesc = "Your pet is now a teenager. It's becoming more independent but still needs your care.";
    static const std::string adultDesc = "Your pet has reached adulthood. It's strong, confident, and loyal to you.";
    static const std::string masterDesc = "Your pet has reached its final form! It's magnificent and powerful.";
    static const std::string ancientDesc = "Your pet has reached the ancient level! It's a legendary creature with immense power.";
    
    switch (m_evolutionLevel) {
        case EvolutionLevel::Egg:
            return eggDesc;
        case EvolutionLevel::Baby:
            return babyDesc;
        case EvolutionLevel::Child:
            return childDesc;
        case EvolutionLevel::Teen:
            return teenDesc;
        case EvolutionLevel::Adult:
            return adultDesc;
        case EvolutionLevel::Master:
            return masterDesc;
        case EvolutionLevel::Ancient:
            return ancientDesc;
        default:
            static const std::string unknown = "Unknown evolution level";
            return unknown;
    }
}

std::string_view PetState::getStatusDescription() const noexcept {
    // More detailed and organic status descriptions for each evolution stage
    static const std::string eggStatus = "A mysterious egg. It seems to be moving slightly...";
    static const std::string babyStatus = "A tiny, adorable creature has hatched! It looks at you with curious eyes.";
    static const std::string childStatus = "Your pet is growing and developing. It's very curious and playful, and enjoys your attention.";
    static const std::string teenStatus = "The teenage period is a time of change. Your pet is becoming more independent but still needs your care.";
    static const std::string adultStatus = "An adult pet is full of strength and energy. It's loyal to you and ready for new adventures.";
    static const std::string masterStatus = "Your pet has achieved mastery! Its abilities and wisdom are impressive, it has become a true legend.";
    static const std::string ancientStatus = "The ancient form of your pet is the embodiment of power and wisdom. It has come a long way under your guidance.";
    
    switch (m_evolutionLevel) {
        case EvolutionLevel::Egg:
            return eggStatus;
        case EvolutionLevel::Baby:
            return babyStatus;
        case EvolutionLevel::Child:
            return childStatus;
        case EvolutionLevel::Teen:
            return teenStatus;
        case EvolutionLevel::Adult:
            return adultStatus;
        case EvolutionLevel::Master:
            return masterStatus;
        case EvolutionLevel::Ancient:
            return ancientStatus;
        default:
            static const std::string unknown = "Unknown pet status";
            return unknown;
    }
}
