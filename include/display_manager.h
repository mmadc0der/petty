#pragma once

#include "pet_state.h"
#include <string_view>

/**
 * @brief Manages display of pet information
 */
class DisplayManager {
public:
    /**
     * @brief Constructor
     * @param petState Reference to the pet state
     */
    explicit DisplayManager(PetState& petState) noexcept;

    /**
     * @brief Display a message about the pet's state change
     * @param message The message to display
     */
    void displayMessage(std::string_view message) const noexcept;

    /**
     * @brief Display the pet header (portrait, name, stats)
     */
    void displayPetHeader() const noexcept;

    /**
     * @brief Clear the console screen
     */
    void clearScreen() const noexcept;

    /**
     * @brief Get the name of the evolution level
     * @param level The evolution level
     * @return The name of the evolution level
     */
    std::string_view getEvolutionLevelName(PetState::EvolutionLevel level) const noexcept;

private:
    // Reference to the pet state
    PetState& m_petState;
};
