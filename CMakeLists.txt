cmake_minimum_required(VERSION 3.10)
project(Notes VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

add_executable(notes
    src/main.cpp
    src/Database.cpp
)

# On Windows you may need extra flags; this works on Linux/macOS
