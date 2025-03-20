#pragma once

#include "pet_state.h"
#include <memory>
#include <string_view>
#include <optional>

/**
 * @brief Handles game mechanics and interactions with the pet
 * 
 * Uses modern C++ features like std::optional and std::string_view
 */
class GameLogic {
public:
    /**
     * @brief Constructor
     * @param petState Reference to the pet state
     */
    explicit GameLogic(PetState& petState);

    /**
     * @brief Show the current status of the pet
     */
    void showStatus() const;

    /**
     * @brief Feed the pet
     */
    void feedPet();

    /**
     * @brief Play with the pet
     */
    void playWithPet();

    /**
     * @brief Show evolution progress
     */
    void showEvolutionProgress() const;

    /**
     * @brief Show achievements, including locked ones with progress
     */
    void showAchievements() const;

private:
    /**
     * @brief Apply time-based effects to the pet
     * 
     * This method calculates how much time has passed since the last interaction
     * and applies appropriate effects (hunger decrease, etc.)
     * 
     * @return Optional string with a message about significant time passing
     */
    std::optional<std::string> applyTimeEffects();

    /**
     * @brief Display a message about the pet's state change
     * @param message The message to display
     */
    void displayMessage(std::string_view message) const;

    /**
     * @brief Display unlocked achievements
     * @param newlyUnlocked Whether to only show newly unlocked achievements
     */
    void displayAchievements(bool newlyUnlocked = false) const;

    /**
     * @brief Display and clear newly unlocked achievements
     */
    void displayNewlyUnlockedAchievements();

    // Reference to the pet state
    PetState& m_petState;
};
