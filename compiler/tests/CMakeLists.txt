# Test configuration

# LLVM integration test
add_executable(hello_llvm_test hello_llvm_test.cpp)
target_link_libraries(hello_llvm_test ${llvm_libs})
add_test(NAME HelloLLVMTest COMMAND hello_llvm_test)

# Tests will be added as we implement components
# Example test structure (to be populated):
# add_executable(lexer_test lexer_test.cpp)
# target_link_libraries(lexer_test lwanga_lexer ${llvm_libs})
# add_test(NAME LexerTest COMMAND lexer_test)
