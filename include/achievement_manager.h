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
    explicit AchievementManager(PetState& petState) noexcept;

    /**
     * @brief Display unlocked achievements
     * @param newlyUnlocked Whether to only show newly unlocked achievements
     * @return True if any achievements were displayed
     */
    bool displayAchievements(bool newlyUnlocked = false) const noexcept;

    /**
     * @brief Display newly unlocked achievements
     * @return True if any achievements were displayed
     */
    bool displayNewlyUnlockedAchievements() noexcept;

    /**
     * @brief Show all achievements, including locked ones with progress
     */
    void showAllAchievements() const noexcept;

private:
    // Reference to the pet state
    PetState& m_petState;
};
