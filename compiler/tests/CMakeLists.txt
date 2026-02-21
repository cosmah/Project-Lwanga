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

# Tests will be added as we implement components
