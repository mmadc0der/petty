#pragma once

#include <cstdint>
#include <string>
#include <chrono>
#include <string_view>
#include <optional>
#include <filesystem>
#include "achievement_system.h"
#include "game_config.h" // Include GameConfig

/**
 * @brief Evolution levels for the pet
 */
enum class EvolutionLevel : uint8_t {
    Egg = 0,
    Baby = 1,
    Child = 2,
    Teen = 3,
    Adult = 4,
    Master = 5,
    Ancient = 6
};

/**
 * @brief Class that holds all state information for the pet
 */
class PetState {
public:
    /**
     * @brief Default constructor
     */
    PetState() noexcept;
    
    /**
     * @brief Initialize the pet with default values
     */
    void initialize() noexcept;
    
    /**
     * @brief Initialize the pet with a given name
     * @param name The name for the pet
     */
    void initialize(std::string_view name) noexcept;
    
    /**
     * @brief Load the pet state from file
     * @return True if loaded successfully, false otherwise
     */
    bool load() noexcept;
    
    /**
     * @brief Save the pet state to file
     * @return True if saved successfully, false otherwise
     */
    bool save() const noexcept;
    
    /**
     * @brief Check if a save file exists
     * @return True if a save file exists, false otherwise
     */
    bool saveFileExists() const noexcept;
    
    /**
     * @brief Get the pet's name
     * @return The pet's name
     */
    std::string_view getName() const noexcept {
        return m_name;
    }
    
    /**
     * @brief Set the pet's name
     * @param name The new name for the pet
     */
    void setName(std::string_view name) noexcept {
        m_name = name;
    }
    
    /**
     * @brief Get the pet's evolution level
     * @return The current evolution level
     */
    EvolutionLevel getEvolutionLevel() const noexcept {
        return m_evolutionLevel;
    }
    
    /**
     * @brief Get the pet's experience points
     * @return The current XP amount
     */
    uint32_t getXP() const noexcept {
        return m_xp;
    }
    
    /**
     * @brief Add experience points to the pet
     * @param amount The amount of XP to add
     * @return True if this caused an evolution, false otherwise
     */
    bool addXP(uint32_t amount) noexcept;
    
    /**
     * @brief Get required XP for next evolution level
     * @return The XP amount needed for the next evolution, or 0 if at max level
     */
    uint32_t getXPForNextLevel() const noexcept;
    
    /**
     * @brief Get the maximum stat value for the current evolution level
     * @return The maximum value for stats at the current level
     */
    float getMaxStatValue() const noexcept;
    
    /**
     * @brief Get the pet's current hunger value
     * @return The current hunger value (raw)
     */
    float getHunger() const noexcept {
        return m_hunger;
    }
    
    /**
     * @brief Get the pet's hunger as a percentage of maximum
     * @return The hunger percentage (0.0-100.0)
     */
    float getHungerPercent() const noexcept;
    
    /**
     * @brief Get the pet's current happiness value
     * @return The current happiness value (raw)
     */
    float getHappiness() const noexcept {
        return m_happiness;
    }
    
    /**
     * @brief Get the pet's happiness as a percentage of maximum
     * @return The happiness percentage (0.0-100.0)
     */
    float getHappinessPercent() const noexcept;
    
    /**
     * @brief Get the pet's current energy value
     * @return The current energy value (raw)
     */
    float getEnergy() const noexcept {
        return m_energy;
    }
    
    /**
     * @brief Get the pet's energy as a percentage of maximum
     * @return The energy percentage (0.0-100.0)
     */
    float getEnergyPercent() const noexcept;
    
    /**
     * @brief Increase the pet's hunger
     * @param amount Amount to increase (may be capped at maximum)
     */
    void increaseHunger(float amount) noexcept;
    
    /**
     * @brief Decrease the pet's hunger
     * @param amount Amount to decrease (will not go below zero)
     */
    void decreaseHunger(float amount) noexcept;
    
    /**
     * @brief Increase the pet's happiness
     * @param amount Amount to increase (may be capped at maximum)
     */
    void increaseHappiness(float amount) noexcept;
    
    /**
     * @brief Decrease the pet's happiness
     * @param amount Amount to decrease (will not go below zero)
     */
    void decreaseHappiness(float amount) noexcept;
    
    /**
     * @brief Increase the pet's energy
     * @param amount Amount to increase (may be capped at maximum)
     */
    void increaseEnergy(float amount) noexcept;
    
    /**
     * @brief Decrease the pet's energy
     * @param amount Amount to decrease (will not go below zero)
     */
    void decreaseEnergy(float amount) noexcept;
    
    /**
     * @brief Get the time of last interaction
     * @return Time point of the last interaction
     */
    std::chrono::system_clock::time_point getLastInteractionTime() const noexcept {
        return m_lastInteractionTime;
    }
    
    /**
     * @brief Update the last interaction time to now
     */
    void updateInteractionTime() noexcept;
    
    /**
     * @brief Get the pet's birth date
     * @return Birth date time point
     */
    std::chrono::system_clock::time_point getBirthDate() const noexcept {
        return m_birthDate;
    }
    
    /**
     * @brief Get the ASCII art representation of the pet
     * @return ASCII art string
     */
    std::string_view getAsciiArt() const noexcept;
    
    /**
     * @brief Get a description of the current evolution level
     * @return Level description
     */
    std::string_view getDescription() const noexcept;
    
    /**
     * @brief Get a description of the pet's current status
     * @return Status description based on current stats
     */
    std::string_view getStatusDescription() const noexcept;
    
    /**
     * @brief Get achievement system reference
     * @return Reference to the pet's achievement system
     */
    AchievementSystem& getAchievementSystem() noexcept {
        return m_achievementSystem;
    }
    
    /**
     * @brief Get const achievement system reference
     * @return Const reference to the pet's achievement system
     */
    const AchievementSystem& getAchievementSystem() const noexcept {
        return m_achievementSystem;
    }
    
private:
    /**
     * @brief Get the file path for save data
     * @return Path to the save file
     */
    std::filesystem::path getStateFilePath() const noexcept;
    
    std::string m_name;
    EvolutionLevel m_evolutionLevel;
    uint32_t m_xp;
    float m_hunger;
    float m_happiness;
    float m_energy;
    std::chrono::system_clock::time_point m_lastInteractionTime;
    std::chrono::system_clock::time_point m_birthDate;
    AchievementSystem m_achievementSystem;
};
