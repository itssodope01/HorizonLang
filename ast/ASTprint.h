#pragma once

namespace ASTPrinter {
    std::string typeKindToString(Type::Kind kind);
    void printAST(const std::shared_ptr<ASTNode>& node, int indent = 0);
}
