#pragma once

#include <array>
#include <string_view>
#include <cstdint>
#include <vector>
#include <bitset>
#include <optional>

/**
 * @brief Enumeration of all possible achievements in the game
 */
enum class AchievementType : uint8_t {
    FirstSteps,     // Feed your pet for the first time
    WellFed,        // Reach 100% hunger
    HappyDays,      // Reach 100% happiness
    FullyRested,    // Reach 100% energy
    Evolution,      // Evolve your pet to the next stage
    Master,         // Reach the highest evolution level
    Playful,        // Play with your pet 5 times
    Dedicated,      // Interact with your pet for 7 consecutive days
    Explorer,       // Try all available commands
    Survivor,       // Keep your pet alive for 30 days
    
    // Add more achievements here
    
    Count           // Special value to get the total number of achievements
};

/**
 * @brief Class to manage the achievement system
 * 
 * Uses a bitset to efficiently store unlocked achievements
 * and provides methods to check, unlock and get information about achievements
 */
class AchievementSystem {
public:
    /**
     * @brief Constructor
     */
    AchievementSystem();
    
    /**
     * @brief Get the number of achievements
     * @return Total number of achievements
     */
    static constexpr size_t getAchievementCount() {
        return static_cast<size_t>(AchievementType::Count);
    }
    
    /**
     * @brief Check if an achievement is unlocked
     * @param type The achievement type to check
     * @return true if the achievement is unlocked, false otherwise
     */
    bool isUnlocked(AchievementType type) const;
    
    /**
     * @brief Unlock an achievement
     * @param type The achievement type to unlock
     * @return true if the achievement was newly unlocked, false if it was already unlocked
     */
    bool unlock(AchievementType type);
    
    /**
     * @brief Get the name of an achievement
     * @param type The achievement type
     * @return The name of the achievement
     */
    static std::string_view getName(AchievementType type);
    
    /**
     * @brief Get the description of an achievement
     * @param type The achievement type
     * @return The description of the achievement
     */
    static std::string_view getDescription(AchievementType type);
    
    /**
     * @brief Get a vector of all unlocked achievements
     * @return Vector of unlocked achievement types
     */
    std::vector<AchievementType> getUnlockedAchievements() const;
    
    /**
     * @brief Get a vector of newly unlocked achievements since the last call
     * @return Vector of newly unlocked achievement types, or empty if none
     */
    std::vector<AchievementType> getNewlyUnlockedAchievements();
    
    /**
     * @brief Reset newly unlocked achievements tracking
     */
    void clearNewlyUnlocked();
    
    /**
     * @brief Get the binary representation of unlocked achievements
     * @return Bitset representing unlocked achievements
     */
    const std::bitset<64>& getUnlockedBitset() const { return m_unlockedAchievements; }
    
    /**
     * @brief Set the unlocked achievements from a binary representation
     * @param bitset Bitset representing unlocked achievements
     */
    void setUnlockedBitset(const std::bitset<64>& bitset);
    
    /**
     * @brief Get the binary representation of unlocked achievements as uint64_t
     * @return uint64_t representing unlocked achievements
     */
    uint64_t getUnlockedBits() const { return m_unlockedAchievements.to_ullong(); }
    
    /**
     * @brief Set the unlocked achievements from a binary representation
     * @param bits uint64_t representing unlocked achievements
     */
    void setUnlockedBits(uint64_t bits) { m_unlockedAchievements = std::bitset<64>(bits); }
    
private:
    // Bitset to store unlocked achievements (64 bits allows for future expansion)
    std::bitset<64> m_unlockedAchievements;
    
    // Bitset to track newly unlocked achievements since last check
    std::bitset<64> m_newlyUnlockedAchievements;
    
    // Static array of achievement names
    static constexpr std::array<std::string_view, static_cast<size_t>(AchievementType::Count)> ACHIEVEMENT_NAMES = {
        "First Steps",
        "Well Fed",
        "Happy Days",
        "Fully Rested",
        "Evolution",
        "Master",
        "Playful",
        "Dedicated",
        "Explorer",
        "Survivor"
    };
    
    // Static array of achievement descriptions
    static constexpr std::array<std::string_view, static_cast<size_t>(AchievementType::Count)> ACHIEVEMENT_DESCRIPTIONS = {
        "Feed your pet for the first time",
        "Reach 100% hunger",
        "Reach 100% happiness",
        "Reach 100% energy",
        "Evolve your pet to the next stage",
        "Reach the highest evolution level",
        "Play with your pet 5 times",
        "Interact with your pet for 7 consecutive days",
        "Try all available commands",
        "Keep your pet alive for 30 days"
    };
};
