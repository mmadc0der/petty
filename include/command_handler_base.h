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
 * @brief Base class for command handling
 * 
 * Provides common functionality for command handling
 * both in command line mode and interactive mode
 */
class CommandHandlerBase {
public:
    /**
     * @brief Virtual destructor
     */
    virtual ~CommandHandlerBase() = default;

    /**
     * @brief Process a command
     * @param args Command arguments
     * @param gameLogic Reference to game logic object
     * @return true if command was processed successfully, false otherwise
     */
    virtual bool processCommand(const std::vector<std::string>& args, GameLogic& gameLogic) const;

    /**
     * @brief Show help information
     */
    virtual void showHelp() const = 0;

protected:
    // Type of command handler function
    using CommandHandler = std::function<void(GameLogic&)>;
    
    // Map of command names to their handlers
    std::unordered_map<std::string_view, CommandHandler> m_commandHandlers;
    
    /**
     * @brief Initialize command handlers
     */
    virtual void initializeCommandHandlers();
};
