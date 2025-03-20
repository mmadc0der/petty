#pragma once

#include "pet_state.h"
#include <vector>
#include <string>

/**
 * @brief Manages achievements and their display
 */
class AchievementManager {
public:
    /**
     * @brief Constructor
     * @param petState Reference to the pet state
     */
    explicit AchievementManager(PetState& petState);

    /**
     * @brief Display unlocked achievements
     * @param newlyUnlocked Whether to only show newly unlocked achievements
     * @return True if any achievements were displayed
     */
    bool displayAchievements(bool newlyUnlocked = false) const;

    /**
     * @brief Display newly unlocked achievements
     * @return True if any achievements were displayed
     */
    bool displayNewlyUnlockedAchievements();

    /**
     * @brief Show all achievements, including locked ones with progress
     */
    void showAllAchievements() const;

private:
    // Reference to the pet state
    PetState& m_petState;
};
