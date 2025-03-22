#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <exception>
#include <algorithm>
#include <string_view>
#include "../include/command_parser.h"
#include "../include/pet_state.h"
#include "../include/game_logic.h"
#include "../include/ui_manager.h"

int main(int argc, char* argv[]) {
    try {
        std::cout << "Starting program..." << std::endl;
        // Parse command line arguments
        std::vector<std::string_view> args;
        args.reserve(argc - 1);  // Reserve memory for arguments
        for (int i = 1; i < argc; ++i) {
            args.push_back(argv[i]);
        }

        std::cout << "Creating CommandParser..." << std::endl;
        // Create command parser
        auto parser = std::make_unique<CommandParser>();
        
        // If no arguments or help command, show usage
        if (args.empty() || (args.size() == 1 && args[0] == "help")) {
            if (args.empty()) {
                std::cout << "No arguments, creating PetState..." << std::endl;
                // No arguments, run interactive mode
                auto petState = std::make_unique<PetState>();
                std::cout << "Loading pet state..." << std::endl;
                bool loadSuccess = petState->load();
                std::cout << "Load result: " << (loadSuccess ? "success" : "failure") << std::endl;
                
                // If load failed, ask if user wants to create a new pet
                if (!loadSuccess) {
                    std::cout << "Load failed, asking to create a new pet..." << std::endl;
                    std::cout << "Failed to load pet state. Would you like to create a new pet? (yes/no): ";
                    std::string response;
                    std::getline(std::cin, response);
                    
                    // Convert to lowercase for case-insensitive comparison
                    std::transform(response.begin(), response.end(), response.begin(), 
                                  [](unsigned char c) { return std::tolower(c); });
                    
                    if (response == "yes" || response == "y") {
                        std::cout << "Creating a new pet..." << std::endl;
                        auto gameLogic = std::make_shared<GameLogic>(*petState);
                        
                        // Set up cyclic reference AFTER creating the object via shared_ptr
                        gameLogic->initializeUIManager();
                        
                        gameLogic->createNewPet(true);
                        // Save the new pet
                        petState->save();
                        std::cout << "Saving new pet..." << std::endl;
                        // Run interactive mode
                        gameLogic->runInteractiveMode();
                        std::cout << "Running interactive mode..." << std::endl;
                    } else {
                        std::cout << "User declined to create a new pet, exiting..." << std::endl;
                        std::cout << "Exiting without creating a new pet." << std::endl;
                        return 1;
                    }
                } else {
                    std::cout << "Load successful, running interactive mode..." << std::endl;
                    // Pet loaded successfully, run interactive mode
                    auto gameLogic = std::make_shared<GameLogic>(*petState);
                    
                    // Set up cyclic reference AFTER creating the object via shared_ptr
                    gameLogic->initializeUIManager();
                    
                    gameLogic->runInteractiveMode();
                    std::cout << "Running interactive mode..." << std::endl;
                }
                return 0;
            } else {
                std::cout << "Help command, showing help..." << std::endl;
                // Help command
                parser->showHelp();
                return 0;
            }
        }

        std::cout << "Loading pet state..." << std::endl;
        // Load pet state
        auto petState = std::make_unique<PetState>();
        bool loadSuccess = petState->load();
        std::cout << "Load result: " << (loadSuccess ? "success" : "failure") << std::endl;
        
        // Create game logic handler
        auto gameLogic = std::make_shared<GameLogic>(*petState);
        std::cout << "Creating GameLogic..." << std::endl;
        
        // Set up cyclic reference AFTER creating the object via shared_ptr
        gameLogic->initializeUIManager();
        std::cout << "Initializing UIManager..." << std::endl;
        
        // If load failed and it's not a "new" command, ask if user wants to create a new pet
        if (!loadSuccess && (args.empty() || args[0] != "new")) {
            std::cout << "Load failed, asking to create a new pet..." << std::endl;
            std::cout << "Failed to load pet state. Would you like to create a new pet? (yes/no): ";
            std::string response;
            std::getline(std::cin, response);
            
            // Convert to lowercase for case-insensitive comparison
            std::transform(response.begin(), response.end(), response.begin(), 
                          [](unsigned char c) { return std::tolower(c); });
            
            if (response == "yes" || response == "y") {
                std::cout << "Creating a new pet..." << std::endl;
                gameLogic->createNewPet(true);
            } else {
                std::cout << "User declined to create a new pet, exiting..." << std::endl;
                std::cout << "Exiting without creating a new pet." << std::endl;
                return 1;
            }
        }
        
        std::cout << "Processing command..." << std::endl;
        // Process the command
        bool success = parser->processCommand(args, *gameLogic);
        std::cout << "Command processing result: " << (success ? "success" : "failure") << std::endl;
        
        // If the command was not recognized, show help
        if (!success) {
            std::cout << "Command not recognized, showing help..." << std::endl;
            parser->showHelp();
            return 1;
        }
        
        return 0;
    } catch (const std::exception& e) {
        std::cout << "Error: " << e.what() << std::endl;
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cout << "Unknown error occurred." << std::endl;
        std::cerr << "Unknown error occurred." << std::endl;
        return 1;
    }
}
