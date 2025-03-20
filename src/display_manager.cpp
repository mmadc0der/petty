#include "../include/display_manager.h"
#include <iostream>
#include <algorithm>

DisplayManager::DisplayManager(PetState& petState)
    : m_petState(petState)
{
}

void DisplayManager::displayMessage(std::string_view message) const {
    std::cout << message << std::endl;
}

void DisplayManager::clearScreen() const {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void DisplayManager::displayPetHeader() const {
    std::cout << m_petState.getAsciiArt() << std::endl;
    
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
        case PetState::EvolutionLevel::Ancient:
            std::cout << "Ancient";
            break;
    }
    std::cout << std::endl;
    
    std::cout << "\nStats:" << std::endl;
    std::cout << "  Hunger: " << static_cast<int>(std::floor(m_petState.getHunger())) << "%" << std::endl;
    std::cout << "  Happiness: " << static_cast<int>(std::floor(m_petState.getHappiness())) << "%" << std::endl;
    std::cout << "  Energy: " << static_cast<int>(std::floor(m_petState.getEnergy())) << "%" << std::endl;
    std::cout << "  XP: " << m_petState.getXP();
    
    if (m_petState.getEvolutionLevel() != PetState::EvolutionLevel::Ancient) {
        std::cout << " / " << m_petState.getXPForNextLevel() << " for next level";
    }
    std::cout << std::endl << std::endl;
}

std::string DisplayManager::getEvolutionLevelName(PetState::EvolutionLevel level) const {
    switch (level) {
        case PetState::EvolutionLevel::Egg:
            return "Egg";
        case PetState::EvolutionLevel::Baby:
            return "Baby";
        case PetState::EvolutionLevel::Child:
            return "Child";
        case PetState::EvolutionLevel::Teen:
            return "Teen";
        case PetState::EvolutionLevel::Adult:
            return "Adult";
        case PetState::EvolutionLevel::Master:
            return "Master";
        case PetState::EvolutionLevel::Ancient:
            return "Ancient";
        default:
            return "Unknown";
    }
}
