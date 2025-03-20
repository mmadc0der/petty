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
    
    // Calculate current time
    auto now = std::chrono::system_clock::now();
    
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
    
    // Format last interaction time as "DD Mon YYYY HH:MM"
    std::stringstream ss;
    ss << std::put_time(tm, "%d %b %Y %H:%M");
    
    // Calculate time since last interaction
    auto timeSinceLastInteraction = std::chrono::duration_cast<std::chrono::seconds>(now - lastTime).count();
    
    // Format time since last interaction in humanized format
    std::string timeString;
    int lastDays = timeSinceLastInteraction / (24 * 60 * 60);
    int hours = (timeSinceLastInteraction % (24 * 60 * 60)) / (60 * 60);
    int minutes = (timeSinceLastInteraction % (60 * 60)) / 60;
    
    if (lastDays > 0) {
        timeString += std::to_string(lastDays) + "d ";
    }
    if (hours > 0 || lastDays > 0) {
        timeString += std::to_string(hours) + "h ";
    }
    timeString += std::to_string(minutes) + "m";
    
    std::cout << "\nLast interaction: " << ss.str() << " (" << timeString << ")" << std::endl;
    
    // Format birth date
    auto birthDate = m_petState.getBirthDate();
    auto birthTimeT = std::chrono::system_clock::to_time_t(birthDate);
    
    std::tm birthTmBuf;
    std::tm* birthTm = &birthTmBuf;
#ifdef _WIN32
    localtime_s(birthTm, &birthTimeT);
#else
    localtime_r(&birthTimeT, birthTm);
#endif
    
    // Format birth date as "DD Mon YYYY"
    std::stringstream birthSs;
    birthSs << std::put_time(birthTm, "%d %b %Y");
    
    // Calculate age
    auto ageSeconds = std::chrono::duration_cast<std::chrono::seconds>(now - birthDate).count();
    
    // Format age in humanized format
    std::string ageString;
    int years = ageSeconds / (365 * 24 * 60 * 60);
    int days = (ageSeconds % (365 * 24 * 60 * 60)) / (24 * 60 * 60);
    
    if (years > 0) {
        ageString += std::to_string(years) + "y ";
    }
    if (days > 0 || years == 0) {
        ageString += std::to_string(days) + "d";
    }
    
    std::cout << "Birth date: " << birthSs.str() << " (" << ageString << ")" << std::endl;
    
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
    m_petState.getAchievementSystem().incrementProgress(AchievementType::Playful);
    
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
    
    // Only increase energy if at least 1 hour has passed
    uint8_t energyIncrease = 0;
    if (hoursPassed >= 1.0) {
        energyIncrease = static_cast<uint8_t>(std::min(2.0 * hoursPassed, 100.0));
        m_petState.increaseEnergy(energyIncrease); // Pet rests while away
    }
    
    m_petState.decreaseHunger(hungerDecrease);
    m_petState.decreaseHappiness(happinessDecrease);
    
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

void GameLogic::showAchievements() const {
    const auto& achievementSystem = m_petState.getAchievementSystem();
    auto unlockedAchievements = achievementSystem.getUnlockedAchievements();
    
    std::cout << "\n===== ACHIEVEMENTS =====\n" << std::endl;
    
    // First show locked achievements with progress
    std::cout << "LOCKED ACHIEVEMENTS:" << std::endl;
    
    bool hasLockedAchievements = false;
    
    // Check for playful achievement progress
    if (std::find(unlockedAchievements.begin(), unlockedAchievements.end(), AchievementType::Playful) 
        == unlockedAchievements.end()) {
        hasLockedAchievements = true;
        std::cout << "  - " << AchievementSystem::getName(AchievementType::Playful) 
                  << ": " << AchievementSystem::getDescription(AchievementType::Playful) 
                  << " (" << achievementSystem.getProgress(AchievementType::Playful) 
                  << "/" << AchievementSystem::getRequiredProgress(AchievementType::Playful) << ")" << std::endl;
    }
    
    // Check for evolution achievement progress
    if (std::find(unlockedAchievements.begin(), unlockedAchievements.end(), AchievementType::Evolution) 
        == unlockedAchievements.end()) {
        hasLockedAchievements = true;
        auto currentLevel = static_cast<int>(m_petState.getEvolutionLevel());
        std::cout << "  - " << AchievementSystem::getName(AchievementType::Evolution) 
                  << ": " << AchievementSystem::getDescription(AchievementType::Evolution) 
                  << " (Level " << currentLevel << "/4)" << std::endl;
    }
    
    // Check for master achievement progress
    if (std::find(unlockedAchievements.begin(), unlockedAchievements.end(), AchievementType::Master) 
        == unlockedAchievements.end()) {
        hasLockedAchievements = true;
        auto currentLevel = static_cast<int>(m_petState.getEvolutionLevel());
        std::cout << "  - " << AchievementSystem::getName(AchievementType::Master) 
                  << ": " << AchievementSystem::getDescription(AchievementType::Master) 
                  << " (Level " << currentLevel << "/5)" << std::endl;
    }
    
    // Check for hunger-based achievement progress
    if (std::find(unlockedAchievements.begin(), unlockedAchievements.end(), AchievementType::WellFed) 
        == unlockedAchievements.end()) {
        hasLockedAchievements = true;
        auto currentHunger = static_cast<int>(m_petState.getHunger());
        std::cout << "  - " << AchievementSystem::getName(AchievementType::WellFed) 
                  << ": " << AchievementSystem::getDescription(AchievementType::WellFed) 
                  << " (" << currentHunger << "/100)" << std::endl;
    }
    
    // Check for happiness-based achievement progress
    if (std::find(unlockedAchievements.begin(), unlockedAchievements.end(), AchievementType::HappyDays) 
        == unlockedAchievements.end()) {
        hasLockedAchievements = true;
        auto currentHappiness = static_cast<int>(m_petState.getHappiness());
        std::cout << "  - " << AchievementSystem::getName(AchievementType::HappyDays) 
                  << ": " << AchievementSystem::getDescription(AchievementType::HappyDays) 
                  << " (" << currentHappiness << "/100)" << std::endl;
    }
    
    // Check for energy-based achievement progress
    if (std::find(unlockedAchievements.begin(), unlockedAchievements.end(), AchievementType::FullyRested) 
        == unlockedAchievements.end()) {
        hasLockedAchievements = true;
        auto currentEnergy = static_cast<int>(m_petState.getEnergy());
        std::cout << "  - " << AchievementSystem::getName(AchievementType::FullyRested) 
                  << ": " << AchievementSystem::getDescription(AchievementType::FullyRested) 
                  << " (" << currentEnergy << "/100)" << std::endl;
    }
    
    if (!hasLockedAchievements) {
        std::cout << "  None - You've unlocked all achievements!" << std::endl;
    }
    
    // Then show unlocked achievements
    std::cout << "\nUNLOCKED ACHIEVEMENTS:" << std::endl;
    
    if (unlockedAchievements.empty()) {
        std::cout << "  None yet. Keep playing!" << std::endl;
    } else {
        for (const auto& achievement : unlockedAchievements) {
            std::cout << "  - " << AchievementSystem::getName(achievement) 
                      << ": " << AchievementSystem::getDescription(achievement) << std::endl;
        }
    }
}
