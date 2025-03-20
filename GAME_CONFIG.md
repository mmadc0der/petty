# Petty Game Configuration

## Overview

The `include/game_config.h` file contains game balance settings that can be modified to create different gaming experiences. All numerical parameters affecting gameplay have now been moved to this file.

## Configuration Presets

The game has four configuration presets available:

1. **Default** - standard balanced gameplay
2. **Easy** - easier gameplay with slower parameter decay
3. **Hard** - more difficult gameplay with faster parameter decay
4. **Realistic** - more realistic parameter changes

To change the current preset, edit the following line in the `include/game_config.h` file:

```cpp
constexpr Preset CURRENT_PRESET = Preset::Default;
```

Replace `Preset::Default` with one of the following options:
- `Preset::Easy`
- `Preset::Hard`
- `Preset::Realistic`

## Configurable Parameters

### Time Thresholds

- `MIN_TIME_THRESHOLD` - minimum time in hours before applying time effects (0.05 = 3 minutes)
- `SIGNIFICANT_TIME_THRESHOLD` - time threshold in hours for showing a "significant time passed" message

### Parameter Change Rates per Hour

For each preset, the following parameters are configured:

- `HUNGER_DECREASE_RATE` - rate of hunger decrease per hour
- `HAPPINESS_DECREASE_RATE` - rate of happiness decrease per hour
- `ENERGY_INCREASE_RATE` - rate of energy increase per hour (when the pet is resting)

### Warning Thresholds

- `HUNGER_WARNING_THRESHOLD` - threshold for the "very hungry" warning
- `HAPPINESS_WARNING_THRESHOLD` - threshold for the "sad and needs attention" warning

### Initial Pet Parameters

- `INITIAL_HUNGER` - initial hunger level
- `INITIAL_HAPPINESS` - initial happiness level
- `INITIAL_ENERGY` - initial energy level

### Interaction Effects

#### Feeding

- `HUNGER_INCREASE` - hunger increase when feeding
- `XP_GAIN` - experience gain when feeding

#### Playing

- `HAPPINESS_INCREASE` - happiness increase when playing
- `ENERGY_DECREASE` - energy decrease when playing
- `XP_GAIN` - experience gain when playing

## Creating Custom Presets

You can create your own presets by adding a new variant to the `Preset` enumeration and corresponding parameter values in each settings section.

## Compilation

After making changes to the configuration file, you need to recompile the project for the changes to take effect.
