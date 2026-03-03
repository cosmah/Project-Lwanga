#include "Parser.h"
#include <stdexcept>
#include <iostream>
#include <set>

namespace lwanga {

Parser::Parser(Lexer& lex) : lexer(lex) {
    advance(); // Load first token
}

void Parser::advance() {
    currentToken = lexer.nextToken();
}

bool Parser::check(TokenType type) const {
    return currentToken.type == type;
}

bool Parser::match(TokenType type) {
    if (check(type)) {
        advance();
        return true;
    }
    return false;
}

bool Parser::expect(TokenType type, const std::string& message) {
    if (check(type)) {
        advance();
        return true;
    }
    reportError(message);
    return false;
}

void Parser::reportError(const std::string& message) {
    errors.push_back(ParseError(message, currentToken.line, currentToken.column));
}

void Parser::synchronize() {
    advance();
    
    while (!check(TokenType::TOK_EOF)) {
        // Synchronize at statement boundaries
        if (check(TokenType::TOK_SEMICOLON)) {
            advance();
            return;
        }
        
        // Synchronize at keyword boundaries
        switch (currentToken.type) {
            case TokenType::TOK_RIGHT_BRACE:
            case TokenType::TOK_FN:
            case TokenType::TOK_LET:
            case TokenType::TOK_IF:
            case TokenType::TOK_WHILE:
            case TokenType::TOK_RETURN:
            case TokenType::TOK_STRUCT:
            case TokenType::TOK_CONST:
            case TokenType::TOK_IMPORT:
                return;
            default:
                break;
        }
        
        advance();
    }
}

std::unique_ptr<ProgramAST> Parser::parse() {
    auto program = std::make_unique<ProgramAST>();
    
    while (!check(TokenType::TOK_EOF)) {
        try {
            // Parse top-level declarations
            if (match(TokenType::TOK_IMPORT)) {
                if (auto imp = parseImport()) program->imports.push_back(std::move(imp));
            } else if (match(TokenType::TOK_CONST)) {
                if (auto con = parseConstant()) program->constants.push_back(std::move(con));
            } else if (match(TokenType::TOK_PACKED)) {
                // Packed struct
                expect(TokenType::TOK_STRUCT, "Expected 'struct' after 'packed'");
                if (auto str = parseStruct(true)) program->structs.push_back(std::move(str));
            } else if (match(TokenType::TOK_STRUCT)) {
                if (auto str = parseStruct(false)) program->structs.push_back(std::move(str));
            } else if (match(TokenType::TOK_NAKED)) {
                expect(TokenType::TOK_FN, "Expected 'fn' after 'naked'");
                if (auto fn = parseFunction(true)) program->functions.push_back(std::move(fn));
            } else if (match(TokenType::TOK_FN)) {
                if (auto fn = parseFunction(false)) program->functions.push_back(std::move(fn));
            } else {
                reportError("Expected top-level declaration (fn, struct, const, import)");
                synchronize();
            }
        } catch (const std::exception& e) {
            reportError(std::string("Parse error: ") + e.what());
            synchronize();
        }
    }
    
    return program;
}

std::unique_ptr<Type> Parser::parseType() {
    if (match(TokenType::TOK_U8)) {
        return Type::makeU8();
    } else if (match(TokenType::TOK_U16)) {
        return Type::makeU16();
    } else if (match(TokenType::TOK_U32)) {
        return Type::makeU32();
    } else if (match(TokenType::TOK_U64)) {
        return Type::makeU64();
    } else if (match(TokenType::TOK_PTR)) {
        return Type::makePtr();
    } else if (match(TokenType::TOK_STAR)) {
        // Pointer type: *Type
        auto pointee = parseType();
        if (!pointee) return nullptr;
        return Type::makePtrTo(std::move(pointee));
    } else if (match(TokenType::TOK_REGISTER)) {
        // Register type - the variable name will be the register name
        auto regType = std::make_unique<Type>(TypeKind::Register);
        return regType;
    } else if (match(TokenType::TOK_LEFT_BRACKET)) {
        // Array type: [type; size]
        auto elementType = parseType();
        expect(TokenType::TOK_SEMICOLON, "Expected ';' in array type");
        
        if (!check(TokenType::TOK_NUMBER)) {
            reportError("Expected array size");
            return nullptr;
        }
        
        size_t size = std::stoull(currentToken.lexeme);
        advance();
        
        expect(TokenType::TOK_RIGHT_BRACKET, "Expected ']' after array type");
        
        auto arrayType = std::make_unique<Type>(TypeKind::Array);
        arrayType->elementType = std::move(elementType);
        arrayType->arraySize = size;
        return arrayType;
    } else if (check(TokenType::TOK_IDENTIFIER)) {
        // Struct type
        std::string structName = currentToken.lexeme;
        advance();
        
        auto structType = std::make_unique<Type>(TypeKind::Struct);
        structType->structName = structName;
        return structType;
    } else if (match(TokenType::TOK_FN)) {
        // Function pointer type: fn(params) -> return
        expect(TokenType::TOK_LEFT_PAREN, "Expected '(' in function pointer type");
        
        std::vector<std::unique_ptr<Type>> paramTypes;
        if (!check(TokenType::TOK_RIGHT_PAREN)) {
            do {
                paramTypes.push_back(parseType());
            } while (match(TokenType::TOK_COMMA));
        }
        
        expect(TokenType::TOK_RIGHT_PAREN, "Expected ')' after function parameters");
        expect(TokenType::TOK_ARROW, "Expected '->' in function pointer type");
        
        auto returnType = parseType();
        
        auto fnType = std::make_unique<Type>(TypeKind::FunctionPointer);
        fnType->paramTypes = std::move(paramTypes);
        fnType->returnType = std::move(returnType);
        return fnType;
    }
    
    reportError("Expected type");
    return nullptr;
}

std::unique_ptr<ImportAST> Parser::parseImport() {
    if (!check(TokenType::TOK_STRING)) {
        reportError("Expected string literal after 'import'");
        return nullptr;
    }
    
    std::string path = currentToken.lexeme;
    advance();
    
    expect(TokenType::TOK_SEMICOLON, "Expected ';' after import");
    
    return std::make_unique<ImportAST>(path);
}

std::unique_ptr<ConstantAST> Parser::parseConstant() {
    if (!check(TokenType::TOK_IDENTIFIER)) {
        reportError("Expected constant name");
        return nullptr;
    }
    
    std::string name = currentToken.lexeme;
    advance();
    
    expect(TokenType::TOK_COLON, "Expected ':' after constant name");
    
    auto type = parseType();
    
    expect(TokenType::TOK_ASSIGN, "Expected '=' in constant declaration");
    
    auto value = parseExpression();
    
    expect(TokenType::TOK_SEMICOLON, "Expected ';' after constant declaration");
    
    return std::make_unique<ConstantAST>(name, std::move(type), std::move(value));
}

std::unique_ptr<StructAST> Parser::parseStruct(bool isPacked) {
    if (!check(TokenType::TOK_IDENTIFIER)) {
        reportError("Expected struct name");
        return nullptr;
    }
    
    std::string name = currentToken.lexeme;
    advance();
    
    expect(TokenType::TOK_LEFT_BRACE, "Expected '{' after struct name");
    
    std::vector<StructField> fields;
    
    while (!check(TokenType::TOK_RIGHT_BRACE) && !check(TokenType::TOK_EOF)) {
        if (!check(TokenType::TOK_IDENTIFIER)) {
            reportError("Expected field name");
            break;
        }
        
        SourceLocation fieldLoc(currentToken.line, currentToken.column);
        std::string fieldName = currentToken.lexeme;
        advance();
        
        expect(TokenType::TOK_COLON, "Expected ':' after field name");
        
        auto fieldType = parseType();
        
        fields.push_back(StructField(fieldName, std::move(fieldType), fieldLoc));
        
        if (!match(TokenType::TOK_COMMA)) {
            break;
        }
    }
    
    expect(TokenType::TOK_RIGHT_BRACE, "Expected '}' after struct fields");
    
    return std::make_unique<StructAST>(name, std::move(fields), isPacked);
}

std::unique_ptr<FunctionAST> Parser::parseFunction(bool isNaked) {
    if (!check(TokenType::TOK_IDENTIFIER)) {
        reportError("Expected function name");
        return nullptr;
    }
    
    std::string name = currentToken.lexeme;
    advance();
    
    expect(TokenType::TOK_LEFT_PAREN, "Expected '(' after function name");
    
    // Parse parameters
    std::vector<Parameter> params;
    if (!check(TokenType::TOK_RIGHT_PAREN)) {
        do {
            if (!check(TokenType::TOK_IDENTIFIER)) {
                reportError("Expected parameter name");
                break;
            }
            
            std::string paramName = currentToken.lexeme;
            SourceLocation paramLoc(currentToken.line, currentToken.column);
            advance();
            
            expect(TokenType::TOK_COLON, "Expected ':' after parameter name");
            
            auto paramType = parseType();
            
            params.push_back(Parameter(paramName, std::move(paramType), paramLoc));
        } while (match(TokenType::TOK_COMMA));
    }
    
    expect(TokenType::TOK_RIGHT_PAREN, "Expected ')' after parameters");
    
    // Parse return type
    std::unique_ptr<Type> returnType;
    if (match(TokenType::TOK_ARROW)) {
        returnType = parseType();
    } else {
        // Default to u64 if no return type specified
        returnType = Type::makeU64();
    }
    
    // Parse function body
    auto body = parseBlock();
    
    return std::make_unique<FunctionAST>(name, std::move(params), 
                                         std::move(returnType), std::move(body), isNaked);
}

std::vector<std::unique_ptr<StmtAST>> Parser::parseBlock() {
    expect(TokenType::TOK_LEFT_BRACE, "Expected '{' at start of block");
    
    std::vector<std::unique_ptr<StmtAST>> statements;
    
    while (!check(TokenType::TOK_RIGHT_BRACE) && !check(TokenType::TOK_EOF)) {
        auto stmt = parseStatement();
        if (stmt) {
            statements.push_back(std::move(stmt));
        } else {
            // Avoid infinite loop on statement parse failure;
            // do not consume the block's closing brace.
            if (!check(TokenType::TOK_RIGHT_BRACE) && !check(TokenType::TOK_EOF)) {
                synchronize();
            }
        }
    }
    
    expect(TokenType::TOK_RIGHT_BRACE, "Expected '}' at end of block");
    
    return statements;
}

std::unique_ptr<StmtAST> Parser::parseStatement() {
    if (match(TokenType::TOK_LET)) {
        return parseVarDecl();
    } else if (match(TokenType::TOK_IF)) {
        return parseIf();
    } else if (match(TokenType::TOK_WHILE)) {
        return parseWhile();
    } else if (match(TokenType::TOK_RETURN)) {
        return parseReturn();
    } else if (match(TokenType::TOK_ASM)) {
        return parseAsm();
    } else if (match(TokenType::TOK_UNSAFE)) {
        return parseUnsafeBlock();
    } else {
        // Expression statement or assignment
        auto expr = parseExpression();
        
        if (match(TokenType::TOK_ASSIGN)) {
            auto value = parseExpression();
            expect(TokenType::TOK_SEMICOLON, "Expected ';' after assignment");
            return std::make_unique<AssignmentStmt>(std::move(expr), std::move(value));
        }
        
        expect(TokenType::TOK_SEMICOLON, "Expected ';' after expression");
        return std::make_unique<ExprStmt>(std::move(expr));
    }
}

std::unique_ptr<VarDeclStmt> Parser::parseVarDecl() {
    bool isMutable = match(TokenType::TOK_MUT);
    
    if (!check(TokenType::TOK_IDENTIFIER)) {
        reportError("Expected variable name");
        return nullptr;
    }
    
    std::string name = currentToken.lexeme;
    advance();
    
    expect(TokenType::TOK_COLON, "Expected ':' after variable name");
    
    auto type = parseType();
    
    std::unique_ptr<ExprAST> initializer;
    if (match(TokenType::TOK_ASSIGN)) {
        initializer = parseExpression();
    }
    
    expect(TokenType::TOK_SEMICOLON, "Expected ';' after variable declaration");
    
    return std::make_unique<VarDeclStmt>(name, std::move(type), 
                                         std::move(initializer), isMutable);
}

std::unique_ptr<IfStmt> Parser::parseIf() {
    expect(TokenType::TOK_LEFT_PAREN, "Expected '(' after 'if'");
    auto condition = parseExpression();
    expect(TokenType::TOK_RIGHT_PAREN, "Expected ')' after if condition");
    
    auto thenBlock = parseBlock();
    
    std::vector<std::unique_ptr<StmtAST>> elseBlock;
    if (match(TokenType::TOK_ELSE)) {
        elseBlock = parseBlock();
    }
    
    return std::make_unique<IfStmt>(std::move(condition), 
                                    std::move(thenBlock), std::move(elseBlock));
}

std::unique_ptr<WhileStmt> Parser::parseWhile() {
    expect(TokenType::TOK_LEFT_PAREN, "Expected '(' after 'while'");
    auto condition = parseExpression();
    expect(TokenType::TOK_RIGHT_PAREN, "Expected ')' after while condition");
    
    auto body = parseBlock();
    
    return std::make_unique<WhileStmt>(std::move(condition), std::move(body));
}

std::unique_ptr<ReturnStmt> Parser::parseReturn() {
    std::unique_ptr<ExprAST> value;
    
    if (!check(TokenType::TOK_SEMICOLON)) {
        value = parseExpression();
    }
    
    expect(TokenType::TOK_SEMICOLON, "Expected ';' after return statement");
    
    return std::make_unique<ReturnStmt>(std::move(value));
}

std::unique_ptr<AsmStmt> Parser::parseAsm() {
    expect(TokenType::TOK_LEFT_BRACE, "Expected '{' after 'asm'");
    
    // Parse assembly code as string literals or raw tokens
    std::string asmCode;
    int braceDepth = 1;
    bool firstToken = true;
    
    // Parse assembly instructions (everything before first ':' or '}')
    int safetyCounter = 0;
    const int MAX_ASM_TOKENS = 10000;  // Safety limit
    
    while (braceDepth > 0 && !check(TokenType::TOK_EOF)) {
        // Safety check to prevent infinite loops
        if (++safetyCounter > MAX_ASM_TOKENS) {
            reportError("Assembly block too large or infinite loop detected");
            break;
        }
        
        // Check for colon (start of constraints) - but only at depth 1
        if (braceDepth == 1 && check(TokenType::TOK_COLON)) {
            break;
        }
        
        if (check(TokenType::TOK_LEFT_BRACE)) {
            braceDepth++;
            asmCode += currentToken.lexeme;
            advance();
        } else if (check(TokenType::TOK_RIGHT_BRACE)) {
            braceDepth--;
            if (braceDepth == 0) break;
            asmCode += currentToken.lexeme;
            advance();
        } else if (check(TokenType::TOK_STRING)) {
            // String literals contain assembly instructions
            if (!firstToken && !asmCode.empty() && asmCode.back() != '\n') {
                asmCode += "\n";
            }
            asmCode += currentToken.lexeme;
            advance();
        } else {
            // Collect raw tokens (for inline assembly syntax)
            // Add newline before instruction mnemonics (identifiers that start a new instruction)
            // Common x86 instruction mnemonics
            static const std::set<std::string> instructionMnemonics = {
                "mov", "push", "pop", "add", "sub", "xor", "and", "or", "not",
                "call", "ret", "jmp", "je", "jne", "jz", "jnz", "jg", "jl",
                "syscall", "int", "lea", "cmp", "test", "inc", "dec", "mul", "div",
                "shl", "shr", "sal", "sar", "rol", "ror", "nop", "hlt"
            };
            
            // If this is an instruction mnemonic and not the first token, add newline
            if (!firstToken && instructionMnemonics.count(currentToken.lexeme) > 0) {
                if (!asmCode.empty() && asmCode.back() != '\n') {
                    asmCode += "\n";
                }
            } else if (!firstToken && !asmCode.empty()) {
                // Determine if we need a space before this token
                bool needSpace = true;
                
                // No space before: comma, closing paren
                if (currentToken.lexeme == "," || currentToken.lexeme == ")") {
                    needSpace = false;
                }
                
                // No space after: %, $, opening paren
                if (asmCode.back() == '%' || asmCode.back() == '$' || asmCode.back() == '(') {
                    needSpace = false;
                }
                
                // No space if last char is already space or newline
                if (asmCode.back() == ' ' || asmCode.back() == '\n') {
                    needSpace = false;
                }
                
                if (needSpace) {
                    asmCode += " ";
                }
            }
            asmCode += currentToken.lexeme;
            advance();
        }
        firstToken = false;
    }
    
    // Check for extended asm syntax (outputs : inputs : clobbers)
    std::vector<AsmOperand> outputs;
    std::vector<AsmOperand> inputs;
    std::vector<std::string> clobbers;
    
    // Parse output operands (if present)
    if (check(TokenType::TOK_COLON)) {
        advance(); // consume ':'
        outputs = parseAsmOperands();
    }
    
    // Parse input operands (if present)
    if (check(TokenType::TOK_COLON)) {
        advance(); // consume ':'
        inputs = parseAsmOperands();
    }
    
    // Parse clobbers (if present)
    if (check(TokenType::TOK_COLON)) {
        advance(); // consume ':'
        clobbers = parseAsmClobbers();
    }
    
    expect(TokenType::TOK_RIGHT_BRACE, "Expected '}' after asm block");
    
    // Create appropriate AsmStmt based on whether we have operands
    if (outputs.empty() && inputs.empty() && clobbers.empty()) {
        return std::make_unique<AsmStmt>(asmCode);
    } else {
        return std::make_unique<AsmStmt>(asmCode, std::move(outputs), std::move(inputs), std::move(clobbers));
    }
}

std::vector<AsmOperand> Parser::parseAsmOperands() {
    std::vector<AsmOperand> operands;
    
    // Parse comma-separated list of operands: "constraint"(expression)
    while (!check(TokenType::TOK_COLON) && !check(TokenType::TOK_RIGHT_BRACE) && !check(TokenType::TOK_EOF)) {
        // Skip whitespace/newlines
        while (check(TokenType::TOK_IDENTIFIER) && currentToken.lexeme == "\n") {
            advance();
        }
        
        if (check(TokenType::TOK_COLON) || check(TokenType::TOK_RIGHT_BRACE)) {
            break;
        }
        
        // Parse constraint string
        if (!check(TokenType::TOK_STRING)) {
            reportError("Expected constraint string in asm operand");
            return operands;
        }
        std::string constraint = currentToken.lexeme;
        advance();
        
        // Expect '('
        if (!check(TokenType::TOK_LEFT_PAREN)) {
            reportError("Expected '(' after constraint in asm operand");
            return operands;
        }
        advance();
        
        // Parse expression (variable reference)
        auto expr = parseExpression();
        if (!expr) {
            reportError("Expected expression in asm operand");
            return operands;
        }
        
        // Expect ')'
        if (!check(TokenType::TOK_RIGHT_PAREN)) {
            reportError("Expected ')' after expression in asm operand");
            return operands;
        }
        advance();
        
        operands.push_back(AsmOperand(constraint, std::move(expr)));
        
        // Check for comma (more operands)
        if (check(TokenType::TOK_COMMA)) {
            advance();
        } else {
            break;
        }
    }
    
    return operands;
}

std::vector<std::string> Parser::parseAsmClobbers() {
    std::vector<std::string> clobbers;
    
    // Parse comma-separated list of clobber strings
    while (!check(TokenType::TOK_RIGHT_BRACE) && !check(TokenType::TOK_EOF)) {
        // Skip whitespace/newlines
        while (check(TokenType::TOK_IDENTIFIER) && currentToken.lexeme == "\n") {
            advance();
        }
        
        if (check(TokenType::TOK_RIGHT_BRACE)) {
            break;
        }
        
        // Parse clobber string (register name or "memory")
        if (!check(TokenType::TOK_STRING)) {
            reportError("Expected clobber string");
            return clobbers;
        }
        clobbers.push_back(currentToken.lexeme);
        advance();
        
        // Check for comma (more clobbers)
        if (check(TokenType::TOK_COMMA)) {
            advance();
        } else {
            break;
        }
    }
    
    return clobbers;
}

std::unique_ptr<UnsafeBlockStmt> Parser::parseUnsafeBlock() {
    auto body = parseBlock();
    return std::make_unique<UnsafeBlockStmt>(std::move(body));
}

// Expression parsing implementation

std::unique_ptr<ExprAST> Parser::parseExpression() {
    return parseBinaryExpr(0);
}

int Parser::getOperatorPrecedence(TokenType type) const {
    switch (type) {
        case TokenType::TOK_STAR:
        case TokenType::TOK_SLASH:
        case TokenType::TOK_PERCENT:
            return 10;
        case TokenType::TOK_PLUS:
        case TokenType::TOK_MINUS:
            return 9;
        case TokenType::TOK_LEFT_SHIFT:
        case TokenType::TOK_RIGHT_SHIFT:
            return 8;
        case TokenType::TOK_LESS:
        case TokenType::TOK_GREATER:
        case TokenType::TOK_LESS_EQUAL:
        case TokenType::TOK_GREATER_EQUAL:
            return 7;
        case TokenType::TOK_EQUAL:
        case TokenType::TOK_NOT_EQUAL:
            return 6;
        case TokenType::TOK_AMPERSAND:
            return 5;
        case TokenType::TOK_CARET:
            return 4;
        case TokenType::TOK_PIPE:
            return 3;
        case TokenType::TOK_LOGICAL_AND:
            return 2;
        case TokenType::TOK_LOGICAL_OR:
            return 1;
        case TokenType::TOK_AS:
            return 2;
        default:
            return -1;
    }
}

BinaryOp Parser::tokenToBinaryOp(TokenType type) const {
    switch (type) {
        case TokenType::TOK_PLUS: return BinaryOp::Add;
        case TokenType::TOK_MINUS: return BinaryOp::Sub;
        case TokenType::TOK_STAR: return BinaryOp::Mul;
        case TokenType::TOK_SLASH: return BinaryOp::Div;
        case TokenType::TOK_PERCENT: return BinaryOp::Mod;
        case TokenType::TOK_AMPERSAND: return BinaryOp::BitAnd;
        case TokenType::TOK_PIPE: return BinaryOp::BitOr;
        case TokenType::TOK_CARET: return BinaryOp::BitXor;
        case TokenType::TOK_LEFT_SHIFT: return BinaryOp::LeftShift;
        case TokenType::TOK_RIGHT_SHIFT: return BinaryOp::RightShift;
        case TokenType::TOK_EQUAL: return BinaryOp::Equal;
        case TokenType::TOK_NOT_EQUAL: return BinaryOp::NotEqual;
        case TokenType::TOK_LESS: return BinaryOp::Less;
        case TokenType::TOK_GREATER: return BinaryOp::Greater;
        case TokenType::TOK_LESS_EQUAL: return BinaryOp::LessEqual;
        case TokenType::TOK_GREATER_EQUAL: return BinaryOp::GreaterEqual;
        case TokenType::TOK_LOGICAL_AND: return BinaryOp::LogicalAnd;
        case TokenType::TOK_LOGICAL_OR: return BinaryOp::LogicalOr;
        default:
            throw std::runtime_error("Invalid binary operator");
    }
}

UnaryOp Parser::tokenToUnaryOp(TokenType type) const {
    switch (type) {
        case TokenType::TOK_MINUS: return UnaryOp::Negate;
        case TokenType::TOK_NOT: return UnaryOp::Not;
        case TokenType::TOK_STAR: return UnaryOp::Deref;
        case TokenType::TOK_AMPERSAND: return UnaryOp::AddressOf;
        default:
            throw std::runtime_error("Invalid unary operator");
    }
}

std::unique_ptr<ExprAST> Parser::parseBinaryExpr(int minPrecedence) {
    auto left = parseUnaryExpr();
    
    while (true) {
        int precedence = getOperatorPrecedence(currentToken.type);
        if (precedence < minPrecedence) {
            break;
        }
        
        // Handle 'as' cast operator specially
        if (currentToken.type == TokenType::TOK_AS) {
            advance();
            auto targetType = parseType();
            left = std::make_unique<CastExpr>(std::move(left), std::move(targetType));
            continue;
        }
        
        TokenType opToken = currentToken.type;
        SourceLocation opLoc(currentToken.line, currentToken.column);
        advance();
        
        auto right = parseBinaryExpr(precedence + 1);
        
        BinaryOp op = tokenToBinaryOp(opToken);
        left = std::make_unique<BinaryExpr>(std::move(left), op, std::move(right), opLoc);
    }
    
    return left;
}

std::unique_ptr<ExprAST> Parser::parseUnaryExpr() {
    // Unary operators: -, !, *, &
    if (check(TokenType::TOK_MINUS) || check(TokenType::TOK_NOT) ||
        check(TokenType::TOK_STAR) || check(TokenType::TOK_AMPERSAND)) {
        TokenType opToken = currentToken.type;
        SourceLocation loc(currentToken.line, currentToken.column);
        advance();
        
        auto operand = parseUnaryExpr();
        UnaryOp op = tokenToUnaryOp(opToken);
        
        return std::make_unique<UnaryExpr>(op, std::move(operand), loc);
    }
    
    return parsePostfixExpr();
}

std::unique_ptr<ExprAST> Parser::parsePostfixExpr() {
    auto expr = parsePrimaryExpr();
    
    while (true) {
        if (check(TokenType::TOK_LEFT_PAREN)) {
            // Function call
            // Capture location of the opening paren BEFORE advancing
            SourceLocation callLoc(currentToken.line, currentToken.column);
            advance(); // match TOK_LEFT_PAREN
            
            std::vector<std::unique_ptr<ExprAST>> args;
            
            if (!check(TokenType::TOK_RIGHT_PAREN)) {
                do {
                    args.push_back(parseExpression());
                } while (match(TokenType::TOK_COMMA));
            }
            
            expect(TokenType::TOK_RIGHT_PAREN, "Expected ')' after function arguments");
            
            expr = std::make_unique<CallExpr>(std::move(expr), std::move(args), callLoc);
        } else if (match(TokenType::TOK_LEFT_BRACKET)) {
            // Array indexing
            auto index = parseExpression();
            expect(TokenType::TOK_RIGHT_BRACKET, "Expected ']' after array index");
            
            expr = std::make_unique<ArrayIndexExpr>(std::move(expr), std::move(index));
        } else if (match(TokenType::TOK_DOT)) {
            // Field access
            if (!check(TokenType::TOK_IDENTIFIER)) {
                reportError("Expected field name after '.'");
                break;
            }
            
            std::string fieldName = currentToken.lexeme;
            advance();
            
            expr = std::make_unique<FieldAccessExpr>(std::move(expr), fieldName);
        } else {
            break;
        }
    }
    
    return expr;
}

std::unique_ptr<ExprAST> Parser::parsePrimaryExpr() {
    // Number literal
    if (check(TokenType::TOK_NUMBER)) {
        uint64_t value = 0;
        try {
            if (currentToken.lexeme.substr(0, 2) == "0x" || currentToken.lexeme.substr(0, 2) == "0X") {
                if (currentToken.lexeme.length() <= 2) {
                    reportError("Invalid hexadecimal literal");
                    advance();
                    return nullptr;
                }
                value = std::stoull(currentToken.lexeme, nullptr, 16);
            } else {
                value = std::stoull(currentToken.lexeme);
            }
        } catch (const std::exception& e) {
            reportError("Invalid numeric literal: " + currentToken.lexeme);
            advance();
            return nullptr;
        }
        SourceLocation loc(currentToken.line, currentToken.column);
        advance();
        return std::make_unique<IntLiteralExpr>(value, loc);
    }
    
    // String literal
    if (check(TokenType::TOK_STRING)) {
        std::string value = currentToken.lexeme;
        SourceLocation loc(currentToken.line, currentToken.column);
        advance();
        return std::make_unique<StringLiteralExpr>(value, loc);
    }
    
    // Syscall
    if (match(TokenType::TOK_SYSCALL)) {
        return parseSyscall();
    }
    
    // Encrypted block
    if (match(TokenType::TOK_ENC)) {
        return parseEncBlock();
    }
    
    // Identifier or struct initialization
    if (check(TokenType::TOK_IDENTIFIER)) {
        std::string name = currentToken.lexeme;
        SourceLocation loc(currentToken.line, currentToken.column);
        advance();
        
        // Check for struct initialization
        if (check(TokenType::TOK_LEFT_BRACE)) {
            return parseStructInit(name);
        }
        
        return std::make_unique<IdentifierExpr>(name, loc);
    }
    
    // Array literal
    if (match(TokenType::TOK_LEFT_BRACKET)) {
        std::vector<std::unique_ptr<ExprAST>> elements;
        
        if (!check(TokenType::TOK_RIGHT_BRACKET)) {
            do {
                elements.push_back(parseExpression());
            } while (match(TokenType::TOK_COMMA));
        }
        
        expect(TokenType::TOK_RIGHT_BRACKET, "Expected ']' after array elements");
        return std::make_unique<ArrayLiteralExpr>(std::move(elements));
    }
    
    // Parenthesized expression
    if (match(TokenType::TOK_LEFT_PAREN)) {
        auto expr = parseExpression();
        expect(TokenType::TOK_RIGHT_PAREN, "Expected ')' after expression");
        return expr;
    }
    
    reportError("Expected expression, found " + currentToken.lexeme);
    advance(); // Consume the offending token
    return nullptr;
}

std::unique_ptr<ExprAST> Parser::parseSyscall() {
    expect(TokenType::TOK_LEFT_PAREN, "Expected '(' after 'syscall'");
    
    // Parse syscall number as an expression (can be a constant or literal)
    auto syscallNumExpr = parseExpression();
    if (!syscallNumExpr) {
        reportError("Expected syscall number expression");
        return nullptr;
    }
    
    std::vector<std::unique_ptr<ExprAST>> args;
    
    while (match(TokenType::TOK_COMMA)) {
        auto arg = parseExpression();
        if (arg) {
            args.push_back(std::move(arg));
        }
    }
    
    expect(TokenType::TOK_RIGHT_PAREN, "Expected ')' after syscall arguments");
    
    return std::make_unique<SyscallExpr>(std::move(syscallNumExpr), std::move(args));
}

std::unique_ptr<ExprAST> Parser::parseEncBlock() {
    expect(TokenType::TOK_LEFT_BRACE, "Expected '{' after 'enc'");
    
    if (!check(TokenType::TOK_STRING)) {
        reportError("Expected string literal in enc block");
        return nullptr;
    }
    
    std::string value = currentToken.lexeme;
    advance();
    
    expect(TokenType::TOK_RIGHT_BRACE, "Expected '}' after enc block");
    
    return std::make_unique<EncBlockExpr>(value);
}

std::unique_ptr<StructInitExpr> Parser::parseStructInit(const std::string& structName) {
    expect(TokenType::TOK_LEFT_BRACE, "Expected '{' for struct initialization");
    
    std::vector<std::pair<std::string, std::unique_ptr<ExprAST>>> fields;
    
    if (!check(TokenType::TOK_RIGHT_BRACE)) {
        do {
            if (!check(TokenType::TOK_IDENTIFIER)) {
                reportError("Expected field name");
                break;
            }
            
            std::string fieldName = currentToken.lexeme;
            advance();
            
            expect(TokenType::TOK_COLON, "Expected ':' after field name");
            
            auto value = parseExpression();
            
            fields.push_back({fieldName, std::move(value)});
        } while (match(TokenType::TOK_COMMA));
    }
    
    expect(TokenType::TOK_RIGHT_BRACE, "Expected '}' after struct fields");
    
    return std::make_unique<StructInitExpr>(structName, std::move(fields));
}

} // namespace lwanga
