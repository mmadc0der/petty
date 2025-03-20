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
    explicit DisplayManager(PetState& petState);

    /**
     * @brief Display a message about the pet's state change
     * @param message The message to display
     */
    void displayMessage(std::string_view message) const;

    /**
     * @brief Display the pet header (portrait, name, stats)
     */
    void displayPetHeader() const;

    /**
     * @brief Clear the console screen
     */
    void clearScreen() const;

    /**
     * @brief Get the name of the evolution level
     * @param level The evolution level
     * @return The name of the evolution level
     */
    std::string getEvolutionLevelName(PetState::EvolutionLevel level) const;

private:
    // Reference to the pet state
    PetState& m_petState;
};
