cmake_minimum_required(VERSION 3.10)
project(OAPW CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# Für den RPi5 kann die nächste Zeile aktiviert werden, ansonsten einfach die  
# in der darauffolgenden -mcpu=native ... Compile Flags verwenden, die auf allen 
# RPi's (4 und 5) funktionieren sollten

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -mcpu=cortex-a76 -mtune=cortex-a76")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -mcpu=native -mtune=native")

include_directories(include)
file(GLOB SOURCES "src/*.cpp")

add_executable(OAPW_Player ${SOURCES})

# Neue Verknüpfungen für Miniaudio (Threads, Mathematik, Dynamisches Laden)
target_link_libraries(OAPW_Player m pthread dl)
