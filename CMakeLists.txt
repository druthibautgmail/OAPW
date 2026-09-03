cmake_minimum_required(VERSION 3.10)
project(OAPW CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -mcpu=cortex-a76 -mtune=cortex-a76")

include_directories(include)
file(GLOB SOURCES "src/*.cpp")

add_executable(OAPW_Player ${SOURCES})

# Neue Verknüpfungen für Miniaudio (Threads, Mathematik, Dynamisches Laden)
target_link_libraries(OAPW_Player m pthread dl)
