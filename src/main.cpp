#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <exception>
#include <filesystem>
#include "../include/command_parser.h"
#include "../include/pet_state.h"
#include "../include/game_logic.h"

int main(int argc, char* argv[]) {
    try {
        // Parse command line arguments
        std::vector<std::string> args;
        args.reserve(argc - 1);  // Резервируем память для аргументов
        for (int i = 1; i < argc; ++i) {
            args.push_back(argv[i]);
        }

        // Create command parser
        auto parser = std::make_unique<CommandParser>();
        
        // If no arguments or help command, show usage
        if (args.empty() || (args.size() == 1 && args[0] == "help")) {
            parser->showHelp();
            return 0;
        }

        // Load pet state
        auto petState = std::make_unique<PetState>();
        if (!petState->load()) {
            std::cout << "Creating a new pet!" << std::endl;
            petState->initialize();
        }

        // Create game logic handler
        auto gameLogic = std::make_unique<GameLogic>(*petState);
        
        // Process the command
        if (!parser->processCommand(args, *gameLogic)) {
            std::cerr << "Unknown command. Try 'pet help' for usage information." << std::endl;
            return 1;
        }

        // Save the updated state
        if (!petState->save()) {
            std::cerr << "Warning: Failed to save pet state." << std::endl;
            return 1;
        }

        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Unknown error occurred." << std::endl;
        return 1;
    }
}
