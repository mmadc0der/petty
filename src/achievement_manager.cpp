#include "../include/achievement_manager.h"
#include <iostream>
#include <algorithm>
#include <cmath>

AchievementManager::AchievementManager(PetState& petState) noexcept
    : m_petState(petState)
{
}

bool AchievementManager::displayAchievements(bool newlyUnlocked) const noexcept {
    const auto& achievementSystem = m_petState.getAchievementSystem();
    auto unlockedAchievements = achievementSystem.getUnlockedAchievements();
    
    if (unlockedAchievements.empty()) {
        if (!newlyUnlocked) {
            std::cout << "\nNo achievements unlocked yet." << std::endl;
        }
        return false;
    }
    
    std::cout << "\nAchievements:" << std::endl;
    for (const auto& achievement : unlockedAchievements) {
        std::cout << "  - " << AchievementSystem::getName(achievement) 
                << ": " << AchievementSystem::getDescription(achievement) << std::endl;
    }
    
    return !unlockedAchievements.empty();
}

bool AchievementManager::displayNewlyUnlockedAchievements() noexcept {
    auto& achievementSystem = m_petState.getAchievementSystem();
    auto newlyUnlocked = achievementSystem.getNewlyUnlockedAchievements();
    
    bool hasDisplayed = false;
    for (const auto& achievement : newlyUnlocked) {
        if (achievement != AchievementType::FirstSteps) { // First Steps is handled separately
            std::cout << "\nAchievement unlocked: " 
                    << AchievementSystem::getName(achievement) 
                    << "!" << std::endl;
            hasDisplayed = true;
        }
    }
    
    achievementSystem.clearNewlyUnlocked();
    return hasDisplayed;
}

void AchievementManager::showAllAchievements() const noexcept {
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
                << " (Level " << currentLevel << "/6)" << std::endl;
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
    
    // Check for eternal achievement progress
    if (std::find(unlockedAchievements.begin(), unlockedAchievements.end(), AchievementType::Eternal) 
        == unlockedAchievements.end()) {
        hasLockedAchievements = true;
        auto currentLevel = static_cast<int>(m_petState.getEvolutionLevel());
        std::cout << "  - " << AchievementSystem::getName(AchievementType::Eternal) 
                << ": " << AchievementSystem::getDescription(AchievementType::Eternal) 
                << " (Level " << currentLevel << "/6)" << std::endl;
    }
    
    // Check for hunger-based achievement progress
    if (std::find(unlockedAchievements.begin(), unlockedAchievements.end(), AchievementType::WellFed) 
        == unlockedAchievements.end()) {
        hasLockedAchievements = true;
        auto currentHunger = static_cast<int>(std::floor(m_petState.getHunger()));
        std::cout << "  - " << AchievementSystem::getName(AchievementType::WellFed) 
                << ": " << AchievementSystem::getDescription(AchievementType::WellFed) 
                << " (" << currentHunger << "/100)" << std::endl;
    }
    
    // Check for happiness-based achievement progress
    if (std::find(unlockedAchievements.begin(), unlockedAchievements.end(), AchievementType::HappyDays) 
        == unlockedAchievements.end()) {
        hasLockedAchievements = true;
        auto currentHappiness = static_cast<int>(std::floor(m_petState.getHappiness()));
        std::cout << "  - " << AchievementSystem::getName(AchievementType::HappyDays) 
                << ": " << AchievementSystem::getDescription(AchievementType::HappyDays) 
                << " (" << currentHappiness << "/100)" << std::endl;
    }
    
    // Check for energy-based achievement progress
    if (std::find(unlockedAchievements.begin(), unlockedAchievements.end(), AchievementType::FullyRested) 
        == unlockedAchievements.end()) {
        hasLockedAchievements = true;
        auto currentEnergy = static_cast<int>(std::floor(m_petState.getEnergy()));
        std::cout << "  - " << AchievementSystem::getName(AchievementType::FullyRested) 
                << ": " << AchievementSystem::getDescription(AchievementType::FullyRested) 
                << " (" << currentEnergy << "/100)" << std::endl;
    }
    
    // Check for dedicated achievement progress
    if (std::find(unlockedAchievements.begin(), unlockedAchievements.end(), AchievementType::Dedicated) 
        == unlockedAchievements.end()) {
        hasLockedAchievements = true;
        std::cout << "  - " << AchievementSystem::getName(AchievementType::Dedicated) 
                << ": " << AchievementSystem::getDescription(AchievementType::Dedicated) 
                << " (" << achievementSystem.getProgress(AchievementType::Dedicated) 
                << "/" << AchievementSystem::getRequiredProgress(AchievementType::Dedicated) << ")" << std::endl;
    }
    
    // Check for explorer achievement progress
    if (std::find(unlockedAchievements.begin(), unlockedAchievements.end(), AchievementType::Explorer) 
        == unlockedAchievements.end()) {
        hasLockedAchievements = true;
        std::cout << "  - " << AchievementSystem::getName(AchievementType::Explorer) 
                << ": " << AchievementSystem::getDescription(AchievementType::Explorer) 
                << " (" << achievementSystem.getProgress(AchievementType::Explorer) 
                << "/" << AchievementSystem::getRequiredProgress(AchievementType::Explorer) << ")" << std::endl;
    }
    
    // Check for survivor achievement progress
    if (std::find(unlockedAchievements.begin(), unlockedAchievements.end(), AchievementType::Survivor) 
        == unlockedAchievements.end()) {
        hasLockedAchievements = true;
        std::cout << "  - " << AchievementSystem::getName(AchievementType::Survivor) 
                << ": " << AchievementSystem::getDescription(AchievementType::Survivor) 
                << " (" << achievementSystem.getProgress(AchievementType::Survivor) 
                << "/" << AchievementSystem::getRequiredProgress(AchievementType::Survivor) << ")" << std::endl;
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
