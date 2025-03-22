#include "../include/interaction_manager.h"
#include "../include/game_config.h"
#include <iostream>
#include <algorithm>
#include <format>
#include <cmath>

InteractionManager::InteractionManager(
    PetState& petState, 
    DisplayManager& displayManager, 
    AchievementManager& achievementManager) noexcept
    : m_petState(petState)
    , m_displayManager(displayManager)
    , m_achievementManager(achievementManager)
{
}

void InteractionManager::feedPet() noexcept {
    // Get maximum stat value for this evolution level
    float maxStatValue = m_petState.getMaxStatValue();
    
    // Check if pet was already full
    bool wasFull = (m_petState.getHunger() >= maxStatValue - 0.01f); // Small epsilon to handle floating point comparisons
    
    // Increase hunger using absolute value from config
    m_petState.increaseHunger(GameConfig::getFeedingHungerIncrease());
    
    // Add XP
    bool evolved = m_petState.addXP(GameConfig::getFeedingXPGain());
    
    // Update interaction time
    m_petState.updateInteractionTime();
    
    // Unlock first steps achievement if first time feeding
    if (m_petState.getAchievementSystem().unlock(AchievementType::FirstSteps)) {
        std::cout << "\nAchievement unlocked: " 
                << AchievementSystem::getName(AchievementType::FirstSteps) 
                << "!" << std::endl;
    }
    
    // Display message
    if (evolved) {
        std::cout << "Your pet " << m_petState.getName() << " has evolved to " 
                << m_displayManager.getEvolutionLevelName(m_petState.getEvolutionLevel()) 
                << "!" << std::endl;
        std::cout << m_petState.getAsciiArt() << std::endl;
        std::cout << m_petState.getDescription() << std::endl;
    } else if (wasFull && m_petState.getHunger() >= maxStatValue - 0.01f) {
        m_displayManager.displayMessage("Your pet is already full! It doesn't want to eat more.");
    } else if (m_petState.getHunger() >= maxStatValue - 0.01f) {
        m_displayManager.displayMessage("Your pet is now full and very satisfied!");
    } else {
        m_displayManager.displayMessage("Your pet enjoys the food and feels less hungry.");
    }
    
    // Check for newly unlocked achievements
    m_achievementManager.displayNewlyUnlockedAchievements();
    
    // Show current hunger level (display absolute value, not percentage)
    std::cout << "Hunger: " << static_cast<int>(std::floor(m_petState.getHunger())) << " / " 
              << static_cast<int>(maxStatValue) << std::endl;
    std::cout << "XP: " << m_petState.getXP();
    if (m_petState.getEvolutionLevel() != EvolutionLevel::Ancient) {
        std::cout << " / " << m_petState.getXPForNextLevel() << " for next level";
    }
    std::cout << std::endl;
}

void InteractionManager::playWithPet() noexcept {
    // Get maximum stat value for this evolution level
    float maxStatValue = m_petState.getMaxStatValue();
    
    // Check if pet was already at max happiness
    bool wasMax = (m_petState.getHappiness() >= maxStatValue - 0.01f); // Small epsilon to handle floating point comparisons
    
    // Increase happiness and decrease energy using absolute values from config
    m_petState.increaseHappiness(GameConfig::getPlayingHappinessIncrease());
    m_petState.decreaseEnergy(GameConfig::getPlayingEnergyDecrease());
    
    // Add XP
    bool evolved = m_petState.addXP(GameConfig::getPlayingXPGain());
    
    // Update interaction time
    m_petState.updateInteractionTime();
    
    // Unlock social achievement if first time playing
    if (m_petState.getAchievementSystem().unlock(AchievementType::Playful)) {
        std::cout << "\nAchievement unlocked: " 
                << AchievementSystem::getName(AchievementType::Playful) 
                << "!" << std::endl;
    }
    
    // Display message
    if (evolved) {
        std::cout << "Your pet " << m_petState.getName() << " has evolved to " 
                << m_displayManager.getEvolutionLevelName(m_petState.getEvolutionLevel()) 
                << "!" << std::endl;
        std::cout << m_petState.getAsciiArt() << std::endl;
        std::cout << m_petState.getDescription() << std::endl;
    } else if (wasMax && m_petState.getHappiness() >= maxStatValue - 0.01f) {
        m_displayManager.displayMessage("Your pet is already extremely happy! It's having the time of its life!");
    } else {
        m_displayManager.displayMessage("Your pet jumps around playfully. It's having fun!");
    }
    
    // Track play count for Playful achievement
    m_petState.getAchievementSystem().incrementProgress(AchievementType::Playful);
    
    // Check for newly unlocked achievements
    m_achievementManager.displayNewlyUnlockedAchievements();
    
    // Show current stats (display absolute values, not percentages)
    std::cout << "Happiness: " << static_cast<int>(std::floor(m_petState.getHappiness())) << " / " 
              << static_cast<int>(maxStatValue) << std::endl;
    std::cout << "Energy: " << static_cast<int>(std::floor(m_petState.getEnergy())) << " / " 
              << static_cast<int>(maxStatValue) << std::endl;
    std::cout << "XP: " << m_petState.getXP();
    if (m_petState.getEvolutionLevel() != EvolutionLevel::Ancient) {
        std::cout << " / " << m_petState.getXPForNextLevel() << " for next level";
    }
    std::cout << std::endl;
}

void InteractionManager::showStatus() const noexcept {
    // Display pet header (ASCII art, name, evolution level)
    m_displayManager.displayPetHeader();
    
    auto now = std::chrono::system_clock::now();
    auto birthDate = m_petState.getBirthDate();
    
    auto birthTime_t = std::chrono::system_clock::to_time_t(birthDate);
    std::tm birthTm;
#ifdef _WIN32
    localtime_s(&birthTm, &birthTime_t);
#else
    localtime_r(&birthTime_t, &birthTm);
#endif
    
    char birthDateStr[20];
    std::strftime(birthDateStr, sizeof(birthDateStr), "%d %b %Y", &birthTm);

    auto age = std::chrono::duration_cast<std::chrono::hours>(now - birthDate).count();
    int ageYears = age / (24 * 365);
    int ageDays = (age % (24 * 365)) / 24;
    
    std::cout << "Birth date: " << birthDateStr;
    std::cout << " (";
    if (ageYears > 0) {
        std::cout << ageYears << "y";
        if (ageDays > 0) {
            std::cout << " ";
        }
    }
    if (ageDays > 0) {
        std::cout << ageDays << "d";
    } else {
        std::cout << age << "h";
    }
    std::cout << ")";
    std::cout << std::endl;
    
    auto lastInteraction = m_petState.getLastInteractionTime();
    
    auto lastInteractionTime_t = std::chrono::system_clock::to_time_t(lastInteraction);
    std::tm lastInteractionTm;
#ifdef _WIN32
    localtime_s(&lastInteractionTm, &lastInteractionTime_t);
#else
    localtime_r(&lastInteractionTime_t, &lastInteractionTm);
#endif
    
    char lastInteractionStr[20];
    std::strftime(lastInteractionStr, sizeof(lastInteractionStr), "%d %b %Y", &lastInteractionTm);
    
    auto timeSinceLastInteraction = std::chrono::duration_cast<std::chrono::minutes>(now - lastInteraction).count();
    int days = timeSinceLastInteraction / (60 * 24);
    int hours = (timeSinceLastInteraction % (60 * 24)) / 60;
    int minutes = timeSinceLastInteraction % 60;
    
    std::cout << "Last interaction: " << lastInteractionStr;
    std::cout << " (";
    if (days > 0) {
        std::cout << days << "d";
        if (hours > 0 || minutes > 0) {
            std::cout << " ";
        }
    }
    if (hours > 0) {
        std::cout << hours << "h";
        if (minutes > 0) {
            std::cout << " ";
        }
    }
    std::cout << minutes << "m";
    std::cout << ")";
    std::cout << std::endl << std::endl;
}

void InteractionManager::showEvolutionProgress() const noexcept {
    std::cout << "\n" << m_petState.getAsciiArt() << std::endl;
    
    std::cout << "Current evolution: ";
    switch (m_petState.getEvolutionLevel()) {
        case EvolutionLevel::Egg:
            std::cout << "Egg (Level 0)";
            break;
        case EvolutionLevel::Baby:
            std::cout << "Baby (Level 1)";
            break;
        case EvolutionLevel::Child:
            std::cout << "Child (Level 2)";
            break;
        case EvolutionLevel::Teen:
            std::cout << "Teen (Level 3)";
            break;
        case EvolutionLevel::Adult:
            std::cout << "Adult (Level 4)";
            break;
        case EvolutionLevel::Master:
            std::cout << "Master (Level 5)";
            break;
        case EvolutionLevel::Ancient:
            std::cout << "Ancient";
            break;
    }
    std::cout << std::endl;
    
    std::cout << "Description: " << m_petState.getDescription() << std::endl;
    
    if (m_petState.getEvolutionLevel() != EvolutionLevel::Ancient) {
        uint32_t currentXP = m_petState.getXP();
        uint32_t requiredXP = m_petState.getXPForNextLevel();
        float percentage = static_cast<float>(currentXP) / requiredXP * 100.0f;
        
        std::cout << "\nProgress to next evolution:" << std::endl;
        std::cout << "XP: " << currentXP << " / " << requiredXP 
                << " (" << static_cast<int>(percentage) << "%)" << std::endl;
        
        // Display a simple progress bar
        std::cout << "[";
        int barWidth = 20;
        int pos = static_cast<int>(barWidth * percentage / 100.0f);
        for (int i = 0; i < barWidth; ++i) {
            if (i < pos) std::cout << "=";
            else if (i == pos) std::cout << ">";
            else std::cout << " ";
        }
        std::cout << "] " << static_cast<int>(percentage) << "%" << std::endl;
        
        // Show next evolution level
        std::cout << "\nNext evolution: ";
        switch (m_petState.getEvolutionLevel()) {
            case EvolutionLevel::Egg:
                std::cout << "Baby (Level 1)";
                break;
            case EvolutionLevel::Baby:
                std::cout << "Child (Level 2)";
                break;
            case EvolutionLevel::Child:
                std::cout << "Teen (Level 3)";
                break;
            case EvolutionLevel::Teen:
                std::cout << "Adult (Level 4)";
                break;
            case EvolutionLevel::Adult:
                std::cout << "Master (Level 5)";
                break;
            case EvolutionLevel::Master:
                std::cout << "Ancient";
                break;
            case EvolutionLevel::Ancient:
                std::cout << "Already at maximum evolution";
                break;
        }
        std::cout << std::endl;
    } else {
        std::cout << "\nYour pet has reached the highest evolution level!" << std::endl;
    }
}

bool InteractionManager::createNewPet(bool force) noexcept {
    // Check if a save file exists and we're not forcing overwrite
    if (m_petState.saveFileExists() && !force) {
        std::cout << "A pet already exists. Do you want to overwrite it? (yes/no): ";
        std::string response;
        std::getline(std::cin, response);
        
        // Convert to lowercase for case-insensitive comparison
        std::transform(response.begin(), response.end(), response.begin(), 
                      [](unsigned char c) { return std::tolower(c); });
        
        if (response != "yes" && response != "y") {
            std::cout << "Pet creation cancelled." << std::endl;
            return false;
        }
    }
    
    // Ask for pet name
    std::cout << "Enter a name for your new pet: ";
    std::string name;
    std::getline(std::cin, name);
    
    // Trim whitespace from beginning and end
    name.erase(0, name.find_first_not_of(" \t\n\r\f\v"));
    name.erase(name.find_last_not_of(" \t\n\r\f\v") + 1);
    
    // Use default name if empty
    if (name.empty()) {
        name = "Unnamed Pet";
    }
    
    // Initialize new pet with the given name
    m_petState.initialize(name);
    std::cout << "\nCreated a new pet named '" << name << "'!" << std::endl;
    
    // Show the new pet's status
    showStatus();
    
    return true;
}
