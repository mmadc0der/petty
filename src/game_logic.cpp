#include "../include/game_logic.h"
#include "../include/ui_manager.h"
#include <iostream>
#include <memory>

GameLogic::GameLogic(PetState& petState)
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

GameLogic::~GameLogic() {
    // No need to manually delete m_uiManager in the destructor as we're using unique_ptr
}

void GameLogic::initializeUIManager() {
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

void GameLogic::showStatus() const {
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

void GameLogic::feedPet() {
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

void GameLogic::playWithPet() {
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

void GameLogic::showEvolutionProgress() const {
    // Display evolution progress
    m_interactionManager->showEvolutionProgress();
}

void GameLogic::showAchievements() const {
    // Display all achievements
    m_achievementManager->showAllAchievements();
}

bool GameLogic::createNewPet(bool force) {
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

void GameLogic::runInteractiveMode() {
    // Run UI manager's interactive mode
    m_uiManager->runInteractiveMode();
}
