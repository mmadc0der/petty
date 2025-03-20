#pragma once

#include "pet_state.h"
#include "display_manager.h"
#include "achievement_manager.h"

/**
 * @brief Manages interactions with the pet
 */
class InteractionManager {
public:
    /**
     * @brief Constructor
     * @param petState Reference to the pet state
     * @param displayManager Reference to the display manager
     * @param achievementManager Reference to the achievement manager
     */
    InteractionManager(PetState& petState, DisplayManager& displayManager, AchievementManager& achievementManager);

    /**
     * @brief Feed the pet
     */
    void feedPet();

    /**
     * @brief Play with the pet
     */
    void playWithPet();

    /**
     * @brief Show the current status of the pet
     */
    void showStatus() const;

    /**
     * @brief Show evolution progress
     */
    void showEvolutionProgress() const;

    /**
     * @brief Create a new pet, optionally overwriting an existing one
     * @param force If true, will create a new pet even if one already exists
     * @return true if a new pet was created, false otherwise
     */
    bool createNewPet(bool force = false);

private:
    // Reference to the pet state
    PetState& m_petState;
    
    // Reference to the display manager
    DisplayManager& m_displayManager;
    
    // Reference to the achievement manager
    AchievementManager& m_achievementManager;
};
