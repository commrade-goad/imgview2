cmake_minimum_required(VERSION 3.20)
project(imgview VERSION 1.1 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Warning level - for GCC/Clang and MSVC
if (MSVC)
    add_compile_options(/W3)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Find dependencies
find_package(PkgConfig REQUIRED)
pkg_check_modules(SDL3 REQUIRED sdl3)
find_package(Threads REQUIRED)

# Source files
set(SOURCES
    src/main.cpp
    src/window.cpp
    src/argparse.cpp
    src/state.cpp
    src/stateman.cpp
)

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Include and link
target_include_directories(${PROJECT_NAME} PRIVATE ${SDL3_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PRIVATE ${SDL3_LIBRARIES} Threads::Threads m)
