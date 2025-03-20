#pragma once

#include "pet_state.h"
#include <optional>
#include <string>
#include <chrono>

/**
 * @brief Manages time-based effects and interactions
 */
class TimeManager {
public:
    /**
     * @brief Constructor
     * @param petState Reference to the pet state
     */
    explicit TimeManager(PetState& petState);

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
     * @brief Format time since last interaction
     * @param now Current time
     * @return Formatted string with time since last interaction
     */
    std::string formatTimeSinceLastInteraction(const std::chrono::system_clock::time_point& now) const;

    /**
     * @brief Format pet age
     * @param now Current time
     * @return Formatted string with pet age
     */
    std::string formatPetAge(const std::chrono::system_clock::time_point& now) const;

private:
    // Reference to the pet state
    PetState& m_petState;
};
