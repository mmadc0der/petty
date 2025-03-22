#include "../include/command_parser.h"
#include "../include/game_logic.h"
#include <iostream>
#include <algorithm>
#include <filesystem>

CommandParser::CommandParser() noexcept {
    // Initialize command handlers
    initializeCommandHandlers();
}

void CommandParser::initializeCommandHandlers() noexcept {
    // Call initialization from base class
    CommandHandlerBase::initializeCommandHandlers();
    
    // Add help command for command line mode
    m_commandHandlers["help"] = [this](GameLogic& /* gameLogic */) -> void {
        // Use the updated showHelp method instead of duplicating code
        this->showHelp();
    };
    
    // Add command for starting interactive mode
    m_commandHandlers["interactive"] = [](GameLogic& gameLogic) -> void {
        gameLogic.runInteractiveMode();
    };
}

bool CommandParser::processCommand(const std::vector<std::string_view>& args, GameLogic& gameLogic) noexcept {
    // If no arguments, show help
    if (args.empty()) {
        showHelp();
        return true;
    }
    
    // Special handling for command 'new' with -f flag
    if (args[0] == "new" && args.size() > 1 && args[1] == "-f") {
        gameLogic.createNewPet(true);
        return true;
    }
    
    // Special handling for command 'new' without flags
    if (args[0] == "new") {
        auto& petState = gameLogic.getPetState();
        
        // Check if a pet already exists by attempting to load it
        bool petExists = petState.load();
        
        if (petExists) {
            std::cout << "A pet already exists. Overwriting will delete your current pet permanently.\n";
            std::cout << "Do you want to create a new pet anyway? (yes/no): ";
            
            std::string response;
            std::getline(std::cin, response);
            
            // Convert to lowercase for case-insensitive comparison
            std::transform(response.begin(), response.end(), response.begin(), 
                          [](unsigned char c) { return std::tolower(c); });
            
            if (response == "yes" || response == "y") {
                // User confirmed, create new pet with force=true
                gameLogic.createNewPet(true);
                return true;
            } else {
                std::cout << "Operation canceled. Your pet is safe." << std::endl;
                return true;
            }
        } else {
            // No existing pet, create a new one
            gameLogic.createNewPet(false);
            return true;
        }
    }
    
    // Use base implementation for processing other commands
    return CommandHandlerBase::processCommand(args, gameLogic);
}

void CommandParser::showHelp() const noexcept {
    std::cout << "Virtual Pet Application - Command Line Mode\n"
              << "------------------------------------------\n"
              << "Usage: pet [command] [options]\n\n";
              
    // Category 1: Pet Interaction
    std::cout << "Pet Interaction:\n"
              << "  status       - Show pet status\n"
              << "  feed         - Feed your pet\n"
              << "  play         - Play with your pet\n"
              << "  evolve       - Show evolution progress\n"
              << "  achievements - Show all achievements and progress\n\n";
              
    // Category 2: Application Management
    std::cout << "Application Management:\n"
              << "  new [-f]     - Create a new pet (use -f to force overwrite)\n"
              << "  help         - Show this help message\n"
              << "  interactive  - Start interactive mode\n"
              << std::endl;
}
