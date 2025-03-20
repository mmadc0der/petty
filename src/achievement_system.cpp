#include "../include/achievement_system.h"
#include <iostream>
#include <algorithm>

AchievementSystem::AchievementSystem() noexcept
    : m_unlockedAchievements(0), 
      m_newlyUnlockedAchievements(0),
      m_progress{}
{
    // Initialize progress array to zeros
    std::fill(m_progress.begin(), m_progress.end(), 0);
}

bool AchievementSystem::isUnlocked(AchievementType type) const noexcept {
    if (type == AchievementType::Count) {
        return false;
    }
    
    size_t index = static_cast<size_t>(type);
    return m_unlockedAchievements.test(index);
}

bool AchievementSystem::unlock(AchievementType type) noexcept {
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

std::string_view AchievementSystem::getName(AchievementType type) noexcept {
    if (type == AchievementType::Count || static_cast<size_t>(type) >= ACHIEVEMENT_NAMES.size()) {
        return "Unknown Achievement";
    }
    
    return ACHIEVEMENT_NAMES[static_cast<size_t>(type)];
}

std::string_view AchievementSystem::getDescription(AchievementType type) noexcept {
    if (type == AchievementType::Count || static_cast<size_t>(type) >= ACHIEVEMENT_DESCRIPTIONS.size()) {
        return "Unknown Achievement Description";
    }
    
    return ACHIEVEMENT_DESCRIPTIONS[static_cast<size_t>(type)];
}

std::vector<AchievementType> AchievementSystem::getUnlockedAchievements() const noexcept {
    std::vector<AchievementType> unlocked;
    
    for (size_t i = 0; i < static_cast<size_t>(AchievementType::Count); ++i) {
        if (m_unlockedAchievements.test(i)) {
            unlocked.push_back(static_cast<AchievementType>(i));
        }
    }
    
    return unlocked;
}

std::vector<AchievementType> AchievementSystem::getNewlyUnlockedAchievements() noexcept {
    std::vector<AchievementType> newlyUnlocked;
    
    for (size_t i = 0; i < static_cast<size_t>(AchievementType::Count); ++i) {
        if (m_newlyUnlockedAchievements.test(i)) {
            newlyUnlocked.push_back(static_cast<AchievementType>(i));
        }
    }
    
    return newlyUnlocked;
}

void AchievementSystem::clearNewlyUnlocked() noexcept {
    m_newlyUnlockedAchievements.reset();
}

void AchievementSystem::setUnlockedBitset(const std::bitset<64>& bitset) noexcept {
    m_unlockedAchievements = bitset;
    m_newlyUnlockedAchievements.reset(); // Clear newly unlocked tracking when setting from saved state
}

void AchievementSystem::incrementProgress(AchievementType type, uint32_t amount) noexcept {
    if (type == AchievementType::Count) {
        return;
    }
    
    // Don't increment if already unlocked
    if (isUnlocked(type)) {
        return;
    }
    
    size_t index = static_cast<size_t>(type);
    m_progress[index] += amount;
    
    // Check if we've reached the required progress
    if (m_progress[index] >= ACHIEVEMENT_REQUIRED_PROGRESS[index]) {
        unlock(type);
    }
}

void AchievementSystem::setProgress(AchievementType type, uint32_t progress) noexcept {
    if (type == AchievementType::Count) {
        return;
    }
    
    // Don't set progress if already unlocked
    if (isUnlocked(type)) {
        return;
    }
    
    size_t index = static_cast<size_t>(type);
    m_progress[index] = progress;
    
    // Check if we've reached the required progress
    if (m_progress[index] >= ACHIEVEMENT_REQUIRED_PROGRESS[index]) {
        unlock(type);
    }
}

uint32_t AchievementSystem::getProgress(AchievementType type) const noexcept {
    if (type == AchievementType::Count) {
        return 0;
    }
    
    // If already unlocked, return the required progress
    if (isUnlocked(type)) {
        return ACHIEVEMENT_REQUIRED_PROGRESS[static_cast<size_t>(type)];
    }
    
    return m_progress[static_cast<size_t>(type)];
}

uint32_t AchievementSystem::getRequiredProgress(AchievementType type) noexcept {
    if (type == AchievementType::Count) {
        return 0;
    }
    
    return ACHIEVEMENT_REQUIRED_PROGRESS[static_cast<size_t>(type)];
}

bool AchievementSystem::save(std::ofstream& file) const noexcept {
    if (!file) {
        return false;
    }
    
    // Write achievements bitset
    uint64_t achievementBits = m_unlockedAchievements.to_ullong();
    file.write(reinterpret_cast<const char*>(&achievementBits), sizeof(achievementBits));
    
    // Write achievement progress for each achievement
    for (size_t i = 0; i < static_cast<size_t>(AchievementType::Count); ++i) {
        uint32_t progress = m_progress[i];
        file.write(reinterpret_cast<const char*>(&progress), sizeof(progress));
    }
    
    return file.good();
}

bool AchievementSystem::load(std::ifstream& file) noexcept {
    if (!file) {
        return false;
    }
    
    // Read achievements bitset
    uint64_t achievementBits = 0;
    file.read(reinterpret_cast<char*>(&achievementBits), sizeof(achievementBits));
    m_unlockedAchievements = achievementBits;
    m_newlyUnlockedAchievements.reset(); // Clear newly unlocked tracking when setting from saved state
    
    // Read achievement progress for each achievement
    for (size_t i = 0; i < static_cast<size_t>(AchievementType::Count); ++i) {
        uint32_t progress = 0;
        file.read(reinterpret_cast<char*>(&progress), sizeof(progress));
        
        // Only set progress if achievement is not already unlocked
        AchievementType type = static_cast<AchievementType>(i);
        if (!isUnlocked(type) && progress > 0) {
            setProgress(type, progress);
        }
    }
    
    return file.good();
}
