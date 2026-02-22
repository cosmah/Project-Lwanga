#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <string>
#include <sys/wait.h>
#include <unistd.h>
#include <cassert>

// End-to-end integration tests for the complete compilation pipeline
class E2ETest {
public:
    std::string compileAndRun(const std::string& source, const std::string& name, 
                              const std::string& flags = "") {
        // Write source file
        std::string sourceFile = name + ".lwanga";
        std::ofstream out(sourceFile);
        out << source;
        out.close();
        
        // Compile
        std::string compileCmd = "./lwangac " + sourceFile + " " + flags + " -o " + name + " 2>&1 >/dev/null";
        int compileResult = system(compileCmd.c_str());
        
        if (compileResult != 0) {
            std::remove(sourceFile.c_str());
            return "COMPILE_ERROR";
        }
        
        // Execute and capture output
        std::string output;
        FILE* pipe = popen(("./" + name).c_str(), "r");
        if (pipe) {
            char buffer[128];
            while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
                output += buffer;
            }
            pclose(pipe);
        }
        
        // Clean up
        std::remove(sourceFile.c_str());
        std::remove(name.c_str());
        
        return output;
    }
    
    int compileAndGetExitCode(const std::string& source, const std::string& name,
                             const std::string& flags = "") {
        // Write source file
        std::string sourceFile = name + ".lwanga";
        std::ofstream out(sourceFile);
        out << source;
        out.close();
        
        // Compile
        std::string compileCmd = "./lwangac " + sourceFile + " " + flags + " -o " + name + " 2>&1 >/dev/null";
        int compileResult = system(compileCmd.c_str());
        
        if (compileResult != 0) {
            std::remove(sourceFile.c_str());
            return -1;
        }
        
        // Execute and get exit code
        int status = system(("./" + name).c_str());
        int exitCode = WEXITSTATUS(status);
        
        // Clean up
        std::remove(sourceFile.c_str());
        std::remove(name.c_str());
        
        return exitCode;
    }
    
    void runTests() {
        int passed = 0;
        int total = 0;
        
        // Test 1: Basic Arithmetic
        total++;
        std::cout << "Test " << total << ": Basic arithmetic... ";
        std::string source1 = R"(
fn main() -> u64 {
    let a: u64 = 10;
    let b: u64 = 20;
    let c: u64 = a + b;
    return c;
}
)";
        int exitCode = compileAndGetExitCode(source1, "test_arithmetic");
        if (exitCode == 30) {
            std::cout << "PASSED\n";
            passed++;
        } else {
            std::cout << "FAILED (expected 30, got " << exitCode << ")\n";
        }
        
        // Test 2: Function Calls
        total++;
        std::cout << "Test " << total << ": Function calls... ";
        std::string source2 = R"(
fn add(x: u64, y: u64) -> u64 {
    return x + y;
}

fn multiply(x: u64, y: u64) -> u64 {
    return x * y;
}

fn main() -> u64 {
    let a: u64 = add(5, 10);
    let b: u64 = multiply(a, 2);
    return b;
}
)";
        exitCode = compileAndGetExitCode(source2, "test_functions");
        if (exitCode == 30) {
            std::cout << "PASSED\n";
            passed++;
        } else {
            std::cout << "FAILED (expected 30, got " << exitCode << ")\n";
        }
        
        // Test 3: Control Flow
        total++;
        std::cout << "Test " << total << ": Control flow (if/else)... ";
        std::string source3 = R"(
fn main() -> u64 {
    let x: u64 = 10;
    if (x > 5) {
        return 1;
    } else {
        return 0;
    }
    return 99;
}
)";
        exitCode = compileAndGetExitCode(source3, "test_if");
        if (exitCode == 1) {
            std::cout << "PASSED\n";
            passed++;
        } else {
            std::cout << "FAILED (expected 1, got " << exitCode << ")\n";
        }
        
        // Test 4: While Loop
        total++;
        std::cout << "Test " << total << ": While loop... ";
        std::string source4 = R"(
fn main() -> u64 {
    let mut sum: u64 = 0;
    let mut i: u64 = 1;
    while (i <= 10) {
        sum = sum + i;
        i = i + 1;
    }
    return sum;
}
)";
        exitCode = compileAndGetExitCode(source4, "test_while");
        if (exitCode == 55) {
            std::cout << "PASSED\n";
            passed++;
        } else {
            std::cout << "FAILED (expected 55, got " << exitCode << ")\n";
        }
        
        // Test 5: Bitwise Operations
        total++;
        std::cout << "Test " << total << ": Bitwise operations... ";
        std::string source5 = R"(
fn main() -> u64 {
    let a: u64 = 15;
    let b: u64 = 7;
    let c: u64 = a & b;
    return c;
}
)";
        exitCode = compileAndGetExitCode(source5, "test_bitwise");
        if (exitCode == 7) {
            std::cout << "PASSED\n";
            passed++;
        } else {
            std::cout << "FAILED (expected 7, got " << exitCode << ")\n";
        }
        
        // Test 6: Type Casting
        total++;
        std::cout << "Test " << total << ": Type casting... ";
        std::string source6 = R"(
fn main() -> u64 {
    let x: u64 = 100;
    let p: ptr = x as ptr;
    let y: u64 = p as u64;
    return y;
}
)";
        exitCode = compileAndGetExitCode(source6, "test_cast");
        if (exitCode == 100) {
            std::cout << "PASSED\n";
            passed++;
        } else {
            std::cout << "FAILED (expected 100, got " << exitCode << ")\n";
        }
        
        // Test 7: Constants
        total++;
        std::cout << "Test " << total << ": Constants... ";
        std::string source7 = R"(
const VALUE: u64 = 42;

fn main() -> u64 {
    return VALUE;
}
)";
        exitCode = compileAndGetExitCode(source7, "test_const");
        if (exitCode == 42) {
            std::cout << "PASSED\n";
            passed++;
        } else {
            std::cout << "FAILED (expected 42, got " << exitCode << ")\n";
        }
        
        // Test 8: Structs
        total++;
        std::cout << "Test " << total << ": Structs... ";
        std::string source8 = R"(
struct Point {
    x: u64,
    y: u64
}

fn main() -> u64 {
    let p: Point = Point { x: 10, y: 20 };
    return p.x + p.y;
}
)";
        exitCode = compileAndGetExitCode(source8, "test_struct");
        if (exitCode == 30) {
            std::cout << "PASSED\n";
            passed++;
        } else {
            std::cout << "FAILED (expected 30, got " << exitCode << ")\n";
        }
        
        // Test 9: Optimization Levels
        total++;
        std::cout << "Test " << total << ": Optimization levels... ";
        std::string source9 = R"(
fn compute() -> u64 {
    let a: u64 = 5;
    let b: u64 = 10;
    let c: u64 = a * b;
    let d: u64 = c + 7;
    return d;
}

fn main() -> u64 {
    return compute();
}
)";
        int exitO0 = compileAndGetExitCode(source9, "test_O0", "-O 0");
        int exitO1 = compileAndGetExitCode(source9, "test_O1", "-O 1");
        int exitO2 = compileAndGetExitCode(source9, "test_O2", "-O 2");
        int exitO3 = compileAndGetExitCode(source9, "test_O3", "-O 3");
        
        if (exitO0 == 57 && exitO1 == 57 && exitO2 == 57 && exitO3 == 57) {
            std::cout << "PASSED\n";
            passed++;
        } else {
            std::cout << "FAILED (O0=" << exitO0 << ", O1=" << exitO1 
                     << ", O2=" << exitO2 << ", O3=" << exitO3 << ")\n";
        }
        
        // Test 10: Complex Program
        total++;
        std::cout << "Test " << total << ": Complex program... ";
        std::string source10 = R"(
const MAX: u64 = 100;

struct Data {
    value: u64,
    count: u64
}

fn process(d: Data) -> u64 {
    let mut result: u64 = d.value;
    let mut i: u64 = 0;
    while (i < d.count) {
        result = result + 1;
        i = i + 1;
    }
    return result;
}

fn main() -> u64 {
    let data: Data = Data { value: 10, count: 5 };
    let result: u64 = process(data);
    if (result > MAX) {
        return 1;
    } else {
        return result;
    }
}
)";
        exitCode = compileAndGetExitCode(source10, "test_complex");
        if (exitCode == 15) {
            std::cout << "PASSED\n";
            passed++;
        } else {
            std::cout << "FAILED (expected 15, got " << exitCode << ")\n";
        }
        
        std::cout << "\n=== Results ===\n";
        std::cout << "Passed: " << passed << "/" << total << "\n";
        
        if (passed != total) {
            exit(1);
        }
    }
};

int main() {
    std::cout << "=== End-to-End Integration Tests ===\n\n";
    
    E2ETest test;
    test.runTests();
    
    std::cout << "\nAll E2E tests passed!\n";
    return 0;
}
