cmake_minimum_required(VERSION 3.13)
project(LwangaCompiler VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Find LLVM
find_package(LLVM REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

# Add LLVM definitions and include directories
include_directories(${LLVM_INCLUDE_DIRS})
separate_arguments(LLVM_DEFINITIONS_LIST NATIVE_COMMAND ${LLVM_DEFINITIONS})
add_definitions(${LLVM_DEFINITIONS_LIST})

# Add project include directory
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# LLVM components we need
llvm_map_components_to_libnames(llvm_libs
    Core
    Support
    Target
    X86
    AArch64
    ExecutionEngine
    MCJIT
    native
    AllTargetsInfos
    AllTargetsDescs
    AllTargetsAsmParsers
    AllTargetsCodeGens
)

# Lexer library
add_library(lwanga_lexer
    src/lexer/Token.cpp
    src/lexer/Lexer.cpp
    src/lexer/Preprocessor.cpp
)
target_include_directories(lwanga_lexer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Parser library
add_library(lwanga_parser
    src/parser/Parser.cpp
)
target_include_directories(lwanga_parser PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(lwanga_parser lwanga_lexer)

# Type system library
add_library(lwanga_types
    src/types/TypeSystem.cpp
    src/types/SymbolTable.cpp
    src/types/TypeChecker.cpp
)
target_include_directories(lwanga_types PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(lwanga_types lwanga_parser lwanga_lexer)

# IR Generator library
add_library(lwanga_codegen
    src/codegen/IRGenerator.cpp
)
target_include_directories(lwanga_codegen PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(lwanga_codegen lwanga_types lwanga_parser lwanga_lexer ${llvm_libs})

# Backend library
add_library(lwanga_backend
    src/backend/Backend.cpp
    src/backend/Obfuscator.cpp
)
target_include_directories(lwanga_backend PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(lwanga_backend ${llvm_libs})

# Module loader library
add_library(lwanga_modules
    src/driver/ModuleLoader.cpp
)
target_include_directories(lwanga_modules PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(lwanga_modules lwanga_parser lwanga_lexer)

# Compiler sources
set(COMPILER_SOURCES
    src/driver/main.cpp
)

# Create the compiler executable
add_executable(lwangac ${COMPILER_SOURCES})

# Link against LLVM, lexer, parser, types, codegen, backend, and modules
target_link_libraries(lwangac lwanga_lexer lwanga_parser lwanga_types lwanga_codegen lwanga_backend lwanga_modules ${llvm_libs})

# Compiler flags
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(lwangac PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Install target
install(TARGETS lwangac DESTINATION bin)

# Enable testing
enable_testing()
add_subdirectory(tests)

# Add formatter tool
add_subdirectory(../tools/formatter ${CMAKE_CURRENT_BINARY_DIR}/formatter)

# Add syntax highlighter tool
add_subdirectory(../tools/highlighter ${CMAKE_CURRENT_BINARY_DIR}/highlighter)
