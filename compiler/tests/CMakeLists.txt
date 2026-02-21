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

# Tests will be added as we implement components
