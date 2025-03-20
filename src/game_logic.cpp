#include "../include/game_logic.h"
#include <iostream>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <format>

GameLogic::GameLogic(PetState& petState)
    : m_petState(petState)
{
    // Apply time-based effects when creating the game logic
    auto timeEffectMessage = applyTimeEffects();
    if (timeEffectMessage) {
        displayMessage(*timeEffectMessage);
    }
}

void GameLogic::showStatus() const {
    std::cout << "\n" << m_petState.getAsciiArt() << std::endl;
    
    std::cout << "Name: " << m_petState.getName() << std::endl;
    std::cout << "Evolution: ";
    
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
    }
    std::cout << std::endl;
    
    std::cout << "Description: " << m_petState.getDescription() << std::endl;
    
    std::cout << "\nStats:" << std::endl;
    std::cout << "  XP: " << m_petState.getXP();
    
    if (m_petState.getEvolutionLevel() != PetState::EvolutionLevel::Master) {
        std::cout << " / " << m_petState.getXPForNextLevel() << " for next level";
    }
    std::cout << std::endl;
    
    std::cout << "  Hunger: " << static_cast<int>(m_petState.getHunger()) << "%" << std::endl;
    std::cout << "  Happiness: " << static_cast<int>(m_petState.getHappiness()) << "%" << std::endl;
    std::cout << "  Energy: " << static_cast<int>(m_petState.getEnergy()) << "%" << std::endl;
    
    // Display last interaction time
    auto lastTime = m_petState.getLastInteractionTime();
    auto timeT = std::chrono::system_clock::to_time_t(lastTime);
    
    // Use localtime_s instead of localtime for safety
    std::tm tmBuf;
    std::tm* tm = &tmBuf;
#ifdef _WIN32
    localtime_s(tm, &timeT);
#else
    localtime_r(&timeT, tm);
#endif
    
    std::stringstream ss;
    ss << std::put_time(tm, "%Y-%m-%d %H:%M:%S");
    
    std::cout << "\nLast interaction: " << ss.str() << std::endl;
    
    // Display achievements
    displayAchievements();
}

void GameLogic::feedPet() {
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
        std::cout << "Your pet has evolved to " 
                  << static_cast<int>(m_petState.getEvolutionLevel()) 
                  << "!" << std::endl;
        std::cout << m_petState.getAsciiArt() << std::endl;
        std::cout << m_petState.getDescription() << std::endl;
    } else if (wasFull && m_petState.getHunger() == 100) {
        displayMessage("Your pet is already full! It doesn't want to eat more.");
    } else if (m_petState.getHunger() == 100) {
        displayMessage("Your pet is now full and very satisfied!");
    } else {
        displayMessage("Your pet enjoys the food and feels less hungry.");
    }
    
    // Check for newly unlocked achievements
    displayNewlyUnlockedAchievements();
    
    // Show current hunger level
    std::cout << "Hunger: " << static_cast<int>(m_petState.getHunger()) << "%" << std::endl;
    std::cout << "XP: " << m_petState.getXP();
    if (m_petState.getEvolutionLevel() != PetState::EvolutionLevel::Master) {
        std::cout << " / " << m_petState.getXPForNextLevel() << " for next level";
    }
    std::cout << std::endl;
}

void GameLogic::playWithPet() {
    // Increase happiness and decrease energy
    m_petState.increaseHappiness(15);
    m_petState.decreaseEnergy(10);
    
    // Add XP
    bool evolved = m_petState.addXP(150);
    
    // Update interaction time
    m_petState.updateInteractionTime();
    
    // Display message
    if (evolved) {
        std::cout << "Your pet has evolved to " 
                  << static_cast<int>(m_petState.getEvolutionLevel()) 
                  << "!" << std::endl;
        std::cout << m_petState.getAsciiArt() << std::endl;
        std::cout << m_petState.getDescription() << std::endl;
    } else if (m_petState.getHappiness() == 100) {
        displayMessage("Your pet is extremely happy! It's having the time of its life!");
    } else {
        displayMessage("Your pet jumps around playfully. It's having fun!");
    }
    
    // Track play count for Playful achievement
    static int playCount = 0;
    playCount++;
    
    if (playCount >= 5) {
        m_petState.getAchievementSystem().unlock(AchievementType::Playful);
    }
    
    // Check for newly unlocked achievements
    displayNewlyUnlockedAchievements();
    
    // Show current stats
    std::cout << "Happiness: " << static_cast<int>(m_petState.getHappiness()) << "%" << std::endl;
    std::cout << "Energy: " << static_cast<int>(m_petState.getEnergy()) << "%" << std::endl;
    std::cout << "XP: " << m_petState.getXP();
    if (m_petState.getEvolutionLevel() != PetState::EvolutionLevel::Master) {
        std::cout << " / " << m_petState.getXPForNextLevel() << " for next level";
    }
    std::cout << std::endl;
}

void GameLogic::showEvolutionProgress() const {
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
    }
    std::cout << std::endl;
    
    std::cout << "Description: " << m_petState.getDescription() << std::endl;
    
    if (m_petState.getEvolutionLevel() != PetState::EvolutionLevel::Master) {
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
            else std::cout << " ";
        }
        
        std::cout << "] " << static_cast<int>(percentage) << "%" << std::endl;
        
        std::cout << "\nNext evolution: ";
        switch (static_cast<PetState::EvolutionLevel>(
                static_cast<uint8_t>(m_petState.getEvolutionLevel()) + 1)) {
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
            default:
                std::cout << "Unknown";
        }
        std::cout << std::endl;
    } else {
        std::cout << "\nYour pet has reached the maximum evolution level!" << std::endl;
        std::cout << "Congratulations on raising a master pet!" << std::endl;
    }
}

std::optional<std::string> GameLogic::applyTimeEffects() {
    auto lastTime = m_petState.getLastInteractionTime();
    if (lastTime == std::chrono::system_clock::time_point{}) {
        // First interaction, no effects to apply
        return std::nullopt;
    }
    
    auto now = std::chrono::system_clock::now();
    auto duration = now - lastTime;
    
    // Calculate hours passed
    double hoursPassed = std::chrono::duration<double, std::ratio<3600, 1>>(duration).count();
    
    if (hoursPassed < 0.1) {
        // Less than 6 minutes, no significant effects
        return std::nullopt;
    }
    
    // Apply effects based on time passed
    // For each hour, decrease hunger, happiness, and energy
    uint8_t hungerDecrease = static_cast<uint8_t>(std::min(5.0 * hoursPassed, 100.0));
    uint8_t happinessDecrease = static_cast<uint8_t>(std::min(3.0 * hoursPassed, 100.0));
    uint8_t energyIncrease = static_cast<uint8_t>(std::min(2.0 * hoursPassed, 100.0));
    
    m_petState.decreaseHunger(hungerDecrease);
    m_petState.decreaseHappiness(happinessDecrease);
    m_petState.increaseEnergy(energyIncrease); // Pet rests while away
    
    // Generate message if significant time has passed
    if (hoursPassed > 1.0) {
        std::stringstream ss;
        ss << std::fixed << std::setprecision(1);
        
        if (hoursPassed < 24.0) {
            ss << hoursPassed << " hours have passed since your last visit.";
        } else {
            double daysPassed = hoursPassed / 24.0;
            ss << daysPassed << " days have passed since your last visit.";
        }
        
        std::string message = ss.str();
        
        if (m_petState.getHunger() < 20) {
            message += "\nYour pet is very hungry!";
        }
        
        if (m_petState.getHappiness() < 20) {
            message += "\nYour pet is feeling lonely and sad.";
        }
        
        return message;
    }
    
    return std::nullopt;
}

void GameLogic::displayMessage(std::string_view message) const {
    std::cout << message << std::endl;
}

void GameLogic::displayAchievements(bool newlyUnlocked) const {
    const auto& achievementSystem = m_petState.getAchievementSystem();
    auto unlockedAchievements = achievementSystem.getUnlockedAchievements();
    
    if (unlockedAchievements.empty()) {
        if (!newlyUnlocked) {
            std::cout << "\nNo achievements unlocked yet." << std::endl;
        }
        return;
    }
    
    std::cout << "\nAchievements:" << std::endl;
    for (const auto& achievement : unlockedAchievements) {
        std::cout << "  - " << AchievementSystem::getName(achievement) 
                  << ": " << AchievementSystem::getDescription(achievement) << std::endl;
    }
}

void GameLogic::displayNewlyUnlockedAchievements() {
    auto& achievementSystem = m_petState.getAchievementSystem();
    auto newlyUnlocked = achievementSystem.getNewlyUnlockedAchievements();
    
    for (const auto& achievement : newlyUnlocked) {
        if (achievement != AchievementType::FirstSteps) { // First Steps is handled separately
            std::cout << "\nAchievement unlocked: " 
                      << AchievementSystem::getName(achievement) 
                      << "!" << std::endl;
        }
    }
    
    achievementSystem.clearNewlyUnlocked();
}
