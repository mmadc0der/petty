#include "../include/game_logic.h"
#include "../include/command_parser.h"
#include <iostream>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <format>
#include <optional>

GameLogic::GameLogic(PetState& petState)
    : m_petState(petState)
{
    // Apply time-based effects when creating the game logic
    auto timeEffectMessage = applyTimeEffects();
    if (timeEffectMessage) {
        displayMessage(*timeEffectMessage);
    }
}

void GameLogic::showStatus() const {
    std::cout << "\n" << m_petState.getAsciiArt() << std::endl;
    
    std::cout << "Name: " << m_petState.getName() << std::endl;
    std::cout << "Evolution: ";
    
    switch (m_petState.getEvolutionLevel()) {
        case PetState::EvolutionLevel::Egg:
            std::cout << "Egg (Level 0)";
            break;
        case PetState::EvolutionLevel::Baby:
            std::cout << "Baby (Level 1)";
            break;
        case PetState::EvolutionLevel::Child:
            std::cout << "Child (Level 2)";
            break;
        case PetState::EvolutionLevel::Teen:
            std::cout << "Teen (Level 3)";
            break;
        case PetState::EvolutionLevel::Adult:
            std::cout << "Adult (Level 4)";
            break;
        case PetState::EvolutionLevel::Master:
            std::cout << "Master (Level 5)";
            break;
        case PetState::EvolutionLevel::Ancient:
            std::cout << "Ancient";
            break;
    }
    std::cout << std::endl;
    
    std::cout << "Description: " << m_petState.getDescription() << std::endl;
    
    std::cout << "\nStats:" << std::endl;
    std::cout << "  XP: " << m_petState.getXP();
    
    if (m_petState.getEvolutionLevel() != PetState::EvolutionLevel::Ancient) {
        std::cout << " / " << m_petState.getXPForNextLevel() << " for next level";
    }
    std::cout << std::endl;
    
    std::cout << "  Hunger: " << static_cast<int>(std::floor(m_petState.getHunger())) << "%" << std::endl;
    std::cout << "  Happiness: " << static_cast<int>(std::floor(m_petState.getHappiness())) << "%" << std::endl;
    std::cout << "  Energy: " << static_cast<int>(std::floor(m_petState.getEnergy())) << "%" << std::endl;
    
    // Calculate current time
    auto now = std::chrono::system_clock::now();
    
    // Display last interaction time
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
    
    std::cout << "\nLast interaction: " << ss.str() << " (" << timeString << ")" << std::endl;
    
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
    
    std::cout << "Birth date: " << birthSs.str() << " (" << ageString << ")" << std::endl;
    
    // Display achievements
    displayAchievements();
}

void GameLogic::feedPet() {
    // Check if pet was already full
    bool wasFull = (m_petState.getHunger() == 100);
    
    // Increase hunger and add XP
    m_petState.increaseHunger(20);
    bool evolved = m_petState.addXP(10);
    
    // Update interaction time
    m_petState.updateInteractionTime();
    
    // Unlock first steps achievement if first time feeding
    if (m_petState.getAchievementSystem().unlock(AchievementType::FirstSteps)) {
        std::cout << "\nAchievement unlocked: " 
                  << AchievementSystem::getName(AchievementType::FirstSteps) 
                  << "!" << std::endl;
    }
    
    // Display message
    if (evolved) {
        std::cout << "Your pet " << m_petState.getName() << " has evolved to " 
                  << getEvolutionLevelName(m_petState.getEvolutionLevel()) 
                  << "!" << std::endl;
        std::cout << m_petState.getAsciiArt() << std::endl;
        std::cout << m_petState.getDescription() << std::endl;
    } else if (wasFull && m_petState.getHunger() == 100) {
        displayMessage("Your pet is already full! It doesn't want to eat more.");
    } else if (m_petState.getHunger() == 100) {
        displayMessage("Your pet is now full and very satisfied!");
    } else {
        displayMessage("Your pet enjoys the food and feels less hungry.");
    }
    
    // Check for newly unlocked achievements
    displayNewlyUnlockedAchievements();
    
    // Show current hunger level
    std::cout << "Hunger: " << static_cast<int>(std::floor(m_petState.getHunger())) << "%" << std::endl;
    std::cout << "XP: " << m_petState.getXP();
    if (m_petState.getEvolutionLevel() != PetState::EvolutionLevel::Ancient) {
        std::cout << " / " << m_petState.getXPForNextLevel() << " for next level";
    }
    std::cout << std::endl;
}

void GameLogic::playWithPet() {
    // Increase happiness and decrease energy
    m_petState.increaseHappiness(15);
    m_petState.decreaseEnergy(10);
    
    // Add XP
    bool evolved = m_petState.addXP(1500);
    
    // Update interaction time
    m_petState.updateInteractionTime();
    
    // Display message
    if (evolved) {
        std::cout << "Your pet " << m_petState.getName() << " has evolved to " 
                  << getEvolutionLevelName(m_petState.getEvolutionLevel()) 
                  << "!" << std::endl;
        std::cout << m_petState.getAsciiArt() << std::endl;
        std::cout << m_petState.getDescription() << std::endl;
    } else if (m_petState.getHappiness() == 100) {
        displayMessage("Your pet is extremely happy! It's having the time of its life!");
    } else {
        displayMessage("Your pet jumps around playfully. It's having fun!");
    }
    
    // Track play count for Playful achievement
    m_petState.getAchievementSystem().incrementProgress(AchievementType::Playful);
    
    // Check for newly unlocked achievements
    displayNewlyUnlockedAchievements();
    
    // Show current stats
    std::cout << "Happiness: " << static_cast<int>(std::floor(m_petState.getHappiness())) << "%" << std::endl;
    std::cout << "Energy: " << static_cast<int>(std::floor(m_petState.getEnergy())) << "%" << std::endl;
    std::cout << "XP: " << m_petState.getXP();
    if (m_petState.getEvolutionLevel() != PetState::EvolutionLevel::Ancient) {
        std::cout << " / " << m_petState.getXPForNextLevel() << " for next level";
    }
    std::cout << std::endl;
}

void GameLogic::showEvolutionProgress() const {
    std::cout << "\n" << m_petState.getAsciiArt() << std::endl;
    
    std::cout << "Current evolution: ";
    switch (m_petState.getEvolutionLevel()) {
        case PetState::EvolutionLevel::Egg:
            std::cout << "Egg (Level 0)";
            break;
        case PetState::EvolutionLevel::Baby:
            std::cout << "Baby (Level 1)";
            break;
        case PetState::EvolutionLevel::Child:
            std::cout << "Child (Level 2)";
            break;
        case PetState::EvolutionLevel::Teen:
            std::cout << "Teen (Level 3)";
            break;
        case PetState::EvolutionLevel::Adult:
            std::cout << "Adult (Level 4)";
            break;
        case PetState::EvolutionLevel::Master:
            std::cout << "Master (Level 5)";
            break;
        case PetState::EvolutionLevel::Ancient:
            std::cout << "Ancient";
            break;
    }
    std::cout << std::endl;
    
    std::cout << "Description: " << m_petState.getDescription() << std::endl;
    
    if (m_petState.getEvolutionLevel() != PetState::EvolutionLevel::Ancient) {
        uint32_t currentXP = m_petState.getXP();
        uint32_t requiredXP = m_petState.getXPForNextLevel();
        float percentage = static_cast<float>(currentXP) / requiredXP * 100.0f;
        
        std::cout << "\nProgress to next evolution:" << std::endl;
        std::cout << "XP: " << currentXP << " / " << requiredXP 
                  << " (" << static_cast<int>(percentage) << "%)" << std::endl;
        
        // Display a simple progress bar
        std::cout << "[";
        int barWidth = 20;
        int pos = static_cast<int>(barWidth * percentage / 100.0f);
        
        for (int i = 0; i < barWidth; ++i) {
            if (i < pos) std::cout << "=";
            else std::cout << " ";
        }
        
        std::cout << "] " << static_cast<int>(percentage) << "%" << std::endl;
        
        std::cout << "\nNext evolution: ";
        switch (static_cast<PetState::EvolutionLevel>(
                static_cast<uint8_t>(m_petState.getEvolutionLevel()) + 1)) {
            case PetState::EvolutionLevel::Baby:
                std::cout << "Baby (Level 1)";
                break;
            case PetState::EvolutionLevel::Child:
                std::cout << "Child (Level 2)";
                break;
            case PetState::EvolutionLevel::Teen:
                std::cout << "Teen (Level 3)";
                break;
            case PetState::EvolutionLevel::Adult:
                std::cout << "Adult (Level 4)";
                break;
            case PetState::EvolutionLevel::Master:
                std::cout << "Master (Level 5)";
                break;
            case PetState::EvolutionLevel::Ancient:
                std::cout << "Ancient (Level 6)";
                break;
            default:
                std::cout << "Unknown";
        }
        std::cout << std::endl;
    } else {
        std::cout << "\nYour pet has reached the maximum evolution level!" << std::endl;
        std::cout << "Congratulations on raising an ancient pet!" << std::endl;
    }
}

std::optional<std::string> GameLogic::applyTimeEffects() {
    auto lastTime = m_petState.getLastInteractionTime();
    if (lastTime == std::chrono::system_clock::time_point{}) {
        // First interaction, no effects to apply
        return std::nullopt;
    }
    
    auto now = std::chrono::system_clock::now();
    auto duration = now - lastTime;
    
    // Calculate hours passed
    double hoursPassed = std::chrono::duration<double, std::ratio<3600, 1>>(duration).count();
    
    if (hoursPassed < 0.01) {  
        // Less than 36 seconds, no significant effects
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

void GameLogic::displayMessage(std::string_view message) const {
    std::cout << message << std::endl;
}

void GameLogic::displayAchievements(bool newlyUnlocked) const {
    const auto& achievementSystem = m_petState.getAchievementSystem();
    auto unlockedAchievements = achievementSystem.getUnlockedAchievements();
    
    if (unlockedAchievements.empty()) {
        if (!newlyUnlocked) {
            std::cout << "\nNo achievements unlocked yet." << std::endl;
        }
        return;
    }
    
    std::cout << "\nAchievements:" << std::endl;
    for (const auto& achievement : unlockedAchievements) {
        std::cout << "  - " << AchievementSystem::getName(achievement) 
                  << ": " << AchievementSystem::getDescription(achievement) << std::endl;
    }
}

void GameLogic::displayNewlyUnlockedAchievements() {
    auto& achievementSystem = m_petState.getAchievementSystem();
    auto newlyUnlocked = achievementSystem.getNewlyUnlockedAchievements();
    
    for (const auto& achievement : newlyUnlocked) {
        if (achievement != AchievementType::FirstSteps) { // First Steps is handled separately
            std::cout << "\nAchievement unlocked: " 
                      << AchievementSystem::getName(achievement) 
                      << "!" << std::endl;
        }
    }
    
    achievementSystem.clearNewlyUnlocked();
}

void GameLogic::showAchievements() const {
    const auto& achievementSystem = m_petState.getAchievementSystem();
    auto unlockedAchievements = achievementSystem.getUnlockedAchievements();
    
    std::cout << "\n===== ACHIEVEMENTS =====\n" << std::endl;
    
    // First show locked achievements with progress
    std::cout << "LOCKED ACHIEVEMENTS:" << std::endl;
    
    bool hasLockedAchievements = false;
    
    // Check for playful achievement progress
    if (std::find(unlockedAchievements.begin(), unlockedAchievements.end(), AchievementType::Playful) 
        == unlockedAchievements.end()) {
        hasLockedAchievements = true;
        std::cout << "  - " << AchievementSystem::getName(AchievementType::Playful) 
                  << ": " << AchievementSystem::getDescription(AchievementType::Playful) 
                  << " (" << achievementSystem.getProgress(AchievementType::Playful) 
                  << "/" << AchievementSystem::getRequiredProgress(AchievementType::Playful) << ")" << std::endl;
    }
    
    // Check for evolution achievement progress
    if (std::find(unlockedAchievements.begin(), unlockedAchievements.end(), AchievementType::Evolution) 
        == unlockedAchievements.end()) {
        hasLockedAchievements = true;
        auto currentLevel = static_cast<int>(m_petState.getEvolutionLevel());
        std::cout << "  - " << AchievementSystem::getName(AchievementType::Evolution) 
                  << ": " << AchievementSystem::getDescription(AchievementType::Evolution) 
                  << " (Level " << currentLevel << "/6)" << std::endl;
    }
    
    // Check for master achievement progress
    if (std::find(unlockedAchievements.begin(), unlockedAchievements.end(), AchievementType::Master) 
        == unlockedAchievements.end()) {
        hasLockedAchievements = true;
        auto currentLevel = static_cast<int>(m_petState.getEvolutionLevel());
        std::cout << "  - " << AchievementSystem::getName(AchievementType::Master) 
                  << ": " << AchievementSystem::getDescription(AchievementType::Master) 
                  << " (Level " << currentLevel << "/5)" << std::endl;
    }
    
    // Check for eternal achievement progress
    if (std::find(unlockedAchievements.begin(), unlockedAchievements.end(), AchievementType::Eternal) 
        == unlockedAchievements.end()) {
        hasLockedAchievements = true;
        auto currentLevel = static_cast<int>(m_petState.getEvolutionLevel());
        std::cout << "  - " << AchievementSystem::getName(AchievementType::Eternal) 
                  << ": " << AchievementSystem::getDescription(AchievementType::Eternal) 
                  << " (Level " << currentLevel << "/6)" << std::endl;
    }
    
    // Check for hunger-based achievement progress
    if (std::find(unlockedAchievements.begin(), unlockedAchievements.end(), AchievementType::WellFed) 
        == unlockedAchievements.end()) {
        hasLockedAchievements = true;
        auto currentHunger = static_cast<int>(std::floor(m_petState.getHunger()));
        std::cout << "  - " << AchievementSystem::getName(AchievementType::WellFed) 
                  << ": " << AchievementSystem::getDescription(AchievementType::WellFed) 
                  << " (" << currentHunger << "/100)" << std::endl;
    }
    
    // Check for happiness-based achievement progress
    if (std::find(unlockedAchievements.begin(), unlockedAchievements.end(), AchievementType::HappyDays) 
        == unlockedAchievements.end()) {
        hasLockedAchievements = true;
        auto currentHappiness = static_cast<int>(std::floor(m_petState.getHappiness()));
        std::cout << "  - " << AchievementSystem::getName(AchievementType::HappyDays) 
                  << ": " << AchievementSystem::getDescription(AchievementType::HappyDays) 
                  << " (" << currentHappiness << "/100)" << std::endl;
    }
    
    // Check for energy-based achievement progress
    if (std::find(unlockedAchievements.begin(), unlockedAchievements.end(), AchievementType::FullyRested) 
        == unlockedAchievements.end()) {
        hasLockedAchievements = true;
        auto currentEnergy = static_cast<int>(std::floor(m_petState.getEnergy()));
        std::cout << "  - " << AchievementSystem::getName(AchievementType::FullyRested) 
                  << ": " << AchievementSystem::getDescription(AchievementType::FullyRested) 
                  << " (" << currentEnergy << "/100)" << std::endl;
    }
    
    if (!hasLockedAchievements) {
        std::cout << "  None - You've unlocked all achievements!" << std::endl;
    }
    
    // Then show unlocked achievements
    std::cout << "\nUNLOCKED ACHIEVEMENTS:" << std::endl;
    
    if (unlockedAchievements.empty()) {
        std::cout << "  None yet. Keep playing!" << std::endl;
    } else {
        for (const auto& achievement : unlockedAchievements) {
            std::cout << "  - " << AchievementSystem::getName(achievement) 
                      << ": " << AchievementSystem::getDescription(achievement) << std::endl;
        }
    }
}

bool GameLogic::createNewPet(bool force) {
    // Check if a save file exists and we're not forcing overwrite
    if (m_petState.saveFileExists() && !force) {
        std::cout << "A pet already exists. Do you want to overwrite it? (yes/no): ";
        std::string response;
        std::getline(std::cin, response);
        
        // Convert to lowercase for case-insensitive comparison
        std::transform(response.begin(), response.end(), response.begin(), 
                      [](unsigned char c) { return std::tolower(c); });
        
        if (response != "yes" && response != "y") {
            std::cout << "Pet creation cancelled." << std::endl;
            return false;
        }
    }
    
    // Ask for pet name
    std::cout << "Enter a name for your new pet: ";
    std::string name;
    std::getline(std::cin, name);
    
    // Trim whitespace from beginning and end
    name.erase(0, name.find_first_not_of(" \t\n\r\f\v"));
    name.erase(name.find_last_not_of(" \t\n\r\f\v") + 1);
    
    // Use default name if empty
    if (name.empty()) {
        name = "Unnamed Pet";
    }
    
    // Initialize new pet with the given name
    m_petState.initialize(name);
    std::cout << "\nCreated a new pet named '" << name << "'!" << std::endl;
    
    // Show the new pet's status
    showStatus();
    
    return true;
}

std::string GameLogic::getEvolutionLevelName(PetState::EvolutionLevel level) const {
    switch (level) {
        case PetState::EvolutionLevel::Egg:
            return "Egg";
        case PetState::EvolutionLevel::Baby:
            return "Baby";
        case PetState::EvolutionLevel::Child:
            return "Child";
        case PetState::EvolutionLevel::Teen:
            return "Teen";
        case PetState::EvolutionLevel::Adult:
            return "Adult";
        case PetState::EvolutionLevel::Master:
            return "Master";
        case PetState::EvolutionLevel::Ancient:
            return "Ancient";
        default:
            return "Unknown";
    }
}

void GameLogic::clearScreen() const {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void GameLogic::displayPetHeader() const {
    std::cout << m_petState.getAsciiArt() << std::endl;
    
    std::cout << "Name: " << m_petState.getName() << std::endl;
    std::cout << "Evolution: ";
    
    switch (m_petState.getEvolutionLevel()) {
        case PetState::EvolutionLevel::Egg:
            std::cout << "Egg (Level 0)";
            break;
        case PetState::EvolutionLevel::Baby:
            std::cout << "Baby (Level 1)";
            break;
        case PetState::EvolutionLevel::Child:
            std::cout << "Child (Level 2)";
            break;
        case PetState::EvolutionLevel::Teen:
            std::cout << "Teen (Level 3)";
            break;
        case PetState::EvolutionLevel::Adult:
            std::cout << "Adult (Level 4)";
            break;
        case PetState::EvolutionLevel::Master:
            std::cout << "Master (Level 5)";
            break;
        case PetState::EvolutionLevel::Ancient:
            std::cout << "Ancient";
            break;
    }
    std::cout << std::endl;
    
    std::cout << "\nStats:" << std::endl;
    std::cout << "  Hunger: " << static_cast<int>(std::floor(m_petState.getHunger())) << "%" << std::endl;
    std::cout << "  Happiness: " << static_cast<int>(std::floor(m_petState.getHappiness())) << "%" << std::endl;
    std::cout << "  Energy: " << static_cast<int>(std::floor(m_petState.getEnergy())) << "%" << std::endl;
    std::cout << "  XP: " << m_petState.getXP();
    
    if (m_petState.getEvolutionLevel() != PetState::EvolutionLevel::Ancient) {
        std::cout << " / " << m_petState.getXPForNextLevel() << " for next level";
    }
    std::cout << std::endl << std::endl;
}

void GameLogic::runInteractiveMode() {
    // Apply time effects first
    auto message = applyTimeEffects();
    if (message) {
        std::cout << *message << std::endl;
    }
    
    // Display newly unlocked achievements
    displayNewlyUnlockedAchievements();
    
    // Clear screen and show pet header
    clearScreen();
    displayPetHeader();
    
    // Interactive loop
    std::string command;
    bool running = true;
    
    while (running) {
        std::cout << " > ";
        std::getline(std::cin, command);
        
        // Convert to lowercase
        std::string lowerCommand = command;
        std::transform(lowerCommand.begin(), lowerCommand.end(), lowerCommand.begin(),
                      [](unsigned char c) { return std::tolower(c); });
        
        // Special commands for interactive mode
        if (lowerCommand == "exit") {
            running = false;
        } else if (lowerCommand == "clear") {
            clearScreen();
            displayPetHeader();
        } else {
            // Parse the command
            std::vector<std::string> args;
            std::istringstream iss(command);
            std::string arg;
            while (iss >> arg) {
                args.push_back(arg);
            }
            
            if (!args.empty()) {
                // Create a command parser and process the command
                CommandParser parser;
                if (args[0] == "help") {
                    parser.showHelp();
                } else if (!parser.processCommand(args, *this)) {
                    std::cout << "Unknown command. Type 'help' for usage information." << std::endl;
                }
            }
            
            // Save the pet state after each command
            m_petState.save();
        }
    }
}
