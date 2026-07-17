cmake_minimum_required(VERSION 3.20)

project(
    OAPW-Core
    VERSION 0.1.0
    DESCRIPTION "Open Ambiophony for PipeWire"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(oapw
    src/main.cpp
    src/geometry_model.cpp
    src/delay_engine.cpp
)

target_include_directories(oapw
    PRIVATE
        include
)

target_compile_options(oapw
    PRIVATE
        -Wall
        -Wextra
        -Wpedantic
)
