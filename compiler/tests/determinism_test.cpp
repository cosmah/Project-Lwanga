#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <vector>
#include <string>
#include <cassert>

// Test that compilation is deterministic - same input produces same output
class DeterminismTest {
public:
    std::string testProgram;
    
    DeterminismTest() {
        testProgram = R"(
fn add(a: u64, b: u64) -> u64 {
    return a + b;
}

fn main() -> u64 {
    let x: u64 = 10;
    let y: u64 = 20;
    let z: u64 = add(x, y);
    return z;
}
)";
    }
    
    std::string readBinaryFile(const std::string& path) {
        std::ifstream file(path, std::ios::binary);
        if (!file.is_open()) {
            return "";
        }
        std::stringstream buffer;
        buffer << file.rdbuf();
        return buffer.str();
    }
    
    bool compileProgram(const std::string& source, const std::string& output) {
        // Write source to temp file
        std::string sourceFile = output + ".lwanga";
        std::ofstream out(sourceFile);
        out << source;
        out.close();
        
        // Compile
        std::string cmd = "./lwangac " + sourceFile + " -o " + output + " 2>&1 >/dev/null";
        int result = system(cmd.c_str());
        
        // Clean up source
        std::remove(sourceFile.c_str());
        
        return result == 0;
    }
    
    void testSameSourceProducesSameBinary() {
        std::cout << "Test: Same source produces same binary... ";
        
        // Compile the same program twice
        assert(compileProgram(testProgram, "test_det1"));
        assert(compileProgram(testProgram, "test_det2"));
        
        // Read both binaries
        std::string binary1 = readBinaryFile("test_det1");
        std::string binary2 = readBinaryFile("test_det2");
        
        assert(!binary1.empty());
        assert(!binary2.empty());
        
        // Binaries should be identical
        if (binary1 == binary2) {
            std::cout << "PASSED\n";
        } else {
            std::cout << "FAILED - Compilation is not deterministic\n";
            exit(1);
        }
        
        // Clean up
        std::remove("test_det1");
        std::remove("test_det2");
    }
    
    void testMultipleCompilationsWithSameFlags() {
        std::cout << "Test: Multiple compilations with same flags... ";
        
        std::vector<std::string> outputs;
        
        // Compile 5 times with same flags
        for (int i = 0; i < 5; i++) {
            std::string output = "test_multi" + std::to_string(i);
            assert(compileProgram(testProgram, output));
            outputs.push_back(output);
        }
        
        // Read first binary as reference
        std::string reference = readBinaryFile(outputs[0]);
        assert(!reference.empty());
        
        // All others should match
        bool allMatch = true;
        for (size_t i = 1; i < outputs.size(); i++) {
            std::string current = readBinaryFile(outputs[i]);
            if (reference != current) {
                std::cout << "FAILED - Compilation " << i << " differs from reference\n";
                allMatch = false;
                break;
            }
        }
        
        if (allMatch) {
            std::cout << "PASSED\n";
        }
        
        // Clean up
        for (const auto& output : outputs) {
            std::remove(output.c_str());
        }
        
        if (!allMatch) exit(1);
    }
};

int main() {
    std::cout << "=== Determinism Tests ===\n";
    
    DeterminismTest test;
    test.testSameSourceProducesSameBinary();
    test.testMultipleCompilationsWithSameFlags();
    
    std::cout << "\nAll determinism tests passed!\n";
    return 0;
}
