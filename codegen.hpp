#include <fstream>
#include <set>
#include <string>

#include "parser.hpp"
#include <boost/format.hpp>

int global_counter = 0; // counter for jump labels
std::map<std::string, std::shared_ptr<Function>> global_functions;

std::map<std::string, std::string> unary_ops = {
    {"-",  "    neg     %eax\n"},
    {"~",  "    not     %eax\n"},
    {"!",  "    cmpl    $0, %eax\n"
           "    movl    $0, %eax\n"
           "    sete    %al\n"},
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

std::string codegen_x86_program(Program program);
std::string codegen_x86_function(std::shared_ptr<Function> function);
std::string codegen_x86_block_item(std::shared_ptr<BlockItem> item, 
                                   std::map<std::string, int>& local_addresses,
                                   std::set<std::string>& current_scope,
                                   int& stack_index,
                                   int& inner_loop_stack_index,
                                   int inner_loop_count);
std::string codegen_x86_declaration_list(std::shared_ptr<DeclarationList> declist,
                                         std::map<std::string, int>& local_addresses,
                                         std::set<std::string>& current_scope,
                                         int& stack_index,
                                         int& inner_loop_stack_index);
std::string codegen_x86_declaration(std::shared_ptr<Declaration> item,
                                    std::map<std::string, int>& local_addresses,
                                    std::set<std::string>& current_scope,
                                    int& stack_index,
                                    int& inner_loop_stack_index);
std::string codegen_x86_statement(std::shared_ptr<Statement> stat, 
                                  std::map<std::string, int> local_addresses,
                                  std::set<std::string>& current_scope,
                                  int stack_index,
                                  int& inner_loop_stack_index,
                                  int inner_loop_count);
std::string codegen_x86_expression_comma(std::shared_ptr<ExpressionComma> exp, 
                                         std::map<std::string, int> local_addresses);
std::string codegen_x86_expression_assignment(std::shared_ptr<ExpressionAssignment> exp, 
                                              std::map<std::string, int> local_addresses);
std::string codegen_x86_expression_conditional(std::shared_ptr<ExpressionConditional> exp, 
                                               std::map<std::string, int> local_addresses);
std::string codegen_x86_expression_logic_or(std::shared_ptr<ExpressionLogicOr> exp, 
                                            std::map<std::string, int> local_addresses);
std::string codegen_x86_expression_logic_and(std::shared_ptr<ExpressionLogicAnd> exp, 
                                             std::map<std::string, int> local_addresses);
std::string codegen_x86_expression_bitwise_or(std::shared_ptr<ExpressionBitwiseOr> exp, 
                                              std::map<std::string, int> local_addresses);
std::string codegen_x86_expression_bitwise_xor(std::shared_ptr<ExpressionBitwiseXor> exp,
                                               std::map<std::string, int> local_addresses);
std::string codegen_x86_expression_bitwise_and(std::shared_ptr<ExpressionBitwiseAnd> exp, 
                                               std::map<std::string, int> local_addresses);
std::string codegen_x86_expression_equality(std::shared_ptr<ExpressionEquality> exp,
                                            std::map<std::string, int> local_addresses);
std::string codegen_x86_expression_relational(std::shared_ptr<ExpressionRelational> exp,
                                              std::map<std::string, int> local_addresses);
std::string codegen_x86_expression_shift(std::shared_ptr<ExpressionShift> exp, 
                                         std::map<std::string, int> local_addresses);
std::string codegen_x86_expression_add(std::shared_ptr<ExpressionAdd> exp, 
                                       std::map<std::string, int> local_addresses);
std::string codegen_x86_expression_mult(std::shared_ptr<ExpressionMult> exp, 
                                        std::map<std::string, int> local_addresses);
std::string codegen_x86_expression_unary(std::shared_ptr<ExpressionUnary> exp, 
                                         std::map<std::string, int> local_addresses);
std::string codegen_x86_expression_postfix(std::shared_ptr<ExpressionPostfix> exp,
                                           std::map<std::string, int> local_addresses);

std::string codegen_x86(Program prog) {
    std::string out;

    for (auto function : prog.functions) {
        out += codegen_x86_function(function);
    }

    return out;
}

std::string codegen_x86_function(std::shared_ptr<Function> function) {
    std::map<std::string, int> locals;
    std::set<std::string> current_scope;
    int inner_loop_count = -1;
    int stack_index = -4;   // stack offset for variables
    int inner_loop_stack_index = 0; // stack position of inner loop scope for continue and break

    if (global_functions.count(function->id)) {
        if (global_functions[function->id]->defined && function->defined) {
            throw std::runtime_error("multiple definitions for function: " + function->id + "\n");
        }
        if (function->params.size() != global_functions[function->id]->params.size()) {
            throw std::runtime_error("conflicting types for function: " + function->id + "\n");
        }
        auto params = function->params.begin();
        for (auto params1: global_functions[function->id]->params) {
            if (!(params->first == params1.first)) {
                throw std::runtime_error("conflicting types for function: " + function->id + "\n");
            }
            std::advance(params, 1);
        }
    }
    global_functions[function->id] = function;

    if (!function->defined) {
        return std::string("");
    }

    if (function->params.size()) {
        // iterating forwards over the arguments allows us to 
        // place their positions relative to ebp in reverse order
        auto params = function->params.begin();
        for (int i=0; i<function->params.size(); i++) {
            locals[params->second] = 8 + 4*i;
            current_scope.insert(params->second);
            std::advance(params, 1);
        }
    }
    boost::format out_format(
        ".globl _%s\n_"
        "%s:\n"
        "    pushl   %%ebp\n"
        "    movl    %%esp, %%ebp\n"
    );
    out_format % function->id % function->id;
    std::string out = out_format.str();
    for (auto item : function->items) {
        out += codegen_x86_block_item(item, locals, current_scope, stack_index, inner_loop_stack_index, inner_loop_count);
    }
    out += "    movl    $0, %eax\n"
           "    movl    %ebp, %esp\n"               // add function epilogue (ensures all function return eventually)
           "    pop     %ebp\n"
           "    ret\n";

    return out;
}

std::string codegen_x86_block_item(std::shared_ptr<BlockItem> item, 
                                   std::map<std::string, int>& local_addresses, 
                                   std::set<std::string>& current_scope, 
                                   int& stack_index,
                                   int& inner_loop_stack_index,
                                   int inner_loop_count) {
    if (item->item_type == "statement") {
        return codegen_x86_statement(item->statement, local_addresses, current_scope, stack_index, inner_loop_stack_index, inner_loop_count);
    } else { // if (item->item_type == "variable_declaration") {
        return codegen_x86_declaration_list(item->declaration_list, local_addresses, current_scope, stack_index, inner_loop_stack_index);
    }
}

std::string codegen_x86_declaration_list(std::shared_ptr<DeclarationList> declist,
                                         std::map<std::string, int>& local_addresses,
                                         std::set<std::string>& current_scope,
                                         int& stack_index,
                                         int& inner_loop_stack_index) {
    if (declist->declarations.size() == 1) {
        return codegen_x86_declaration(declist->declarations.front(), local_addresses, current_scope, stack_index, inner_loop_stack_index);
    } else {
        std::string out = "";
        for (auto decl: declist->declarations) {
            out += codegen_x86_declaration(decl, local_addresses, current_scope, stack_index, inner_loop_stack_index);
        }
        return out;
    }
}

std::string codegen_x86_declaration(std::shared_ptr<Declaration> decl,
                                              std::map<std::string, int>& local_addresses,
                                              std::set<std::string>& current_scope,
                                              int& stack_index,
                                              int& inner_loop_stack_index) {
    if (current_scope.count(decl->var_id)) {
        throw std::runtime_error("variable '" + decl->var_id + "' already declared in this scope\n");
    }
    local_addresses[decl->var_id] = stack_index;
    current_scope.insert(decl->var_id);
    stack_index -= 4;

    if (decl->initialised) {
        boost::format format_str(
            "%s"                         // asm for variable value (stored in eax)
            "    pushl   %%eax\n"        // push variable onto stack
        );
        format_str % codegen_x86_expression_assignment(decl->init_exp, local_addresses);
        return format_str.str();
    } else {
        return std::string("    subl    $4, %esp\n");       // move the stack pointer past the new variable
    }
}

std::string codegen_x86_statement(std::shared_ptr<Statement> stat, 
                                  std::map<std::string, int> local_addresses, 
                                  std::set<std::string>& current_scope,
                                  int stack_index,
                                  int& inner_loop_stack_index,
                                  int inner_loop_count) {
    if (stat->statement_type == "expression") {
        return codegen_x86_expression_comma(stat->expression1, local_addresses);
    } else if (stat->statement_type == "conditional") {
        std::string out = codegen_x86_expression_comma(
            stat->expression1, local_addresses);           // asm for condition (stored in eax)
        boost::format out_format("    cmpl    $0, %%eax\n"      // test value of condition
                                 "    je      _e%d\n"           // if condition is 0, jump to else code
                                 "%s"                           // asm for if code
                                 "    jmp     _end%d\n"         // jump past else code
                                 "_e%d:\n"                      // label for else code
                                 "%s"                           // asm for else code
                                 "_end%d:\n");                  // label for after else code
        
        int local_counter = global_counter;
        global_counter++;
        out_format % local_counter
                   % codegen_x86_statement(stat->statement1, local_addresses, current_scope, stack_index, inner_loop_stack_index, inner_loop_count)
                   % local_counter
                   % local_counter
                   % codegen_x86_statement(stat->statement2, local_addresses, current_scope, stack_index, inner_loop_stack_index, inner_loop_count)
                   % local_counter;

        out += out_format.str();
        return out;
    } else if (stat->statement_type.find("for") == 0) {
        std::string out = "";
        std::set<std::string> current_scope;
        int local_counter = global_counter;
        int inner_loop_count = global_counter;
        global_counter++;
        
        if (stat->statement_type == "for_declaration") {
            out = codegen_x86_block_item(stat->items.front(), 
                                         local_addresses, 
                                         current_scope, 
                                         stack_index, 
                                         inner_loop_stack_index,
                                         inner_loop_count);                 // asm for init declaration
        } else { // if (stat->statement_type == "for_expression") {
            out = codegen_x86_expression_comma(
                stat->expression1, local_addresses);                   // asm for init expression
        }
        int inner_loop_stack_index = stack_index; // save stack position of the loop body scope for continue and break statements
        boost::format out_format("_cond%d:\n"                               // label for loop condition
                                 "%s"                                       // asm for condition expression (stored in eax)
                                 "    cmpl    $0, %%eax\n"                  // compare condition to 0
                                 "    je      _end%d\n"                     // if the condition is false, jump to the end of the loop
                                 "%s"                                       // asm for loop body statement
                                 "_cont%d:\n"                               // label for continue statement
                                 "%s"                                       // asm for post expression
                                 "    jmp    _cond%d\n"                     // jump to loop condition
                                 "_end%d:\n"                                // label for end of loop
                                );
        out_format % local_counter
                   % codegen_x86_expression_comma(stat->expression2, local_addresses)
                   % local_counter
                   % codegen_x86_statement(stat->statement1, local_addresses, current_scope, stack_index, inner_loop_stack_index, inner_loop_count)
                   % local_counter
                   % codegen_x86_expression_comma(stat->expression3, local_addresses)
                   % local_counter
                   % local_counter;

        out += out_format.str();

        if (stat->statement_type == "for_declaration") {
            boost::format dealloc("    addl    $%d, %%esp\n");              // deallocate variables from header scope
            dealloc % (4*current_scope.size());
            out += dealloc.str();
        }

        return out;
    } else if (stat->statement_type == "while" || stat->statement_type == "do") {
        std::string out = "";
        int local_counter = global_counter;
        int inner_loop_count = global_counter;
        global_counter++;
        int inner_loop_stack_index = stack_index; // save stack position of the loop body scope for continue and break statements
        
        std::string cond = codegen_x86_expression_comma(stat->expression1, local_addresses);
        std::string body = codegen_x86_statement(stat->statement1, local_addresses, current_scope, stack_index, inner_loop_stack_index, inner_loop_count);

        std::string out_format = "";
        boost::format loop_format;

        if (stat->statement_type == "while") {
            out_format = "_cond%d:\n"                                       // label for loop condition
                         "%s"                                               // asm for condition expression (stored in eax)
                         "    cmpl    $0, %%eax\n"                          // compare condition to 0
                         "    je      _end%d\n"                             // if condition is false, jump to end of loop
                         "%s"                                               // asm for loop body statement
                         "_cont%d:\n"                                       // label for continue statement
                         "    jmp     _cond%d\n"                            // jump to condition
                         "_end%d:\n";                                       // label for end of loop
            loop_format = boost::format(out_format);
            loop_format % local_counter
                        % cond
                        % local_counter
                        % body
                        % local_counter
                        % local_counter
                        % local_counter;
        } else { // if (stat->statement_type == "do") {
            out_format = "_start%d:\n"                                      // label for loop start
                         "%s"                                               // asm for loop body statement
                         "%s"                                               // asm for condition expression (stored in eax)
                         "    cmpl    $0, %%eax\n"                          // compare condition to 0
                         "    je      _end%d\n"                             // if condition is false, jump to end of loop
                         "_cont%d:\n"                                       // label for continue statement
                         "    jmp     _start%d\n"                           // jump to start of loop
                         "_end%d:\n";                                       // label for end of loop
            loop_format = boost::format(out_format);
            loop_format % local_counter
                        % body
                        % cond
                        % local_counter
                        % local_counter
                        % local_counter
                        % local_counter;
        }
        out += loop_format.str();
        return out;
    } else if (stat->statement_type == "break") {
        if (inner_loop_count == -1) {
            throw std::runtime_error("encountered 'break' outside of a loop\n");
        }
        boost::format out_format;
        if (inner_loop_stack_index - stack_index > 0) {
            out_format = boost::format("    addl    $%d, %%esp\n"
                                       "    jmp     _end%d\n");
            out_format % (inner_loop_stack_index - stack_index)
                       % inner_loop_count;
        } else {
            out_format = boost::format("    jmp     _end%d\n");
            out_format % inner_loop_count;
        }
        return out_format.str();
    } else if (stat->statement_type == "continue") {
        if (inner_loop_count == -1) {
            throw std::runtime_error("encountered 'continue' outside of a loop\n");
        }
        boost::format out_format;
        if (inner_loop_stack_index - stack_index > 0) {
            out_format = boost::format("    addl    $%d, %%esp\n"
                                       "    jmp     _cont%d\n");
            out_format % (inner_loop_stack_index - stack_index)
                       % inner_loop_count;
        } else {
            out_format = boost::format("    jmp     _cont%d\n");
            out_format % inner_loop_count;
        }
        return out_format.str();
        return out_format.str();
    } else if (stat->statement_type == "compound") {
        std::string out = "";
        std::set<std::string> current_scope;

        for (auto item: stat->items) {
            out += codegen_x86_block_item(item, local_addresses, current_scope, stack_index, inner_loop_stack_index, inner_loop_count);
        }
        if (current_scope.size()) {
            boost::format out_format("    addl    $%d, %%esp\n");               // deallocate variables from the inner scope
            out_format % (4*current_scope.size());
            out += out_format.str();
        }
        return out;
    } else { // if (stat->statement_type == "return") {
        boost::format out_format("%s"
                                 "    movl    %%ebp, %%esp\n"
                                 "    pop     %%ebp\n"
                                 "    ret\n"
                                 );
        out_format % codegen_x86_expression_comma(stat->expression1, local_addresses);
        return out_format.str();
    }
}

std::string codegen_x86_expression_comma(std::shared_ptr<ExpressionComma> exp, std::map<std::string, int> local_addresses) {
    if (exp->exp_type == "null") {
        return std::string("");
    } else { // if (exp->exp_type == "assignment") {
        std::string out = "";
        for (auto expression: exp->expressions) {
            out += codegen_x86_expression_assignment(expression, local_addresses);
        }
        return out;
    }
}

std::string codegen_x86_expression_assignment(std::shared_ptr<ExpressionAssignment> exp, std::map<std::string, int> local_addresses) {
    if (exp->exp_type == "conditional") {
        return codegen_x86_expression_conditional(exp->expression, local_addresses);
    } else { // if (exp->exp_type == "assignment") {
        if (!local_addresses.count(exp->assign_id)) {
            throw std::runtime_error("variable '" + exp->assign_id + "' used before declaration\n");
        }
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

std::string codegen_x86_expression_conditional(std::shared_ptr<ExpressionConditional> exp, std::map<std::string, int> local_addresses) {
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

        int local_counter = global_counter;
        global_counter++;
        format_str % local_counter
                   % codegen_x86_expression_comma(exp->exp_true, local_addresses)
                   % local_counter
                   % local_counter
                   % codegen_x86_expression_conditional(exp->exp_false, local_addresses)
                   % local_counter;

        out += format_str.str();
        return out;
    }
}

std::string codegen_x86_expression_logic_or(std::shared_ptr<ExpressionLogicOr> exp, std::map<std::string, int> local_addresses) {
    if (exp->expressions.size() == 1) {
        return codegen_x86_expression_logic_and(exp->expressions.front(), local_addresses);
    } else {
        std::string out = codegen_x86_expression_logic_and(
            exp->expressions.front(), local_addresses);             // asm for first operand
        std::string base_format_str = "    pushl   %%eax\n"         // push first operand to stack
                                      "%s"                          // asm for second operand
                                      "    pop     %%ecx\n"         // pop first operand to ecx
                                      "    orl     %%ecx, %%eax\n"  // compute first operand | second operand, sets FLAGS
                                      "    movl    $0, %%eax\n"     // zero-out eax (keeping FLAGS intact)
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

std::string codegen_x86_expression_logic_and(std::shared_ptr<ExpressionLogicAnd> exp, std::map<std::string, int> local_addresses) {
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
                                      "    movl    $0, %%eax\n"     // zero out eax (keeping FLAGS intact)
                                      "    setne   %%al\n"          // set al to 1 iff second operand !=0
                                      "    andb    %%cl, %%al\n";   // set al to al & cl

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

std::string codegen_x86_expression_bitwise_or(std::shared_ptr<ExpressionBitwiseOr> exp, std::map<std::string, int> local_addresses) {
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

std::string codegen_x86_expression_bitwise_xor(std::shared_ptr<ExpressionBitwiseXor> exp, std::map<std::string, int> local_addresses) {
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

std::string codegen_x86_expression_bitwise_and(std::shared_ptr<ExpressionBitwiseAnd> exp, std::map<std::string, int> local_addresses) {
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

std::string codegen_x86_expression_equality(std::shared_ptr<ExpressionEquality> exp, std::map<std::string, int> local_addresses) {
    if (exp->expressions.size() == 1) {
        return codegen_x86_expression_relational(exp->expressions.front(), local_addresses);
    } else {
        std::string out = codegen_x86_expression_relational(
            exp->expressions.front(), local_addresses);             // asm for first operand
        std::string base_format_str = "    pushl   %%eax\n"         // push first oeprand to stack
                                      "%s"                          // asm for second operand
                                      "    pop     %%ecx\n"         // pop first operand to ecx
                                      "    cmpl    %%eax, %%ecx\n"  // compare first operand to second operand and set FLAGS
                                      "    movl    $0, %%eax\n"     // zero-out eax (keeping FLAGS intact)
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

std::string codegen_x86_expression_relational(std::shared_ptr<ExpressionRelational> exp, std::map<std::string, int> local_addresses) {
    if (exp->expressions.size() == 1) {
        return codegen_x86_expression_shift(exp->expressions.front(), local_addresses);
    } else {
        std::string out = codegen_x86_expression_shift(
            exp->expressions.front(), local_addresses);             // asm for first operand
        std::string base_format_str = "    pushl   %%eax\n"         // push first oeprand to stack
                                      "%s"                          // asm for second operand
                                      "    pop     %%ecx\n"         // pop first operand to ecx
                                      "    cmpl    %%eax, %%ecx\n"  // compares first operand to second operand and sets FLAGS
                                      "    movl    $0, %%eax\n"     // zero-out eax (keeping FLAGS intact)
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

std::string codegen_x86_expression_shift(std::shared_ptr<ExpressionShift> exp, std::map<std::string, int> local_addresses) {
    if (exp->expressions.size() == 1) {
        return codegen_x86_expression_add(exp->expressions.front(), local_addresses);
    } else {
        std::string out = codegen_x86_expression_add(
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
                format_str % codegen_x86_expression_add(*expression, local_addresses);
                out += format_str.str();
            } else { // if (op == ">>") {
                boost::format format_str(shr_format_str);
                format_str % codegen_x86_expression_add(*expression, local_addresses);
                out += format_str.str();
            }
            std::advance(expression, 1);
        }
        return out;
    }
}

std::string codegen_x86_expression_add(std::shared_ptr<ExpressionAdd> exp, std::map<std::string, int> local_addresses) {
    if (exp->expressions.size() == 1) {
        return codegen_x86_expression_mult(exp->expressions.front(), local_addresses);
    } else {
        std::string out = 
            codegen_x86_expression_mult(exp->expressions.front(), 
                                        local_addresses);           // asm for first operand (stored in eax)
        std::string add_format_str = "    pushl   %%eax\n"          // push first operand to stack
                                     "%s"                           // asm for second operand (stored in eax)
                                     "    pop     %%ecx\n"          // pop first operand to ecx
                                     "    addl    %%ecx, %%eax\n";  // add ecx and eax, store result in eax

        std::string sub_format_str = "    pushl   %%eax\n"          // push first operand to stack
                                     "%s"                           // asm for second operand
                                     "    movl    %%eax, %%ecx\n"   // move second operand to ecx
                                     "    pop     %%eax\n"          // pop first operand to eax
                                     "    subl    %%ecx, %%eax\n";  // compute eax - ecx and store in eax

        auto exp_mult = exp->expressions.begin();
        std::advance(exp_mult, 1);
        for (auto op: exp->operators) {
            if (op == "+") {
                boost::format format_str(add_format_str);
                format_str % codegen_x86_expression_mult(*exp_mult, local_addresses);
                out += format_str.str();
            } else { // if (op == "-") {
                boost::format format_str(sub_format_str);
                format_str % codegen_x86_expression_mult(*exp_mult, local_addresses);
                out += format_str.str();
            }
            std::advance(exp_mult, 1);
        }
        return out;
    }
}

std::string codegen_x86_expression_mult(std::shared_ptr<ExpressionMult> exp, std::map<std::string, int> local_addresses) {
    if (exp->expressions.size() == 1) {
        return codegen_x86_expression_unary(exp->expressions.front(), local_addresses);
    } else {
        std::string out = 
            codegen_x86_expression_unary(exp->expressions.front(),
                                         local_addresses);          // asm for first operand (stored in eax)
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

        auto exp_unary = exp->expressions.begin();
        std::advance(exp_unary, 1);
        for (auto op: exp->operators) {
            if (op == "*") {
                boost::format out_format(mul_format_str);
                out_format % codegen_x86_expression_unary(*exp_unary, local_addresses);
                out += out_format.str();
            } else if (op == "/") {
                boost::format out_format(div_format_str);
                out_format % codegen_x86_expression_unary(*exp_unary, local_addresses);
                out += out_format.str();
            } else { // if (*op == "%") {
                boost::format out_format(mod_format_str);
                out_format % codegen_x86_expression_unary(*exp_unary, local_addresses);
                out += out_format.str();
            }
            std::advance(exp_unary, 1);
        }
        return out;
    }
}

std::string codegen_x86_expression_unary(std::shared_ptr<ExpressionUnary> exp, std::map<std::string, int> local_addresses) {
    if (exp->exp_type == "postfix") {
        return codegen_x86_expression_postfix(exp->postfix_exp, local_addresses);
    } else { // if (exp->exp_type == "unary_op") {
        if (exp->unaryop == "++" || exp->unaryop == "--") {
            if (!local_addresses.count(exp->prefix_id)) {
                throw std::runtime_error("prefix operator takes a modifiable rvalue\n");
            }
            if (!local_addresses.count(exp->prefix_id)) {
                throw std::runtime_error("identifier '" + exp->prefix_id + "' not declared in this scope\n");
            }
            boost::format out_format(
                "%s    %d(%%ebp)\n"                 // increment/decrement the variable in memory
                "movl    %d(%%ebp), %%eax\n"        // return the incremented value
            );
            out_format % (exp->unaryop == "++"? "    incl": "    decl")
                       % local_addresses[exp->prefix_id]
                       % local_addresses[exp->prefix_id];

            return out_format.str();

        } else {
            std::string out = codegen_x86_expression_unary(exp->unary_exp, local_addresses) + 
                              unary_ops[exp->unaryop];
            return out;
        }
    }
}

std::string codegen_x86_expression_postfix(std::shared_ptr<ExpressionPostfix> exp,
                                           std::map<std::string, int> local_addresses) {
    if (exp->exp_type == "const_int") {
        boost::format out_format(
            "    movl    $%d, %%eax\n"
        );
        out_format % exp->value_int;
        return out_format.str();
    } else if (exp->exp_type == "variable") {
        boost::format out_format(
            "    movl    %d(%%ebp), %%eax\n"                // move the variable from the stack to eax
        );
        if (!local_addresses.count(exp->id)) {
            throw std::runtime_error("identifier '" + exp->id + "' not declared in this scope\n");
        }
        out_format % local_addresses[exp->id];
        return out_format.str();
    } else if (exp->exp_type == "postfix") {
        boost::format out_format(
            "    movl    %d(%%ebp), %%eax\n"
            "    %s    %d(%%ebp)\n"
        );
        out_format % local_addresses[exp->id]
                   % (exp->postfix_op == "++"? "incl": "decl")
                   % local_addresses[exp->id];
        return out_format.str();
    } else if (exp->exp_type == "bracket_exp") {
        return codegen_x86_expression_comma(exp->bracket_exp, local_addresses);

    } else { // if (exp->exp_type == "function_call") {
        if (!global_functions.count(exp->id)) {
            std::cout << "implicit declaration of function: " << exp->id << "\n";
        } else if (exp->args.size() > global_functions[exp->id]->params.size()) {
            throw std::runtime_error("too many arguments to function: " + exp->id + "\n");
        } else if (exp->args.size() < global_functions[exp->id]->params.size()) {
            throw std::runtime_error("too few arguments to function: " + exp->id + "\n");
        }

        boost::format out_format(
            "%s"                          // asm for function arguments
            "    call    _%s\n"           // push return address and jump to function label
        );
        std::string args = "";
        int arg_count = 0;
        auto arg = exp->args.rbegin();
        for (int i=0; i<exp->args.size(); i++) {
            boost::format arg_format(
                "%s"                        // asm for argument value
                "    pushl   %%eax\n"       // push argument to stack
            );
            arg_format % codegen_x86_expression_assignment(*arg, local_addresses);
            args += arg_format.str();

            std::advance(arg, 1);
            arg_count++;
        }
        out_format % args % exp->id;
        
        boost::format dealloc_format("");
        if (arg_count) {
            dealloc_format = boost::format("    addl    $%d, %%esp\n");
            dealloc_format % (4*arg_count);
        }
        return out_format.str() + dealloc_format.str();;
    }
}