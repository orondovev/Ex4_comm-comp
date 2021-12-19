cmake_minimum_required(VERSION 3.16)  # CMake version check
project(Ex4_comm-comp C)               # Create project "simple_example"
set(CMAKE_C_STANDARD 99)            # Enable c++14 standard

# Add main.cpp file of project root directory as source file
set(SOURCE_FILES Measure.c)
set(SOURCE_FILES sender.c)


# Add executable target with source files listed in SOURCE_FILES variable
add_executable(Ex4_comm-comp ${SOURCE_FILES})