#pragma once

#include <cstdint>

/**
 * @brief Game configuration constants
 * 
 * This file contains configuration constants that control the game balance.
 * These can be modified to create different gameplay experiences.
 */
namespace GameConfig {

    /**
     * @brief Preset configurations for different gameplay styles
     */
    enum class Preset {
        Default,    // Standard balanced gameplay
        Easy,       // Easier gameplay with slower stat decay
        Hard,       // Harder gameplay with faster stat decay
        Realistic   // More realistic stat changes
    };

    // Current preset to use
    constexpr Preset CURRENT_PRESET = Preset::Default;

    /**
     * @brief Maximum stat values based on evolution level
     */
    namespace MaxStats {
        // Egg (Level 0)
        constexpr float EGG_MAX_STAT = 60.0f;
        
        // Baby (Level 1)
        constexpr float BABY_MAX_STAT = 60.0f;
        
        // Child (Level 2)
        constexpr float CHILD_MAX_STAT = 60.0f;
        
        // Teen (Level 3)
        constexpr float TEEN_MAX_STAT = 80.0f;
        
        // Adult (Level 4)
        constexpr float ADULT_MAX_STAT = 80.0f;
        
        // Master (Level 5)
        constexpr float MASTER_MAX_STAT = 100.0f;
        
        // Ancient (Level 6)
        constexpr float ANCIENT_MAX_STAT = 120.0f;
    }

    /**
     * @brief Time threshold constants
     */
    namespace Time {
        // Minimum time in hours before applying time effects (0.05 = 3 minutes)
        constexpr double MIN_TIME_THRESHOLD = 0.05;
        
        // Time threshold in hours for showing "significant time passed" message
        constexpr double SIGNIFICANT_TIME_THRESHOLD = 2.0;
    }

    /**
     * @brief Stat change rates per hour based on preset
     */
    namespace StatRates {
        // Default preset values
        namespace Default {
            // Hunger decrease per hour
            constexpr float HUNGER_DECREASE_RATE = 5.0f;
            
            // Happiness decrease per hour
            constexpr float HAPPINESS_DECREASE_RATE = 3.0f;
            
            // Energy increase per hour (when resting)
            constexpr float ENERGY_INCREASE_RATE = 10.0f;
        }
        
        // Easy preset values (slower decay)
        namespace Easy {
            constexpr float HUNGER_DECREASE_RATE = 3.0f;
            constexpr float HAPPINESS_DECREASE_RATE = 2.0f;
            constexpr float ENERGY_INCREASE_RATE = 15.0f;
        }
        
        // Hard preset values (faster decay)
        namespace Hard {
            constexpr float HUNGER_DECREASE_RATE = 8.0f;
            constexpr float HAPPINESS_DECREASE_RATE = 5.0f;
            constexpr float ENERGY_INCREASE_RATE = 7.0f;
        }
        
        // Realistic preset values
        namespace Realistic {
            constexpr float HUNGER_DECREASE_RATE = 6.0f;
            constexpr float HAPPINESS_DECREASE_RATE = 4.0f;
            constexpr float ENERGY_INCREASE_RATE = 8.0f;
        }
    }

    /**
     * @brief Stat warning thresholds
     */
    namespace Warnings {
        // Thresholds for warnings (absolute values)
        constexpr float HUNGER_WARNING_THRESHOLD = 12.0f;
        constexpr float HAPPINESS_WARNING_THRESHOLD = 12.0f;
        
        // Percentage versions kept for compatibility with existing code
        constexpr float HUNGER_WARNING_THRESHOLD_PERCENT = 0.2f;
        constexpr float HAPPINESS_WARNING_THRESHOLD_PERCENT = 0.2f;
    }

    /**
     * @brief Initial pet stats
     */
    namespace InitialStats {
        // Initial values as absolute values
        constexpr float INITIAL_HUNGER = 30.0f;
        constexpr float INITIAL_HAPPINESS = 30.0f;
        constexpr float INITIAL_ENERGY = 30.0f;
        
        // Percentage version kept for compatibility
        constexpr float INITIAL_STAT_PERCENT = 0.5f;
    }

    /**
     * @brief Interaction effects
     */
    namespace Interactions {
        // Feeding effects
        namespace Feeding {
            // Default preset values
            namespace Default {
                constexpr float HUNGER_INCREASE = 12.0f;
                constexpr uint32_t XP_GAIN = 10;
            }
            
            // Easy preset values
            namespace Easy {
                constexpr float HUNGER_INCREASE = 15.0f;
                constexpr uint32_t XP_GAIN = 15;
            }
            
            // Hard preset values
            namespace Hard {
                constexpr float HUNGER_INCREASE = 9.0f;
                constexpr uint32_t XP_GAIN = 8;
            }
            
            // Realistic preset values
            namespace Realistic {
                constexpr float HUNGER_INCREASE = 10.8f;
                constexpr uint32_t XP_GAIN = 10;
            }
        }
        
        // Playing effects
        namespace Playing {
            // Default preset values
            namespace Default {
                constexpr float HAPPINESS_INCREASE = 12.0f;
                constexpr float ENERGY_DECREASE = 8.0f;
                constexpr uint32_t XP_GAIN = 15;
            }
            
            // Easy preset values
            namespace Easy {
                constexpr float HAPPINESS_INCREASE = 16.0f;
                constexpr float ENERGY_DECREASE = 6.4f;
                constexpr uint32_t XP_GAIN = 20;
            }
            
            // Hard preset values
            namespace Hard {
                constexpr float HAPPINESS_INCREASE = 9.6f;
                constexpr float ENERGY_DECREASE = 12.0f;
                constexpr uint32_t XP_GAIN = 12;
            }
            
            // Realistic preset values
            namespace Realistic {
                constexpr float HAPPINESS_INCREASE = 12.0f;
                constexpr float ENERGY_DECREASE = 9.6f;
                constexpr uint32_t XP_GAIN = 14;
            }
        }
    }

    /**
     * @brief Get the maximum stat value based on evolution level
     * @param evolutionLevel The current evolution level of the pet
     * @return Maximum stat value for the given evolution level
     */
    constexpr float getMaxStatForEvolutionLevel(uint8_t evolutionLevel) {
        switch (evolutionLevel) {
            case 0: // Egg
                return MaxStats::EGG_MAX_STAT;
            case 1: // Baby
                return MaxStats::BABY_MAX_STAT;
            case 2: // Child
                return MaxStats::CHILD_MAX_STAT;
            case 3: // Teen
                return MaxStats::TEEN_MAX_STAT;
            case 4: // Adult
                return MaxStats::ADULT_MAX_STAT;
            case 5: // Master
                return MaxStats::MASTER_MAX_STAT;
            case 6: // Ancient
                return MaxStats::ANCIENT_MAX_STAT;
            default:
                return MaxStats::EGG_MAX_STAT;
        }
    }

    /**
     * @brief Get the current hunger decrease rate based on the selected preset
     * @return Hunger decrease rate per hour
     */
    constexpr float getHungerDecreaseRate() {
        switch (CURRENT_PRESET) {
            case Preset::Easy:
                return StatRates::Easy::HUNGER_DECREASE_RATE;
            case Preset::Hard:
                return StatRates::Hard::HUNGER_DECREASE_RATE;
            case Preset::Realistic:
                return StatRates::Realistic::HUNGER_DECREASE_RATE;
            case Preset::Default:
            default:
                return StatRates::Default::HUNGER_DECREASE_RATE;
        }
    }

    /**
     * @brief Get the current happiness decrease rate based on the selected preset
     * @return Happiness decrease rate per hour
     */
    constexpr float getHappinessDecreaseRate() {
        switch (CURRENT_PRESET) {
            case Preset::Easy:
                return StatRates::Easy::HAPPINESS_DECREASE_RATE;
            case Preset::Hard:
                return StatRates::Hard::HAPPINESS_DECREASE_RATE;
            case Preset::Realistic:
                return StatRates::Realistic::HAPPINESS_DECREASE_RATE;
            case Preset::Default:
            default:
                return StatRates::Default::HAPPINESS_DECREASE_RATE;
        }
    }

    /**
     * @brief Get the current energy increase rate based on the selected preset
     * @return Energy increase rate per hour
     */
    constexpr float getEnergyIncreaseRate() {
        switch (CURRENT_PRESET) {
            case Preset::Easy:
                return StatRates::Easy::ENERGY_INCREASE_RATE;
            case Preset::Hard:
                return StatRates::Hard::ENERGY_INCREASE_RATE;
            case Preset::Realistic:
                return StatRates::Realistic::ENERGY_INCREASE_RATE;
            case Preset::Default:
            default:
                return StatRates::Default::ENERGY_INCREASE_RATE;
        }
    }
    
    /**
     * @brief Get the current feeding hunger increase based on the selected preset
     * @return Hunger increase amount
     */
    constexpr float getFeedingHungerIncrease() {
        switch (CURRENT_PRESET) {
            case Preset::Easy:
                return Interactions::Feeding::Easy::HUNGER_INCREASE;
            case Preset::Hard:
                return Interactions::Feeding::Hard::HUNGER_INCREASE;
            case Preset::Realistic:
                return Interactions::Feeding::Realistic::HUNGER_INCREASE;
            case Preset::Default:
            default:
                return Interactions::Feeding::Default::HUNGER_INCREASE;
        }
    }
    
    /**
     * @brief Get the current feeding XP gain based on the selected preset
     * @return XP gain amount
     */
    constexpr uint32_t getFeedingXPGain() {
        switch (CURRENT_PRESET) {
            case Preset::Easy:
                return Interactions::Feeding::Easy::XP_GAIN;
            case Preset::Hard:
                return Interactions::Feeding::Hard::XP_GAIN;
            case Preset::Realistic:
                return Interactions::Feeding::Realistic::XP_GAIN;
            case Preset::Default:
            default:
                return Interactions::Feeding::Default::XP_GAIN;
        }
    }
    
    /**
     * @brief Get the current playing happiness increase based on the selected preset
     * @return Happiness increase amount
     */
    constexpr float getPlayingHappinessIncrease() {
        switch (CURRENT_PRESET) {
            case Preset::Easy:
                return Interactions::Playing::Easy::HAPPINESS_INCREASE;
            case Preset::Hard:
                return Interactions::Playing::Hard::HAPPINESS_INCREASE;
            case Preset::Realistic:
                return Interactions::Playing::Realistic::HAPPINESS_INCREASE;
            case Preset::Default:
            default:
                return Interactions::Playing::Default::HAPPINESS_INCREASE;
        }
    }

    /**
     * @brief Get the current playing energy decrease based on the selected preset
     * @return Energy decrease amount
     */
    constexpr float getPlayingEnergyDecrease() {
        switch (CURRENT_PRESET) {
            case Preset::Easy:
                return Interactions::Playing::Easy::ENERGY_DECREASE;
            case Preset::Hard:
                return Interactions::Playing::Hard::ENERGY_DECREASE;
            case Preset::Realistic:
                return Interactions::Playing::Realistic::ENERGY_DECREASE;
            case Preset::Default:
            default:
                return Interactions::Playing::Default::ENERGY_DECREASE;
        }
    }
    
    /**
     * @brief Get the current playing XP gain based on the selected preset
     * @return XP gain amount
     */
    constexpr uint32_t getPlayingXPGain() {
        switch (CURRENT_PRESET) {
            case Preset::Easy:
                return Interactions::Playing::Easy::XP_GAIN;
            case Preset::Hard:
                return Interactions::Playing::Hard::XP_GAIN;
            case Preset::Realistic:
                return Interactions::Playing::Realistic::XP_GAIN;
            case Preset::Default:
            default:
                return Interactions::Playing::Default::XP_GAIN;
        }
    }
}
