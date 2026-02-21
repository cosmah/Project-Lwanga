#ifndef LWANGA_PARSER_H
#define LWANGA_PARSER_H

#include "AST.h"
#include "Lexer.h"
#include "Token.h"
#include <memory>
#include <vector>
#include <string>

namespace lwanga {

class ParseError {
public:
    std::string message;
    uint32_t line;
    uint32_t column;
    
    ParseError(const std::string& msg, uint32_t ln, uint32_t col)
        : message(msg), line(ln), column(col) {}
};

class Parser {
public:
    explicit Parser(Lexer& lex);
    
    // Main entry point
    std::unique_ptr<ProgramAST> parse();
    
    // Get collected errors
    const std::vector<ParseError>& getErrors() const { return errors; }
    bool hasErrors() const { return !errors.empty(); }

private:
    Lexer& lexer;
    Token currentToken;
    std::vector<ParseError> errors;
    
    // Token management
    void advance();
    bool check(TokenType type) const;
    bool match(TokenType type);
    bool expect(TokenType type, const std::string& message);
    
    // Error handling
    void reportError(const std::string& message);
    void synchronize();
    
    // Top-level parsing
    std::unique_ptr<FunctionAST> parseFunction(bool isNaked = false);
    std::unique_ptr<StructAST> parseStruct(bool isPacked = false);
    std::unique_ptr<ConstantAST> parseConstant();
    std::unique_ptr<ImportAST> parseImport();
    
    // Type parsing
    std::unique_ptr<Type> parseType();
    
    // Statement parsing
    std::unique_ptr<StmtAST> parseStatement();
    std::unique_ptr<VarDeclStmt> parseVarDecl();
    std::unique_ptr<AssignmentStmt> parseAssignment(std::unique_ptr<ExprAST> target);
    std::unique_ptr<IfStmt> parseIf();
    std::unique_ptr<WhileStmt> parseWhile();
    std::unique_ptr<ReturnStmt> parseReturn();
    std::unique_ptr<AsmStmt> parseAsm();
    std::vector<AsmOperand> parseAsmOperands();
    std::vector<std::string> parseAsmClobbers();
    std::unique_ptr<UnsafeBlockStmt> parseUnsafeBlock();
    std::vector<std::unique_ptr<StmtAST>> parseBlock();
    
    // Expression parsing (with precedence)
    std::unique_ptr<ExprAST> parseExpression();
    std::unique_ptr<ExprAST> parseAssignmentExpr();
    std::unique_ptr<ExprAST> parseBinaryExpr(int minPrecedence);
    std::unique_ptr<ExprAST> parseUnaryExpr();
    std::unique_ptr<ExprAST> parsePostfixExpr();
    std::unique_ptr<ExprAST> parsePrimaryExpr();
    
    // Special expressions
    std::unique_ptr<ExprAST> parseSyscall();
    std::unique_ptr<ExprAST> parseEncBlock();
    std::unique_ptr<StructInitExpr> parseStructInit(const std::string& structName);
    
    // Helper functions
    int getOperatorPrecedence(TokenType type) const;
    BinaryOp tokenToBinaryOp(TokenType type) const;
    UnaryOp tokenToUnaryOp(TokenType type) const;
};

} // namespace lwanga

#endif // LWANGA_PARSER_H
