#include "../include/ui_manager.h"
#include <iostream>
#include <algorithm>
#include <sstream>
#include <chrono>

UIManager::UIManager(
    PetState& petState,
    DisplayManager& displayManager,
    AchievementManager& achievementManager,
    InteractionManager& interactionManager,
    TimeManager& timeManager)
    : m_petState(petState)
    , m_displayManager(displayManager)
    , m_achievementManager(achievementManager)
    , m_interactionManager(interactionManager)
    , m_timeManager(timeManager)
{
}

void UIManager::runInteractiveMode() {
    // Apply time effects first
    auto message = m_timeManager.applyTimeEffects();
    if (message) {
        std::cout << *message << std::endl;
    }
    
    // Display newly unlocked achievements
    m_achievementManager.displayNewlyUnlockedAchievements();
    
    // Clear screen and show pet header
    m_displayManager.clearScreen();
    m_displayManager.displayPetHeader();
    
    // Interactive loop
    std::string command;
    bool running = true;
    auto lastTimeCheck = std::chrono::system_clock::now();
    
    while (running) {
        std::cout << "> ";
        std::getline(std::cin, command);
        
        // Check if we need to apply time effects (every 5 minutes)
        auto now = std::chrono::system_clock::now();
        auto timeSinceLastCheck = std::chrono::duration<double, std::ratio<60, 1>>(now - lastTimeCheck).count();
        if (timeSinceLastCheck >= 5.0) {  // 5 minutes
            auto timeMessage = m_timeManager.applyTimeEffects();
            if (timeMessage) {
                std::cout << *timeMessage << std::endl;
            }
            lastTimeCheck = now;
        }
        
        // Convert to lowercase
        std::string lowerCommand = command;
        std::transform(lowerCommand.begin(), lowerCommand.end(), lowerCommand.begin(),
                      [](unsigned char c) { return std::tolower(c); });
        
        // Special commands for interactive mode
        if (lowerCommand == "exit") {
            running = false;
        } else if (lowerCommand == "clear") {
            m_displayManager.clearScreen();
            m_displayManager.displayPetHeader();
        } else {
            // Parse the command
            std::vector<std::string> args;
            std::istringstream iss(command);
            std::string arg;
            while (iss >> arg) {
                args.push_back(arg);
            }
            
            if (!args.empty()) {
                if (!processCommand(args)) {
                    std::cout << "Unknown command. Type 'help' for usage information." << std::endl;
                }
            }
            
            // Save the pet state after each command
            m_petState.save();
        }
    }
}

bool UIManager::processCommand(const std::vector<std::string>& args) {
    if (args.empty()) {
        return false;
    }
    
    std::string command = args[0];
    std::transform(command.begin(), command.end(), command.begin(), 
                   [](unsigned char c) { return std::tolower(c); });
    
    if (command == "status") {
        m_interactionManager.showStatus();
        return true;
    } else if (command == "feed") {
        m_interactionManager.feedPet();
        return true;
    } else if (command == "play") {
        m_interactionManager.playWithPet();
        return true;
    } else if (command == "evolve") {
        m_interactionManager.showEvolutionProgress();
        return true;
    } else if (command == "achievements") {
        m_achievementManager.showAllAchievements();
        return true;
    } else if (command == "new") {
        m_interactionManager.createNewPet();
        return true;
    } else if (command == "help") {
        showHelp();
        return true;
    }
    
    return false;
}

void UIManager::showHelp() const {
    std::cout << "Virtual Pet Application\n"
              << "----------------------\n"
              << "Commands:\n"
              << "  status       - Show pet status\n"
              << "  feed         - Feed your pet\n"
              << "  play         - Play with your pet\n"
              << "  evolve       - Show evolution progress\n"
              << "  achievements - Show all achievements and progress\n"
              << "  new          - Create a new pet\n"
              << "  help         - Show this help message\n"
              << "  clear        - Clear the screen\n"
              << "  exit         - Exit the application\n"
              << std::endl;
}
