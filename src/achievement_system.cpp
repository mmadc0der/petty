#include "../include/achievement_system.h"
#include <iostream>
#include <algorithm>
#include <set>
#include <string>

AchievementSystem::AchievementSystem() noexcept
    : m_unlockedAchievements(0), 
      m_newlyUnlockedAchievements(0),
      m_progress{},
      m_usedCommands{}
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

void AchievementSystem::trackUniqueCommand(const std::string& command) noexcept {
    // Only basic commands from the help menu should be considered for the Explorer achievement
    static const std::set<std::string> validCommands = {
        "status", "feed", "play", "evolve", "achievements", "help", "clear"
    };
    
    // Skip the command if it's not one of the main commands
    if (validCommands.find(command) == validCommands.end()) {
        return;
    }
    
    // If achievement is already unlocked, do nothing
    if (isUnlocked(AchievementType::Explorer)) {
        return;
    }
    
    // Add command to the list of used commands
    m_usedCommands.insert(command);
    
    // If the user has used all commands, unlock the achievement
    if (m_usedCommands.size() >= validCommands.size()) {
        unlock(AchievementType::Explorer);
    } else {
        // Update progress for the Explorer achievement
        setProgress(AchievementType::Explorer, static_cast<uint32_t>(m_usedCommands.size()));
    }
}

bool AchievementSystem::save(std::ofstream& file) const noexcept {
    if (!file) {
        return false;
    }
    
    // Write achievements bitset
    uint64_t achievementBits = m_unlockedAchievements.to_ullong();
    file.write(reinterpret_cast<const char*>(&achievementBits), sizeof(achievementBits));
    
    // Write newly unlocked achievements bitset
    uint64_t newAchievementBits = m_newlyUnlockedAchievements.to_ullong();
    file.write(reinterpret_cast<const char*>(&newAchievementBits), sizeof(newAchievementBits));
    
    // Write achievement progress for each achievement
    for (size_t i = 0; i < static_cast<size_t>(AchievementType::Count); ++i) {
        uint32_t progress = m_progress[i];
        file.write(reinterpret_cast<const char*>(&progress), sizeof(progress));
    }
    
    // Write used commands for Explorer achievement
    uint32_t usedCommandsSize = static_cast<uint32_t>(m_usedCommands.size());
    file.write(reinterpret_cast<const char*>(&usedCommandsSize), sizeof(usedCommandsSize));
    for (const auto& command : m_usedCommands) {
        uint32_t commandLength = static_cast<uint32_t>(command.length());
        file.write(reinterpret_cast<const char*>(&commandLength), sizeof(commandLength));
        file.write(command.c_str(), commandLength);
    }
    
    return file.good();
}

bool AchievementSystem::load(std::ifstream& file, uint8_t version) noexcept {
    if (!file) {
        return false;
    }
    
    // Read achievements bitset
    uint64_t achievementBits = 0;
    file.read(reinterpret_cast<char*>(&achievementBits), sizeof(achievementBits));
    m_unlockedAchievements = std::bitset<64>(achievementBits);
    
    // Read newly unlocked achievements bitset (only for version 4+)
    if (version >= 4) {
        uint64_t newAchievementBits = 0;
        file.read(reinterpret_cast<char*>(&newAchievementBits), sizeof(newAchievementBits));
        m_newlyUnlockedAchievements = std::bitset<64>(newAchievementBits);
    } else {
        // For older versions, clear the newly unlocked achievements
        m_newlyUnlockedAchievements.reset();
    }
    
    // Read progress for each achievement
    for (size_t i = 0; i < static_cast<size_t>(AchievementType::Count); ++i) {
        uint32_t progress = 0;
        file.read(reinterpret_cast<char*>(&progress), sizeof(progress));
        m_progress[i] = progress;
    }
    
    // Read used commands count
    uint32_t commandCount = 0;
    file.read(reinterpret_cast<char*>(&commandCount), sizeof(commandCount));
    
    // Проверяем на разумные пределы - не более 100 команд
    const uint32_t maxReasonableCommands = 100;
    if (commandCount > maxReasonableCommands) {
        commandCount = maxReasonableCommands;
    }
    
    // Read each command
    for (uint32_t i = 0; i < commandCount; ++i) {
        uint32_t length = 0;
        file.read(reinterpret_cast<char*>(&length), sizeof(length));
        
        // Проверяем на разумные пределы - команда не может быть длиннее 50 символов
        const uint32_t maxReasonableLength = 50;
        if (length > maxReasonableLength || length == 0) {
            continue;
        }
        
        std::string command(length, ' ');
        file.read(&command[0], length);
        
        m_usedCommands.insert(command);
    }
    
    return true; // Изменено с !file.fail() на true, так как мы теперь обрабатываем ошибки сами
}
