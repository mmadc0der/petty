# Petty Architecture Overview


## Module Interactions

### Core Component Interactions

The Petty application follows a component-based architecture where each module has specific responsibilities and communicates with other modules through well-defined interfaces. Below is a detailed description of how these modules interact with each other:

#### GameLogic as the Central Coordinator

`GameLogic` serves as the central coordinator for all other components. It:

1. **Initializes and Owns Other Components**:
   ```cpp
   // In GameLogic constructor
   m_displayManager = std::make_unique<DisplayManager>(m_petState);
   m_achievementManager = std::make_unique<AchievementManager>(m_petState);
   m_interactionManager = std::make_unique<InteractionManager>(m_petState, *m_displayManager, *m_achievementManager);
   m_timeManager = std::make_unique<TimeManager>(m_petState);
   ```

2. **Delegates User Commands**:
   - When a user issues a command (e.g., "feed"), `GameLogic` delegates to the appropriate component:
   ```cpp
   // In GameLogic::feedPet()
   void GameLogic::feedPet() noexcept {
       auto timeMessage = m_timeManager->applyTimeEffects();
       if (timeMessage) {
           m_displayManager->displayMessage(*timeMessage);
       }
       m_interactionManager->feedPet();
       displayNewlyUnlockedAchievements();
       m_petState.save();
   }
   ```

3. **Manages Application Flow**:
   - Controls the sequence of operations during interactions
   - Ensures state is saved after significant actions
   - Coordinates time effects application before interactions

#### Data Flow Between Components

1. **PetState → Other Components**:
   - `PetState` is the central data store that other components read from and write to
   - Components like `DisplayManager`, `InteractionManager`, and `AchievementManager` receive a reference to `PetState` in their constructors
   - Example data flow: When `InteractionManager::feedPet()` is called, it modifies hunger in `PetState` and adds XP

2. **TimeManager → PetState**:
   - `TimeManager` calculates time-based effects and applies them to `PetState`
   - It reads the last interaction time from `PetState` and updates stats based on elapsed time
   ```cpp
   // In TimeManager::applyTimeEffects()
   auto now = std::chrono::system_clock::now();
   auto lastInteraction = m_petState.getLastInteractionTime();
   auto duration = std::chrono::duration_cast<std::chrono::hours>(now - lastInteraction);
   
   if (duration.count() > 0) {
       m_petState.decreaseHunger(GameConfig::getHungerDecreaseRate() * duration.count());
       m_petState.decreaseHappiness(GameConfig::getHappinessDecreaseRate() * duration.count());
       m_petState.increaseEnergy(GameConfig::getEnergyIncreaseRate() * duration.count());
   }
   ```

3. **InteractionManager → AchievementManager**:
   - After interactions, `InteractionManager` triggers achievement checks in `AchievementManager`
   - Example: After feeding, it checks if the "WellFed" achievement should be unlocked
   ```cpp
   // In InteractionManager::feedPet()
   void InteractionManager::feedPet() noexcept {
       // Update pet stats
       m_petState.increaseHunger(GameConfig::getFeedingHungerIncrease());
       m_petState.addXP(GameConfig::getFeedingXPGain());
       
       // Trigger achievement check
       if (!m_petState.getAchievementSystem().isUnlocked(AchievementType::FirstSteps)) {
           m_petState.getAchievementSystem().unlock(AchievementType::FirstSteps);
       }
   }
   ```

4. **UIManager → GameLogic**:
   - `UIManager` processes user input and calls appropriate methods on `GameLogic`
   - It maintains a weak reference to `GameLogic` to avoid circular dependencies
   ```cpp
   // In UIManager::processCommand()
   bool UIManager::processCommand(const std::vector<std::string_view>& args) noexcept {
       if (auto gameLogic = m_gameLogic.lock()) {
           gameLogic->trackCommand(std::string(args[0]));
           return CommandHandlerBase::processCommand(args, *gameLogic);
       }
       return false;
   }
   ```

#### Event Flow and State Updates

1. **Command Execution Flow**:
   ```
   User Input → UIManager → GameLogic → InteractionManager → PetState → AchievementSystem
                                      ↓
                                 DisplayManager
   ```

2. **Time Effect Flow**:
   ```
   GameLogic → TimeManager → PetState → DisplayManager (for warnings)
   ```

3. **Achievement Unlock Flow**:
   ```
   InteractionManager → AchievementSystem → AchievementManager → DisplayManager
   ```

### Configuration and Persistence Layer

1. **GameConfig Integration**:
   - All components access game parameters through the `GameConfig` namespace
   - This creates a consistent configuration layer across the application
   - Example: `TimeManager` uses `GameConfig::getHungerDecreaseRate()` to determine stat decay

2. **Persistence Flow**:
   - `GameLogic` initiates save operations after significant interactions
   - `PetState` handles the actual serialization of pet data
   - `AchievementSystem` has its own save/load methods called by `PetState`
   ```cpp
   // In PetState::save()
   bool PetState::save() const noexcept {
       std::ofstream file(m_saveFilePath, std::ios::binary);
       if (!file) {
           return false;
       }
       
       // Write version and basic pet data
       uint8_t version = 4;
       file.write(reinterpret_cast<const char*>(&version), sizeof(version));
       // ... write other pet data ...
       
       // Save achievements
       m_achievementSystem.save(file);
       
       return true;
   }
   ```

### Error Handling and Robustness

1. **Graceful Degradation**:
   - Components use `noexcept` to prevent exceptions from propagating
   - File operations return boolean success indicators rather than throwing exceptions
   - Example: `PetState::load()` returns false if the save file cannot be opened

2. **Defensive Programming**:
   - Stats are bounded to prevent invalid values
   - Time calculations handle edge cases like system time changes
   - Commands are processed case-insensitively for better user experience

### Dependency Graph

The following diagram illustrates the dependency relationships between components:

```
                  ┌───────────────┐
                  │  GameConfig   │
                  └───────┬───────┘
                          │
                          ▼
┌───────────┐     ┌───────────────┐     ┌───────────────┐
│ UIManager │◄────┤   GameLogic   │────►│ DisplayManager│
└─────┬─────┘     └───────┬───────┘     └───────────────┘
      │                   │
      │           ┌───────┴───────┐
      │           │               │
      │     ┌─────▼─────┐   ┌─────▼─────┐
      └────►│ PetState  │◄──┤TimeManager│
            └─────┬─────┘   └───────────┘
                  │
            ┌─────▼─────┐
            │Achievement│
            │  System   │
            └───────────┘
```

This architecture ensures a clean separation of concerns while maintaining efficient communication between components, making the codebase maintainable and extensible.

## Game Configuration System ([`include/game_config.h`](include/game_config.h))

The game configuration system is responsible for managing all the game balance parameters and constants. It is implemented as a **header-only file** ([`include/game_config.h`](include/game_config.h)) and uses namespaces to organize related constants.

### Key Features:
1. **Presets**: Defines different gameplay styles (`Default`, `Easy`, `Hard`, `Realistic`) with varying stat decay rates and interaction effects.
2. **Stat Management**: Contains maximum stat values for each evolution level (e.g., `EGG_MAX_STAT`, `ADULT_MAX_STAT`).
3. **Time Effects**: Controls time thresholds for applying stat changes and significant time notifications.
4. **Stat Rates**: Defines stat change rates (e.g., hunger decrease, happiness decrease, energy increase) per hour for each preset.
5. **Warnings**: Sets thresholds for low stat warnings (e.g., hunger, happiness).
6. **Initial Stats**: Specifies starting values for pet stats (e.g., hunger, happiness, energy).
7. **Interaction Effects**: Defines effects of feeding and playing on stats and XP gain, with values for each preset.
8. **Evolution XP**: Specifies XP requirements for each evolution level (e.g., `EGG_TO_BABY`, `MASTER_TO_ANCIENT`).

### Functions:
- **Stat Getters**: Functions like [`getMaxStatForEvolutionLevel`](include/game_config.h#L202-L226), [`getHungerDecreaseRate`](include/game_config.h#L254-L270), and [`getFeedingHungerIncrease`](include/game_config.h#L308-L324) retrieve values based on the current preset.
- **Evolution XP**: [`getEvolutionXPRequirement`](include/game_config.h#L228-L252) returns the XP needed for the next evolution level.

### Interactions:
- **Pet System**: Provides stat values and thresholds used by the pet system to manage pet behavior and evolution.
- **Interaction System**: Supplies effects for feeding and playing interactions.
- **Time System**: Uses time thresholds to determine when to apply stat changes.

### Implementation Details:
- **Header-Only Design**: All functions and constants are defined directly in the header file, eliminating the need for a separate `.cpp` file. This simplifies the build process and ensures that changes to configuration values are immediately reflected across the codebase.

## Pet State System ([`include/pet_state.h`](include/pet_state.h), [`src/pet_state.cpp`](src/pet_state.cpp))

The pet state system manages all persistent state information for the pet, including stats, evolution level, and achievements. It is implemented through the `PetState` class.

### Key Features:
1. **Stat Management**: Tracks hunger, happiness, and energy with float precision.
2. **Evolution System**: Manages evolution levels and XP progression.
3. **Persistence**: Handles saving and loading of pet state to/from file.
4. **Achievement Tracking**: Integrates with the `AchievementSystem` to track player progress.
5. **Time Tracking**: Records last interaction time for time-based effects.

### Class Structure:
```cpp
class PetState {
public:
    PetState() noexcept;
    void initialize() noexcept;
    void initialize(std::string_view name) noexcept;
    bool load() noexcept;
    bool save() const noexcept;
    bool saveFileExists() const noexcept;
    std::string_view getName() const noexcept;
    void setName(std::string_view name) noexcept;
    EvolutionLevel getEvolutionLevel() const noexcept;
    uint32_t getXP() const noexcept;
    bool addXP(uint32_t amount) noexcept;
    uint32_t getXPForNextLevel() const noexcept;
    float getMaxStatValue() const noexcept;
    float getHunger() const noexcept;
    float getHungerPercent() const noexcept;
    float getHappiness() const noexcept;
    float getHappinessPercent() const noexcept;
    float getEnergy() const noexcept;
    float getEnergyPercent() const noexcept;
    void increaseHunger(float amount) noexcept;
    void decreaseHunger(float amount) noexcept;
    void increaseHappiness(float amount) noexcept;
    void decreaseHappiness(float amount) noexcept;
    void increaseEnergy(float amount) noexcept;
    void decreaseEnergy(float amount) noexcept;
    std::chrono::system_clock::time_point getLastInteractionTime() const noexcept;
    void updateLastInteractionTime() noexcept;
    AchievementSystem& getAchievementSystem() noexcept;

private:
    std::string m_name;
    EvolutionLevel m_evolutionLevel;
    uint32_t m_xp;
    float m_hunger;
    float m_happiness;
    float m_energy;
    std::chrono::system_clock::time_point m_lastInteractionTime;
    AchievementSystem m_achievementSystem;
    std::filesystem::path m_saveFilePath;
};
```

### Detailed Method Descriptions:

#### Initialization:
- **initialize()**: Resets all stats to default values for a new pet.
- **initialize(std::string_view name)**: Initializes with a specific pet name.

#### Persistence:
- **load()**: Loads pet state from file, returns true if successful.
- **save()**: Saves current state to file, returns true if successful.
- **saveFileExists()**: Checks if a save file exists.

#### Stat Management:
- **getHunger()**, **getHappiness()**, **getEnergy()**: Get current raw stat values.
- **getHungerPercent()**, **getHappinessPercent()**, **getEnergyPercent()**: Get stats as percentages.
- **increaseHunger()**, **decreaseHunger()**: Modify hunger with bounds checking.
- **increaseHappiness()**, **decreaseHappiness()**: Modify happiness with bounds checking.
- **increaseEnergy()**, **decreaseEnergy()**: Modify energy with bounds checking.

#### Evolution System:
- **getEvolutionLevel()**: Returns current evolution level.
- **getXP()**: Returns current XP amount.
- **addXP()**: Adds XP, returns true if evolution occurred.
- **getXPForNextLevel()**: Returns XP needed for next evolution.

#### Achievement Tracking:
- **getAchievementSystem()**: Returns reference to the `AchievementSystem`.

### Implementation Details:
- **Modern C++ Features**: Uses `std::filesystem` for file operations, `std::chrono` for time tracking.
- **Stat Precision**: Uses `float` for smooth stat transitions.
- **Error Handling**: Gracefully handles file I/O errors.

## Achievement Management System ([`include/achievement_manager.h`](include/achievement_manager.h), [`src/achievement_manager.cpp`](src/achievement_manager.cpp))

The achievement management system is responsible for displaying and tracking player achievements. It is implemented through the `AchievementManager` class, which works closely with the `AchievementSystem` to manage achievement states.

### Key Features:
1. **Achievement Display**: Shows unlocked achievements, including newly unlocked ones.
2. **Progress Tracking**: Displays progress towards locked achievements.
3. **Achievement Categories**: Organizes achievements into locked and unlocked categories for better visibility.
4. **Dynamic Updates**: Automatically updates achievement states based on player actions.

### Class Structure:
```cpp
class AchievementManager {
public:
    explicit AchievementManager(PetState& petState) noexcept;
    bool displayAchievements(bool newlyUnlocked = false) const noexcept;
    bool displayNewlyUnlockedAchievements() noexcept;
    void showAllAchievements() const noexcept;

private:
    PetState& m_petState;
};
```

### Detailed Method Descriptions:

#### Constructor:
- **AchievementManager(PetState& petState)**: Initializes the manager with a reference to the pet state.

#### Achievement Display:
- **displayAchievements(bool newlyUnlocked)**: Displays all unlocked achievements. If `newlyUnlocked` is true, only shows recently unlocked achievements.
- **displayNewlyUnlockedAchievements()**: Specifically displays achievements that were unlocked since the last check.
- **showAllAchievements()**: Shows both locked and unlocked achievements, including progress towards locked ones.

### Implementation Details:
- **Stat Integration**: Uses pet state information to track achievement progress.
- **User Feedback**: Provides clear visual feedback when achievements are unlocked.
- **Progress Tracking**: Shows detailed progress for locked achievements, including percentage completion.

### Interactions:
- **Pet State System**: Uses pet state information to track achievement progress.
- **Achievement System**: Integrates with the underlying achievement tracking system.
- **UI System**: Provides achievement information for display in the user interface.

## Game Logic System ([`include/game_logic.h`](include/game_logic.h), [`src/game_logic.cpp`](src/game_logic.cpp))

The game logic system is the core coordinator of all game components, managing the flow of interactions and state updates. It is implemented through the `GameLogic` class, which inherits from `std::enable_shared_from_this` to support shared pointer management.

### Key Responsibilities:
1. **Manager Coordination**: Orchestrates interactions between `DisplayManager`, `AchievementManager`, `InteractionManager`, and `TimeManager`.
2. **State Management**: Maintains a reference to the `PetState` and ensures proper state transitions.
3. **Interaction Handling**: Processes core game interactions like feeding and playing.
4. **UI Integration**: Manages the `UIManager` lifecycle and provides necessary references for UI interactions.
5. **Achievement Tracking**: Monitors and records player actions for achievement progress.

### Class Structure:
```cpp
class GameLogic : public std::enable_shared_from_this<GameLogic> {
public:
    explicit GameLogic(PetState& petState) noexcept;
    void initializeUIManager() noexcept;
    void showStatus() const noexcept;
    void feedPet() noexcept;
    void playWithPet() noexcept;
    void showEvolutionProgress() const noexcept;
    void showAchievements() const noexcept;
    bool createNewPet(bool force = false) noexcept;
    void runInteractiveMode() noexcept;
    void clearScreen() const noexcept;
    void displayPetHeader() const noexcept;
    void displayNewlyUnlockedAchievements();
    void trackCommand(const std::string& command) noexcept;
    PetState& getPetState() noexcept { return m_petState; }

private:
    PetState& m_petState;
    std::unique_ptr<DisplayManager> m_displayManager;
    std::unique_ptr<AchievementManager> m_achievementManager;
    std::unique_ptr<InteractionManager> m_interactionManager;
    std::unique_ptr<TimeManager> m_timeManager;
    std::unique_ptr<UIManager> m_uiManager;
};
```

### Detailed Method Descriptions:

#### Constructor:
- **GameLogic(PetState& petState)**: Initializes all manager objects with references to the pet state. Uses `std::make_unique` for manager instantiation.

#### UI Management:
- **initializeUIManager()**: Sets up the `UIManager` with necessary references to other managers. Uses `shared_from_this()` to establish a shared pointer connection.

#### Core Game Interactions:
- **showStatus()**: Displays the pet's current status, applies time effects, and shows any newly unlocked achievements.
- **feedPet()**: Handles the feeding interaction, applies time effects, and saves the pet state.
- **playWithPet()**: Manages the play interaction, applies time effects, and saves the pet state.

#### Game Management:
- **createNewPet(bool force)**: Creates a new pet, optionally overwriting an existing one. Returns true if a new pet was created.
- **runInteractiveMode()**: Starts the command loop through the `UIManager`.

#### Achievement Tracking:
- **trackCommand(const std::string& command)**: Records unique commands for the Explorer achievement.

### Implementation Details:
- **Modern C++ Features**: Uses `std::unique_ptr` for manager ownership, `std::optional` for return values, and `std::string_view` for string parameters.
- **State Persistence**: Automatically saves the pet state after significant interactions.
- **Time Integration**: Applies time effects before displaying status or processing interactions.

## Interaction Management System ([`include/interaction_manager.h`](include/interaction_manager.h), [`src/interaction_manager.cpp`](src/interaction_manager.cpp))

The interaction management system handles all direct interactions between the player and the pet. It is implemented through the `InteractionManager` class.

### Key Responsibilities:
1. **Interaction Handling**: Manages core interactions like feeding and playing with the pet.
2. **Stat Updates**: Applies stat changes based on interactions using values from `GameConfig`.
3. **Achievement Tracking**: Triggers and tracks achievement progress for interactions.
4. **State Display**: Shows current pet status and evolution progress.
5. **Pet Creation**: Handles creation of new pets.

### Class Structure:
```cpp
class InteractionManager {
public:
    InteractionManager(PetState& petState, DisplayManager& displayManager, AchievementManager& achievementManager) noexcept;
    void feedPet() noexcept;
    void playWithPet() noexcept;
    void showStatus() const noexcept;
    void showEvolutionProgress() const noexcept;
    bool createNewPet(bool force = false) noexcept;

private:
    PetState& m_petState;
    DisplayManager& m_displayManager;
    AchievementManager& m_achievementManager;
};
```

### Detailed Method Descriptions:

#### Constructor:
- **InteractionManager(PetState&, DisplayManager&, AchievementManager&)**: Initializes with references to required systems.

#### Core Interactions:
- **feedPet()**:
  - Increases pet's hunger using `GameConfig::getFeedingHungerIncrease()`
  - Adds XP using `GameConfig::getFeedingXPGain()`
  - Updates interaction time
  - Unlocks FirstSteps achievement if first feeding
  - Displays appropriate messages based on pet's state

- **playWithPet()**:
  - Increases happiness using `GameConfig::getPlayingHappinessIncrease()`
  - Decreases energy using `GameConfig::getPlayingEnergyDecrease()`
  - Adds XP using `GameConfig::getPlayingXPGain()`
  - Updates interaction time
  - Tracks progress for Playful achievement
  - Displays appropriate messages based on pet's state

#### Information Display:
- **showStatus()**:
  - Displays pet's current status including name, evolution level, ASCII art
  - Shows time since last interaction and age
  - Displays current stats (hunger, happiness, energy) and XP progress

- **showEvolutionProgress()**:
  - Displays pet's current evolution level and ASCII art
  - Shows progress towards next evolution level

#### Pet Management:
- **createNewPet(bool force)**:
  - Creates new pet with default stats
  - If force=true, overwrites existing pet
  - Returns true if new pet was created

### Implementation Details:
- **Stat Precision**: Uses float values for smooth stat transitions
- **Achievement Integration**: Tracks interaction-related achievements
- **Time Tracking**: Updates last interaction time for time-based effects
- **Error Handling**: Gracefully handles edge cases like full stats
- **Modern C++ Features**: Uses `std::chrono` for time tracking, `std::format` for string formatting

## Display Management System ([`include/display_manager.h`](include/display_manager.h), [`src/display_manager.cpp`](src/display_manager.cpp))

The display management system is responsible for handling all console output and visual representation of the pet's state. It is implemented through the `DisplayManager` class, which provides methods for displaying pet information, messages, and clearing the screen.

### Key Features:
1. **Pet Information Display**: Shows detailed pet stats (hunger, happiness, energy, XP) and evolution level.
2. **Message Display**: Displays custom messages to the user.
3. **Screen Management**: Provides functionality to clear the console screen.
4. **Cross-Platform Support**: Handles screen clearing differently for Windows and Unix-based systems.

### Class Structure:
```cpp
class DisplayManager {
public:
    explicit DisplayManager(PetState& petState) noexcept;
    void displayMessage(std::string_view message) const noexcept;
    void displayPetHeader() const noexcept;
    void clearScreen() const noexcept;
    std::string_view getEvolutionLevelName(EvolutionLevel level) const noexcept;

private:
    PetState& m_petState;
};
```

### Detailed Method Descriptions:

#### Constructor:
- **DisplayManager(PetState& petState)**: Initializes the manager with a reference to the pet state.

#### Information Display:
- **displayPetHeader()**: Displays the pet's ASCII art, name, evolution level, status description, and detailed statistics.
- **getEvolutionLevelName(EvolutionLevel level)**: Returns a string representation of the given evolution level.

#### Message Handling:
- **displayMessage(std::string_view message)**: Outputs a message to the console.

#### Screen Management:
- **clearScreen()**: Clears the console screen, using platform-specific commands (`cls` for Windows, `clear` for Unix).

### Implementation Details:
- **Stat Integration**: Uses pet state information to display current stats and evolution progress.
- **Cross-Platform Support**: Implements platform-specific screen clearing using preprocessor directives.
- **Consistent Formatting**: Maintains a consistent output format for all displayed information.

### Interactions:
- **Pet State System**: Uses pet state information to display current stats and evolution progress.
- **Game Logic System**: Called by game logic to display messages and update the UI.
- **Achievement System**: Displays achievement-related information when achievements are unlocked.

## Achievement System ([`include/achievement_system.h`](include/achievement_system.h), [`src/achievement_system.cpp`](src/achievement_system.cpp))

The achievement system manages all player achievements and progress tracking. It is implemented through the `AchievementSystem` class.

### Key Features:
1. **Achievement Types**: Defines various achievement categories (FirstSteps, WellFed, HappyDays, etc.) through the `AchievementType` enum.
2. **Progress Tracking**: Tracks progress for achievements that require multiple steps.
3. **Unlock Management**: Uses a bitset to efficiently store unlocked achievements.
4. **Command Tracking**: Tracks unique commands for the Explorer achievement.
5. **Persistence**: Handles saving and loading of achievement state.

### Class Structure:
```cpp
class AchievementSystem {
public:
    AchievementSystem() noexcept;
    bool isUnlocked(AchievementType type) const noexcept;
    bool unlock(AchievementType type) noexcept;
    static std::string_view getName(AchievementType type) noexcept;
    static std::string_view getDescription(AchievementType type) noexcept;
    std::vector<AchievementType> getUnlockedAchievements() const noexcept;
    std::vector<AchievementType> getNewlyUnlockedAchievements() noexcept;
    void clearNewlyUnlocked() noexcept;
    void setUnlockedBitset(const std::bitset<64>& bitset) noexcept;
    uint64_t getUnlockedBits() const noexcept;
    void setUnlockedBits(uint64_t bits) noexcept;
    void incrementProgress(AchievementType type, uint32_t amount = 1) noexcept;
    void setProgress(AchievementType type, uint32_t progress) noexcept;
    uint32_t getProgress(AchievementType type) const noexcept;
    static uint32_t getRequiredProgress(AchievementType type) noexcept;
    void reset() noexcept;
    void trackUniqueCommand(const std::string& command) noexcept;
    bool save(std::ofstream& file) const noexcept;
    bool load(std::ifstream& file, uint8_t version = 4) noexcept;

private:
    std::bitset<64> m_unlockedAchievements;
    std::bitset<64> m_newlyUnlockedAchievements;
    std::array<uint32_t, static_cast<size_t>(AchievementType::Count)> m_progress;
    std::set<std::string> m_usedCommands;
};
```

### Detailed Method Descriptions:

#### Achievement Management:
- **isUnlocked()**: Checks if an achievement is unlocked.
- **unlock()**: Unlocks an achievement if not already unlocked.
- **getName()**: Returns the name of an achievement.
- **getDescription()**: Returns the description of an achievement.

#### Progress Tracking:
- **incrementProgress()**: Increments progress for an achievement.
- **setProgress()**: Directly sets progress for an achievement.
- **getProgress()**: Returns current progress for an achievement.
- **getRequiredProgress()**: Returns required progress for an achievement.

#### State Management:
- **reset()**: Resets all achievements and progress.
- **trackUniqueCommand()**: Tracks unique commands for the Explorer achievement.

#### Persistence:
- **save()**: Saves achievement state to file.
- **load()**: Loads achievement state from file.

### Implementation Details:
- **Efficient Storage**: Uses `std::bitset` for compact achievement storage.
- **Progress Tracking**: Maintains progress array for achievements requiring multiple steps.
- **Command Tracking**: Uses `std::set` to track unique commands efficiently.
- **Version Control**: Supports versioning for save/load operations.

## Command Handler Base System ([`include/command_handler_base.h`](include/command_handler_base.h), [`src/command_handler_base.cpp`](src/command_handler_base.cpp))

The Command Handler Base system provides a foundation for processing user commands in both command-line and interactive modes. It is implemented through the `CommandHandlerBase` class, which serves as an abstract base class for specific command handlers.

### Key Features:
1. **Command Processing**: Handles command execution and argument parsing.
2. **Command Registration**: Allows dynamic registration of command handlers.
3. **Case Insensitivity**: Processes commands case-insensitively.
4. **Error Handling**: Returns boolean status for command processing success.

### Class Structure:
```cpp
class CommandHandlerBase {
public:
    virtual ~CommandHandlerBase() = default;
    virtual bool processCommand(const std::vector<std::string_view>& args, GameLogic& gameLogic) noexcept;
    virtual void showHelp() const noexcept = 0;

protected:
    using CommandHandler = std::function<void(GameLogic&)>;
    std::unordered_map<std::string_view, CommandHandler> m_commandHandlers;
    virtual void initializeCommandHandlers() noexcept = 0;
};
```

### Detailed Method Descriptions:

#### Command Processing:
- **processCommand()**: Processes a command with its arguments. Converts command to lowercase and looks up the corresponding handler. Returns true if command was found and executed successfully.

#### Command Registration:
- **initializeCommandHandlers()**: Pure virtual method that concrete implementations must override to register their specific command handlers.

### Implementation Details:
- **Modern C++ Features**: Uses `std::string_view` for efficient string handling, `std::function` for command handlers, and `std::unordered_map` for fast command lookup.
- **Case Insensitivity**: Converts commands to lowercase before processing to ensure case-insensitive matching.
- **Command Tracking**: Integrates with `GameLogic` to track commands for achievement progress.

### Registered Commands:
- **status**: Shows current pet status
- **feed**: Feeds the pet
- **play**: Plays with the pet
- **evolve**: Shows evolution progress
- **achievements**: Shows unlocked achievements
- **new**: Creates a new pet

### Detailed Description for CommandHandlerBase
The `CommandHandlerBase` class is designed to provide a flexible and extensible framework for handling user commands. It uses a combination of virtual functions and protected member variables to allow derived classes to implement specific command handling logic. The `processCommand` function is responsible for executing the command and its arguments, while the `showHelp` function provides a way to display help information for the available commands. The `initializeCommandHandlers` function is a pure virtual function that must be implemented by derived classes to register their specific command handlers.

The `CommandHandlerBase` class also uses modern C++ features such as `std::string_view` and `std::function` to provide efficient and flexible command handling. The `std::unordered_map` is used to store the command handlers, allowing for fast lookup and execution of commands.

Overall, the `CommandHandlerBase` class provides a solid foundation for building a command-driven application, and its design allows for easy extension and customization of the command handling logic.

## Command Parser System ([`include/command_parser.h`](include/command_parser.h), [`src/command_parser.cpp`](src/command_parser.cpp))

The Command Parser system handles command line argument parsing and dispatching, providing both command-line and interactive modes. It is implemented through the `CommandParser` class which inherits from `CommandHandlerBase`.

### Key Features:
1. **Command Processing**: Parses and processes command line arguments using modern C++ features like `std::string_view`.
2. **Interactive Mode**: Provides seamless transition to interactive mode.
3. **Help System**: Displays comprehensive help information for all available commands.
4. **Special Command Handling**: Implements specific logic for commands like `new` with force flag.

### Class Structure:
```cpp
class CommandParser : public CommandHandlerBase {
public:
    CommandParser() noexcept;
    bool processCommand(const std::vector<std::string_view>& args, GameLogic& gameLogic) noexcept override;
    void showHelp() const noexcept override;
    
private:
    void initializeCommandHandlers() noexcept override;
};
```

### Detailed Method Descriptions:

#### Constructor:
- **CommandParser()**: Initializes command handlers including help and interactive mode commands.

#### Command Processing:
- **processCommand()**: Processes command line arguments with special handling for `new` command and force flag.
- **initializeCommandHandlers()**: Sets up command handlers including help and interactive mode.

#### Help System:
- **showHelp()**: Displays categorized help information including pet interaction and application management commands.

### Implementation Details:
- **Modern C++ Features**: Uses `std::string_view`, `std::function`, and `std::unordered_map` for efficient command handling.
- **Error Handling**: Gracefully handles invalid commands and edge cases.
- **User Interaction**: Provides confirmation prompts for potentially destructive operations.

## Time Management System ([`include/time_manager.h`](include/time_manager.h), [`src/time_manager.cpp`](src/time_manager.cpp))

The time management system is responsible for handling all time-based effects and calculations in the game. It is implemented through the `TimeManager` class, which uses `std::chrono` for precise time tracking.

### Key Features:
1. **Time-Based Effects**: Calculates and applies stat changes based on time passed since last interaction.
2. **Time Formatting**: Provides human-readable formatting for time since last interaction and pet age.
3. **Threshold Handling**: Uses configurable thresholds from `game_config.h` to determine when to apply effects.
4. **Warning System**: Generates messages when significant time has passed or when stats reach warning levels.

### Class Structure:
```cpp
class TimeManager {
public:
    explicit TimeManager(PetState& petState) noexcept;
    std::optional<std::string> applyTimeEffects() noexcept;
    std::string formatTimeSinceLastInteraction(const std::chrono::system_clock::time_point& now) const noexcept;
    std::string formatPetAge(const std::chrono::system_clock::time_point& now) const noexcept;

private:
    PetState& m_petState;
};
```

### Detailed Method Descriptions:

#### Constructor:
- **TimeManager(PetState& petState)**: Initializes the manager with a reference to the pet state.

#### Time-Based Effects:
- **applyTimeEffects()**: 
  - Calculates time passed since last interaction
  - Applies stat changes (hunger decrease, happiness decrease, energy increase)
  - Returns optional message if significant time has passed or stats reach warning levels
  - Uses thresholds from `GameConfig::Time` and `GameConfig::Warnings`

#### Time Formatting:
- **formatTimeSinceLastInteraction()**: 
  - Formats last interaction time as "DD Mon YYYY HH:MM"
  - Appends humanized duration (e.g., "1d 3h 15m")
  - Uses safe time functions (`localtime_s` on Windows, `localtime_r` on Unix)

- **formatPetAge()**:
  - Formats birth date as "DD Mon YYYY"
  - Appends humanized age (e.g., "1y 23d" or "15h" for young pets)
  - Uses same safe time functions as above

### Implementation Details:
- **Modern C++ Features**: Uses `std::chrono` for precise time calculations and `std::format` for string formatting.
- **Safe Time Handling**: Implements platform-specific safe time functions to avoid potential issues with `localtime`.
- **Configurable Thresholds**: Uses values from `game_config.h` for time thresholds and warning levels.
- **Efficient Calculations**: Optimizes time calculations to minimize overhead during frequent calls.

### Interactions:
- **Pet State System**: Reads and modifies pet stats based on time passed.
- **Game Config System**: Uses thresholds and rates from the configuration system.
- **UI System**: Provides formatted time strings for display in the user interface.

## User Interface Manager ([`include/ui_manager.h`](include/ui_manager.h), [`src/ui_manager.cpp`](src/ui_manager.cpp))

The User Interface Manager is responsible for handling the interactive command-line interface and processing user commands. It is implemented through the `UIManager` class, which inherits from `CommandHandlerBase`.

### Key Responsibilities:
1. **Interactive Mode**: Manages the interactive command loop where users can enter commands directly.
2. **Command Processing**: Handles parsing and execution of user commands.
3. **UI Integration**: Coordinates with `DisplayManager` for screen updates and `AchievementManager` for achievement notifications.
4. **Command Handlers**: Provides a framework for registering and executing command-specific logic.

### Class Structure:
```cpp
class UIManager : public CommandHandlerBase {
public:
    UIManager(
        PetState& petState,
        DisplayManager& displayManager,
        AchievementManager& achievementManager,
        InteractionManager& interactionManager,
        TimeManager& timeManager
    ) noexcept;
    
    void setGameLogic(std::shared_ptr<GameLogic> gameLogic) noexcept;
    void runInteractiveMode() noexcept;
    bool processCommand(const std::vector<std::string_view>& args) noexcept;
    void showHelp() const noexcept override;

private:
    void initializeCommandHandlers() noexcept override;
    
    std::weak_ptr<GameLogic> m_gameLogic;
    PetState& m_petState;
    DisplayManager& m_displayManager;
    AchievementManager& m_achievementManager;
    InteractionManager& m_interactionManager;
    TimeManager& m_timeManager;
};
```

### Detailed Method Descriptions:

#### Constructor:
- **UIManager(...)**: Initializes the manager with references to all required components.

#### Game Logic Integration:
- **setGameLogic(std::shared_ptr<GameLogic> gameLogic)**: Sets the reference to the game logic for command processing.

#### Interactive Mode:
- **runInteractiveMode()**: Starts the interactive command loop, handling user input and updating the interface.

#### Command Processing:
- **processCommand(const std::vector<std::string_view>& args)**: Processes a command with its arguments, returning true if the command was recognized and handled.

#### Help System:
- **showHelp()**: Displays available commands and their usage information.

### Implementation Details:
- **Command Handling**: Uses a map of command handlers for flexible command processing.
- **Interactive Loop**: Includes time-based effects and achievement notifications.
- **Error Handling**: Provides clear feedback for invalid commands.

### Interactions:
- **Game Logic**: Uses game logic for executing commands and managing state.
- **Display Manager**: Coordinates screen updates and clears.
- **Achievement Manager**: Displays newly unlocked achievements.
- **Time Manager**: Applies time-based effects during the interactive loop.
