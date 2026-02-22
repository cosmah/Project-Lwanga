cmake_minimum_required(VERSION 3.15)
project(lwangafmt)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../compiler/include
)

# Formatter library
add_library(lwanga_formatter
    src/Formatter.cpp
)

# Link with compiler libraries
target_link_libraries(lwanga_formatter
    lwanga_lexer
    lwanga_parser
    lwanga_types
)

# Formatter binary
add_executable(lwangafmt
    src/main.cpp
)

target_link_libraries(lwangafmt
    lwanga_formatter
    lwanga_lexer
    lwanga_parser
    lwanga_types
)

# Install
install(TARGETS lwangafmt DESTINATION bin)
