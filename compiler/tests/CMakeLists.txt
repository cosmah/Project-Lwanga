# Test configuration

# LLVM integration test
add_executable(hello_llvm_test hello_llvm_test.cpp)
target_link_libraries(hello_llvm_test ${llvm_libs})
add_test(NAME HelloLLVMTest COMMAND hello_llvm_test)

# Lexer test
add_executable(lexer_test lexer_test.cpp)
target_link_libraries(lexer_test lwanga_lexer)
add_test(NAME LexerTest COMMAND lexer_test)

# Parser test
add_executable(parser_test parser_test.cpp)
target_link_libraries(parser_test lwanga_parser lwanga_lexer)
add_test(NAME ParserTest COMMAND parser_test)

# TypeSystem test
add_executable(type_system_test type_system_test.cpp)
target_link_libraries(type_system_test lwanga_types)
add_test(NAME TypeSystemTest COMMAND type_system_test)

# Integration test (Lexer + Parser + TypeChecker)
add_executable(integration_test integration_test.cpp)
target_link_libraries(integration_test lwanga_types lwanga_parser lwanga_lexer)
add_test(NAME IntegrationTest COMMAND integration_test)

# IR Generator test
add_executable(ir_generator_test ir_generator_test.cpp)
target_link_libraries(ir_generator_test lwanga_codegen lwanga_types lwanga_parser lwanga_lexer ${llvm_libs})
add_test(NAME IRGeneratorTest COMMAND ir_generator_test)

# Comprehensive integration test (Tasks 1-11)
add_executable(comprehensive_test comprehensive_test.cpp)
target_link_libraries(comprehensive_test lwanga_codegen lwanga_types lwanga_parser lwanga_lexer ${llvm_libs})
add_test(NAME ComprehensiveTest COMMAND comprehensive_test)

# Encrypted string test (Task 13)
add_executable(enc_block_test enc_block_test.cpp)
target_link_libraries(enc_block_test lwanga_codegen lwanga_types lwanga_parser lwanga_lexer ${llvm_libs})
add_test(NAME EncBlockTest COMMAND enc_block_test)

# Encrypted string property test (Task 13.3)
add_executable(enc_property_test enc_property_test.cpp)
target_link_libraries(enc_property_test lwanga_codegen lwanga_types lwanga_parser lwanga_lexer ${llvm_libs})
add_test(NAME EncPropertyTest COMMAND enc_property_test)

# Function pointer test (Task 15)
add_executable(function_pointer_test function_pointer_test.cpp)
target_link_libraries(function_pointer_test lwanga_codegen lwanga_types lwanga_parser lwanga_lexer ${llvm_libs})
add_test(NAME FunctionPointerTest COMMAND function_pointer_test)

# Register access test (Task 16)
add_executable(register_test register_test.cpp)
target_link_libraries(register_test lwanga_codegen lwanga_types lwanga_parser lwanga_lexer ${llvm_libs})
add_test(NAME RegisterTest COMMAND register_test)

# Unsafe block test (Task 17)
add_executable(unsafe_test unsafe_test.cpp)
target_link_libraries(unsafe_test lwanga_types lwanga_parser lwanga_lexer)
add_test(NAME UnsafeTest COMMAND unsafe_test)

# Preprocessor test (Task 19)
add_executable(preprocessor_test preprocessor_test.cpp)
target_link_libraries(preprocessor_test lwanga_lexer)
add_test(NAME PreprocessorTest COMMAND preprocessor_test)

# Module system test (Task 18)
add_executable(module_test module_test.cpp)
target_link_libraries(module_test lwanga_modules lwanga_parser lwanga_lexer)
add_test(NAME ModuleTest COMMAND module_test)

# Backend test (Task 20)
add_executable(backend_test backend_test.cpp)
target_link_libraries(backend_test lwanga_backend lwanga_codegen lwanga_types lwanga_parser lwanga_lexer ${llvm_libs})
add_test(NAME BackendTest COMMAND backend_test)

# Obfuscation test (Task 21)
add_executable(obfuscation_test obfuscation_test.cpp)
target_link_libraries(obfuscation_test lwanga_backend lwanga_codegen lwanga_types lwanga_parser lwanga_lexer ${llvm_libs})
add_test(NAME ObfuscationTest COMMAND obfuscation_test)

# Driver integration test (Task 22.6)
add_executable(driver_integration_test driver_integration_test.cpp)
# No linking needed - runs lwangac as subprocess
add_test(NAME DriverIntegrationTest 
         COMMAND driver_integration_test
         WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

# Cross-compilation test (Task 23.3)
add_executable(cross_compilation_test cross_compilation_test.cpp)
target_link_libraries(cross_compilation_test lwanga_backend lwanga_codegen lwanga_types lwanga_parser lwanga_lexer ${llvm_libs})
add_test(NAME CrossCompilationTest COMMAND cross_compilation_test)

# Tests will be added as we implement components
