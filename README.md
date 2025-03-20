# Petty - Virtual Pet Command Line Application

A command-line virtual pet (Tamagotchi-style) application written in modern C++.

## Features

- Command-line interface with multiple commands
- Pet state persistence in binary format
- Evolution system with multiple stages
- Stats tracking (hunger, happiness, energy)
- Achievement system
- Time-based effects (pet gets hungry/lonely over time)
- Interactive mode for more convenient interaction

## Commands

- `status` - Show current pet status
- `feed` - Feed your pet
- `play` - Play with your pet
- `evolve` - Check evolution progress
- `achievements` - Show all achievements and progress
- `new` - Create a new pet
- `help` - Show help information
- `clear` - Clear the screen
- `exit` - Exit the application

## Building

### Prerequisites

- CMake 3.14 or higher
- C++17 compatible compiler

### Build Instructions

```bash
# Create a build directory
mkdir build
cd build

# Configure
cmake ..

# Build
cmake --build .
```

## Usage

After building, you can run the application:

```bash
# Run in interactive mode
./pet

# Show help
./pet help

# Show pet status
./pet status

# Feed your pet
./pet feed

# Play with your pet
./pet play

# Check evolution progress
./pet evolve

# Show achievements
./pet achievements
```

## State Files

The pet's state is stored in:
- Windows: `%APPDATA%\pet\state.dat`
- Linux: `~/.pet_state`

Achievements are stored in:
- Windows: `%APPDATA%\pet\achievements.dat`
- Linux: `~/.pet_achievements`

## Evolution Levels

Your pet can evolve through the following stages:
1. Egg
2. Baby
3. Child
4. Teen
5. Adult
6. Master
7. Ancient

Each evolution requires a certain amount of XP, which you gain by interacting with your pet.

## Achievements

The game includes several achievements to unlock:
- First Steps: Feed your pet for the first time
- Playful: Play with your pet 5 times
- Well Fed: Reach 100% hunger
- Happy Days: Reach 100% happiness
- Fully Rested: Reach 100% energy
- Evolution: Evolve your pet to the next stage
- Master: Reach the Master evolution level
- Eternal: Reach the Ancient evolution level
- Dedicated: Interact with your pet for 7 consecutive days
- Explorer: Try all available commands
- Survivor: Keep your pet alive for 30 days

## Technical Features

- Modern C++17 features
- noexcept specifiers for performance improvement
- Using string_view instead of std::string where possible
- Modular architecture with clear separation of responsibilities
- Binary format for efficient data storage

## Authors

The Petty project is an educational project to demonstrate modern C++ capabilities.
