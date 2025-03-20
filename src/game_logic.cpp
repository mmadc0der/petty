#include "../include/game_logic.h"
#include <iostream>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <format>
#include <optional>

GameLogic::GameLogic(PetState& petState)
    : m_petState(petState)
    , m_displayManager(std::make_unique<DisplayManager>(petState))
    , m_achievementManager(std::make_unique<AchievementManager>(petState))
    , m_timeManager(std::make_unique<TimeManager>(petState))
{
    // Create managers in the correct order
    m_interactionManager = std::make_unique<InteractionManager>(
        petState, 
        *m_displayManager, 
        *m_achievementManager
    );
    
    m_uiManager = std::make_unique<UIManager>(
        petState,
        *m_displayManager,
        *m_achievementManager,
        *m_interactionManager,
        *m_timeManager
    );
    
    // Apply time-based effects when creating the game logic
    auto timeEffectMessage = m_timeManager->applyTimeEffects();
    if (timeEffectMessage) {
        m_displayManager->displayMessage(*timeEffectMessage);
    }
}

void GameLogic::showStatus() const {
    m_interactionManager->showStatus();
}

void GameLogic::feedPet() {
    m_interactionManager->feedPet();
}

void GameLogic::playWithPet() {
    m_interactionManager->playWithPet();
}

void GameLogic::showEvolutionProgress() const {
    m_interactionManager->showEvolutionProgress();
}

void GameLogic::showAchievements() const {
    m_achievementManager->showAllAchievements();
}

bool GameLogic::createNewPet(bool force) {
    return m_interactionManager->createNewPet(force);
}

void GameLogic::runInteractiveMode() {
    m_uiManager->runInteractiveMode();
}

void GameLogic::clearScreen() const {
    m_displayManager->clearScreen();
}

void GameLogic::displayPetHeader() const {
    m_displayManager->displayPetHeader();
}

void GameLogic::displayNewlyUnlockedAchievements() {
    m_achievementManager->displayNewlyUnlockedAchievements();
}
