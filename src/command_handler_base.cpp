#include "../include/command_handler_base.h"
#include "../include/game_logic.h"
#include <algorithm>

void CommandHandlerBase::initializeCommandHandlers() {
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

bool CommandHandlerBase::processCommand(const std::vector<std::string>& args, GameLogic& gameLogic) const {
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
