#pragma once

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
class CommandParser {
public:
    /**
     * @brief Constructor
     */
    CommandParser();

    /**
     * @brief Displays help information about available commands
     */
    void showHelp() const;

    /**
     * @brief Processes the command line arguments and executes the corresponding action
     * @param args Vector of command line arguments
     * @param gameLogic Reference to the game logic handler
     * @return true if the command was recognized and processed, false otherwise
     */
    bool processCommand(const std::vector<std::string>& args, GameLogic& gameLogic) const;

private:
    // Command handler function type
    using CommandHandler = std::function<void(GameLogic&)>;
    
    // Map of command names to handler functions
    std::unordered_map<std::string_view, CommandHandler> m_commandHandlers;
};
