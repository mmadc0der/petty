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
        std::cout << "Запуск программы..." << std::endl;
        // Parse command line arguments
        std::vector<std::string_view> args;
        args.reserve(argc - 1);  // Reserve memory for arguments
        for (int i = 1; i < argc; ++i) {
            args.push_back(argv[i]);
        }

        std::cout << "Создание CommandParser..." << std::endl;
        // Create command parser
        auto parser = std::make_unique<CommandParser>();
        
        // If no arguments or help command, show usage
        if (args.empty() || (args.size() == 1 && args[0] == "help")) {
            if (args.empty()) {
                std::cout << "Режим без аргументов, создание PetState..." << std::endl;
                // No arguments, run interactive mode
                auto petState = std::make_unique<PetState>();
                std::cout << "Попытка загрузки состояния питомца..." << std::endl;
                bool loadSuccess = petState->load();
                std::cout << "Результат загрузки: " << (loadSuccess ? "успешно" : "не удалось") << std::endl;
                
                // If load failed, ask if user wants to create a new pet
                if (!loadSuccess) {
                    std::cout << "Загрузка не удалась, предложение создания нового питомца..." << std::endl;
                    std::cout << "Failed to load pet state. Would you like to create a new pet? (yes/no): ";
                    std::string response;
                    std::getline(std::cin, response);
                    
                    // Convert to lowercase for case-insensitive comparison
                    std::transform(response.begin(), response.end(), response.begin(), 
                                  [](unsigned char c) { return std::tolower(c); });
                    
                    if (response == "yes" || response == "y") {
                        std::cout << "Создание нового питомца..." << std::endl;
                        auto gameLogic = std::make_shared<GameLogic>(*petState);
                        
                        // Set up cyclic reference AFTER creating the object via shared_ptr
                        gameLogic->initializeUIManager();
                        
                        gameLogic->createNewPet(true);
                        // Save the new pet
                        petState->save();
                        std::cout << "Сохранение нового питомца..." << std::endl;
                        // Run interactive mode
                        gameLogic->runInteractiveMode();
                        std::cout << "Запуск интерактивного режима..." << std::endl;
                    } else {
                        std::cout << "Пользователь отказался от создания нового питомца, выход..." << std::endl;
                        std::cout << "Exiting without creating a new pet." << std::endl;
                        return 1;
                    }
                } else {
                    std::cout << "Загрузка успешна, запуск интерактивного режима..." << std::endl;
                    // Pet loaded successfully, run interactive mode
                    auto gameLogic = std::make_shared<GameLogic>(*petState);
                    
                    // Set up cyclic reference AFTER creating the object via shared_ptr
                    gameLogic->initializeUIManager();
                    
                    gameLogic->runInteractiveMode();
                    std::cout << "Запуск интерактивного режима..." << std::endl;
                }
                return 0;
            } else {
                std::cout << "Режим помощи, вывод справки..." << std::endl;
                // Help command
                parser->showHelp();
                return 0;
            }
        }

        std::cout << "Загрузка состояния питомца..." << std::endl;
        // Load pet state
        auto petState = std::make_unique<PetState>();
        bool loadSuccess = petState->load();
        std::cout << "Результат загрузки: " << (loadSuccess ? "успешно" : "не удалось") << std::endl;
        
        // Create game logic handler
        auto gameLogic = std::make_shared<GameLogic>(*petState);
        std::cout << "Создание GameLogic..." << std::endl;
        
        // Set up cyclic reference AFTER creating the object via shared_ptr
        gameLogic->initializeUIManager();
        std::cout << "Инициализация UIManager..." << std::endl;
        
        // If load failed and it's not a "new" command, ask if user wants to create a new pet
        if (!loadSuccess && (args.empty() || args[0] != "new")) {
            std::cout << "Загрузка не удалась, предложение создания нового питомца..." << std::endl;
            std::cout << "Failed to load pet state. Would you like to create a new pet? (yes/no): ";
            std::string response;
            std::getline(std::cin, response);
            
            // Convert to lowercase for case-insensitive comparison
            std::transform(response.begin(), response.end(), response.begin(), 
                          [](unsigned char c) { return std::tolower(c); });
            
            if (response == "yes" || response == "y") {
                std::cout << "Создание нового питомца..." << std::endl;
                gameLogic->createNewPet(true);
            } else {
                std::cout << "Пользователь отказался от создания нового питомца, выход..." << std::endl;
                std::cout << "Exiting without creating a new pet." << std::endl;
                return 1;
            }
        }
        
        std::cout << "Обработка команды..." << std::endl;
        // Process the command
        bool success = parser->processCommand(args, *gameLogic);
        std::cout << "Результат обработки команды: " << (success ? "успешно" : "не удалось") << std::endl;
        
        // If the command was not recognized, show help
        if (!success) {
            std::cout << "Команда не распознана, вывод справки..." << std::endl;
            parser->showHelp();
            return 1;
        }
        
        return 0;
    } catch (const std::exception& e) {
        std::cout << "Ошибка: " << e.what() << std::endl;
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cout << "Неизвестная ошибка..." << std::endl;
        std::cerr << "Unknown error occurred." << std::endl;
        return 1;
    }
}
