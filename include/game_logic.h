#pragma once

#include "pet_state.h"
#include "display_manager.h"
#include "achievement_manager.h"
#include "interaction_manager.h"
#include "time_manager.h"
#include <memory>
#include <string_view>
#include <optional>

// Forward declarations
class UIManager;

/**
 * @brief Main game logic class that coordinates all managers
 * 
 * Uses modern C++ features like std::optional and std::string_view
 */
class GameLogic : public std::enable_shared_from_this<GameLogic> {
public:
    /**
     * @brief Constructor
     * @param petState Reference to the pet state
     */
    explicit GameLogic(PetState& petState);

    /**
     * @brief Destructor
     */
    ~GameLogic();

    /**
     * @brief Initializes UIManager with cyclic reference setup
     * 
     * Should be called after creating the object via std::make_shared
     */
    void initializeUIManager();

    /**
     * @brief Show pet status
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
     * @brief Show all achievements and progress
     */
    void showAchievements() const;

    /**
     * @brief Create a new pet, optionally overwriting the existing one
     * @param force If true, create a new pet even if one already exists
     * @return True if a new pet was created, false otherwise
     */
    bool createNewPet(bool force = false);

    /**
     * @brief Run interactive mode (command loop)
     */
    void runInteractiveMode();

    /**
     * @brief Clear the console screen
     */
    void clearScreen() const;

    /**
     * @brief Display the pet header (portrait, name, stats)
     */
    void displayPetHeader() const;

    /**
     * @brief Display newly unlocked achievements
     */
    void displayNewlyUnlockedAchievements();

private:
    // Reference to the pet state
    PetState& m_petState;

    // Unique pointers to managers
    std::unique_ptr<DisplayManager> m_displayManager;
    std::unique_ptr<AchievementManager> m_achievementManager;
    std::unique_ptr<InteractionManager> m_interactionManager;
    std::unique_ptr<TimeManager> m_timeManager;

    // UI Manager - using std::unique_ptr for UIManager
    std::unique_ptr<UIManager> m_uiManager;
};
