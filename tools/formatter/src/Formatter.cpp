#include "Formatter.h"
#include "../../../compiler/include/AST.h"
#include <sstream>
#include <fstream>
#include <algorithm>
#include <ctime>

namespace lwanga {

FormatterConfig FormatterConfig::getDefault() {
    return FormatterConfig();
}

FormatterConfig FormatterConfig::loadFromFile(const std::string& path) {
    FormatterConfig config = getDefault();
    
    std::ifstream file(path);
    if (!file.is_open()) {
        return config;  // Return default if file doesn't exist
    }
    
    std::string line;
    std::string currentSection;
    
    while (std::getline(file, line)) {
        // Trim whitespace
        line.erase(0, line.find_first_not_of(" \t"));
        line.erase(line.find_last_not_of(" \t") + 1);
        
        // Skip empty lines and comments
        if (line.empty() || line[0] == '#') {
            continue;
        }
        
        // Parse section headers [section]
        if (line[0] == '[' && line.back() == ']') {
            currentSection = line.substr(1, line.length() - 2);
            continue;
        }
        
        // Parse key = value pairs
        size_t eqPos = line.find('=');
        if (eqPos == std::string::npos) {
            continue;
        }
        
        std::string key = line.substr(0, eqPos);
        std::string value = line.substr(eqPos + 1);
        
        // Trim key and value
        key.erase(0, key.find_first_not_of(" \t"));
        key.erase(key.find_last_not_of(" \t") + 1);
        value.erase(0, value.find_first_not_of(" \t"));
        value.erase(value.find_last_not_of(" \t") + 1);
        
        // Remove quotes from string values
        if (value.length() >= 2 && value.front() == '"' && value.back() == '"') {
            value = value.substr(1, value.length() - 2);
        }
        
        // Parse values based on section and key
        if (currentSection == "formatting") {
            if (key == "indent_width") {
                config.indentWidth = std::stoi(value);
            } else if (key == "max_line_width") {
                config.maxLineWidth = std::stoi(value);
            } else if (key == "brace_style") {
                config.braceStyle = value;
            } else if (key == "space_around_operators") {
                config.spaceAroundOperators = (value == "true");
            } else if (key == "trailing_commas") {
                config.trailingCommas = (value == "true");
            } else if (key == "sort_imports") {
                config.sortImports = (value == "true");
            }
        } else if (currentSection == "header") {
            if (key == "add_logo") {
                config.addLogo = (value == "true");
            } else if (key == "logo_style") {
                config.logoStyle = value;
            } else if (key == "include_metadata") {
                config.includeMetadata = (value == "true");
            } else if (key == "author") {
                config.author = value;
            } else if (key == "license") {
                config.license = value;
            }
        } else if (currentSection == "assembly") {
            if (key == "preserve_formatting") {
                config.preserveAsmFormatting = (value == "true");
            } else if (key == "indent_instructions") {
                config.indentAsmInstructions = (value == "true");
            }
        }
    }
    
    return config;
}

Formatter::Formatter(const FormatterConfig& cfg) : config(cfg) {}

std::string Formatter::indent(int level) const {
    return std::string(level * config.indentWidth, ' ');
}

std::string Formatter::getLogoAsciiArt() const {
    if (config.logoStyle == "minimal") {
        return "// ╦  ╦ ╦╔═╗╔╗╔╔═╗╔═╗\n"
               "// ║  ║║║╠═╣║║║║ ╦╠═╣\n"
               "// ╩═╝╚╩╝╩ ╩╝╚╝╚═╝╩ ╩\n";
    } else if (config.logoStyle == "block") {
        return "//  ██╗     ██╗    ██╗ █████╗ ███╗   ██╗ ██████╗  █████╗ \n"
               "//  ██║     ██║    ██║██╔══██╗████╗  ██║██╔════╝ ██╔══██╗\n"
               "//  ██║     ██║ █╗ ██║███████║██╔██╗ ██║██║  ███╗███████║\n"
               "//  ██║     ██║███╗██║██╔══██║██║╚██╗██║██║   ██║██╔══██║\n"
               "//  ███████╗╚███╔███╔╝██║  ██║██║ ╚████║╚██████╔╝██║  ██║\n"
               "//  ╚══════╝ ╚══╝╚══╝ ╚═╝  ╚═╝╚═╝  ╚═══╝ ╚═════╝ ╚═╝  ╚═╝\n";
    } else if (config.logoStyle == "banner") {
        return "// ┌─────────────────────────────────────────────────────────────┐\n"
               "// │                                                             │\n"
               "// │   ██╗     ██╗    ██╗ █████╗ ███╗   ██╗ ██████╗  █████╗      │\n"
               "// │   ██║     ██║    ██║██╔══██╗████╗  ██║██╔════╝ ██╔══██╗     │\n"
               "// │   ██║     ██║ █╗ ██║███████║██╔██╗ ██║██║  ███╗███████║     │\n"
               "// │   ██║     ██║███╗██║██╔══██║██║╚██╗██║██║   ██║██╔══██║     │\n"
               "// │   ███████╗╚███╔███╔╝██║  ██║██║ ╚████║╚██████╔╝██║  ██║     │\n"
               "// │   ╚══════╝ ╚══╝╚══╝ ╚═╝  ╚═╝╚═╝  ╚═══╝ ╚═════╝ ╚═╝  ╚═╝     │\n"
               "// │                                                             │\n"
               "// │   Security-Focused Systems Programming Language             │\n"
               "// │                                                             │\n"
               "// └─────────────────────────────────────────────────────────────┘\n";
    }
    return "";
}

std::string Formatter::generateHeader(const std::string& filename) const {
    std::stringstream ss;
    
    // Add logo if enabled
    if (config.addLogo && config.logoStyle != "none") {
        ss << getLogoAsciiArt();
        ss << "//\n";
    }
    
    // Add metadata if enabled
    if (config.includeMetadata) {
        ss << "// File: " << filename << "\n";
        
        if (!config.author.empty()) {
            ss << "// Author: " << config.author << "\n";
        }
        
        // Add current date
        time_t now = time(nullptr);
        char dateStr[20];
        strftime(dateStr, sizeof(dateStr), "%Y-%m-%d", localtime(&now));
        ss << "// Created: " << dateStr << "\n";
        
        if (!config.license.empty()) {
            ss << "// License: " << config.license << "\n";
        }
        
        ss << "//\n";
    }
    
    return ss.str();
}

std::string Formatter::formatType(Type* type) {
    if (!type) return "unknown";
    
    switch (type->kind) {
        case TypeKind::U8: return "u8";
        case TypeKind::U16: return "u16";
        case TypeKind::U32: return "u32";
        case TypeKind::U64: return "u64";
        case TypeKind::Ptr: return "ptr";
        case TypeKind::Register: return "register";
        case TypeKind::Array:
            return "[" + formatType(type->elementType.get()) + "; " + 
                   std::to_string(type->arraySize) + "]";
        case TypeKind::Struct:
            return type->structName;
        case TypeKind::FunctionPointer: {
            std::string result = "fn(";
            for (size_t i = 0; i < type->paramTypes.size(); i++) {
                if (i > 0) result += ", ";
                result += formatType(type->paramTypes[i].get());
            }
            result += ") -> " + formatType(type->returnType.get());
            return result;
        }
        default:
            return "unknown";
    }
}

std::string Formatter::formatBinaryOp(BinaryOp op) {
    std::string opStr;
    switch (op) {
        case BinaryOp::Add: opStr = "+"; break;
        case BinaryOp::Sub: opStr = "-"; break;
        case BinaryOp::Mul: opStr = "*"; break;
        case BinaryOp::Div: opStr = "/"; break;
        case BinaryOp::Mod: opStr = "%"; break;
        case BinaryOp::BitAnd: opStr = "&"; break;
        case BinaryOp::BitOr: opStr = "|"; break;
        case BinaryOp::BitXor: opStr = "^"; break;
        case BinaryOp::LeftShift: opStr = "<<"; break;
        case BinaryOp::RightShift: opStr = ">>"; break;
        case BinaryOp::Equal: opStr = "=="; break;
        case BinaryOp::NotEqual: opStr = "!="; break;
        case BinaryOp::Less: opStr = "<"; break;
        case BinaryOp::Greater: opStr = ">"; break;
        case BinaryOp::LessEqual: opStr = "<="; break;
        case BinaryOp::GreaterEqual: opStr = ">="; break;
        default: opStr = "?"; break;
    }
    
    if (config.spaceAroundOperators) {
        return " " + opStr + " ";
    }
    return opStr;
}

std::string Formatter::formatUnaryOp(UnaryOp op) {
    switch (op) {
        case UnaryOp::Negate: return "-";
        case UnaryOp::Not: return "!";
        case UnaryOp::Deref: return "*";
        case UnaryOp::AddressOf: return "&";
        default: return "?";
    }
}

std::string Formatter::formatExpression(ExprAST* expr) {
    if (!expr) return "";
    
    if (auto* intLit = dynamic_cast<IntLiteralExpr*>(expr)) {
        return std::to_string(intLit->value);
    } else if (auto* strLit = dynamic_cast<StringLiteralExpr*>(expr)) {
        return "\"" + strLit->value + "\"";
    } else if (auto* ident = dynamic_cast<IdentifierExpr*>(expr)) {
        return ident->name;
    } else if (auto* binary = dynamic_cast<BinaryExpr*>(expr)) {
        return formatExpression(binary->left.get()) + 
               formatBinaryOp(binary->op) + 
               formatExpression(binary->right.get());
    } else if (auto* unary = dynamic_cast<UnaryExpr*>(expr)) {
        return formatUnaryOp(unary->op) + formatExpression(unary->operand.get());
    } else if (auto* call = dynamic_cast<CallExpr*>(expr)) {
        std::string result = formatExpression(call->callee.get()) + "(";
        for (size_t i = 0; i < call->args.size(); i++) {
            if (i > 0) result += ", ";
            result += formatExpression(call->args[i].get());
        }
        result += ")";
        return result;
    } else if (auto* cast = dynamic_cast<CastExpr*>(expr)) {
        return formatExpression(cast->expr.get()) + " as " + formatType(cast->targetType.get());
    } else if (auto* arrayLit = dynamic_cast<ArrayLiteralExpr*>(expr)) {
        std::string result = "[";
        for (size_t i = 0; i < arrayLit->elements.size(); i++) {
            if (i > 0) result += ", ";
            result += formatExpression(arrayLit->elements[i].get());
        }
        result += "]";
        return result;
    } else if (auto* syscall = dynamic_cast<SyscallExpr*>(expr)) {
        std::string result = "syscall(";
        result += formatExpression(syscall->syscallNumber.get());
        for (const auto& arg : syscall->args) {
            result += ", " + formatExpression(arg.get());
        }
        result += ")";
        return result;
    } else if (auto* fieldAccess = dynamic_cast<FieldAccessExpr*>(expr)) {
        return formatExpression(fieldAccess->object.get()) + "." + fieldAccess->fieldName;
    } else if (auto* arrayIndex = dynamic_cast<ArrayIndexExpr*>(expr)) {
        return formatExpression(arrayIndex->array.get()) + "[" + 
               formatExpression(arrayIndex->index.get()) + "]";
    }
    
    return "/* unknown expression */";
}

std::string Formatter::formatStatement(StmtAST* stmt, int indentLevel) {
    if (!stmt) return "";
    
    std::stringstream ss;
    std::string ind = indent(indentLevel);
    
    if (auto* varDecl = dynamic_cast<VarDeclStmt*>(stmt)) {
        ss << ind << "let ";
        if (varDecl->isMutable) ss << "mut ";
        ss << varDecl->name << ": " << formatType(varDecl->type.get());
        if (varDecl->initializer) {
            ss << " = " << formatExpression(varDecl->initializer.get());
        }
        ss << ";\n";
    } else if (auto* assignment = dynamic_cast<AssignmentStmt*>(stmt)) {
        ss << ind << formatExpression(assignment->target.get()) 
           << " = " << formatExpression(assignment->value.get()) << ";\n";
    } else if (auto* exprStmt = dynamic_cast<ExprStmt*>(stmt)) {
        ss << ind << formatExpression(exprStmt->expr.get()) << ";\n";
    } else if (auto* returnStmt = dynamic_cast<ReturnStmt*>(stmt)) {
        ss << ind << "return";
        if (returnStmt->value) {
            ss << " " << formatExpression(returnStmt->value.get());
        }
        ss << ";\n";
    } else if (auto* ifStmt = dynamic_cast<IfStmt*>(stmt)) {
        ss << ind << "if (" << formatExpression(ifStmt->condition.get()) << ") {\n";
        ss << formatBlock(ifStmt->thenBlock, indentLevel + 1);
        ss << ind << "}";
        if (!ifStmt->elseBlock.empty()) {
            ss << " else {\n";
            ss << formatBlock(ifStmt->elseBlock, indentLevel + 1);
            ss << ind << "}";
        }
        ss << "\n";
    } else if (auto* whileStmt = dynamic_cast<WhileStmt*>(stmt)) {
        ss << ind << "while (" << formatExpression(whileStmt->condition.get()) << ") {\n";
        ss << formatBlock(whileStmt->body, indentLevel + 1);
        ss << ind << "}\n";
    } else if (auto* asmStmt = dynamic_cast<AsmStmt*>(stmt)) {
        ss << ind << "asm {\n";
        if (config.preserveAsmFormatting) {
            // Preserve user's assembly formatting
            ss << asmStmt->asmCode;
            if (!asmStmt->asmCode.empty() && asmStmt->asmCode.back() != '\n') {
                ss << "\n";
            }
        }
        ss << ind << "}\n";
    } else if (auto* unsafeBlock = dynamic_cast<UnsafeBlockStmt*>(stmt)) {
        ss << ind << "unsafe {\n";
        ss << formatBlock(unsafeBlock->body, indentLevel + 1);
        ss << ind << "}\n";
    }
    
    return ss.str();
}

std::string Formatter::formatBlock(const std::vector<std::unique_ptr<StmtAST>>& statements, int indentLevel) {
    std::stringstream ss;
    for (const auto& stmt : statements) {
        ss << formatStatement(stmt.get(), indentLevel);
    }
    return ss.str();
}

std::string Formatter::formatParameters(const std::vector<Parameter>& params) {
    std::string result;
    for (size_t i = 0; i < params.size(); i++) {
        if (i > 0) result += ", ";
        result += params[i].name + ": " + formatType(params[i].type.get());
    }
    return result;
}

std::string Formatter::formatFunction(FunctionAST* func) {
    std::stringstream ss;
    
    if (func->isNaked) {
        ss << "naked ";
    }
    
    ss << "fn " << func->name << "(" << formatParameters(func->params) << ")";
    
    if (func->returnType) {
        ss << " -> " << formatType(func->returnType.get());
    }
    
    ss << " {\n";
    ss << formatBlock(func->body, 1);
    ss << "}\n";
    
    return ss.str();
}

std::string Formatter::formatStruct(StructAST* structDef) {
    std::stringstream ss;
    
    if (structDef->isPacked) {
        ss << "packed ";
    }
    
    ss << "struct " << structDef->name << " {\n";
    
    for (size_t i = 0; i < structDef->fields.size(); i++) {
        const auto& field = structDef->fields[i];
        ss << indent(1) << field.name << ": " << formatType(field.type.get());
        if (i < structDef->fields.size() - 1 || config.trailingCommas) {
            ss << ",";
        }
        ss << "\n";
    }
    
    ss << "}\n";
    
    return ss.str();
}

std::string Formatter::formatConstant(ConstantAST* constant) {
    std::stringstream ss;
    ss << "const " << constant->name << ": " << formatType(constant->type.get());
    ss << " = " << formatExpression(constant->value.get()) << ";\n";
    return ss.str();
}

std::string Formatter::formatImport(ImportAST* import) {
    return "import \"" + import->path + "\";\n";
}

std::string Formatter::format(ProgramAST* program) {
    std::stringstream ss;
    
    // Sort imports if configured
    std::vector<ImportAST*> imports;
    for (const auto& import : program->imports) {
        imports.push_back(import.get());
    }
    
    if (config.sortImports) {
        std::sort(imports.begin(), imports.end(), [](ImportAST* a, ImportAST* b) {
            return a->path < b->path;
        });
    }
    
    // Format imports
    for (auto* import : imports) {
        ss << formatImport(import);
    }
    if (!imports.empty()) ss << "\n";
    
    // Format constants
    for (const auto& constant : program->constants) {
        ss << formatConstant(constant.get()) << "\n";
    }
    
    // Format structs
    for (const auto& structDef : program->structs) {
        ss << formatStruct(structDef.get()) << "\n";
    }
    
    // Format functions
    for (const auto& func : program->functions) {
        ss << formatFunction(func.get()) << "\n";
    }
    
    return ss.str();
}

} // namespace lwanga
