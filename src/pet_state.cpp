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
    , m_hunger(50)
    , m_happiness(50)
    , m_energy(50)
    , m_lastInteractionTime(std::chrono::system_clock::now())
{
    // Initialize achievements
    m_achievements = {
        {"First Steps", "Feed your pet for the first time", false},
        {"Playful", "Play with your pet 5 times", false},
        {"Well Fed", "Reach 100% hunger", false},
        {"Happy Days", "Reach 100% happiness", false},
        {"Energetic", "Reach 100% energy", false},
        {"Evolution", "Evolve your pet to the next stage", false},
        {"Master", "Reach the final evolution stage", false}
    };
}

void PetState::initialize() {
    m_name = "Unnamed Pet";
    m_evolutionLevel = EvolutionLevel::Egg;
    m_xp = 0;
    m_hunger = 50;
    m_happiness = 50;
    m_energy = 50;
    updateInteractionTime();
    
    // Reset achievements
    for (auto& achievement : m_achievements) {
        achievement.unlocked = false;
    }
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

bool PetState::load() {
    try {
        auto statePath = getStateFilePath();
        
        // Check if file exists
        if (!std::filesystem::exists(statePath)) {
            return false;
        }
        
        std::ifstream file(statePath, std::ios::binary);
        if (!file) {
            std::cerr << "Failed to open state file for reading: " << statePath.string() << std::endl;
            return false;
        }
        
        // File format version
        uint8_t version;
        file.read(reinterpret_cast<char*>(&version), sizeof(version));
        if (version != 1) {
            std::cerr << "Unsupported state file version: " << static_cast<int>(version) << std::endl;
            return false;
        }
        
        // Read name length and name
        uint16_t nameLength;
        file.read(reinterpret_cast<char*>(&nameLength), sizeof(nameLength));
        m_name.resize(nameLength);
        file.read(&m_name[0], nameLength);
        
        // Read basic stats
        uint8_t evolutionLevel;
        file.read(reinterpret_cast<char*>(&evolutionLevel), sizeof(evolutionLevel));
        m_evolutionLevel = static_cast<EvolutionLevel>(evolutionLevel);
        
        file.read(reinterpret_cast<char*>(&m_xp), sizeof(m_xp));
        file.read(reinterpret_cast<char*>(&m_hunger), sizeof(m_hunger));
        file.read(reinterpret_cast<char*>(&m_happiness), sizeof(m_happiness));
        file.read(reinterpret_cast<char*>(&m_energy), sizeof(m_energy));
        
        // Read last interaction time as seconds since epoch
        uint64_t lastInteractionSeconds;
        file.read(reinterpret_cast<char*>(&lastInteractionSeconds), sizeof(lastInteractionSeconds));
        m_lastInteractionTime = std::chrono::system_clock::from_time_t(static_cast<std::time_t>(lastInteractionSeconds));
        
        // Read achievements
        uint8_t achievementCount;
        file.read(reinterpret_cast<char*>(&achievementCount), sizeof(achievementCount));
        
        m_achievements.clear();
        for (uint8_t i = 0; i < achievementCount; ++i) {
            Achievement achievement;
            
            // Read name
            uint16_t nameLen;
            file.read(reinterpret_cast<char*>(&nameLen), sizeof(nameLen));
            achievement.name.resize(nameLen);
            file.read(&achievement.name[0], nameLen);
            
            // Read description
            uint16_t descLen;
            file.read(reinterpret_cast<char*>(&descLen), sizeof(descLen));
            achievement.description.resize(descLen);
            file.read(&achievement.description[0], descLen);
            
            // Read unlocked status
            file.read(reinterpret_cast<char*>(&achievement.unlocked), sizeof(achievement.unlocked));
            
            m_achievements.push_back(achievement);
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
        const uint8_t version = 1;
        file.write(reinterpret_cast<const char*>(&version), sizeof(version));
        
        // Write name
        uint16_t nameLength = static_cast<uint16_t>(m_name.length());
        file.write(reinterpret_cast<const char*>(&nameLength), sizeof(nameLength));
        file.write(m_name.c_str(), nameLength);
        
        // Write basic stats
        uint8_t evolutionLevel = static_cast<uint8_t>(m_evolutionLevel);
        file.write(reinterpret_cast<const char*>(&evolutionLevel), sizeof(evolutionLevel));
        
        file.write(reinterpret_cast<const char*>(&m_xp), sizeof(m_xp));
        file.write(reinterpret_cast<const char*>(&m_hunger), sizeof(m_hunger));
        file.write(reinterpret_cast<const char*>(&m_happiness), sizeof(m_happiness));
        file.write(reinterpret_cast<const char*>(&m_energy), sizeof(m_energy));
        
        // Write last interaction time as seconds since epoch
        uint64_t lastInteractionSeconds = 
            std::chrono::duration_cast<std::chrono::seconds>(
                m_lastInteractionTime.time_since_epoch()).count();
        file.write(reinterpret_cast<const char*>(&lastInteractionSeconds), sizeof(lastInteractionSeconds));
        
        // Write achievements
        uint8_t achievementCount = static_cast<uint8_t>(m_achievements.size());
        file.write(reinterpret_cast<const char*>(&achievementCount), sizeof(achievementCount));
        
        for (const auto& achievement : m_achievements) {
            // Write name
            uint16_t nameLen = static_cast<uint16_t>(achievement.name.length());
            file.write(reinterpret_cast<const char*>(&nameLen), sizeof(nameLen));
            file.write(achievement.name.c_str(), nameLen);
            
            // Write description
            uint16_t descLen = static_cast<uint16_t>(achievement.description.length());
            file.write(reinterpret_cast<const char*>(&descLen), sizeof(descLen));
            file.write(achievement.description.c_str(), descLen);
            
            // Write unlocked status
            file.write(reinterpret_cast<const char*>(&achievement.unlocked), sizeof(achievement.unlocked));
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

bool PetState::addXP(uint32_t amount) {
    m_xp += amount;
    
    // Check if we should evolve
    uint32_t xpForNextLevel = getXPForNextLevel();
    
    if (m_xp >= xpForNextLevel && m_evolutionLevel < EvolutionLevel::Master) {
        m_evolutionLevel = static_cast<EvolutionLevel>(static_cast<uint8_t>(m_evolutionLevel) + 1);
        
        // Unlock achievement if evolved
        for (auto& achievement : m_achievements) {
            if (achievement.name == "Evolution") {
                achievement.unlocked = true;
            }
            if (achievement.name == "Master" && m_evolutionLevel == EvolutionLevel::Master) {
                achievement.unlocked = true;
            }
        }
        
        return true; // Evolved
    }
    
    return false; // No evolution
}

void PetState::increaseHunger(uint8_t amount) {
    uint16_t newValue = static_cast<uint16_t>(m_hunger) + amount;
    m_hunger = (newValue > 100) ? 100 : static_cast<uint8_t>(newValue);
    
    // Check for achievement
    if (m_hunger == 100) {
        for (auto& achievement : m_achievements) {
            if (achievement.name == "Well Fed") {
                achievement.unlocked = true;
            }
        }
    }
}

void PetState::increaseHappiness(uint8_t amount) {
    uint16_t newValue = static_cast<uint16_t>(m_happiness) + amount;
    m_happiness = (newValue > 100) ? 100 : static_cast<uint8_t>(newValue);
    
    // Check for achievement
    if (m_happiness == 100) {
        for (auto& achievement : m_achievements) {
            if (achievement.name == "Happy Days") {
                achievement.unlocked = true;
            }
        }
    }
}

void PetState::increaseEnergy(uint8_t amount) {
    uint16_t newValue = static_cast<uint16_t>(m_energy) + amount;
    m_energy = (newValue > 100) ? 100 : static_cast<uint8_t>(newValue);
    
    // Check for achievement
    if (m_energy == 100) {
        for (auto& achievement : m_achievements) {
            if (achievement.name == "Energetic") {
                achievement.unlocked = true;
            }
        }
    }
}

void PetState::decreaseHunger(uint8_t amount) {
    m_hunger = (m_hunger > amount) ? m_hunger - amount : 0;
}

void PetState::decreaseHappiness(uint8_t amount) {
    m_happiness = (m_happiness > amount) ? m_happiness - amount : 0;
}

void PetState::decreaseEnergy(uint8_t amount) {
    m_energy = (m_energy > amount) ? m_energy - amount : 0;
}

void PetState::updateInteractionTime() {
    m_lastInteractionTime = std::chrono::system_clock::now();
}

uint32_t PetState::getXPForNextLevel() const {
    // XP required for each evolution level
    static const std::array<uint32_t, 5> xpRequirements = {
        100,   // Egg -> Baby
        300,   // Baby -> Child
        600,   // Child -> Teen
        1000,  // Teen -> Adult
        2000   // Adult -> Master
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
  .--.
 /    \
|      |
 \    /
  '--'
)";
    static const std::string babyArt = R"(
  ^__^
 (o.o)
 (___) 
)";
    static const std::string childArt = R"(
  ^___^
 (o   o)
 (>.<) 
  ---  
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
      /\_/\
 /\  / o o \
(  )/ > ^ < \
 \/  \~~~~~/ 
     /     \
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
        default:
            static const std::string unknown = "Unknown evolution level";
            return unknown;
    }
}

void PetState::checkAchievements() {
    // This method can be expanded to check for more complex achievements
    // Currently, most achievements are checked in their respective methods
}
