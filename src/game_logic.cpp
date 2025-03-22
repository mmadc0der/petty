#include "../include/game_logic.h"
#include "../include/ui_manager.h"
#include <iostream>
#include <algorithm>
#include <format>
#include <memory>

GameLogic::GameLogic(PetState& petState) noexcept
    : m_petState(petState)
{
    // Initialize all managers
    m_displayManager = std::make_unique<DisplayManager>(m_petState);
    m_achievementManager = std::make_unique<AchievementManager>(m_petState);
    m_timeManager = std::make_unique<TimeManager>(m_petState);
    m_interactionManager = std::make_unique<InteractionManager>(
        m_petState, *m_displayManager, *m_achievementManager);
    
    // Note: UIManager will be initialized later via initializeUIManager()
}

void GameLogic::initializeUIManager() noexcept {
    // Create UI manager with necessary references to other managers
    m_uiManager = std::make_unique<UIManager>(
        m_petState, 
        *m_displayManager, 
        *m_achievementManager, 
        *m_interactionManager, 
        *m_timeManager
    );
    
    // Set up the connection with UI manager after creating all objects
    // Using shared_from_this() to get a shared_ptr to this object
    // This method should be called after creating GameLogic via std::make_shared
    m_uiManager->setGameLogic(shared_from_this());
}

void GameLogic::showStatus() const noexcept {
    // Apply time effects first
    auto message = m_timeManager->applyTimeEffects();
    if (message) {
        std::cout << *message << std::endl;
    }
    
    // Display newly unlocked achievements
    m_achievementManager->displayNewlyUnlockedAchievements();
    
    // Display the pet header
    m_displayManager->clearScreen();
    m_displayManager->displayPetHeader();
    
    // Display additional information about the pet
    m_interactionManager->showStatus();
}

void GameLogic::feedPet() noexcept {
    // Apply time effects first
    auto message = m_timeManager->applyTimeEffects();
    if (message) {
        std::cout << *message << std::endl;
    }
    
    // Feed the pet
    m_interactionManager->feedPet();
    
    // Save the pet state
    m_petState.save();
}

void GameLogic::playWithPet() noexcept {
    // Apply time effects first
    auto message = m_timeManager->applyTimeEffects();
    if (message) {
        std::cout << *message << std::endl;
    }
    
    // Play with the pet
    m_interactionManager->playWithPet();
    
    // Save the pet state
    m_petState.save();
}

void GameLogic::showEvolutionProgress() const noexcept {
    // Display evolution progress
    m_interactionManager->showEvolutionProgress();
}

void GameLogic::showAchievements() const noexcept {
    // Display all achievements
    m_achievementManager->showAllAchievements();
}

bool GameLogic::createNewPet(bool force) noexcept {
    // Check if a pet already exists
    if (m_petState.saveFileExists() && !force) {
        std::cout << "A pet already exists. Use -f to force creation of a new pet." << std::endl;
        return false;
    }
    
    // Create a new pet - using the interface from InteractionManager
    m_interactionManager->createNewPet(force);
    
    // Save the pet state
    m_petState.save();
    
    return true;
}

void GameLogic::runInteractiveMode() noexcept {
    // Run UI manager's interactive mode
    m_uiManager->runInteractiveMode();
}

void GameLogic::trackCommand(const std::string& command) noexcept {
    // Pass the command to the achievement system for tracking
    m_petState.getAchievementSystem().trackUniqueCommand(command);
}
