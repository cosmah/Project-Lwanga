cmake_minimum_required(VERSION 3.13)
project(LwangaCompiler VERSION 0.1.0 LANGUAGES CXX)

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
)

# Compiler sources
set(COMPILER_SOURCES
    src/driver/main.cpp
)

# Create the compiler executable
add_executable(lwangac ${COMPILER_SOURCES})

# Link against LLVM
target_link_libraries(lwangac ${llvm_libs})

# Compiler flags
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(lwangac PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Install target
install(TARGETS lwangac DESTINATION bin)

# Enable testing
enable_testing()
add_subdirectory(tests)
