#include "../include/interaction_manager.h"
#include <iostream>
#include <algorithm>
#include <format>

InteractionManager::InteractionManager(
    PetState& petState, 
    DisplayManager& displayManager, 
    AchievementManager& achievementManager)
    : m_petState(petState)
    , m_displayManager(displayManager)
    , m_achievementManager(achievementManager)
{
}

void InteractionManager::feedPet() {
    // Check if pet was already full
    bool wasFull = (m_petState.getHunger() == 100);
    
    // Increase hunger and add XP
    m_petState.increaseHunger(20);
    bool evolved = m_petState.addXP(10);
    
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
    } else if (wasFull && m_petState.getHunger() == 100) {
        m_displayManager.displayMessage("Your pet is already full! It doesn't want to eat more.");
    } else if (m_petState.getHunger() == 100) {
        m_displayManager.displayMessage("Your pet is now full and very satisfied!");
    } else {
        m_displayManager.displayMessage("Your pet enjoys the food and feels less hungry.");
    }
    
    // Check for newly unlocked achievements
    m_achievementManager.displayNewlyUnlockedAchievements();
    
    // Show current hunger level
    std::cout << "Hunger: " << static_cast<int>(std::floor(m_petState.getHunger())) << "%" << std::endl;
    std::cout << "XP: " << m_petState.getXP();
    if (m_petState.getEvolutionLevel() != PetState::EvolutionLevel::Ancient) {
        std::cout << " / " << m_petState.getXPForNextLevel() << " for next level";
    }
    std::cout << std::endl;
}

void InteractionManager::playWithPet() {
    // Increase happiness and decrease energy
    m_petState.increaseHappiness(15);
    m_petState.decreaseEnergy(10);
    
    // Add XP
    bool evolved = m_petState.addXP(15);
    
    // Update interaction time
    m_petState.updateInteractionTime();
    
    // Display message
    if (evolved) {
        std::cout << "Your pet " << m_petState.getName() << " has evolved to " 
                << m_displayManager.getEvolutionLevelName(m_petState.getEvolutionLevel()) 
                << "!" << std::endl;
        std::cout << m_petState.getAsciiArt() << std::endl;
        std::cout << m_petState.getDescription() << std::endl;
    } else if (m_petState.getHappiness() == 100) {
        m_displayManager.displayMessage("Your pet is extremely happy! It's having the time of its life!");
    } else {
        m_displayManager.displayMessage("Your pet jumps around playfully. It's having fun!");
    }
    
    // Track play count for Playful achievement
    m_petState.getAchievementSystem().incrementProgress(AchievementType::Playful);
    
    // Check for newly unlocked achievements
    m_achievementManager.displayNewlyUnlockedAchievements();
    
    // Show current stats
    std::cout << "Happiness: " << static_cast<int>(std::floor(m_petState.getHappiness())) << "%" << std::endl;
    std::cout << "Energy: " << static_cast<int>(std::floor(m_petState.getEnergy())) << "%" << std::endl;
    std::cout << "XP: " << m_petState.getXP();
    if (m_petState.getEvolutionLevel() != PetState::EvolutionLevel::Ancient) {
        std::cout << " / " << m_petState.getXPForNextLevel() << " for next level";
    }
    std::cout << std::endl;
}

void InteractionManager::showStatus() const {
    m_displayManager.displayPetHeader();
}

void InteractionManager::showEvolutionProgress() const {
    std::cout << "\n" << m_petState.getAsciiArt() << std::endl;
    
    std::cout << "Current evolution: ";
    switch (m_petState.getEvolutionLevel()) {
        case PetState::EvolutionLevel::Egg:
            std::cout << "Egg (Level 0)";
            break;
        case PetState::EvolutionLevel::Baby:
            std::cout << "Baby (Level 1)";
            break;
        case PetState::EvolutionLevel::Child:
            std::cout << "Child (Level 2)";
            break;
        case PetState::EvolutionLevel::Teen:
            std::cout << "Teen (Level 3)";
            break;
        case PetState::EvolutionLevel::Adult:
            std::cout << "Adult (Level 4)";
            break;
        case PetState::EvolutionLevel::Master:
            std::cout << "Master (Level 5)";
            break;
        case PetState::EvolutionLevel::Ancient:
            std::cout << "Ancient";
            break;
    }
    std::cout << std::endl;
    
    std::cout << "Description: " << m_petState.getDescription() << std::endl;
    
    if (m_petState.getEvolutionLevel() != PetState::EvolutionLevel::Ancient) {
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
            case PetState::EvolutionLevel::Egg:
                std::cout << "Baby (Level 1)";
                break;
            case PetState::EvolutionLevel::Baby:
                std::cout << "Child (Level 2)";
                break;
            case PetState::EvolutionLevel::Child:
                std::cout << "Teen (Level 3)";
                break;
            case PetState::EvolutionLevel::Teen:
                std::cout << "Adult (Level 4)";
                break;
            case PetState::EvolutionLevel::Adult:
                std::cout << "Master (Level 5)";
                break;
            case PetState::EvolutionLevel::Master:
                std::cout << "Ancient";
                break;
            case PetState::EvolutionLevel::Ancient:
                std::cout << "Already at maximum evolution";
                break;
        }
        std::cout << std::endl;
    } else {
        std::cout << "\nYour pet has reached the highest evolution level!" << std::endl;
    }
}

bool InteractionManager::createNewPet(bool force) {
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
