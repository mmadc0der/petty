#include "../include/pet_state.h"
#include <fstream>
#include <iostream>
#include <chrono>
#include <cstring>
#include <stdexcept>
#include <array>

#ifdef _WIN32
#include <windows.h>
#include <shlobj.h>
#else
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#endif

PetState::PetState()
    : m_name("Unnamed Pet")
    , m_evolutionLevel(EvolutionLevel::Egg)
    , m_xp(0)
    , m_hunger(50.0f)
    , m_happiness(50.0f)
    , m_energy(50.0f)
    , m_lastInteractionTime(std::chrono::system_clock::now())
    , m_birthDate(std::chrono::system_clock::now())
{
}

void PetState::initialize() {
    initialize("Unnamed Pet");
}

void PetState::initialize(std::string_view name) {
    m_name = name;
    m_evolutionLevel = EvolutionLevel::Egg;
    m_xp = 0;
    m_hunger = 50.0f;
    m_happiness = 50.0f;
    m_energy = 50.0f;
    m_lastInteractionTime = std::chrono::system_clock::now();
    m_birthDate = std::chrono::system_clock::now();
    
    // Initialize achievements - all unlocked = false by default
}

std::filesystem::path PetState::getStateFilePath() const {
    std::filesystem::path statePath;
    
#ifdef _WIN32
    // Windows: %APPDATA%\pet\state.dat
    char appDataPath[MAX_PATH];
    if (SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_APPDATA, NULL, 0, appDataPath))) {
        statePath = std::filesystem::path(appDataPath) / "pet";
    } else {
        throw std::runtime_error("Could not determine APPDATA directory");
    }
#else
    // Linux: ~/.pet_state
    const char* homeDir = getenv("HOME");
    if (!homeDir) {
        struct passwd* pwd = getpwuid(getuid());
        if (pwd) {
            homeDir = pwd->pw_dir;
        } else {
            throw std::runtime_error("Could not determine HOME directory");
        }
    }
    statePath = std::filesystem::path(homeDir) / ".pet_state";
    return statePath;
#endif

#ifdef _WIN32
    // Create directory if it doesn't exist
    std::filesystem::create_directories(statePath);
    return statePath / "state.dat";
#endif
}

bool PetState::saveFileExists() const {
    try {
        auto statePath = getStateFilePath();
        return std::filesystem::exists(statePath);
    } catch (const std::exception& e) {
        std::cerr << "Error checking save file: " << e.what() << std::endl;
        return false;
    }
}

bool PetState::load() {
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
        
        if (version > 3) {
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
            
            m_hunger = static_cast<float>(hunger);
            m_happiness = static_cast<float>(happiness);
            m_energy = static_cast<float>(energy);
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
            // For older versions, set birth date to now
            m_birthDate = std::chrono::system_clock::now();
        }
        
        // Read achievement progress if version >= 2
        if (version >= 2) {
            if (!m_achievementSystem.load(file)) {
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

bool PetState::save() const {
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
        const uint8_t version = 3; // Increased version to 3 to support float stats
        file.write(reinterpret_cast<const char*>(&version), sizeof(version));
        
        // Write name
        uint16_t nameLength = static_cast<uint16_t>(m_name.length());
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
        
        // Write last interaction time as seconds since epoch
        uint64_t lastInteractionSeconds = 
            std::chrono::duration_cast<std::chrono::seconds>(
                m_lastInteractionTime.time_since_epoch()).count();
        file.write(reinterpret_cast<const char*>(&lastInteractionSeconds), sizeof(lastInteractionSeconds));
        
        // Write birth date as seconds since epoch
        uint64_t birthDateSeconds = 
            std::chrono::duration_cast<std::chrono::seconds>(
                m_birthDate.time_since_epoch()).count();
        file.write(reinterpret_cast<const char*>(&birthDateSeconds), sizeof(birthDateSeconds));
        
        // Write achievement progress
        if (!m_achievementSystem.save(file)) {
            std::cerr << "Failed to save achievement progress" << std::endl;
            return false;
        }
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Exception while saving state: " << e.what() << std::endl;
        return false;
    }
}

bool PetState::addXP(uint32_t amount) {
    m_xp += amount;
    
    // Check if we should evolve
    uint32_t xpForNextLevel = getXPForNextLevel();
    
    if (m_xp >= xpForNextLevel && m_evolutionLevel < EvolutionLevel::Ancient) {
        m_evolutionLevel = static_cast<EvolutionLevel>(static_cast<uint8_t>(m_evolutionLevel) + 1);
        
        // Unlock evolution achievement
        m_achievementSystem.unlock(AchievementType::Evolution);
        
        // Unlock master achievement if reached max level
        if (m_evolutionLevel == EvolutionLevel::Master) {
            m_achievementSystem.unlock(AchievementType::Master);
        }
        
        // Unlock eternal achievement if reached ancient level
        if (m_evolutionLevel == EvolutionLevel::Ancient) {
            m_achievementSystem.unlock(AchievementType::Eternal);
        }
        
        return true; // Evolved
    }
    
    return false; // No evolution
}

void PetState::increaseHunger(float amount) {
    float oldHunger = m_hunger;
    m_hunger = std::min<float>(100.0f, m_hunger + amount);
    // Check for achievement
    if (oldHunger < 99.0f && m_hunger >= 100.0f) {
        m_achievementSystem.unlock(AchievementType::WellFed);
    }
}

void PetState::decreaseHunger(float amount) {
    m_hunger = std::max<float>(0.0f, m_hunger - amount);
}

void PetState::increaseHappiness(float amount) {
    float oldHappiness = m_happiness;
    m_happiness = std::min<float>(100.0f, m_happiness + amount);
    // Check for achievement
    if (oldHappiness < 99.0f && m_happiness >= 100.0f) {
        m_achievementSystem.unlock(AchievementType::HappyDays);
    }
}

void PetState::decreaseHappiness(float amount) {
    m_happiness = std::max<float>(0.0f, m_happiness - amount);
}

void PetState::increaseEnergy(float amount) {
    float oldEnergy = m_energy;
    m_energy = std::min<float>(100.0f, m_energy + amount);
    // Check for achievement
    if (oldEnergy < 99.0f && m_energy >= 100.0f) {
        m_achievementSystem.unlock(AchievementType::FullyRested);
    }
}

void PetState::decreaseEnergy(float amount) {
    m_energy = std::max<float>(0.0f, m_energy - amount);
}

void PetState::updateInteractionTime() {
    m_lastInteractionTime = std::chrono::system_clock::now();
}

uint32_t PetState::getXPForNextLevel() const {
    // XP required for each evolution level
    static const std::array<uint32_t, 6> xpRequirements = {
        100,   // Egg -> Baby
        300,   // Baby -> Child
        600,   // Child -> Teen
        1000,  // Teen -> Adult
        2000,  // Adult -> Master
        10000  // Master -> Ancient
    };
    
    uint8_t currentLevel = static_cast<uint8_t>(m_evolutionLevel);
    if (currentLevel >= xpRequirements.size()) {
        return UINT32_MAX; // No more evolution possible
    }
    
    return xpRequirements[currentLevel];
}

std::string_view PetState::getAsciiArt() const {
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

std::string_view PetState::getDescription() const {
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

std::string_view PetState::getStatusDescription() const {
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
