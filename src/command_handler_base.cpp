#include "../include/command_handler_base.h"
#include "../include/game_logic.h"
#include <algorithm>

void CommandHandlerBase::initializeCommandHandlers() noexcept {
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

bool CommandHandlerBase::processCommand(const std::vector<std::string_view>& args, GameLogic& gameLogic) noexcept {
    if (args.empty()) {
        return false;
    }
    
    std::string_view command = args[0];
    // Преобразовать команду к нижнему регистру
    // Для string_view нужно создать новую строку для преобразования
    std::string lowerCommand(command);
    std::transform(lowerCommand.begin(), lowerCommand.end(), lowerCommand.begin(), 
                   [](unsigned char c) { return std::tolower(c); });
    
    auto it = m_commandHandlers.find(lowerCommand);
    if (it != m_commandHandlers.end()) {
        // Отслеживаем команду для достижения Explorer
        gameLogic.trackCommand(std::string(lowerCommand));
        
        it->second(gameLogic);
        return true;
    }
    
    return false;
}
