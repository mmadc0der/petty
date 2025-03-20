#include "../include/time_manager.h"
#include <iostream>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <algorithm>

TimeManager::TimeManager(PetState& petState)
    : m_petState(petState)
{
}

std::optional<std::string> TimeManager::applyTimeEffects() {
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
    // 0.05 hours = 3 minutes
    if (hoursPassed < 0.05) {  
        // Less than 3 minutes, no significant effects
        return std::nullopt;
    }
    
    // Apply effects based on time passed
    // For each hour, decrease hunger, happiness, and increase energy
    float hungerDecrease = static_cast<float>(std::min(5.0 * hoursPassed, 100.0));
    float happinessDecrease = static_cast<float>(std::min(3.0 * hoursPassed, 100.0));
    
    // Always increase energy proportional to time passed (10 energy per hour)
    float energyIncrease = static_cast<float>(std::min(10.0 * hoursPassed, 100.0));
    m_petState.increaseEnergy(energyIncrease); // Pet rests while away
    
    m_petState.decreaseHunger(hungerDecrease);
    m_petState.decreaseHappiness(happinessDecrease);
    
    // Update last interaction time ONLY if we actually applied effects
    m_petState.updateInteractionTime();
    
    // Generate message if significant time has passed
    if (hoursPassed > 1.0) {
        std::stringstream ss;
        ss << std::fixed << std::setprecision(1);
        
        if (hoursPassed < 24.0) {
            ss << hoursPassed << " hours have passed since your last visit.";
        } else {
            double daysPassed = hoursPassed / 24.0;
            ss << daysPassed << " days have passed since your last visit.";
        }
        
        std::string message = ss.str();
        
        if (m_petState.getHunger() < 20.0f) {
            message += "\nYour pet is very hungry!";
        }
        
        if (m_petState.getHappiness() < 20.0f) {
            message += "\nYour pet is sad and needs attention!";
        }
        
        return message;
    }
    
    return std::nullopt;
}

std::string TimeManager::formatTimeSinceLastInteraction(const std::chrono::system_clock::time_point& now) const {
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
    std::stringstream ss;
    ss << std::put_time(tm, "%d %b %Y %H:%M");
    
    // Calculate time since last interaction
    auto timeSinceLastInteraction = std::chrono::duration_cast<std::chrono::seconds>(now - lastTime).count();
    
    // Format time since last interaction in humanized format
    std::string timeString;
    int lastDays = timeSinceLastInteraction / (24 * 60 * 60);
    int hours = (timeSinceLastInteraction % (24 * 60 * 60)) / (60 * 60);
    int minutes = (timeSinceLastInteraction % (60 * 60)) / 60;
    
    if (lastDays > 0) {
        timeString += std::to_string(lastDays) + "d ";
    }
    if (hours > 0 || lastDays > 0) {
        timeString += std::to_string(hours) + "h ";
    }
    timeString += std::to_string(minutes) + "m";
    
    return ss.str() + " (" + timeString + ")";
}

std::string TimeManager::formatPetAge(const std::chrono::system_clock::time_point& now) const {
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
    std::stringstream birthSs;
    birthSs << std::put_time(birthTm, "%d %b %Y");
    
    // Calculate age
    auto ageSeconds = std::chrono::duration_cast<std::chrono::seconds>(now - birthDate).count();
    
    // Format age in humanized format
    std::string ageString;
    int years = ageSeconds / (365 * 24 * 60 * 60);
    int days = (ageSeconds % (365 * 24 * 60 * 60)) / (24 * 60 * 60);
    
    if (years > 0) {
        ageString += std::to_string(years) + "y ";
    }
    if (days > 0 || years == 0) {
        ageString += std::to_string(days) + "d";
    }
    
    return birthSs.str() + " (" + ageString + ")";
}
