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
    InteractionManager(PetState& petState, DisplayManager& displayManager, AchievementManager& achievementManager) noexcept;

    /**
     * @brief Feed the pet
     */
    void feedPet() noexcept;

    /**
     * @brief Play with the pet
     */
    void playWithPet() noexcept;

    /**
     * @brief Show the current status of the pet
     */
    void showStatus() const noexcept;

    /**
     * @brief Show evolution progress
     */
    void showEvolutionProgress() const noexcept;

    /**
     * @brief Create a new pet, optionally overwriting an existing one
     * @param force If true, will create a new pet even if one already exists
     * @return true if a new pet was created, false otherwise
     */
    bool createNewPet(bool force = false) noexcept;

private:
    // Reference to the pet state
    PetState& m_petState;
    
    // Reference to the display manager
    DisplayManager& m_displayManager;
    
    // Reference to the achievement manager
    AchievementManager& m_achievementManager;
};
