#pragma once

#include "pet_state.h"
#include "display_manager.h"
#include "achievement_manager.h"
#include "interaction_manager.h"
#include "time_manager.h"
#include <string>
#include <vector>

/**
 * @brief Manages user interface and command processing
 */
class UIManager {
public:
    /**
     * @brief Constructor
     * @param petState Reference to the pet state
     * @param displayManager Reference to the display manager
     * @param achievementManager Reference to the achievement manager
     * @param interactionManager Reference to the interaction manager
     * @param timeManager Reference to the time manager
     */
    UIManager(
        PetState& petState,
        DisplayManager& displayManager,
        AchievementManager& achievementManager,
        InteractionManager& interactionManager,
        TimeManager& timeManager
    );

    /**
     * @brief Run interactive mode
     * 
     * This method runs an interactive mode where the user can enter commands
     * directly without restarting the application.
     */
    void runInteractiveMode();

    /**
     * @brief Process a command
     * @param args Command arguments
     * @return True if the command was processed successfully
     */
    bool processCommand(const std::vector<std::string>& args);
    
    /**
     * @brief Show help information
     */
    void showHelp() const;

private:
    // Reference to the pet state
    PetState& m_petState;
    
    // Reference to the display manager
    DisplayManager& m_displayManager;
    
    // Reference to the achievement manager
    AchievementManager& m_achievementManager;
    
    // Reference to the interaction manager
    InteractionManager& m_interactionManager;
    
    // Reference to the time manager
    TimeManager& m_timeManager;
};
