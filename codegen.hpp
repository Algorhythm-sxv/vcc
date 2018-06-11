#include <fstream>
#include <string>

#include "parser.hpp"
#include <boost/format.hpp>

int global_counter = 0;

std::map<std::string, std::string> unary_ops = {
    {"-",  "    neg     %eax\n"},
    {"~",  "    not     %eax\n"},
    {"!",  "    cmpl    $0, %eax\n"
           "    movl    $0, %eax\n"
           "    sete    %al\n"},
    {"++", "    addl    $1, %eax\n"},
    {"--", "    subl    $1, %eax\n"}
};

std::map<std::string, std::string> comparison_ops {
    {">",  "    setg    %al\n"},
    {"<",  "    setl    %al\n"},
    {">=", "    setge   %al\n"},
    {"<=", "    setle   %al\n"},
    {"!=", "    setne   %al\n"},
    {"==", "    sete    %al\n"}
} ;

std::map<std::string, std::string> assignment_ops = {
    {"=",   ""},
    {"+=",  "    movl    %d(%%ebp), %%ecx\n"
            "    addl    %%ecx, %%eax\n"},
    {"-=",  "    movl    %%eax, %%ecx\n"
            "    movl    %d(%%ebp), %%eax\n"
            "    subl    %%ecx, %%eax\n"},
    {"*=",  "    movl    %d(%%ebp), %%ecx\n"
            "    imul    %%ecx, %%eax\n"},
    {"/=",  "    movl    %%eax, %%ecx\n"
            "    movl    $0, %%edx\n"
            "    movl    %d(%%ebp), %%eax\n"
            "    divl    %%ecx\n"},
    {"%=",  "    movl    %%eax, %%ecx\n"
            "    movl    $0, %%edx\n"
            "    movl    %d(%%ebp), %%eax\n"
            "    divl    %%ecx\n"
            "    movl    %%edx, %%eax\n"},
    {"<<=", "    movl    %%eax, %%ecx\n"
            "    movl    %d(%%ebp), %%eax\n"
            "    shl     %%cl, %%eax\n"},
    {">>=", "    movl    %%eax, %%ecx\n"
            "    movl    %d(%%ebp), %%eax\n"
            "    shr     %%cl, %%eax\n"},
    {"&=",  "    movl    %d(%%ebp), %%ecx\n"
            "    andl    %%ecx, %%eax\n"},
    {"^=",  "    movl    %d(%%ebp), %%ecx\n"
            "    xorl    %%ecx, %%eax\n"},
    {"|=",  "    movl    %d(%%ebp), %%ecx\n"
            "    orl     %%ecx, %%eax\n"}
};

std::string codegen_x86_program(Program& program);
std::string codegen_x86_function(std::shared_ptr<Function>& function);
std::string codegen_x86_statement(std::shared_ptr<Statement>& statement, 
                                  std::map<std::string, int>& local_addresses);
std::string codegen_x86_block_item(std::shared_ptr<BlockItem>& item, 
                                  std::map<std::string, int>& local_addresses);
std::string codegen_x86_expression(std::shared_ptr<Expression>& exp, 
                                   std::map<std::string, int>& local_addresses);
std::string codegen_x86_expression_assignment(std::shared_ptr<ExpressionAssignment>& exp, 
                                   std::map<std::string, int>& local_addresses);
std::string codegen_x86_expression_conditional(std::shared_ptr<ExpressionConditional>& exp, 
                                   std::map<std::string, int>& local_addresses);
std::string codegen_x86_expression_logic_or(std::shared_ptr<ExpressionLogicOr>& exp, 
                                            std::map<std::string, int>& local_addresses);
std::string codegen_x86_expression_logic_and(std::shared_ptr<ExpressionLogicAnd>& exp, 
                                             std::map<std::string, int>& local_addresses);
std::string codegen_x86_expression_bitwise_or(std::shared_ptr<ExpressionBitwiseOr>& exp, std::map<std::string, int>& local_addresses);
std::string codegen_x86_expression_bitwise_xor(std::shared_ptr<ExpressionBitwiseXor>& exp,
                                               std::map<std::string, int>& local_addresses);
std::string codegen_x86_expression_bitwise_and(std::shared_ptr<ExpressionBitwiseAnd>& exp, 
                                               std::map<std::string, int>& local_addresses);
std::string codegen_x86_expression_equality(std::shared_ptr<ExpressionEquality>& exp,
                                            std::map<std::string, int>& local_addresses);
std::string codegen_x86_expression_relational(std::shared_ptr<ExpressionRelational>& exp,
                                              std::map<std::string, int>& local_addresses);
std::string codegen_x86_expression_shift(std::shared_ptr<ExpressionShift>& exp, 
                                         std::map<std::string, int>& local_addresses);
std::string codegen_x86_expression_add_sub(std::shared_ptr<ExpressionAddSub>& exp, 
                                           std::map<std::string, int>& local_addresses);
std::string codegen_x86_term(std::shared_ptr<Term>& term, 
                             std::map<std::string, int>& local_addresses);
std::string codegen_x86_factor(std::shared_ptr<Factor>& factor, 
                               std::map<std::string, int>& local_addresses);

std::string codegen_x86(Program& prog) {
    std::string out;

    for (auto function : prog.functions) {
        out += codegen_x86_function(function);
    }

    return out;
}

std::string codegen_x86_function(std::shared_ptr<Function>& function) {
    std::map<std::string, int> locals;

    boost::format out_format(
        ".globl _%s\n_"
        "%s:\n"
        "    pushl   %%ebp\n"
        "    movl    %%esp, %%ebp\n"
    );

    out_format % function->id % function->id;
    std::string out = out_format.str();
    for (auto item : function->items) {
        out += codegen_x86_block_item(item, locals);
    }

    out += "    movl    $0, %eax\n"
           "    movl    %ebp, %esp\n"               // add function epilogue (ensures all function return eventually)
           "    pop     %ebp\n"
           "    ret\n";

    return out;
}

std::string codegen_x86_block_item(std::shared_ptr<BlockItem>& item, std::map<std::string, int>& local_addresses) {
    if (item->item_type == "statement") {
        return codegen_x86_statement(item->statement, local_addresses);
    } else { // if (item->item_type == "variable_declaration") {
        if (!local_addresses.empty()) {
            auto prev = local_addresses.rbegin()->first;                   // get the address of the previous variable
            local_addresses[item->var_id] = local_addresses[prev] - 4;     // add the address of the new variable
        } else {
            local_addresses[item->var_id] = -4;
        }
        if (item->initialised) {
            boost::format format_str(
                "%s"                         // asm for variable value (stored in eax)
                "    pushl   %%eax\n"        // push variable onto stack
            );
            format_str % codegen_x86_expression(item->init_exp, local_addresses);
            return format_str.str();
        } else {
            return std::string("    movl    -4(%esp), %esp\n");           // move the stack pointer past the new variable
        }
    }
}

std::string codegen_x86_statement(std::shared_ptr<Statement>& statement, std::map<std::string, int>& local_addresses) {
    if (statement->statement_type == "expression") {
        return codegen_x86_expression(statement->expression, local_addresses);
    } else if (statement->statement_type == "conditional") {
        std::string out = codegen_x86_expression(
            statement->expression, local_addresses);            // asm for condition (stored in eax)
        boost::format format_str("    cmpl    $0, %%eax\n"      // test value of condition
                                 "    je      _e%d\n"           // if condition is 0, jump to else code
                                 "%s"                           // asm for if code
                                 "    jmp     _end%d\n"         // jump past else code
                                 "_e%d:\n"                      // label for else code
                                 "%s"                           // asm for else code
                                 "_end%d:\n");                  // label for after else code
        
        format_str % global_counter
                   % codegen_x86_statement(statement->if_statement, local_addresses)
                   % global_counter
                   % global_counter
                   % codegen_x86_statement(statement->else_statement, local_addresses)
                   % global_counter;

        global_counter++;

        out += format_str.str();
        return out;
    } else { // if (statement->statement_type == "return") {
        boost::format out_format(
            "%s"
            "    movl    %%ebp, %%esp\n"
            "    pop     %%ebp\n"
            "    ret\n"
        );
        out_format % codegen_x86_expression(statement->expression, local_addresses);
        return out_format.str();
    }
}

std::string codegen_x86_expression(std::shared_ptr<Expression>& exp, std::map<std::string, int>& local_addresses) {
    std::string out = "";
    for (auto expression: exp->expressions) {
        out += codegen_x86_expression_assignment(expression, local_addresses);
    }
    return out;
}

std::string codegen_x86_expression_assignment(std::shared_ptr<ExpressionAssignment>& exp, std::map<std::string, int>& local_addresses) {
    if (exp->exp_type == "conditional") {
        return codegen_x86_expression_conditional(exp->expression, local_addresses);
    } else { // if (exp->exp_type == "assignment") {
        boost::format format_str(
            "%s"                                                // asm for variable value (stored in eax)
            "%s"                                                // asm for assignment operation
            "    movl    %%eax, %d(%%ebp)\n"                    // move the variable to the correct offset from the base pointer
        );
        boost::format operation(assignment_ops[exp->assign_type]);
        if (exp->assign_type != "=") {
            operation % local_addresses[exp->assign_id];
        }

        format_str % codegen_x86_expression_assignment(exp->assign_exp, local_addresses)
                   % operation.str()
                   % local_addresses[exp->assign_id];

        return format_str.str();
    }
}

std::string codegen_x86_expression_conditional(std::shared_ptr<ExpressionConditional>& exp, std::map<std::string, int>& local_addresses) {
    if (exp->exp_type == "logic_or") {
        return codegen_x86_expression_logic_or(exp->condition, local_addresses);
    } else { // if (exp->exp_type == "conditional") {
        std::string out = codegen_x86_expression_logic_or(
            exp->condition, local_addresses);                   // asm for condition (stored in eax)
        boost::format format_str("    cmpl    $0, %%eax\n"      // check value of condition
                                 "    je      _e%d\n"           // if the condition is 0, jump to else code
                                 "%s"                           // asm for if code
                                 "    jmp     _end%d\n"         // jump past else code
                                 "_e%d:\n"                      // label for else code
                                 "%s"                           // asm for else code
                                 "_end%d:\n");                  // label for end of else code

        format_str % global_counter
                   % codegen_x86_expression(exp->exp_true, local_addresses)
                   % global_counter
                   % global_counter
                   % codegen_x86_expression_conditional(exp->exp_false, local_addresses)
                   % global_counter;
        global_counter++;

        out += format_str.str();
        return out;
    }
}

std::string codegen_x86_expression_logic_or(std::shared_ptr<ExpressionLogicOr>& exp, std::map<std::string, int>& local_addresses) {
    if (exp->expressions.size() == 1) {
        return codegen_x86_expression_logic_and(exp->expressions.front(), local_addresses);
    } else {
        std::string out = codegen_x86_expression_logic_and(
            exp->expressions.front(), local_addresses);             // asm for first operand
        std::string base_format_str = "    pushl   %%eax\n"         // push first operand to stack
                                      "%s"                          // asm for second operand
                                      "    pop     %%ecx\n"         // pop first operand to ecx
                                      "    orl     %%ecx, %%eax\n"  // compute first operand | second operand, sets FLAGS
                                      "    movl    $0, %%eax\n"     // zero-out eax
                                      "    setne   %%al\n";         // set al to 1 iff first|second != 0

        auto expression = exp->expressions.begin();
        std::advance(expression, 1);
        for (int i=0; i<exp->expressions.size()-1; i++) {
            boost::format format_str(base_format_str);
            format_str % codegen_x86_expression_logic_and(*expression, local_addresses);
            out += format_str.str();
            std::advance(expression, 1);
        }
        return out;
    }
}

std::string codegen_x86_expression_logic_and(std::shared_ptr<ExpressionLogicAnd>& exp, std::map<std::string, int>& local_addresses) {
    if (exp->expressions.size() == 1) {
        return codegen_x86_expression_bitwise_or(exp->expressions.front(), local_addresses);
    } else {
        std::string out = codegen_x86_expression_bitwise_or(
            exp->expressions.front(), local_addresses);             // asm for first operand
        std::string base_format_str = "    pushl   %%eax\n"         // push first operand to stack
                                      "%s"                          // asm for second operand
                                      "    pop     %%ecx\n"         // pop first operand to ecx
                                      "    cmpl    $0, %%ecx\n"     // compare first operand to 0
                                      "    setne   %%cl\n"          // set cl to 1 iff first operand != 0
                                      "    cmpl    $0, %%eax\n"     // compare second operand to 0
                                      "    setne   %%al\n"          // set al to 1 iff second operand !=0
                                      "    andl    %%cl, %%al\n";   // set al to al & cl

        auto expression = exp->expressions.begin();
        std::advance(expression, 1);
        for (int i=0; i<exp->expressions.size()-1; i++) {
            boost::format format_str(base_format_str);
            format_str % codegen_x86_expression_bitwise_or(*expression, local_addresses);
            out += format_str.str();
            std::advance(expression, 1);
        }
        return out;
    }
}

std::string codegen_x86_expression_bitwise_or(std::shared_ptr<ExpressionBitwiseOr>& exp, std::map<std::string, int>& local_addresses) {
    if (exp->expressions.size() == 1) {
        return codegen_x86_expression_bitwise_xor(exp->expressions.front(), local_addresses);
    } else {
        std::string out = codegen_x86_expression_bitwise_xor(
            exp->expressions.front(), local_addresses);             // asm for first operand (stored in eax)
        std::string xor_format_str = "    pushl   %%eax\n"          // push first operand to stack
                                     "%s"                           // asm for second operand (stored in eax)
                                     "    pop     %%ecx\n"          // pop first operand to ecx
                                     "    orl     %%ecx, %%eax\n";  // calculate first operand | second operand and store in eax

        auto expression = exp->expressions.begin();
        std::advance(expression, 1);
        for (int i=0; i<exp->expressions.size()-1; i++) {
            boost::format format_str(xor_format_str);
            format_str % codegen_x86_expression_bitwise_xor(*expression, local_addresses);
            out += format_str.str();
            std::advance(expression, 1);
        }
        return out;
    }
}

std::string codegen_x86_expression_bitwise_xor(std::shared_ptr<ExpressionBitwiseXor>& exp, std::map<std::string, int>& local_addresses) {
    if (exp->expressions.size() == 1) {
        return codegen_x86_expression_bitwise_and(exp->expressions.front(), local_addresses);
    } else {
        std::string out = codegen_x86_expression_bitwise_and(
            exp->expressions.front(), local_addresses);             // asm for first operand (stored in eax)
        std::string xor_format_str = "    pushl   %%eax\n"          // push first operand to stack
                                     "%s"                           // asm for second operand (stored in eax)
                                     "    pop     %%ecx\n"          // pop first operand to ecx
                                     "    xorl    %%ecx, %%eax\n";  // calculate first operand ^ second operand and store in eax

        auto expression = exp->expressions.begin();
        std::advance(expression, 1);
        for (int i=0; i<exp->expressions.size()-1; i++) {
            boost::format format_str(xor_format_str);
            format_str % codegen_x86_expression_bitwise_and(*expression, local_addresses);
            out += format_str.str();
            std::advance(expression, 1);
        }
        return out;
    }
}

std::string codegen_x86_expression_bitwise_and(std::shared_ptr<ExpressionBitwiseAnd>& exp, std::map<std::string, int>& local_addresses) {
    if (exp->expressions.size() == 1) {
        return codegen_x86_expression_equality(exp->expressions.front(), local_addresses);
    } else {
        std::string out = codegen_x86_expression_equality(
            exp->expressions.front(), local_addresses);             // asm for first operand (stored in eax)
        std::string and_format_str = "    pushl   %%eax\n"          // push first operand to stack
                                     "%s"                           // asm for second operand (stored in eax)
                                     "    pop     %%ecx\n"          // pop first operand to ecx
                                     "    and     %%eax, %%ecx\n";  // calculate first operand & second operand and store in eax

        auto expression = exp->expressions.begin();
        std::advance(expression, 1);
        for (int i=0; i<exp->expressions.size()-1; i++) {
            boost::format format_str(and_format_str);
            format_str % codegen_x86_expression_equality(*expression, local_addresses);
            out += format_str.str();
            std::advance(expression, 1);
        }
        return out;
    }
}

std::string codegen_x86_expression_equality(std::shared_ptr<ExpressionEquality>& exp, std::map<std::string, int>& local_addresses) {
    if (exp->expressions.size() == 1) {
        return codegen_x86_expression_relational(exp->expressions.front(), local_addresses);
    } else {
        std::string out = codegen_x86_expression_relational(
            exp->expressions.front(), local_addresses);             // asm for first operand
        std::string base_format_str = "    pushl   %%eax\n"         // push first oeprand to stack
                                      "%s"                          // asm for second operand
                                      "    pop     %%ecx\n"         // pop first operand to ecx
                                      "    cmpl    %%eax, %%ecx\n"  // compare first operand to second operand and set FLAGS
                                      "    movl    $0, %%eax\n"     // zero-out eax
                                      "%s";                         // asm for comparison

        auto expression = exp->expressions.begin();
        std::advance(expression, 1);
        for (auto op: exp->operators) {
            boost::format format_str(base_format_str);
            format_str % codegen_x86_expression_relational(*expression, local_addresses) % comparison_ops[op];
            out += format_str.str();
            std::advance(expression, 1);
        }
        return out;
    }
}

std::string codegen_x86_expression_relational(std::shared_ptr<ExpressionRelational>& exp, std::map<std::string, int>& local_addresses) {
    if (exp->expressions.size() == 1) {
        return codegen_x86_expression_shift(exp->expressions.front(), local_addresses);
    } else {
        std::string out = codegen_x86_expression_shift(
            exp->expressions.front(), local_addresses);             // asm for first operand
        std::string base_format_str = "    pushl   %%eax\n"         // push first oeprand to stack
                                      "%s"                          // asm for second operand
                                      "    pop     %%ecx\n"         // pop first operand to ecx
                                      "    cmpl    %%eax, %%ecx\n"  // compares first operand to second operand and sets FLAGS
                                      "    movl    $0, %%eax\n"     // zero-out eax
                                      "%s";                         // asm for comparison

        auto expression = exp->expressions.begin();
        std::advance(expression, 1);
        for (auto op: exp->operators) {
            boost::format format_str(base_format_str);
            format_str % codegen_x86_expression_shift(*expression, local_addresses) % comparison_ops[op];
            out += format_str.str();
            std::advance(expression, 1);
        }
        return out;
    }
}

std::string codegen_x86_expression_shift(std::shared_ptr<ExpressionShift>& exp, std::map<std::string, int>& local_addresses) {
    if (exp->expressions.size() == 1) {
        return codegen_x86_expression_add_sub(exp->expressions.front(), local_addresses);
    } else {
        std::string out = codegen_x86_expression_add_sub(
            exp->expressions.front(), local_addresses);             // asm for first operand (stored in eax)
        std::string shl_format_str = "    pushl   %%eax\n"          // push first operand to stack
                                     "%s"                           // asm for second operand (stored in eax)
                                     "    movl    %%eax, %%ecx\n"   // move second operand to ecx
                                     "    pop     %%eax\n"          // pop first operand to eax
                                     "    shl     %%cl, %%eax\n";   // shift eax left by cl, store in eax
        
        std::string shr_format_str = "    pushl   %%eax\n"          // push first operand to stack
                                     "%s"                           // asm for second operand (stored in eax)
                                     "    movl    %%eax, %%ecx\n"   // move second operand to ecx
                                     "    pop     %%eax\n"          // pop first operand to eax
                                     "    shr     %%cl, %%eax\n";   // shift eax right by cl, store in eax

        auto expression = exp->expressions.begin();
        std::advance(expression, 1);
        for (auto op: exp->operators) {
            if (op == "<<") {
                boost::format format_str(shl_format_str);
                format_str % codegen_x86_expression_add_sub(*expression, local_addresses);
                out += format_str.str();
            } else { // if (op == ">>") {
                boost::format format_str(shr_format_str);
                format_str % codegen_x86_expression_add_sub(*expression, local_addresses);
                out += format_str.str();
            }
            std::advance(expression, 1);
        }
        return out;
    }
}

std::string codegen_x86_expression_add_sub(std::shared_ptr<ExpressionAddSub>& exp, std::map<std::string, int>& local_addresses) {
    if (exp->terms.size() == 1) {
        return codegen_x86_term(exp->terms.front(), local_addresses);
    } else {
        std::string out = 
            codegen_x86_term(exp->terms.front(), 
                             local_addresses);                      // asm for first operand (stored in eax)
        std::string add_format_str = "    pushl   %%eax\n"          // push first operand to stack
                                     "%s"                           // asm for second operand (stored in eax)
                                     "    pop     %%ecx\n"          // pop first operand to ecx
                                     "    addl    %%ecx, %%eax\n";  // add ecx and eax, store result in eax

        std::string sub_format_str = "    pushl   %%eax\n"          // push first operand to stack
                                     "%s"                           // asm for second operand
                                     "    movl    %%eax, %%ecx\n"   // move second operand to ecx
                                     "    pop     %%eax\n"          // pop first operand to eax
                                     "    subl    %%ecx, %%eax\n";  // compute eax - ecx and store in eax

        auto term = exp->terms.begin();
        std::advance(term, 1);
        for (auto op: exp->operators) {
            if (op == "+") {
                boost::format format_str(add_format_str);
                format_str % codegen_x86_term(*term, local_addresses);
                out += format_str.str();
            } else { // if (op == "-") {
                boost::format format_str(sub_format_str);
                format_str % codegen_x86_term(*term, local_addresses);
                out += format_str.str();
            }
            std::advance(term, 1);
        }
        return out;
    }
}

std::string codegen_x86_term(std::shared_ptr<Term>& term, std::map<std::string, int>& local_addresses) {
    if (term->factors.size() == 1) {
        return codegen_x86_factor(term->factors.front(), local_addresses);
    } else {
        std::string out = 
            codegen_x86_factor(term->factors.front(),
                               local_addresses);                    // asm for first operand (stored in eax)
        std::string mul_format_str = "    pushl   %%eax\n"          // push first operand to stack
                                     "%s"                           // asm for second operand (stored in eax)
                                     "    pop     %%ecx\n"          // pop first operand to ecx
                                     "    imul    %%ecx, %%eax\n";  // asm for operation

        std::string div_format_str = "    pushl   %%eax\n"          // push first operand to stack
                                     "%s"                           // asm for second operand (stored in eax)
                                     "    movl    %%eax, %%ecx\n"   // move second operand to ecx
                                     "    pop     %%eax\n"          // pop first operand to eax
                                     "    movl    $0, %%edx\n"      // zero out edx
                                     "    idivl   %%ecx\n";         // compute [edx:eax]/ecx, quotient goes to eax, remainder to edx
        
        std::string mod_format_str = div_format_str +
                                     "    movl    %%edx, %%eax\n";  // moves the remainder stored in edx to eax

        auto factor = term->factors.begin();
        std::advance(factor, 1);
        for (auto op: term->operators) {
            if (op == "*") {
                boost::format format_str(mul_format_str);
                format_str % codegen_x86_factor(*factor, local_addresses);
                out += format_str.str();
            } else if (op == "/") {
                boost::format format_str(div_format_str);
                format_str % codegen_x86_factor(*factor, local_addresses);
            } else { // if (*op == "%") {
                boost::format format_str(mod_format_str);
                format_str % codegen_x86_factor(*factor, local_addresses);
            }
            std::advance(factor, 1);
        }
        return out;
    }
}

std::string codegen_x86_factor(std::shared_ptr<Factor>& factor, std::map<std::string, int>& local_addresses) {
    if (factor->factor_type == "bracket_expression") {
        return codegen_x86_expression(factor->expression, local_addresses);

    } else if (factor->factor_type == "unary_op") {
        boost::format out_format(
            "%s"
            "%s"
        );
        out_format % codegen_x86_factor(factor->factor, local_addresses) % unary_ops[factor->unaryop];
        return out_format.str();

    } else if (factor->factor_type == "variable") {
        boost::format format_str(
            "    movl    %d(%%ebp), %%eax\n"                // move the variable from the stack to eax
        );
        std::string id;
        try {
            id = local_addresses[factor->id];
        } catch(...) {
            throw std::runtime_error("identifier '" + id + "' not declared in this scope\n");
        }
        format_str % local_addresses[factor->id];
        return format_str.str();

    } else { // if (factor->factor_type == "const") {
        boost::format out_format(
            "    movl    $%d, %%eax\n"
        );
        out_format % factor->value;
        return out_format.str();
    }
}