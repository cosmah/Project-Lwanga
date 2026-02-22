#pragma once

#include <string>
#include <memory>
#include <vector>

// Forward declarations
namespace lwanga {
    class ProgramAST;
    class ExprAST;
    class StmtAST;
    class FunctionAST;
    class StructAST;
    class ConstantAST;
    class ImportAST;
    class Type;
    class Parameter;
    enum class BinaryOp;
    enum class UnaryOp;
}

namespace lwanga {

struct FormatterConfig {
    int indentWidth = 4;
    int maxLineWidth = 100;
    std::string braceStyle = "kr";  // "kr" or "allman"
    bool spaceAroundOperators = true;
    bool trailingCommas = true;
    bool sortImports = true;
    bool preserveAsmFormatting = true;
    bool indentAsmInstructions = true;
    
    // Header options
    bool addLogo = false;
    std::string logoStyle = "minimal";  // "minimal", "block", "banner", "none"
    bool includeMetadata = false;
    std::string author = "";
    std::string license = "";
    
    static FormatterConfig loadFromFile(const std::string& path);
    static FormatterConfig getDefault();
};

class Formatter {
public:
    explicit Formatter(const FormatterConfig& config = FormatterConfig::getDefault());
    
    // Format a complete program
    std::string format(ProgramAST* program);
    
    // Format individual components
    std::string formatFunction(FunctionAST* func);
    std::string formatStruct(StructAST* structDef);
    std::string formatConstant(ConstantAST* constant);
    std::string formatImport(ImportAST* import);
    
    // Format statements and expressions
    std::string formatStatement(StmtAST* stmt, int indentLevel = 0);
    std::string formatExpression(ExprAST* expr);
    
    // Utility methods
    std::string indent(int level) const;
    std::string wrapLine(const std::string& line, int indentLevel) const;
    
    // Header generation
    std::string generateHeader(const std::string& filename) const;
    
private:
    FormatterConfig config;
    int currentIndentLevel = 0;
    
    // Helper methods
    std::string formatBlock(const std::vector<std::unique_ptr<StmtAST>>& statements, int indentLevel);
    std::string formatParameters(const std::vector<Parameter>& params);
    std::string formatType(Type* type);
    std::string formatBinaryOp(BinaryOp op);
    std::string formatUnaryOp(UnaryOp op);
    
    // Logo generation
    std::string getLogoAsciiArt() const;
};

} // namespace lwanga
