#include "../include/display_manager.h"
#include <iostream>
#include <algorithm>
#include <cmath>
#include <chrono>
#include <iomanip>
#include <format>

DisplayManager::DisplayManager(PetState& petState) noexcept
    : m_petState(petState)
{
}

void DisplayManager::displayMessage(std::string_view message) const noexcept {
    std::cout << message << std::endl;
}

void DisplayManager::clearScreen() const noexcept {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void DisplayManager::displayPetHeader() const noexcept {
    std::cout << m_petState.getAsciiArt() << std::endl;
    
    std::cout << "Name: " << m_petState.getName() << std::endl;
    std::cout << "Evolution: ";
    
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
    
    // Add pet description - using the more detailed status description instead of the basic description
    std::cout << "Status: " << m_petState.getStatusDescription() << std::endl;
    
    // Get maximum stat value for current evolution level
    float maxStatValue = m_petState.getMaxStatValue();
    
    // Statistics right after the description - display absolute values not percentages
    std::cout << "\nStats:" << std::endl;
    std::cout << "  Hunger: " << static_cast<int>(std::floor(m_petState.getHunger())) << " / "
              << static_cast<int>(maxStatValue) << std::endl;
    std::cout << "  Happiness: " << static_cast<int>(std::floor(m_petState.getHappiness())) << " / "
              << static_cast<int>(maxStatValue) << std::endl;
    std::cout << "  Energy: " << static_cast<int>(std::floor(m_petState.getEnergy())) << " / "
              << static_cast<int>(maxStatValue) << std::endl;
    std::cout << "  XP: " << m_petState.getXP();
    
    if (m_petState.getEvolutionLevel() != EvolutionLevel::Ancient) {
        std::cout << " / " << m_petState.getXPForNextLevel() << " for next level";
    }
    std::cout << std::endl;
    
    // Add achievements information
    const auto& achievementSystem = m_petState.getAchievementSystem();
    auto unlockedAchievements = achievementSystem.getUnlockedAchievements();
    std::cout << "Achievements: " << unlockedAchievements.size() << "/" << static_cast<int>(AchievementType::Count) << " unlocked" << std::endl << std::endl;
}

std::string_view DisplayManager::getEvolutionLevelName(EvolutionLevel level) const noexcept {
    switch (level) {
        case EvolutionLevel::Egg:
            return "Egg";
        case EvolutionLevel::Baby:
            return "Baby";
        case EvolutionLevel::Child:
            return "Child";
        case EvolutionLevel::Teen:
            return "Teen";
        case EvolutionLevel::Adult:
            return "Adult";
        case EvolutionLevel::Master:
            return "Master";
        case EvolutionLevel::Ancient:
            return "Ancient";
        default:
            return "Unknown";
    }
}
