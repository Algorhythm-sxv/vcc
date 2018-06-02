#include <fstream>
#include <string>

#include "parser.hpp"
#include <boost/format.hpp>


std::string codegen_x86_expression(Expression expression) {
    return std::to_string(expression.value);
}

std::string codegen_x86_statement(Statement statement) {
    boost::format out_format(
        "movl   $%d, %%eax\n"
        "ret\n"
    );

    out_format % codegen_x86_expression(statement.expression);
    std::string out = out_format.str();
    return out;
}

std::string codegen_x86_function(Function function) {
    boost::format out_format(
        ".globl _%s\n_"
        "%s:\n"
    );

    out_format % function.id % function.id;
    std::string out = out_format.str();
    for (Statement statement : function.statements) {
        out += codegen_x86_statement(statement);
    }

    return out;
}

std::string codegen_x86(Program prog) {
    std::string out;

    for (Function function : prog.functions) {
        out += codegen_x86_function(function);
    }

    return out;
}
