#pragma once

#include "command_handler_base.h"
#include <string>
#include <vector>
#include <string_view>
#include <functional>
#include <unordered_map>
#include <memory>

// Forward declaration
class GameLogic;

/**
 * @brief Handles command line argument parsing and dispatching
 * 
 * Uses modern C++ features like string_view and std::function for command handling
 */
class CommandParser : public CommandHandlerBase {
public:
    /**
     * @brief Constructor
     */
    CommandParser() noexcept;
    
    /**
     * @brief Process command line arguments
     * @param args Vector of command line arguments
     * @param gameLogic Reference to game logic
     * @return True if the command was processed successfully
     */
    bool processCommand(const std::vector<std::string_view>& args, GameLogic& gameLogic) noexcept override;
    
    /**
     * @brief Show help information
     */
    void showHelp() const noexcept override;
    
private:
    /**
     * @brief Initialize command handlers
     */
    void initializeCommandHandlers() noexcept override;
};
