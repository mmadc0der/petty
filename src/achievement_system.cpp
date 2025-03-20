#include "../include/achievement_system.h"
#include <iostream>
#include <algorithm>

AchievementSystem::AchievementSystem()
    : m_unlockedAchievements(0), m_newlyUnlockedAchievements(0)
{
}

bool AchievementSystem::isUnlocked(AchievementType type) const {
    if (type == AchievementType::Count) {
        return false;
    }
    
    size_t index = static_cast<size_t>(type);
    return m_unlockedAchievements.test(index);
}

bool AchievementSystem::unlock(AchievementType type) {
    if (type == AchievementType::Count) {
        return false;
    }
    
    size_t index = static_cast<size_t>(type);
    
    // Check if already unlocked
    if (m_unlockedAchievements.test(index)) {
        return false;
    }
    
    // Unlock the achievement
    m_unlockedAchievements.set(index);
    m_newlyUnlockedAchievements.set(index);
    
    return true;
}

std::string_view AchievementSystem::getName(AchievementType type) {
    if (type == AchievementType::Count || static_cast<size_t>(type) >= ACHIEVEMENT_NAMES.size()) {
        return "Unknown Achievement";
    }
    
    return ACHIEVEMENT_NAMES[static_cast<size_t>(type)];
}

std::string_view AchievementSystem::getDescription(AchievementType type) {
    if (type == AchievementType::Count || static_cast<size_t>(type) >= ACHIEVEMENT_DESCRIPTIONS.size()) {
        return "Unknown Achievement Description";
    }
    
    return ACHIEVEMENT_DESCRIPTIONS[static_cast<size_t>(type)];
}

std::vector<AchievementType> AchievementSystem::getUnlockedAchievements() const {
    std::vector<AchievementType> unlocked;
    
    for (size_t i = 0; i < static_cast<size_t>(AchievementType::Count); ++i) {
        if (m_unlockedAchievements.test(i)) {
            unlocked.push_back(static_cast<AchievementType>(i));
        }
    }
    
    return unlocked;
}

std::vector<AchievementType> AchievementSystem::getNewlyUnlockedAchievements() {
    std::vector<AchievementType> newlyUnlocked;
    
    for (size_t i = 0; i < static_cast<size_t>(AchievementType::Count); ++i) {
        if (m_newlyUnlockedAchievements.test(i)) {
            newlyUnlocked.push_back(static_cast<AchievementType>(i));
        }
    }
    
    return newlyUnlocked;
}

void AchievementSystem::clearNewlyUnlocked() {
    m_newlyUnlockedAchievements.reset();
}

void AchievementSystem::setUnlockedBitset(const std::bitset<64>& bitset) {
    m_unlockedAchievements = bitset;
    m_newlyUnlockedAchievements.reset(); // Clear newly unlocked tracking when setting from saved state
}
