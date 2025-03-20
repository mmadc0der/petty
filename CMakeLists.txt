cmake_minimum_required(VERSION 3.14)
project(pet VERSION 0.1.0 LANGUAGES CXX)

# Set C++ standard to C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Add executable
add_executable(pet
    src/main.cpp
    src/pet_state.cpp
    src/display_manager.cpp
    src/achievement_manager.cpp
    src/achievement_system.cpp
    src/interaction_manager.cpp
    src/time_manager.cpp
    src/game_logic.cpp
    src/ui_manager.cpp
    src/command_parser.cpp
    src/command_handler_base.cpp
)

# Include directories - updated to use the new include directory
target_include_directories(pet PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Set output directory
set_target_properties(pet PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# Add compiler warnings
if(MSVC)
    target_compile_options(pet PRIVATE /W4)
else()
    target_compile_options(pet PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Enable Address Sanitizer in Debug mode (except on Windows)
if(CMAKE_BUILD_TYPE STREQUAL "Debug" AND NOT MSVC)
    target_compile_options(pet PRIVATE -fsanitize=address)
    target_link_options(pet PRIVATE -fsanitize=address)
endif()

# Install target
install(TARGETS pet
    RUNTIME DESTINATION bin
)
