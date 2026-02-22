#include <cassert>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <initializer_list>
#include <sys/wait.h>
#include <unistd.h>

// Helper to write a test file
void writeTestFile(const std::string& filename, const std::string& content) {
    std::ofstream file(filename);
    assert(file.is_open());
    file << content;
    file.close();
}

// Helper to run a command and get exit code
int runCommand(const std::string& cmd) {
    int status = system(cmd.c_str());
    if (WIFEXITED(status)) {
        return WEXITSTATUS(status);
    }
    return -1;
}

// Helper to check if file exists
bool fileExists(const std::string& filename) {
    std::ifstream file(filename);
    return file.good();
}

// Helper to clean up files
void cleanup(std::initializer_list<const char*> files) {
    for (const auto& file : files) {
        std::remove(file);
    }
}

void cleanup(const char* file) {
    std::remove(file);
}

void cleanup(const char* file1, const char* file2) {
    std::remove(file1);
    std::remove(file2);
}

void cleanup(const char* file1, const char* file2, const char* file3) {
    std::remove(file1);
    std::remove(file2);
    std::remove(file3);
}

void cleanup(const char* file1, const char* file2, const char* file3, const char* file4) {
    std::remove(file1);
    std::remove(file2);
    std::remove(file3);
    std::remove(file4);
}

// Test 1: Basic compilation
void testBasicCompilation() {
    std::cout << "Test 1: Basic compilation... ";
    
    writeTestFile("test_basic.lwanga", R"(
fn main() -> u64 {
    return 42;
}
)");
    
    int result = runCommand("./lwangac test_basic.lwanga -o test_basic 2>&1 > /dev/null");
    assert(result == 0);
    assert(fileExists("test_basic"));
    
    cleanup("test_basic.lwanga", "test_basic");
    std::cout << "PASSED\n";
}

// Test 2: JIT execution
void testJITExecution() {
    std::cout << "Test 2: JIT execution... ";
    
    writeTestFile("test_jit.lwanga", R"(
fn main() -> u64 {
    return 99;
}
)");
    
    int result = runCommand("./lwangac test_jit.lwanga --jit 2>&1 > /dev/null");
    assert(result == 99);
    
    cleanup("test_jit.lwanga");
    std::cout << "PASSED\n";
}

// Test 3: Optimization levels
void testOptimizationLevels() {
    std::cout << "Test 3: Optimization levels... ";
    
    writeTestFile("test_opt.lwanga", R"(
fn add(a: u64, b: u64) -> u64 {
    return a + b;
}

fn main() -> u64 {
    return add(10, 20);
}
)");
    
    // Test O0
    int result = runCommand("./lwangac test_opt.lwanga -O 0 -o test_opt_0 2>&1 > /dev/null");
    assert(result == 0);
    assert(fileExists("test_opt_0"));
    
    // Test O1
    result = runCommand("./lwangac test_opt.lwanga -O 1 -o test_opt_1 2>&1 > /dev/null");
    assert(result == 0);
    assert(fileExists("test_opt_1"));
    
    // Test O2
    result = runCommand("./lwangac test_opt.lwanga -O 2 -o test_opt_2 2>&1 > /dev/null");
    assert(result == 0);
    assert(fileExists("test_opt_2"));
    
    // Test O3
    result = runCommand("./lwangac test_opt.lwanga -O 3 -o test_opt_3 2>&1 > /dev/null");
    assert(result == 0);
    assert(fileExists("test_opt_3"));
    
    cleanup("test_opt.lwanga", "test_opt_0", "test_opt_1", "test_opt_2");
    std::remove("test_opt_3");
    std::cout << "PASSED\n";
}

// Test 4: Debug info generation
void testDebugInfo() {
    std::cout << "Test 4: Debug info generation... ";
    
    writeTestFile("test_debug.lwanga", R"(
fn main() -> u64 {
    let x: u64 = 42;
    return x;
}
)");
    
    // Compile with debug info
    int result = runCommand("./lwangac test_debug.lwanga -g -o test_debug_g 2>&1 > /dev/null");
    assert(result == 0);
    assert(fileExists("test_debug_g"));
    
    // Check for debug info
    result = runCommand("file test_debug_g | grep -q 'with debug_info'");
    assert(result == 0);
    
    // Compile without debug info
    result = runCommand("./lwangac test_debug.lwanga -o test_debug_no_g 2>&1 > /dev/null");
    assert(result == 0);
    assert(fileExists("test_debug_no_g"));
    
    // Check no debug info marker
    result = runCommand("file test_debug_no_g | grep -q 'with debug_info'");
    assert(result != 0);  // Should NOT have debug_info
    
    cleanup("test_debug.lwanga", "test_debug_g", "test_debug_no_g");
    std::cout << "PASSED\n";
}

// Test 5: Verbose mode
void testVerboseMode() {
    std::cout << "Test 5: Verbose mode... ";
    
    writeTestFile("test_verbose.lwanga", R"(
fn main() -> u64 {
    return 1;
}
)");
    
    // Run with verbose flag and check output contains stage markers
    int result = runCommand("./lwangac test_verbose.lwanga -v -o test_verbose 2>&1 | grep -q '\\[1/6\\] Loading modules'");
    assert(result == 0);
    
    result = runCommand("./lwangac test_verbose.lwanga -v -o test_verbose 2>&1 | grep -q '\\[2/6\\] Type checking'");
    assert(result == 0);
    
    result = runCommand("./lwangac test_verbose.lwanga -v -o test_verbose 2>&1 | grep -q 'Total time:'");
    assert(result == 0);
    
    cleanup("test_verbose.lwanga", "test_verbose");
    std::cout << "PASSED\n";
}

// Test 6: LLVM IR emission
void testLLVMIREmission() {
    std::cout << "Test 6: LLVM IR emission... ";
    
    writeTestFile("test_ir.lwanga", R"(
fn main() -> u64 {
    return 5;
}
)");
    
    // Check that --emit-llvm produces IR output
    int result = runCommand("./lwangac test_ir.lwanga --emit-llvm 2>&1 | grep -q 'define i64 @main'");
    assert(result == 0);
    
    result = runCommand("./lwangac test_ir.lwanga --emit-llvm 2>&1 | grep -q 'ModuleID'");
    assert(result == 0);
    
    cleanup("test_ir.lwanga", "a.out");
    std::cout << "PASSED\n";
}

// Test 7: Error reporting
void testErrorReporting() {
    std::cout << "Test 7: Error reporting... ";
    
    // Parse error
    writeTestFile("test_parse_error.lwanga", R"(
fn main() -> u64 {
    let x: u64 = 42
    return x;
}
)");
    
    int result = runCommand("./lwangac test_parse_error.lwanga 2>&1 | grep -q 'Expected'");
    assert(result == 0);
    
    // Type error
    writeTestFile("test_type_error.lwanga", R"(
fn main() -> u64 {
    let x: u64 = "string";
    return x;
}
)");
    
    result = runCommand("./lwangac test_type_error.lwanga 2>&1 | grep -q 'Type mismatch'");
    assert(result == 0);
    
    cleanup("test_parse_error.lwanga", "test_type_error.lwanga");
    std::cout << "PASSED\n";
}

// Test 8: Obfuscation flag
void testObfuscation() {
    std::cout << "Test 8: Obfuscation flag... ";
    
    writeTestFile("test_obf.lwanga", R"(
fn add(a: u64, b: u64) -> u64 {
    return a + b;
}

fn main() -> u64 {
    return add(5, 10);
}
)");
    
    // Compile with obfuscation
    int result = runCommand("./lwangac test_obf.lwanga --obfuscate -o test_obf 2>&1 > /dev/null");
    assert(result == 0);
    assert(fileExists("test_obf"));
    
    // Test with JIT
    result = runCommand("./lwangac test_obf.lwanga --obfuscate --jit 2>&1 > /dev/null");
    assert(result == 15);  // 5 + 10
    
    cleanup("test_obf.lwanga", "test_obf");
    std::cout << "PASSED\n";
}

// Test 9: Position-independent code
void testPIC() {
    std::cout << "Test 9: Position-independent code... ";
    
    writeTestFile("test_pic.lwanga", R"(
fn main() -> u64 {
    return 7;
}
)");
    
    int result = runCommand("./lwangac test_pic.lwanga --pic -o test_pic 2>&1 > /dev/null");
    assert(result == 0);
    assert(fileExists("test_pic"));
    
    cleanup("test_pic.lwanga", "test_pic");
    std::cout << "PASSED\n";
}

// Test 10: Combined flags
void testCombinedFlags() {
    std::cout << "Test 10: Combined flags... ";
    
    writeTestFile("test_combined.lwanga", R"(
fn multiply(a: u64, b: u64) -> u64 {
    return a * b;
}

fn main() -> u64 {
    return multiply(6, 7);
}
)");
    
    // Test multiple flags together
    int result = runCommand("./lwangac test_combined.lwanga -O 2 -g --pic -v -o test_combined 2>&1 | grep -q 'Compilation successful'");
    assert(result == 0);
    assert(fileExists("test_combined"));
    
    // Verify debug info is present
    result = runCommand("file test_combined | grep -q 'with debug_info'");
    assert(result == 0);
    
    cleanup("test_combined.lwanga", "test_combined");
    std::cout << "PASSED\n";
}

// Test 11: Module imports
void testModuleImports() {
    std::cout << "Test 11: Module imports... ";
    
    writeTestFile("test_lib.lwanga", R"(
fn helper() -> u64 {
    return 100;
}
)");
    
    writeTestFile("test_main_import.lwanga", R"(
import "test_lib.lwanga";

fn main() -> u64 {
    return helper();
}
)");
    
    int result = runCommand("./lwangac test_main_import.lwanga --jit 2>&1 > /dev/null");
    assert(result == 100);
    
    cleanup("test_lib.lwanga", "test_main_import.lwanga");
    std::cout << "PASSED\n";
}

// Test 12: Output file naming
void testOutputNaming() {
    std::cout << "Test 12: Output file naming... ";
    
    writeTestFile("test_output.lwanga", R"(
fn main() -> u64 {
    return 0;
}
)");
    
    // Test custom output name
    int result = runCommand("./lwangac test_output.lwanga -o my_custom_name 2>&1 > /dev/null");
    assert(result == 0);
    assert(fileExists("my_custom_name"));
    
    // Test default output name
    result = runCommand("./lwangac test_output.lwanga 2>&1 > /dev/null");
    assert(result == 0);
    assert(fileExists("a.out"));
    
    cleanup("test_output.lwanga", "my_custom_name", "a.out");
    std::cout << "PASSED\n";
}

int main() {
    std::cout << "=== Compiler Driver Integration Tests ===\n\n";
    
    testBasicCompilation();
    testJITExecution();
    testOptimizationLevels();
    testDebugInfo();
    testVerboseMode();
    testLLVMIREmission();
    testErrorReporting();
    testObfuscation();
    testPIC();
    testCombinedFlags();
    testModuleImports();
    testOutputNaming();
    
    std::cout << "\n=== All Integration Tests Passed! ===\n";
    return 0;
}
