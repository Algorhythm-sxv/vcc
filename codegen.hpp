#include <fstream>
#include <string>

#include "parser.hpp"
#include <boost/format.hpp>

std::map<std::string, std::string> unary_ops = {
    {"-", "neg     %eax\n"},
    {"~", "not     %eax\n"},
    {"!", "cmpl    $0, %eax\n"
          "movl    $0, %eax\n"
          "sete    %al\n"}
};

std::string codegen_x86_expression(std::shared_ptr<Expression>& expression) {
    if (expression->exp_type == "unary_op") {
        boost::format out_format(
            "%s"
            "%s"
        );

        out_format % codegen_x86_expression(expression->expression) % unary_ops[expression->unaryop->op];
        return out_format.str();
    } else if (expression->exp_type == "const") {
        boost::format out_format(
            "movl    $%d, %%eax\n"
        );
        out_format % expression->value;
        return out_format.str();
    }
}

std::string codegen_x86_statement(std::shared_ptr<Statement>& statement) {
    boost::format out_format(
        "%s"
        "ret\n"
    );

    out_format % codegen_x86_expression(statement->expression);
    return out_format.str();
}

std::string codegen_x86_function(std::shared_ptr<Function>& function) {
    boost::format out_format(
        ".globl _%s\n_"
        "%s:\n"
    );

    out_format % function->id % function->id;
    std::string out = out_format.str();
    for (auto statement : function->statements) {
        out += codegen_x86_statement(statement);
    }

    return out;
}

std::string codegen_x86(Program& prog) {
    std::string out;

    for (auto function : prog.functions) {
        out += codegen_x86_function(function);
    }

    return out;
}
