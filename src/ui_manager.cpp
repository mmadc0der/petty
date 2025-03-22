#include "../include/ui_manager.h"
#include "../include/game_logic.h"
#include <iostream>
#include <algorithm>
#include <format>
#include <chrono>
#include <memory> // Added for std::weak_ptr
#include <string_view>

UIManager::UIManager(
    PetState& petState,
    DisplayManager& displayManager,
    AchievementManager& achievementManager,
    InteractionManager& interactionManager,
    TimeManager& timeManager) noexcept
    : m_petState(petState)
    , m_displayManager(displayManager)
    , m_achievementManager(achievementManager)
    , m_interactionManager(interactionManager)
    , m_timeManager(timeManager)
{
    // Initialize command handlers
    initializeCommandHandlers();
}

void UIManager::setGameLogic(std::shared_ptr<GameLogic> gameLogic) noexcept {
    m_gameLogic = gameLogic; // Save weak_ptr
}

void UIManager::initializeCommandHandlers() noexcept {
    // Call initialization from base class
    CommandHandlerBase::initializeCommandHandlers();
    
    // Add specific handlers for UIManager
    // Add help command for interactive mode
    m_commandHandlers["help"] = [this](GameLogic& /* gameLogic */) -> void {
        // Call showHelp method
        this->showHelp();
    };
}

void UIManager::runInteractiveMode() noexcept {
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
            std::vector<std::string_view> args;
            std::string_view commandView = command;
            
            // Разбиваем строку на аргументы
            size_t start = 0;
            size_t end = 0;
            while ((end = commandView.find(' ', start)) != std::string_view::npos) {
                if (end > start) {
                    args.push_back(commandView.substr(start, end - start));
                }
                start = end + 1;
            }
            
            // Добавляем последний аргумент
            if (start < commandView.size()) {
                args.push_back(commandView.substr(start));
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

bool UIManager::processCommand(const std::vector<std::string_view>& args) noexcept {
    if (args.empty()) {
        return false;
    }
    
    // Check if GameLogic object still exists
    if (auto gameLogic = m_gameLogic.lock()) {
        // If object exists, use base implementation for command processing
        return CommandHandlerBase::processCommand(args, *gameLogic);
    }
    
    return false;
}

void UIManager::showHelp() const noexcept {
    std::cout << "Virtual Pet Application\n"
              << "----------------------\n\n";
              
    // Category 1: Pet Interaction
    std::cout << "Pet Interaction:\n"
              << "  status       - Show pet status\n"
              << "  feed         - Feed your pet\n"
              << "  play         - Play with your pet\n"
              << "  evolve       - Show evolution progress\n"
              << "  achievements - Show all achievements and progress\n\n";
              
    // Category 2: Interface Management
    std::cout << "Interface Management:\n"
              << "  clear        - Clear the screen\n"
              << "  help         - Show this help message\n"
              << "  exit         - Exit the application\n\n";
}
