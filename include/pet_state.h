#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include <filesystem>
#include <optional>
#include <memory>
#include <chrono>

/**
 * @brief Represents an achievement that the pet can earn
 */
struct Achievement {
    std::string name;
    std::string description;
    bool unlocked;
};

/**
 * @brief Represents the current state of the pet
 * 
 * Uses modern C++ features like std::optional, std::chrono, and std::filesystem
 */
class PetState {
public:
    // Evolution levels
    enum class EvolutionLevel : uint8_t {
        Egg = 0,
        Baby = 1,
        Child = 2,
        Teen = 3,
        Adult = 4,
        Master = 5
    };

    // Using std::chrono for time representation
    using TimePoint = std::chrono::system_clock::time_point;

    PetState();
    ~PetState() = default;

    // Prevent copying
    PetState(const PetState&) = delete;
    PetState& operator=(const PetState&) = delete;

    // Allow moving
    PetState(PetState&&) noexcept = default;
    PetState& operator=(PetState&&) noexcept = default;

    /**
     * @brief Initialize a new pet with default values
     */
    void initialize();

    /**
     * @brief Load pet state from file
     * @return true if loading was successful, false otherwise
     */
    bool load();

    /**
     * @brief Save pet state to file
     * @return true if saving was successful, false otherwise
     */
    bool save() const;

    /**
     * @brief Get the current evolution level
     * @return The current evolution level
     */
    EvolutionLevel getEvolutionLevel() const { return m_evolutionLevel; }

    /**
     * @brief Get the current experience points
     * @return The current XP
     */
    uint32_t getXP() const { return m_xp; }

    /**
     * @brief Get the hunger level (0-100)
     * @return The current hunger level
     */
    uint8_t getHunger() const { return m_hunger; }

    /**
     * @brief Get the happiness level (0-100)
     * @return The current happiness level
     */
    uint8_t getHappiness() const { return m_happiness; }

    /**
     * @brief Get the energy level (0-100)
     * @return The current energy level
     */
    uint8_t getEnergy() const { return m_energy; }

    /**
     * @brief Get the list of achievements
     * @return Vector of achievements
     */
    std::vector<Achievement>& getAchievements() { return m_achievements; }
    
    /**
     * @brief Get the list of achievements (const version)
     * @return Vector of achievements
     */
    const std::vector<Achievement>& getAchievements() const { return m_achievements; }

    /**
     * @brief Add experience points and check for evolution
     * @param amount Amount of XP to add
     * @return true if the pet evolved, false otherwise
     */
    bool addXP(uint32_t amount);

    /**
     * @brief Increase hunger level
     * @param amount Amount to increase (capped at 100)
     */
    void increaseHunger(uint8_t amount);

    /**
     * @brief Increase happiness level
     * @param amount Amount to increase (capped at 100)
     */
    void increaseHappiness(uint8_t amount);

    /**
     * @brief Increase energy level
     * @param amount Amount to increase (capped at 100)
     */
    void increaseEnergy(uint8_t amount);

    /**
     * @brief Decrease hunger level
     * @param amount Amount to decrease (minimum 0)
     */
    void decreaseHunger(uint8_t amount);

    /**
     * @brief Decrease happiness level
     * @param amount Amount to decrease (minimum 0)
     */
    void decreaseHappiness(uint8_t amount);

    /**
     * @brief Decrease energy level
     * @param amount Amount to decrease (minimum 0)
     */
    void decreaseEnergy(uint8_t amount);

    /**
     * @brief Get the pet's name
     * @return The pet's name
     */
    const std::string& getName() const { return m_name; }

    /**
     * @brief Set the pet's name
     * @param name New name for the pet
     */
    void setName(const std::string& name) { m_name = name; }

    /**
     * @brief Get the last interaction time
     * @return Last interaction time
     */
    TimePoint getLastInteractionTime() const { return m_lastInteractionTime; }

    /**
     * @brief Update the last interaction time to now
     */
    void updateInteractionTime();

    /**
     * @brief Get the XP required for the next evolution level
     * @return XP required for evolution
     */
    uint32_t getXPForNextLevel() const;

    /**
     * @brief Get ASCII art for the current evolution level
     * @return ASCII art string
     */
    std::string_view getAsciiArt() const;

    /**
     * @brief Get description for the current evolution level
     * @return Description string
     */
    std::string_view getDescription() const;

private:
    /**
     * @brief Get the path to the state file
     * @return Path to the state file
     */
    std::filesystem::path getStateFilePath() const;

    /**
     * @brief Check and unlock achievements based on current state
     */
    void checkAchievements();

    // Pet state data
    std::string m_name;
    EvolutionLevel m_evolutionLevel;
    uint32_t m_xp;
    uint8_t m_hunger;     // 0-100, 0 = starving, 100 = full
    uint8_t m_happiness;  // 0-100, 0 = sad, 100 = very happy
    uint8_t m_energy;     // 0-100, 0 = exhausted, 100 = energetic
    TimePoint m_lastInteractionTime;
    std::vector<Achievement> m_achievements;
};
