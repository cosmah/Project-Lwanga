#include "ModuleLoader.h"
#include <cassert>
#include <iostream>
#include <fstream>
#include <filesystem>

using namespace lwanga;

// Helper to create a temporary test file
void createTestFile(const std::string& path, const std::string& content) {
    std::filesystem::create_directories(std::filesystem::path(path).parent_path());
    std::ofstream file(path);
    file << content;
    file.close();
}

// Helper to clean up test files
void cleanupTestFiles(const std::vector<std::string>& paths) {
    for (const auto& path : paths) {
        std::filesystem::remove(path);
    }
}

void testBasicImport() {
    std::cout << "Testing basic import..." << std::endl;
    
    // Create test files
    std::string mainFile = "/tmp/lwanga_test_main.lwanga";
    std::string moduleFile = "/tmp/lwanga_test_module.lwanga";
    
    createTestFile(moduleFile, R"(
fn helper() -> u32 {
    return 42;
}
)");
    
    createTestFile(mainFile, R"(
import "lwanga_test_module.lwanga";

fn main() -> u32 {
    return helper();
}
)");
    
    ModuleLoader loader;
    auto program = loader.loadModule(mainFile);
    
    assert(program != nullptr);
    assert(!loader.hasErrors());
    
    // Should have 2 functions: helper and main
    assert(program->functions.size() == 2);
    
    cleanupTestFiles({mainFile, moduleFile});
    std::cout << "  ✓ Basic import works" << std::endl;
}

void testCircularImportDetection() {
    std::cout << "Testing circular import detection..." << std::endl;
    
    std::string fileA = "/tmp/lwanga_test_a.lwanga";
    std::string fileB = "/tmp/lwanga_test_b.lwanga";
    
    createTestFile(fileA, R"(
import "lwanga_test_b.lwanga";

fn funcA() -> u32 {
    return 1;
}
)");
    
    createTestFile(fileB, R"(
import "lwanga_test_a.lwanga";

fn funcB() -> u32 {
    return 2;
}
)");
    
    ModuleLoader loader;
    auto program = loader.loadModule(fileA);
    
    // Should detect circular import
    assert(program == nullptr);
    assert(loader.hasErrors());
    
    bool foundCircularError = false;
    for (const auto& error : loader.getErrors()) {
        if (error.find("Circular import") != std::string::npos) {
            foundCircularError = true;
            break;
        }
    }
    assert(foundCircularError);
    
    cleanupTestFiles({fileA, fileB});
    std::cout << "  ✓ Circular import detection works" << std::endl;
}

void testMultipleImports() {
    std::cout << "Testing multiple imports..." << std::endl;
    
    std::string mainFile = "/tmp/lwanga_test_multi_main.lwanga";
    std::string module1 = "/tmp/lwanga_test_module1.lwanga";
    std::string module2 = "/tmp/lwanga_test_module2.lwanga";
    
    createTestFile(module1, R"(
fn func1() -> u32 {
    return 10;
}
)");
    
    createTestFile(module2, R"(
fn func2() -> u32 {
    return 20;
}
)");
    
    createTestFile(mainFile, R"(
import "lwanga_test_module1.lwanga";
import "lwanga_test_module2.lwanga";

fn main() -> u32 {
    return func1() + func2();
}
)");
    
    ModuleLoader loader;
    auto program = loader.loadModule(mainFile);
    
    assert(program != nullptr);
    assert(!loader.hasErrors());
    
    // Should have 3 functions: func1, func2, and main
    assert(program->functions.size() == 3);
    
    cleanupTestFiles({mainFile, module1, module2});
    std::cout << "  ✓ Multiple imports work" << std::endl;
}

void testNestedImports() {
    std::cout << "Testing nested imports..." << std::endl;
    
    std::string mainFile = "/tmp/lwanga_test_nested_main.lwanga";
    std::string moduleA = "/tmp/lwanga_test_nested_a.lwanga";
    std::string moduleB = "/tmp/lwanga_test_nested_b.lwanga";
    
    createTestFile(moduleB, R"(
fn funcB() -> u32 {
    return 100;
}
)");
    
    createTestFile(moduleA, R"(
import "lwanga_test_nested_b.lwanga";

fn funcA() -> u32 {
    return funcB() + 50;
}
)");
    
    createTestFile(mainFile, R"(
import "lwanga_test_nested_a.lwanga";

fn main() -> u32 {
    return funcA();
}
)");
    
    ModuleLoader loader;
    auto program = loader.loadModule(mainFile);
    
    assert(program != nullptr);
    assert(!loader.hasErrors());
    
    // Should have 3 functions: funcB, funcA, and main
    assert(program->functions.size() == 3);
    
    cleanupTestFiles({mainFile, moduleA, moduleB});
    std::cout << "  ✓ Nested imports work" << std::endl;
}

void testImportWithStructs() {
    std::cout << "Testing import with structs..." << std::endl;
    
    std::string mainFile = "/tmp/lwanga_test_struct_main.lwanga";
    std::string moduleFile = "/tmp/lwanga_test_struct_module.lwanga";
    
    createTestFile(moduleFile, R"(
struct Point {
    x: u32,
    y: u32
}

fn createPoint(x: u32, y: u32) -> Point {
    return Point { x: x, y: y };
}
)");
    
    createTestFile(mainFile, R"(
import "lwanga_test_struct_module.lwanga";

fn main() -> u32 {
    let p: Point = createPoint(10, 20);
    return p.x + p.y;
}
)");
    
    ModuleLoader loader;
    auto program = loader.loadModule(mainFile);
    
    assert(program != nullptr);
    assert(!loader.hasErrors());
    
    // Should have 1 struct and 2 functions
    assert(program->structs.size() == 1);
    assert(program->functions.size() == 2);
    
    cleanupTestFiles({mainFile, moduleFile});
    std::cout << "  ✓ Import with structs works" << std::endl;
}

void testImportWithConstants() {
    std::cout << "Testing import with constants..." << std::endl;
    
    std::string mainFile = "/tmp/lwanga_test_const_main.lwanga";
    std::string moduleFile = "/tmp/lwanga_test_const_module.lwanga";
    
    createTestFile(moduleFile, R"(
const MAX_SIZE: u32 = 1024;

fn getMaxSize() -> u32 {
    return MAX_SIZE;
}
)");
    
    createTestFile(mainFile, R"(
import "lwanga_test_const_module.lwanga";

fn main() -> u32 {
    return getMaxSize();
}
)");
    
    ModuleLoader loader;
    auto program = loader.loadModule(mainFile);
    
    assert(program != nullptr);
    assert(!loader.hasErrors());
    
    // Should have 1 constant and 2 functions
    assert(program->constants.size() == 1);
    assert(program->functions.size() == 2);
    
    cleanupTestFiles({mainFile, moduleFile});
    std::cout << "  ✓ Import with constants works" << std::endl;
}

void testMissingFile() {
    std::cout << "Testing missing file error..." << std::endl;
    
    std::string mainFile = "/tmp/lwanga_test_missing_main.lwanga";
    
    createTestFile(mainFile, R"(
import "nonexistent_file.lwanga";

fn main() -> u32 {
    return 0;
}
)");
    
    ModuleLoader loader;
    auto program = loader.loadModule(mainFile);
    
    // Should fail to load
    assert(program == nullptr);
    assert(loader.hasErrors());
    
    bool foundFileError = false;
    for (const auto& error : loader.getErrors()) {
        if (error.find("Failed to open file") != std::string::npos) {
            foundFileError = true;
            break;
        }
    }
    assert(foundFileError);
    
    cleanupTestFiles({mainFile});
    std::cout << "  ✓ Missing file error detection works" << std::endl;
}

int main() {
    std::cout << "\n=== Module System Tests ===" << std::endl;
    
    testBasicImport();
    testCircularImportDetection();
    testMultipleImports();
    testNestedImports();
    testImportWithStructs();
    testImportWithConstants();
    testMissingFile();
    
    std::cout << "\n✓ All module system tests passed!" << std::endl;
    return 0;
}
