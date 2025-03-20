#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include <filesystem>
#include <chrono>
#include <string_view>
#include <memory>
#include "achievement_system.h"

/**
 * @brief Represents the state of the virtual pet
 * 
 * Uses modern C++ features like std::chrono for time representation
 * and std::string_view for better performance
 */
class PetState {
public:
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
     * @brief Constructor
     */
    PetState() noexcept;
    
    /**
     * @brief Check if a saved state file exists
     * @return true if a state file exists, false otherwise
     */
    bool saveFileExists() const noexcept;
    
    /**
     * @brief Initialize a new pet with default values
     */
    void initialize() noexcept;
    
    /**
     * @brief Initialize a new pet with a specific name
     * @param name The name for the new pet
     */
    void initialize(std::string_view name) noexcept;
    
    /**
     * @brief Load pet state from file
     * @return true if loaded successfully, false otherwise
     */
    bool load() noexcept;
    
    /**
     * @brief Save pet state to file
     * @return true if saved successfully, false otherwise
     */
    bool save() const noexcept;
    
    /**
     * @brief Get the pet's name
     * @return The pet's name
     */
    const std::string& getName() const noexcept { return m_name; }
    
    /**
     * @brief Set the pet's name
     * @param name The new name for the pet
     */
    void setName(std::string_view name) noexcept { m_name = name; }
    
    /**
     * @brief Get the pet's evolution level
     * @return The current evolution level
     */
    EvolutionLevel getEvolutionLevel() const noexcept { return m_evolutionLevel; }
    
    /**
     * @brief Get the pet's XP
     * @return The current XP
     */
    uint32_t getXP() const noexcept { return m_xp; }
    
    /**
     * @brief Get the XP required for the next evolution level
     * @return The XP required for the next level
     */
    uint32_t getXPForNextLevel() const noexcept;
    
    /**
     * @brief Add XP to the pet
     * @param amount The amount of XP to add
     * @return true if the pet evolved, false otherwise
     */
    bool addXP(uint32_t amount) noexcept;
    
    /**
     * @brief Get the pet's hunger level
     * @return The current hunger level (0-100)
     */
    float getHunger() const noexcept { return m_hunger; }
    
    /**
     * @brief Increase the pet's hunger level
     * @param amount The amount to increase (capped at 100)
     */
    void increaseHunger(float amount) noexcept;
    
    /**
     * @brief Decrease the pet's hunger level
     * @param amount The amount to decrease (capped at 0)
     */
    void decreaseHunger(float amount) noexcept;
    
    /**
     * @brief Get the pet's happiness level
     * @return The current happiness level (0-100)
     */
    float getHappiness() const noexcept { return m_happiness; }
    
    /**
     * @brief Increase the pet's happiness level
     * @param amount The amount to increase (capped at 100)
     */
    void increaseHappiness(float amount) noexcept;
    
    /**
     * @brief Decrease the pet's happiness level
     * @param amount The amount to decrease (capped at 0)
     */
    void decreaseHappiness(float amount) noexcept;
    
    /**
     * @brief Get the pet's energy level
     * @return The current energy level (0-100)
     */
    float getEnergy() const noexcept { return m_energy; }
    
    /**
     * @brief Increase the pet's energy level
     * @param amount The amount to increase (capped at 100)
     */
    void increaseEnergy(float amount) noexcept;
    
    /**
     * @brief Decrease the pet's energy level
     * @param amount The amount to decrease (capped at 0)
     */
    void decreaseEnergy(float amount) noexcept;
    
    /**
     * @brief Get the last interaction time
     * @return The time of the last interaction
     */
    std::chrono::system_clock::time_point getLastInteractionTime() const noexcept { return m_lastInteractionTime; }
    
    /**
     * @brief Update the last interaction time to now
     */
    void updateInteractionTime() noexcept;
    
    /**
     * @brief Get the pet's birth date
     * @return The time of the pet's creation
     */
    std::chrono::system_clock::time_point getBirthDate() const noexcept { return m_birthDate; }
    
    /**
     * @brief Get the pet's ASCII art representation
     * @return ASCII art string for the current evolution level
     */
    std::string_view getAsciiArt() const noexcept;
    
    /**
     * @brief Get the description of the pet based on its evolution level
     * @return Description string
     */
    std::string_view getDescription() const noexcept;
    
    /**
     * @brief Get a more detailed status description of the pet
     * @return Detailed status description string
     */
    std::string_view getStatusDescription() const noexcept;
    
    /**
     * @brief Get the achievement system
     * @return Reference to the achievement system
     */
    AchievementSystem& getAchievementSystem() noexcept { return m_achievementSystem; }
    
    /**
     * @brief Get the achievement system (const version)
     * @return Const reference to the achievement system
     */
    const AchievementSystem& getAchievementSystem() const noexcept { return m_achievementSystem; }
    
    /**
     * @brief Prevent copying
     */
    PetState(const PetState&) = delete;
    PetState& operator=(const PetState&) = delete;

    /**
     * @brief Allow moving
     */
    PetState(PetState&&) noexcept = default;
    PetState& operator=(PetState&&) noexcept = default;

    /**
     * @brief Destructor
     */
    ~PetState() = default;

private:
    /**
     * @brief Get the path to the state file
     * @return Path to the state file
     */
    std::filesystem::path getStateFilePath() const noexcept;
    
    // Basic pet properties
    std::string m_name;
    EvolutionLevel m_evolutionLevel;
    uint32_t m_xp;
    
    // Pet stats (0-100)
    float m_hunger;
    float m_happiness;
    float m_energy;
    
    // Last interaction time
    std::chrono::system_clock::time_point m_lastInteractionTime;
    
    // Birth date (creation time)
    std::chrono::system_clock::time_point m_birthDate;
    
    // Achievement system
    AchievementSystem m_achievementSystem;
};
