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

std::map<std::string, std::string> binary_ops = {

};

std::string codegen_x86_program(Program& program);
std::string codegen_x86_function(std::shared_ptr<Function>& function);
std::string codegen_x86_statement(std::shared_ptr<Statement>& statement);
std::string codegen_x86_expression(std::shared_ptr<Expression>& expression);
std::string codegen_x86_term(std::shared_ptr<Term>& term);
std::string codegen_x86_factor(std::shared_ptr<Factor>& factor);

std::string codegen_x86(Program& prog) {
    std::string out;

    for (auto function : prog.functions) {
        out += codegen_x86_function(function);
    }

    return out;
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

std::string codegen_x86_statement(std::shared_ptr<Statement>& statement) {
    boost::format out_format(
        "%s"
        "ret\n"
    );

    out_format % codegen_x86_expression(statement->expression);
    return out_format.str();
}

std::string codegen_x86_expression(std::shared_ptr<Expression>& expression) {
    if (expression->terms.size() == 1) {
        return codegen_x86_term(expression->terms.front());
    } else {
        std::string format_str = "%s";                          // asm for first operand (stored in eax)
        std::string add_format_str = "push    %%eax\n"          // push first operand to stack
                                     "%s"                       // asm for second operand
                                     "pop     %%ecx\n"          // pop first operand to ecx
                                     "addl    %%ecx, %%eax\n";  // add ecx and eax, store result in eax

        std::string sub_format_str = "push    %%eax\n"          // push first operand to stack
                                     "%s"                       // asm for second operand
                                     "movl    %%eax, %%ecx\n"   // move second operand to ecx
                                     "pop     %%eax\n"          // pop first operand to eax
                                     "subl    %%ecx, %%eax\n";  // compute eax - ecx and store in eax

        for (auto op: expression->operators) {
            if (op == "+") {
                format_str += add_format_str;
            } else { // if (op == "-") {
                format_str += sub_format_str;
            }
        }

        boost::format out_format(format_str);

        for (auto term: expression->terms){
            out_format % codegen_x86_term(term);
        }

        return out_format.str();
    }
}

std::string codegen_x86_term(std::shared_ptr<Term>& term) {
    if (term->factors.size() == 1) {
        return codegen_x86_factor(term->factors.front());
    } else {
        std::string format_str = "%s";                          // asm for first operand (stored in eax)
        std::string mul_format_str = "push    %%eax\n"          // push first operand to stack
                                     "%s"                       // asm for second operand (stored in eax)
                                     "pop     %%ecx\n"          // pop first operand to ecx
                                     "imul    %%ecx, %%eax\n";  // asm for operation

        std::string div_format_str = "push    %%eax\n"          // push first operand to stack
                                     "%s"                       // asm for second operand (stored in eax)
                                     "movl    %%eax, %%ecx\n"   // move second operand to ecx
                                     "pop     %%eax\n"          // pop first operand to eax
                                     "movl    $0, %%edx\n"      // zero out edx
                                     "idivl   %%ecx\n";         // compute [edx:eax]/ecx, quotient goes to eax, remainder to edx

        for (auto op: term->operators) {
            if (op == "*") {
                format_str += mul_format_str;
            } else { // if (*op == "/") {
                format_str += div_format_str;
            }
        }

        boost::format out_format(format_str);

        for (auto factor: term->factors){
            out_format % codegen_x86_factor(factor);
        }

        return out_format.str();
    }
}

std::string codegen_x86_factor(std::shared_ptr<Factor>& factor) {
    if (factor->factor_type == "bracket_expression") {
        return codegen_x86_expression(factor->expression);

    } else if (factor->factor_type == "unary_op") {
        boost::format out_format(
            "%s"
            "%s"
        );

        out_format % codegen_x86_factor(factor->factor) % unary_ops[factor->unaryop];
        return out_format.str();

    } else if (factor->factor_type == "const") {
        boost::format out_format(
            "movl    $%d, %%eax\n"
        );
        out_format % factor->value;
        return out_format.str();
    }
}