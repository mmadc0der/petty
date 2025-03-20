#include "../include/command_parser.h"
#include "../include/game_logic.h"
#include <iostream>
#include <algorithm>

CommandParser::CommandParser() {
    // Initialize command handlers
    initializeCommandHandlers();
}

void CommandParser::initializeCommandHandlers() {
    // Call initialization from base class
    CommandHandlerBase::initializeCommandHandlers();
    
    // Add help command for command line mode
    m_commandHandlers["help"] = [](GameLogic& gameLogic) -> void {
        // This command is special as it doesn't delegate execution to GameLogic
        std::cout << "Virtual Pet Application - Command Line Mode\n"
                  << "------------------------------------------\n"
                  << "Usage: pet [command] [options]\n\n"
                  << "Commands:\n"
                  << "  status       - Show pet status\n"
                  << "  feed         - Feed your pet\n"
                  << "  play         - Play with your pet\n"
                  << "  evolve       - Show evolution progress\n"
                  << "  achievements - Show all achievements and progress\n"
                  << "  new [-f]     - Create a new pet (use -f to force overwrite)\n"
                  << "  help         - Show this help message\n"
                  << "  interactive  - Start interactive mode\n"
                  << std::endl;
    };
    
    // Add command for starting interactive mode
    m_commandHandlers["interactive"] = [](GameLogic& gameLogic) -> void {
        gameLogic.runInteractiveMode();
    };
}

bool CommandParser::processCommand(const std::vector<std::string>& args, GameLogic& gameLogic) const {
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
    
    // Use base implementation for processing other commands
    return CommandHandlerBase::processCommand(args, gameLogic);
}

void CommandParser::showHelp() const {
    std::cout << "Virtual Pet Application - Command Line Mode\n"
              << "------------------------------------------\n"
              << "Usage: pet [command] [options]\n\n"
              << "Commands:\n"
              << "  status       - Show pet status\n"
              << "  feed         - Feed your pet\n"
              << "  play         - Play with your pet\n"
              << "  evolve       - Show evolution progress\n"
              << "  achievements - Show all achievements and progress\n"
              << "  new [-f]     - Create a new pet (use -f to force overwrite)\n"
              << "  help         - Show this help message\n"
              << "  interactive  - Start interactive mode\n"
              << std::endl;
}
