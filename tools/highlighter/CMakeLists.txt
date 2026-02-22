cmake_minimum_required(VERSION 3.15)
project(lwangacat)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/include
)

# Source files
set(HIGHLIGHTER_SOURCES
    src/SyntaxHighlighter.cpp
    src/main.cpp
)

# Reuse lexer from compiler
set(LEXER_SOURCES
    ${CMAKE_SOURCE_DIR}/src/lexer/Lexer.cpp
    ${CMAKE_SOURCE_DIR}/src/lexer/Token.cpp
)

# Create executable
add_executable(lwangacat ${HIGHLIGHTER_SOURCES} ${LEXER_SOURCES})

# Installation
install(TARGETS lwangacat DESTINATION bin)
