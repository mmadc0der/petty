#include "../include/command_parser.h"
#include "../include/game_logic.h"
#include <iostream>
#include <algorithm>
#include <functional>
#include <unordered_map>

CommandParser::CommandParser() {
    // Initialize command handlers using lambda functions
    m_commandHandlers["status"] = [](GameLogic& gameLogic) -> void {
        gameLogic.showStatus();
    };
    
    m_commandHandlers["feed"] = [](GameLogic& gameLogic) -> void {
        gameLogic.feedPet();
    };
    
    m_commandHandlers["play"] = [](GameLogic& gameLogic) -> void {
        gameLogic.playWithPet();
    };
    
    m_commandHandlers["evolve"] = [](GameLogic& gameLogic) -> void {
        gameLogic.showEvolutionProgress();
    };
    
    m_commandHandlers["achievements"] = [](GameLogic& gameLogic) -> void {
        gameLogic.showAchievements();
    };
    
    m_commandHandlers["new"] = [](GameLogic& gameLogic) -> void {
        gameLogic.createNewPet();
    };
}

void CommandParser::showHelp() const {
    std::cout << "Virtual Pet Application\n"
              << "----------------------\n"
              << "Usage: pet [command]\n\n"
              << "Commands:\n"
              << "  new          - Create a new pet\n"
              << "  status       - Show pet status\n"
              << "  feed         - Feed your pet\n"
              << "  play         - Play with your pet\n"
              << "  evolve       - Show evolution progress\n"
              << "  achievements - Show all achievements and progress\n"
              << "  help         - Show this help message\n"
              << std::endl;
}

bool CommandParser::processCommand(const std::vector<std::string>& args, GameLogic& gameLogic) const {
    if (args.empty()) {
        return false;
    }
    
    std::string command = args[0];
    std::transform(command.begin(), command.end(), command.begin(), 
                   [](unsigned char c) { return std::tolower(c); });
    
    auto it = m_commandHandlers.find(command);
    if (it != m_commandHandlers.end()) {
        it->second(gameLogic);
        return true;
    }
    
    return false;
}
