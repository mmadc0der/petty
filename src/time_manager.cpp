#include "../include/time_manager.h"
#include "../include/game_config.h"
#include <iostream>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <format>
#include <algorithm>

TimeManager::TimeManager(PetState& petState) noexcept
    : m_petState(petState)
{
}

std::optional<std::string> TimeManager::applyTimeEffects() noexcept {
    auto lastTime = m_petState.getLastInteractionTime();
    if (lastTime == std::chrono::system_clock::time_point{}) {
        // First interaction, no effects to apply
        return std::nullopt;
    }
    
    auto now = std::chrono::system_clock::now();
    auto duration = now - lastTime;
    
    // Calculate hours passed
    double hoursPassed = std::chrono::duration<double, std::ratio<3600, 1>>(duration).count();
    
    // For command-line mode, we need a higher threshold to avoid changes on frequent status checks
    if (hoursPassed < GameConfig::Time::MIN_TIME_THRESHOLD) {  
        // Less than threshold time, no significant effects
        return std::nullopt;
    }
    
    // Apply effects based on time passed
    // For each hour, decrease hunger, happiness, and increase energy
    float hungerDecrease = static_cast<float>(std::min(GameConfig::getHungerDecreaseRate() * hoursPassed, 100.0));
    float happinessDecrease = static_cast<float>(std::min(GameConfig::getHappinessDecreaseRate() * hoursPassed, 100.0));
    
    // Always increase energy proportional to time passed
    float energyIncrease = static_cast<float>(std::min(GameConfig::getEnergyIncreaseRate() * hoursPassed, 100.0));
    m_petState.increaseEnergy(energyIncrease); // Pet rests while away
    
    m_petState.decreaseHunger(hungerDecrease);
    m_petState.decreaseHappiness(happinessDecrease);
    
    // Update last interaction time ONLY if we actually applied effects
    m_petState.updateInteractionTime();
    
    // Generate message if significant time has passed
    if (hoursPassed > GameConfig::Time::SIGNIFICANT_TIME_THRESHOLD) {
        std::string message;
        
        if (hoursPassed < 24.0) {
            message = std::format("{:.1f} hours have passed since your last visit.", hoursPassed);
        } else {
            double daysPassed = hoursPassed / 24.0;
            message = std::format("{:.1f} days have passed since your last visit.", daysPassed);
        }
        
        if (m_petState.getHunger() < GameConfig::Warnings::HUNGER_WARNING_THRESHOLD) {
            message += "\nYour pet is very hungry!";
        }
        
        if (m_petState.getHappiness() < GameConfig::Warnings::HAPPINESS_WARNING_THRESHOLD) {
            message += "\nYour pet is sad and needs attention!";
        }
        
        return message;
    }
    
    return std::nullopt;
}

std::string TimeManager::formatTimeSinceLastInteraction(const std::chrono::system_clock::time_point& now) const noexcept {
    auto lastTime = m_petState.getLastInteractionTime();
    auto timeT = std::chrono::system_clock::to_time_t(lastTime);
    
    // Use localtime_s instead of localtime for safety
    std::tm tmBuf;
    std::tm* tm = &tmBuf;
#ifdef _WIN32
    localtime_s(tm, &timeT);
#else
    localtime_r(&timeT, tm);
#endif
    
    // Format last interaction time as "DD Mon YYYY HH:MM"
    char timeStr[20];
    std::strftime(timeStr, sizeof(timeStr), "%d %b %Y %H:%M", tm);
    
    // Calculate time since last interaction
    auto timeSinceLastInteraction = std::chrono::duration_cast<std::chrono::seconds>(now - lastTime).count();
    
    // Format time since last interaction in humanized format
    std::string timeString;
    int lastDays = timeSinceLastInteraction / (24 * 60 * 60);
    int hours = (timeSinceLastInteraction % (24 * 60 * 60)) / (60 * 60);
    int minutes = (timeSinceLastInteraction % (60 * 60)) / 60;
    
    if (lastDays > 0) {
        timeString += std::format("{}d ", lastDays);
    }
    if (hours > 0 || lastDays > 0) {
        timeString += std::format("{}h ", hours);
    }
    timeString += std::format("{}m", minutes);
    
    return std::format("{} ({})", timeStr, timeString);
}

std::string TimeManager::formatPetAge(const std::chrono::system_clock::time_point& now) const noexcept {
    // Format birth date
    auto birthDate = m_petState.getBirthDate();
    auto birthTimeT = std::chrono::system_clock::to_time_t(birthDate);
    
    std::tm birthTmBuf;
    std::tm* birthTm = &birthTmBuf;
#ifdef _WIN32
    localtime_s(birthTm, &birthTimeT);
#else
    localtime_r(&birthTimeT, birthTm);
#endif
    
    // Format birth date as "DD Mon YYYY"
    char birthStr[20];
    std::strftime(birthStr, sizeof(birthStr), "%d %b %Y", birthTm);
    
    // Calculate age
    auto ageSeconds = std::chrono::duration_cast<std::chrono::seconds>(now - birthDate).count();
    
    // Format age in humanized format
    std::string ageString;
    int years = ageSeconds / (365 * 24 * 60 * 60);
    int days = (ageSeconds % (365 * 24 * 60 * 60)) / (24 * 60 * 60);
    
    if (years > 0) {
        ageString += std::format("{}y ", years);
    }
    if (days > 0 || years > 0) {
        ageString += std::format("{}d", days);
    } else {
        int hours = ageSeconds / (60 * 60);
        ageString += std::format("{}h", hours);
    }
    
    return std::format("{} ({})", birthStr, ageString);
}
