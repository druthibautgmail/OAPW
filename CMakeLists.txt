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

# ----------------------------------------------------------------------
# Core Library
# ----------------------------------------------------------------------

add_library(oapw_core
    src/geometry_model.cpp
    src/delay_engine.cpp
    src/gain_matrix.cpp
    src/crossfeed_engine.cpp
    src/race_processor.cpp
    src/config/race_configuration_builder.cpp
    src/acoustics/acoustic_model.cpp
)

target_include_directories(oapw_core
    PUBLIC
        include
)

target_compile_options(oapw_core
    PRIVATE
        -Wall
        -Wextra
        -Wpedantic
)

# ----------------------------------------------------------------------
# Main Application
# ----------------------------------------------------------------------

add_executable(oapw
    src/main.cpp
)

target_link_libraries(oapw
    PRIVATE
        oapw_core
)

# ----------------------------------------------------------------------
# Testing
# ----------------------------------------------------------------------

enable_testing()

function(add_oapw_test TEST_NAME TEST_SOURCE)

    add_executable(${TEST_NAME}
        ${TEST_SOURCE}
    )

    target_link_libraries(${TEST_NAME}
        PRIVATE
            oapw_core
    )

    add_test(
        NAME ${TEST_NAME}
        COMMAND ${TEST_NAME}
    )

endfunction()

add_oapw_test(
    test_gain_matrix
    test/test_gain_matrix.cpp
)
add_oapw_test(
    test_crossfeed_engine
    test/test_crossfeed_engine.cpp
)
add_oapw_test(
    test_race_processor
    test/test_race_processor.cpp
)
add_oapw_test(
    test_delay_engine
    test/test_delay_engine.cpp
)
add_oapw_test(
    test_race_configuration_builder
    test/test_race_configuration_builder.cpp
)
add_oapw_test(
    test_acoustic_model
    test/test_acoustic_model.cpp
)
