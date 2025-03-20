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
            if (args.empty()) {
                // No arguments, run interactive mode
                auto petState = std::make_unique<PetState>();
                bool loadSuccess = petState->load();
                
                // If load failed, ask if user wants to create a new pet
                if (!loadSuccess) {
                    std::cout << "Failed to load pet state. Would you like to create a new pet? (yes/no): ";
                    std::string response;
                    std::getline(std::cin, response);
                    
                    // Convert to lowercase for case-insensitive comparison
                    std::transform(response.begin(), response.end(), response.begin(), 
                                  [](unsigned char c) { return std::tolower(c); });
                    
                    if (response == "yes" || response == "y") {
                        auto gameLogic = std::make_unique<GameLogic>(*petState);
                        gameLogic->createNewPet(true);
                        // Save the new pet
                        petState->save();
                        // Run interactive mode
                        gameLogic->runInteractiveMode();
                    } else {
                        std::cout << "Exiting without creating a new pet." << std::endl;
                        return 1;
                    }
                } else {
                    // Pet loaded successfully, run interactive mode
                    auto gameLogic = std::make_unique<GameLogic>(*petState);
                    gameLogic->runInteractiveMode();
                }
                return 0;
            } else {
                // Help command
                parser->showHelp();
                return 0;
            }
        }

        // Load pet state
        auto petState = std::make_unique<PetState>();
        bool loadSuccess = petState->load();
        
        // Create game logic handler
        auto gameLogic = std::make_unique<GameLogic>(*petState);
        
        // If load failed and it's not a "new" command, ask if user wants to create a new pet
        if (!loadSuccess && (args.empty() || args[0] != "new")) {
            std::cout << "Failed to load pet state. Would you like to create a new pet? (yes/no): ";
            std::string response;
            std::getline(std::cin, response);
            
            // Convert to lowercase for case-insensitive comparison
            std::transform(response.begin(), response.end(), response.begin(), 
                          [](unsigned char c) { return std::tolower(c); });
            
            if (response == "yes" || response == "y") {
                gameLogic->createNewPet(true);
                // Save the new pet
                petState->save();
            } else {
                std::cout << "Exiting without creating a new pet." << std::endl;
                return 1;
            }
        }
        
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
