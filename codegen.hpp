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

std::map<std::string, std::string> comparison_ops {
    {">",  "setg    %al\n"},
    {"<",  "setl    %al\n"},
    {">=", "setge   %al\n"},
    {"<=", "setle   %al\n"},
    {"!=", "setne   %al\n"},
    {"==", "sete    %al\n"}
} ;

std::map<std::string, std::string> binary_ops = {

};

std::string codegen_x86_program(Program& program);
std::string codegen_x86_function(std::shared_ptr<Function>& function);
std::string codegen_x86_statement(std::shared_ptr<Statement>& statement);
std::string codegen_x86_expression(std::shared_ptr<Expression>& exp);
std::string codegen_x86_expression_logic_and(std::shared_ptr<ExpressionLogicAnd>& exp);
std::string codegen_x86_expression_bitwise_or(std::shared_ptr<ExpressionBitwiseOr>& exp);
std::string codegen_x86_expression_bitwise_xor(std::shared_ptr<ExpressionBitwiseXor>& exp);
std::string codegen_x86_expression_bitwise_and(std::shared_ptr<ExpressionBitwiseAnd>& exp);
std::string codegen_x86_expression_equality(std::shared_ptr<ExpressionEquality>& exp);
std::string codegen_x86_expression_relational(std::shared_ptr<ExpressionRelational>& exp);
std::string codegen_x86_expression_shift(std::shared_ptr<ExpressionShift>& exp);
std::string codegen_x86_expression_add_sub(std::shared_ptr<ExpressionAddSub>& exp);
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

std::string codegen_x86_expression(std::shared_ptr<Expression>& exp) {
    if (exp->expressions.size() == 1) {
        return codegen_x86_expression_logic_and(exp->expressions.front());
    } else {
        std::string out = codegen_x86_expression_logic_and(
            exp->expressions.front());                   // asm for first operand
        std::string base_format_str = "push    %%eax\n"         // push first operand to stack
                                      "%s"                      // asm for second operand
                                      "pop     %%ecx\n"         // pop first operand to ecx
                                      "orl     %%ecx, %%eax\n"  // compute first operand | second operand, sets FLAGS
                                      "movl    $0, %%eax\n"     // zero-out eax
                                      "setne   %%al\n";         // set al to 1 iff first|second != 0

        auto expression = exp->expressions.begin();
        std::advance(expression, 1);
        for (int i=0; i<exp->expressions.size()-1; i++) {
            boost::format format_str(base_format_str);
            format_str % codegen_x86_expression_logic_and(*expression);
            out += format_str.str();
            std::advance(expression, 1);
        }
        return out;
    }
}

std::string codegen_x86_expression_logic_and(std::shared_ptr<ExpressionLogicAnd>& exp) {
    if (exp->expressions.size() == 1) {
        return codegen_x86_expression_bitwise_or(exp->expressions.front());
    } else {
        std::string out = codegen_x86_expression_bitwise_or(
            exp->expressions.front());                          // asm for first operand
        std::string base_format_str = "push    %%eax\n"         // push first operand to stack
                                      "%s"                      // asm for second operand
                                      "pop     %%ecx\n"         // pop first operand to ecx
                                      "cmpl    $0, %%ecx\n"     // compare first operand to 0
                                      "setne   %%cl\n"          // set cl to 1 iff first operand != 0
                                      "cmpl    $0, %%eax\n"     // compare second operand to 0
                                      "setne   %%al\n"          // set al to 1 iff second operand !=0
                                      "and     %%cl, %%al\n";   // set al to al & cl

        auto expression = exp->expressions.begin();
        std::advance(expression, 1);
        for (int i=0; i<exp->expressions.size()-1; i++) {
            boost::format format_str(base_format_str);
            format_str % codegen_x86_expression_bitwise_or(*expression);
            out += format_str.str();
            std::advance(expression, 1);
        }
        return out;
    }
}

std::string codegen_x86_expression_bitwise_or(std::shared_ptr<ExpressionBitwiseOr>& exp) {
    if (exp->expressions.size() == 1) {
        return codegen_x86_expression_bitwise_xor(exp->expressions.front());
    } else {
        std::string out = codegen_x86_expression_bitwise_xor(
            exp->expressions.front());                          // asm for first operand (stored in eax)
        std::string xor_format_str = "push    %%eax\n"          // push first operand to stack
                                     "%s"                       // asm for second operand (stored in eax)
                                     "pop     %%ecx\n"          // pop first operand to ecx
                                     "or      %%ecx, %%eax\n";  // calculate first operand | second operand and store in eax

        auto expression = exp->expressions.begin();
        std::advance(expression, 1);
        for (int i=0; i<exp->expressions.size()-1; i++) {
            boost::format format_str(xor_format_str);
            format_str % codegen_x86_expression_bitwise_xor(*expression);
            out += format_str.str();
            std::advance(expression, 1);
        }
        return out;
    }
}

std::string codegen_x86_expression_bitwise_xor(std::shared_ptr<ExpressionBitwiseXor>& exp) {
    if (exp->expressions.size() == 1) {
        return codegen_x86_expression_bitwise_and(exp->expressions.front());
    } else {
        std::string out = codegen_x86_expression_bitwise_and(
            exp->expressions.front());                          // asm for first operand (stored in eax)
        std::string xor_format_str = "push    %%eax\n"          // push first operand to stack
                                     "%s"                       // asm for second operand (stored in eax)
                                     "pop     %%ecx\n"          // pop first operand to ecx
                                     "xor     %%ecx, %%eax\n";  // calculate first operand ^ second operand and store in eax

        auto expression = exp->expressions.begin();
        std::advance(expression, 1);
        for (int i=0; i<exp->expressions.size()-1; i++) {
            boost::format format_str(xor_format_str);
            format_str % codegen_x86_expression_bitwise_and(*expression);
            out += format_str.str();
            std::advance(expression, 1);
        }
        return out;
    }
}

std::string codegen_x86_expression_bitwise_and(std::shared_ptr<ExpressionBitwiseAnd>& exp) {
    if (exp->expressions.size() == 1) {
        return codegen_x86_expression_equality(exp->expressions.front());
    } else {
        std::string out = codegen_x86_expression_equality(
            exp->expressions.front());                          // asm for first operand (stored in eax)
        std::string and_format_str = "push    %%eax\n"          // push first operand to stack
                                     "%s"                       // asm for second operand (stored in eax)
                                     "pop     %%ecx\n"          // pop first operand to ecx
                                     "and     %%eax, %%ecx\n";  // calculate first operand & second operand and store in eax

        auto expression = exp->expressions.begin();
        std::advance(expression, 1);
        for (int i=0; i<exp->expressions.size()-1; i++) {
            boost::format format_str(and_format_str);
            format_str % codegen_x86_expression_equality(*expression);
            out += format_str.str();
            std::advance(expression, 1);
        }
        return out;
    }
}

std::string codegen_x86_expression_equality(std::shared_ptr<ExpressionEquality>& exp) {
    if (exp->expressions.size() == 1) {
        return codegen_x86_expression_relational(exp->expressions.front());
    } else {
        std::string out = codegen_x86_expression_relational(
            exp->expressions.front());                          // asm for first operand
        std::string base_format_str = "push    %%eax\n"         // push first oeprand to stack
                                      "%s"                      // asm for second operand
                                      "pop     %%ecx\n"         // pop first operand to ecx
                                      "cmpl    %%eax, %%ecx\n"  // compare first operand to second operand and set FLAGS
                                      "movl    $0, %%eax\n"     // zero-out eax
                                      "%s";                     // asm for comparison

        auto expression = exp->expressions.begin();
        std::advance(expression, 1);
        for (auto op: exp->operators) {
            boost::format format_str(base_format_str);
            format_str % codegen_x86_expression_relational(*expression) % comparison_ops[op];
            out += format_str.str();
            std::advance(expression, 1);
        }
        return out;
    }
}

std::string codegen_x86_expression_relational(std::shared_ptr<ExpressionRelational>& exp) {
    if (exp->expressions.size() == 1) {
        return codegen_x86_expression_shift(exp->expressions.front());
    } else {
        std::string out = codegen_x86_expression_shift(
            exp->expressions.front());                          // asm for first operand
        std::string base_format_str = "push    %%eax\n"         // push first oeprand to stack
                                      "%s"                      // asm for second operand
                                      "pop     %%ecx\n"         // pop first operand to ecx
                                      "cmpl    %%eax, %%ecx\n"  // compares first operand to second operand and sets FLAGS
                                      "movl    $0, %%eax\n"     // zero-out eax
                                      "%s";                     // asm for comparison

        auto expression = exp->expressions.begin();
        std::advance(expression, 1);
        for (auto op: exp->operators) {
            boost::format format_str(base_format_str);
            format_str % codegen_x86_expression_shift(*expression) % comparison_ops[op];
            out += format_str.str();
            std::advance(expression, 1);
        }
        return out;
    }
}

std::string codegen_x86_expression_shift(std::shared_ptr<ExpressionShift>& exp) {
    if (exp->expressions.size() == 1) {
        return codegen_x86_expression_add_sub(exp->expressions.front());
    } else {
        std::string out = codegen_x86_expression_add_sub(
            exp->expressions.front());                          // asm for first operand (stored in eax)
        std::string shl_format_str = "push    %%eax\n"          // push first operand to stack
                                     "%s"                       // asm for second operand (stored in eax)
                                     "movl    %%eax, %%ecx\n"   // move second operand to ecx
                                     "pop     %%eax\n"          // pop first operand to eax
                                     "shl     %%cl, %%eax\n";   // shift eax left by cl, store in eax
        
        std::string shr_format_str = "push    %%eax\n"          // push first operand to stack
                                     "%s"                       // asm for second operand (stored in eax)
                                     "movl    %%eax, %%ecx\n"   // move second operand to ecx
                                     "pop     %%eax\n"          // pop first operand to eax
                                     "shr     %%cl, %%eax\n";   // shift eax right by cl, store in eax

        auto expression = exp->expressions.begin();
        std::advance(expression, 1);
        for (auto op: exp->operators) {
            if (op == "<<") {
                boost::format format_str(shl_format_str);
                format_str % codegen_x86_expression_add_sub(*expression);
                out += format_str.str();
            } else { // if (op == ">>") {
                boost::format format_str(shr_format_str);
                format_str % codegen_x86_expression_add_sub(*expression);
                out += format_str.str();
            }
            std::advance(expression, 1);
        }
        return out;
    }
}

std::string codegen_x86_expression_add_sub(std::shared_ptr<ExpressionAddSub>& exp) {
    if (exp->terms.size() == 1) {
        return codegen_x86_term(exp->terms.front());
    } else {
        std::string out = 
            codegen_x86_term(exp->terms.front());               // asm for first operand (stored in eax)
        std::string add_format_str = "push    %%eax\n"          // push first operand to stack
                                     "%s"                       // asm for second operand (stored in eax)
                                     "pop     %%ecx\n"          // pop first operand to ecx
                                     "addl    %%ecx, %%eax\n";  // add ecx and eax, store result in eax

        std::string sub_format_str = "push    %%eax\n"          // push first operand to stack
                                     "%s"                       // asm for second operand
                                     "movl    %%eax, %%ecx\n"   // move second operand to ecx
                                     "pop     %%eax\n"          // pop first operand to eax
                                     "subl    %%ecx, %%eax\n";  // compute eax - ecx and store in eax

        auto term = exp->terms.begin();
        std::advance(term, 1);
        for (auto op: exp->operators) {
            if (op == "+") {
                boost::format format_str(add_format_str);
                format_str % codegen_x86_term(*term);
                out += format_str.str();
            } else { // if (op == "-") {
                boost::format format_str(sub_format_str);
                format_str % codegen_x86_term(*term);
                out += format_str.str();
            }
            std::advance(term, 1);
        }
        return out;
    }
}

std::string codegen_x86_term(std::shared_ptr<Term>& term) {
    if (term->factors.size() == 1) {
        return codegen_x86_factor(term->factors.front());
    } else {
        std::string out = 
            codegen_x86_factor(term->factors.front());           // asm for first operand (stored in eax)
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
        
        std::string mod_format_str = div_format_str +
                                     "movl    %%edx, %%eax\n";  // moves the remainder stored in edx to eax

        auto factor = term->factors.begin();
        std::advance(factor, 1);
        for (auto op: term->operators) {
            if (op == "*") {
                boost::format format_str(mul_format_str);
                format_str % codegen_x86_factor(*factor);
                out += format_str.str();
            } else if (*op == "/") {
                boost::format format_str(div_format_str);
                format_str % codegen_x86_factor(*factor);
            } else { // if (*op == "%") {
                boost::format format_str(mod_format_str);
                format_str % codegen_x86_factor(*factor);
            }
            std::advance(factor, 1);
        }
        return out;
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

    } else { // if (factor->factor_type == "const") {
        boost::format out_format(
            "movl    $%d, %%eax\n"
        );
        out_format % factor->value;
        return out_format.str();
    }
}