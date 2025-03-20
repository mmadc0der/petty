# Pet - Virtual Pet Command Line Utility

A command-line virtual pet (Tamagotchi-style) application written in modern C++.

## Features

- Command-line interface with multiple commands
- Pet state persistence in a custom binary format
- Evolution system with multiple stages
- Stats tracking (hunger, happiness, energy)
- Achievement system
- Time-based effects (pet gets hungry/lonely over time)

## Commands

- `pet status` - Show current pet status
- `pet feed` - Feed your pet
- `pet play` - Play with your pet
- `pet evolve` - Check evolution progress
- `pet help` - Show help information

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

After building, you can run the application using:

```bash
# Show help
./bin/pet help

# Show pet status
./bin/pet status

# Feed your pet
./bin/pet feed

# Play with your pet
./bin/pet play

# Check evolution progress
./bin/pet evolve
```

## State File

The pet's state is stored in:
- Windows: `%APPDATA%\pet\state.dat`
- Linux: `~/.pet_state`

## Evolution Levels

Your pet can evolve through the following stages:
1. Egg
2. Baby
3. Child
4. Teen
5. Adult
6. Master

Each evolution requires a certain amount of XP, which you gain by interacting with your pet.

## Achievements

The game includes several achievements to unlock:
- First Steps: Feed your pet for the first time
- Playful: Play with your pet 5 times
- Well Fed: Reach 100% hunger
- Happy Days: Reach 100% happiness
- Energetic: Reach 100% energy
- Evolution: Evolve your pet to the next stage
- Master: Reach the final evolution stage
