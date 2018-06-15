// <program> ::= <function>
// <function> ::= "int" <id> "(" <parameter-list> ")" "{" { <block-item> } "}"
// <block-item> ::= <statement> | <declaration-list>
// <declaration-list> ::= "int" <declaration> { "," <declaration> } ";"
// <declaration> ::= <id> [ = <exp> ]
// <statement> ::= "return" <exp> ";"
//               | <exp> ";"
//               | "if" "(" <exp> ")" <statement> [ "else" <statement> ]
//               | "{" { <block-item> } "}"
//               | "for" "(" <exp> ";" <exp> ";" <exp> ")" <statement>
//               | "for" "(" <declaration> <exp> ";" <exp> ")" <statement>
//               | "while" "(" <exp> ")" <statement>
//               | "do" <statement> "while" <exp> ";"
//               | "break" ";"
//               | "continue" ";"

// <exp> ::= <assignment-exp> { "," <assignment-exp> } | None
// <assignment-exp> ::= <id> "=" <exp> | <conditional-exp>
// <conditional-exp> ::= <logical-or-exp> "?" <exp> ":" <conditional-exp>
// <logical-or-exp> ::= <logical-and-exp> { "||" <logical-and-exp> }
// <logical-and-exp> ::= <equality-exp> { "&&" <equality-exp> }
// <equality-exp> ::= <relational-exp> { ("!=" | "==") <relational-exp> }
// <relational-exp> ::= <additive-exp> { ("<" | ">" | "<=" | ">=") <additive-exp> }
// <additive-exp> ::= <multiplicative-exp> { ("+" | "-") <multiplicative-exp> }
// <multiplicative-exp> ::= <factor> { ("*" | "/") <factor> }
// <unary-exp> ::= <unary_op> <unary-exp> | <postfix-exp>
// <unary-op> ::= "+" | "-" | "~" | "!"
// <postfix-exp> ::= <postfix-exp> "(" [ <assignment-exp> { "," <assigment-exp> } ] ")" 
//                   | <id> [("++" | "--") ]
//                   | "(" <exp> ")"
//                   | <int>

#ifndef parser
#include <list>
#include <regex>
#include <set>
#include <string>

#include "json.hpp"

using json = nlohmann::json;

// set of reserved keywords
std::set<std::string> keywords = {"return", "int", "while", "do", "for", "break", "continue", "if", "else"};

// class declarations
class Program;
class Function;
class BlockItem;
class DeclarationList;
class Declaration;
class Statement;
class Expression;
class ExpressionAssignment;
class ExpressionConditional;
class ExpressionLogicOr;
class ExpressionLogicAnd;
class ExpressionBitwiseOr;
class ExpressionBitwiseXor;
class ExpressionBitwiseAnd;
class ExpressionEquality;
class ExpressionRelational;
class ExpressionShift;
class ExpressionAdd;
class ExpressionMult;
class ExpressionUnary;
class ExpressionPostfix;

// function prototypes
// Program parse_program(std::list<std::string>& tokens);
std::shared_ptr<Function> parse_function(std::list<std::string>& tokens);
std::shared_ptr<BlockItem> parse_block_item(std::list<std::string>& tokens);
std::shared_ptr<DeclarationList> parse_declaration_list(std::list<std::string>& tokens);
std::shared_ptr<Declaration> parse_declaration(std::list<std::string>& tokens);
std::shared_ptr<Statement> parse_statement(std::list<std::string>& tokens);
std::shared_ptr<Expression> parse_expression(std::list<std::string>& tokens);
std::shared_ptr<ExpressionAssignment> parse_expression_assignment(std::list<std::string>& tokens);
std::shared_ptr<ExpressionConditional> parse_expression_conditional(std::list<std::string>& tokens);
std::shared_ptr<ExpressionLogicOr> parse_expression_logic_or(std::list<std::string>& tokens);
std::shared_ptr<ExpressionLogicAnd> parse_expression_logic_and(std::list<std::string>& tokens);
std::shared_ptr<ExpressionBitwiseOr> parse_expression_bitwise_or(std::list<std::string>& tokens);
std::shared_ptr<ExpressionBitwiseXor> parse_expression_bitwise_xor(std::list<std::string>& tokens);
std::shared_ptr<ExpressionBitwiseAnd> parse_expression_bitwise_and(std::list<std::string>& tokens);
std::shared_ptr<ExpressionEquality> parse_expression_equality(std::list<std::string>& tokens);
std::shared_ptr<ExpressionRelational> parse_expression_relational(std::list<std::string>& tokens);
std::shared_ptr<ExpressionShift> parse_expression_shift(std::list<std::string>& tokens);
std::shared_ptr<ExpressionAdd> parse_expression_add(std::list<std::string>& tokens);
std::shared_ptr<ExpressionMult> parse_expression_mult(std::list<std::string>& tokens);
std::shared_ptr<ExpressionUnary> parse_expression_unary(std::list<std::string>& tokens);
std::shared_ptr<ExpressionPostfix> parse_expression_postfix(std::list<std::string>& tokens);

json jsonify_program(Program& prog);
json jsonify_function(std::shared_ptr<Function>& fun);
json jsonify_block_item(std::shared_ptr<BlockItem>& item);
json jsonify_declaration(std::shared_ptr<Declaration>& decl);
json jsonify_declaration_list(std::shared_ptr<DeclarationList>& declist);
json jsonify_statement(std::shared_ptr<Statement>& stat);
json jsonify_expression(std::shared_ptr<Expression>& exp);
json jsonify_expression_assignment(std::shared_ptr<ExpressionAssignment>& exp);
json jsonify_expression_conditional(std::shared_ptr<ExpressionConditional>& exp);
json jsonify_expression_logic_or(std::shared_ptr<ExpressionLogicOr>& exp);
json jsonify_expression_logic_and(std::shared_ptr<ExpressionLogicAnd>& exp);
json jsonify_expression_bitwise_or(std::shared_ptr<ExpressionBitwiseOr>& exp);
json jsonify_expression_bitwise_xor(std::shared_ptr<ExpressionBitwiseXor>& exp);
json jsonify_expression_bitwise_and(std::shared_ptr<ExpressionBitwiseAnd>& exp);
json jsonify_expression_equality(std::shared_ptr<ExpressionEquality>& exp);
json jsonify_expression_relational(std::shared_ptr<ExpressionRelational>& exp);
json jsonify_expression_shift(std::shared_ptr<ExpressionShift>& exp);
json jsonify_expression_add(std::shared_ptr<ExpressionAdd>& exp);
json jsonify_expression_mult(std::shared_ptr<ExpressionMult>& exp);
json jsonify_expression_unary(std::shared_ptr<ExpressionUnary>& exp);
json jsonify_expression_postfix(std::shared_ptr<ExpressionPostfix>& exp);

class Program {
    public:
    std::list<std::shared_ptr<Function>> functions;
};

class Function {
    public:
    std::string return_type;
    std::string id;
    // std::shared_ptr<ParameterList> params;
    std::list<std::string> param_ids;
    std::list<std::string> param_types;
    std::list<std::shared_ptr<BlockItem>> items;
};

class BlockItem {
    public:
    std::string item_type;
    std::shared_ptr<DeclarationList> declaration_list;
    std::shared_ptr<Statement> statement;
};

class DeclarationList {
    public:
    std::list<std::shared_ptr<Declaration>> declarations;
    std::string var_type;
};

class Declaration {
    public:
    bool initialised = false;
    std::string var_id;
    std::shared_ptr<ExpressionAssignment> init_exp;
};

class Statement {
    public:
    std::string statement_type;
    std::shared_ptr<Expression> expression1;
    std::shared_ptr<Statement> statement1;
    std::shared_ptr<Statement> statement2;
    std::shared_ptr<Expression> expression2;
    std::shared_ptr<Expression> expression3;
    std::list<std::shared_ptr<BlockItem>> items;
};

class Expression {
    public:
    std::string exp_type;
    std::list<std::shared_ptr<ExpressionAssignment>> expressions;
};

class ExpressionAssignment {
    public:
    std::string exp_type;
    std::string assign_id;
    std::string assign_type;
    std::shared_ptr<ExpressionAssignment> assign_exp;
    std::shared_ptr<ExpressionConditional> expression;
};

class ExpressionConditional {
    public:
    std::string exp_type;
    std::shared_ptr<ExpressionLogicOr> condition;
    std::shared_ptr<Expression> exp_true;
    std::shared_ptr<ExpressionConditional> exp_false;
};

class ExpressionLogicOr {
    public:
    std::list<std::shared_ptr<ExpressionLogicAnd>> expressions;
};

class ExpressionLogicAnd {
    public:
    std::list<std::shared_ptr<ExpressionBitwiseOr>> expressions;
};

class ExpressionBitwiseOr {
    public:
    std::list<std::shared_ptr<ExpressionBitwiseXor>> expressions;
};

class ExpressionBitwiseXor {
    public:
    std::list<std::shared_ptr<ExpressionBitwiseAnd>> expressions;
};

class ExpressionBitwiseAnd {
    public:
    std::list<std::shared_ptr<ExpressionEquality>> expressions;
};

class ExpressionEquality {
    public:
    std::list<std::shared_ptr<ExpressionRelational>> expressions;
    std::list<std::string> operators;
};

class ExpressionRelational {
    public:
    std::list<std::shared_ptr<ExpressionShift>> expressions;
    std::list<std::string> operators;
};

class ExpressionShift {
    public:
    std::list<std::shared_ptr<ExpressionAdd>> expressions;
    std::list<std::string> operators;
};

class ExpressionAdd {
    public:
    std::list<std::shared_ptr<ExpressionMult>> expressions;
    std::list<std::string> operators;
};

class ExpressionMult {
    public:
    std::list<std::shared_ptr<ExpressionUnary>> expressions;
    std::list<std::string> operators;
};

class ExpressionUnary {
    public:
    std::string exp_type;
    std::string unaryop;
    std::string prefix_id;
    std::shared_ptr<ExpressionUnary> unary_exp;
    std::shared_ptr<ExpressionPostfix> postfix_exp;
};

class ExpressionPostfix {
    public:
    std::string exp_type;
    std::string postfix_op;
    std::shared_ptr<ExpressionPostfix> postfix_exp;
    std::list<std::shared_ptr<ExpressionAssignment>> args;
    std::shared_ptr<Expression> bracket_exp;
    std::string id;
    int value;
};

Program parse_program(std::list<std::string> tokens) {
    Program prog;

    while (tokens.size() > 0) {
        prog.functions.push_back(std::shared_ptr<Function>(parse_function(tokens)));
    }

    return prog;
}
json jsonify_program(Program& prog) {
    json ast;
    auto it = prog.functions.begin();

    for (int i=0; i < prog.functions.size(); i++) {
        ast["function" + std::to_string(i)] = jsonify_function(*it);
        std::advance(it, 1);
    }

    return ast;
}

std::shared_ptr<Function> parse_function(std::list<std::string>& tokens) {
    auto fun = std::shared_ptr<Function>(new Function);

    if (tokens.front() != "int") {
        throw std::runtime_error("invalid return type: " + tokens.front() + "\n");
    }
    fun->return_type = tokens.front();
    tokens.pop_front();
    if (!std::regex_match(tokens.front(), std::regex("[A-Za-z_]\\w*"))) {
        throw std::runtime_error("invalid function identifier: " + tokens.front() + "\n");
    }
    fun->id = tokens.front();
    tokens.pop_front();
    
    if (tokens.front() != "(") {
        throw std::runtime_error("expected '(' after function identifier, got " + tokens.front() + "\n");
    }
    tokens.pop_front();

    if (tokens.front() != ")") {
        goto first_param; // skip popping comma for first parameter
        do {
            tokens.pop_front();

            first_param:
            if (tokens.front() != "int") {
                throw std::runtime_error("function parameters must have type declaration");
            }
            fun->param_types.push_back(tokens.front());
            tokens.pop_front();

            if (tokens.front() == ",") {
                throw std::runtime_error("expected identifier in function parameter\n");
            }
            if (keywords.count(tokens.front())) {
                throw std::runtime_error("invalid identifier: " + tokens.front() + "\n");
            }
            fun->param_ids.push_back(tokens.front());
            tokens.pop_front();
        } while (tokens.front() == ",");
    }

    if (tokens.front() != ")") {
        throw std::runtime_error("expected ')' after function parameters, got: " + tokens.front() + "\n");
    }
    tokens.pop_front();

    if (tokens.front() != "{") {
        throw std::runtime_error("expected '{' after function parameters, got: " + tokens.front() + "\n");
    }
    tokens.pop_front();

    while (tokens.front() != "}") {
        fun->items.push_back(parse_block_item(tokens));
    }
    tokens.pop_front();

    return fun;
}

json jsonify_function(std::shared_ptr<Function>& fun) {
    json ast = {
        {"identifier", fun->id},
        {"return_type", fun->return_type}
    };
    if (fun->param_ids.size()) {
        auto param_type = fun->param_types.begin();
        for (auto param: fun->param_ids) {
            json param_json = {
                {"type", *param_type},
                {"id", param}
            };
            ast["parameters"] += param_json;
            std::advance(param_type, 1);
        }
    }
    auto it = fun->items.begin();

    json block_items_json;
    for (int i=0; i < fun->items.size(); i++) {
        block_items_json["blockitem" + std::to_string(i)] = jsonify_block_item(*it);
        std::advance(it, 1);
    }
    ast["block_items"] = block_items_json;

    return ast;
}

std::shared_ptr<BlockItem> parse_block_item(std::list<std::string>& tokens) {
    auto item = std::shared_ptr<BlockItem>(new BlockItem);

    if (tokens.front() == "int") {
        item->item_type = "declaration";
        tokens.pop_front();

        item->declaration_list = parse_declaration_list(tokens);

    } else {
        item->item_type = "statement";
        item->statement = parse_statement(tokens);
    }
    return item;
}

json jsonify_block_item(std::shared_ptr<BlockItem>& item) {
    if (item->item_type == "statement") {
        return jsonify_statement(item->statement);
    } else { // if (item->item_type == "declaration") {
        return jsonify_declaration_list(item->declaration_list);
    }
}

std::shared_ptr<DeclarationList> parse_declaration_list(std::list<std::string>& tokens) {
    auto declist = std::shared_ptr<DeclarationList>(new DeclarationList);

    declist->declarations.push_back(parse_declaration(tokens));

    while (tokens.front() == ",") {
        tokens.pop_front();
        declist->declarations.push_back(parse_declaration(tokens));
    }
    if (tokens.front() != ";") {
        throw std::runtime_error("expected ';' after variable declaration\n");
    }
    tokens.pop_front();
    
    return declist;
}

json jsonify_declaration_list(std::shared_ptr<DeclarationList>& declist) {
    if (declist->declarations.size() == 1) {
        return jsonify_declaration(declist->declarations.front());
    } else {
        json ast;

        for (auto decl: declist->declarations) {
            ast += jsonify_declaration(decl);
        }
        return ast;
    }
}
std::shared_ptr<Declaration> parse_declaration(std::list<std::string>& tokens) {
    auto decl = std::shared_ptr<Declaration>(new Declaration);

    if (!(std::regex_match(tokens.front(), std::regex("[A-Za-z]\\w*")))) {
        throw std::runtime_error("invalid identifier: " + tokens.front() + "\n");
    }
    decl->var_id = tokens.front();
    tokens.pop_front();

    if (tokens.front() == "=") {
        decl->initialised = true;
        tokens.pop_front();
        decl->init_exp = parse_expression_assignment(tokens);
    }
    return decl;
}

json jsonify_declaration(std::shared_ptr<Declaration>& decl) {
    json ast = {
        {"id", decl->var_id}
    };
    if (decl->initialised) {
        ast["init_expression"] = jsonify_expression_assignment(decl->init_exp);
    }
    return ast;
}

std::shared_ptr<Statement> parse_statement(std::list<std::string>& tokens) {
    auto stat = std::shared_ptr<Statement>(new Statement);
    if (tokens.front() == "return") {
        stat->statement_type = "return";
        tokens.pop_front();

        stat->expression1 = parse_expression(tokens);

        if (tokens.front() != ";") {
            throw std::runtime_error("expected ';'\n");
        }
        tokens.pop_front();
    } else if (tokens.front() == "if") {
        stat->statement_type = "conditional";
        tokens.pop_front();

        if (tokens.front() != "(") {
            throw std::runtime_error("exprected '(' after 'if'\n");
        }
        tokens.pop_front();

        stat->expression1 = parse_expression(tokens);

        if (tokens.front() != ")") {
            throw std::runtime_error("expected ')' in if statement\n");
        }
        tokens.pop_front();

        stat->statement1 = parse_statement(tokens);

        if (tokens.front() == "else") {
            tokens.pop_front();
            stat->statement2 = parse_statement(tokens);
        }
    } else if (tokens.front() == "else") {
        throw std::runtime_error("'else' statement has no parent 'if' statement\n");

    } else if (tokens.front() == "for") {
        tokens.pop_front();

        if (tokens.front() != "(") {
            throw std::runtime_error("expected '(' after 'for'\n");
        }
        tokens.pop_front();

        if (tokens.front() == "int"){
            stat->statement_type = "for_declaration";

            stat->items.push_back(parse_block_item(tokens));
        } else {
            stat->statement_type = "for_expression";
            stat->expression1 = parse_expression(tokens);

            if (tokens.front() != ";") {
                throw std::runtime_error("expected ';' after for loop init expression\n");
            }
            tokens.pop_front();
        }

        stat->expression2 = parse_expression(tokens);
        // replace empty conditions with true (1)
        if (stat->expression2->exp_type == "null") {
            std::list<std::string> placeholder_tokens = {"1"};
            stat->expression2 = parse_expression(placeholder_tokens);
        }

        if (tokens.front() != ";") {
            throw std::runtime_error("expected ';' after for loop condition\n");
        }
        tokens.pop_front();

        stat->expression3 = parse_expression(tokens);

        if (tokens.front() != ")") {
            throw std::runtime_error("expected ')' after for loop post expression\n");
        }
        tokens.pop_front();

        stat->statement1 = parse_statement(tokens);

    } else if (tokens.front() == "while") {
        stat->statement_type = "while";
        tokens.pop_front();

        if (tokens.front() != "(") {
            throw std::runtime_error("expected '(' after 'while'\n");
        }
        tokens.pop_front();

        stat->expression1 = parse_expression(tokens);

        if (tokens.front() != ")") {
            throw std::runtime_error("expected ')' after while loop expression\n");
        }
        tokens.pop_front();

        stat->statement1 = parse_statement(tokens);

    } else if (tokens.front() == "do") {
        stat->statement_type = "do";
        tokens.pop_front();

        stat->statement1 = parse_statement(tokens);

        if (tokens.front() != "while") {
            throw std::runtime_error("expected 'while' after 'do'\n");
        }
        tokens.pop_front();
        if (tokens.front() != "(") {
            throw std::runtime_error("expected '(' after 'while'\n");
        }
        tokens.pop_front();

        stat->expression1 = parse_expression(tokens);

        if (tokens.front() != ")") {
            throw std::runtime_error("expected ')' after do-while condition\n");
        }
        tokens.pop_front();

        if (tokens.front() != ";") {
            throw std::runtime_error("expected ';' after 'do ... while'\n");
        }
        tokens.pop_front();

    } else if (tokens.front() == "break") {
        stat->statement_type = "break";
        tokens.pop_front();

        if (tokens.front() != ";") {
            throw std::runtime_error("expected ';' after 'break'\n");
        }
        tokens.pop_front();
    } else if (tokens.front() == "continue") {
        stat->statement_type = "continue";
        tokens.pop_front();

        if (tokens.front() != ";") {
            throw std::runtime_error("expected ';' after 'continue'\n");
        }
        tokens.pop_front();

    } else if (tokens.front() == "{") {
        stat->statement_type = "compound";
        tokens.pop_front();
        while (tokens.front() != "}") {
            stat->items.push_back(parse_block_item(tokens));
        }
        tokens.pop_front();
    } else {
        stat->statement_type = "expression";
        stat->expression1 = parse_expression(tokens);
        if (tokens.front() != ";") {
            throw std::runtime_error("expected ';'\n");
        }
        tokens.pop_front();
    }
    return stat;
}

json jsonify_statement(std::shared_ptr<Statement>& stat) {
    if (stat->statement_type == "expression") {
        return jsonify_expression(stat->expression1);
    } else {
        json ast = {{"type", stat->statement_type}};

        if (stat->statement_type == "conditional") {
            ast["condition"] = jsonify_expression(stat->expression1);
            ast["if_statement"] = jsonify_statement(stat->statement1);
            ast["else_statement"] = jsonify_statement(stat->statement2);
        } else if (stat->statement_type == "for_declaration") {
            ast["init"] = jsonify_block_item(stat->items.front());
            ast["condition"] = jsonify_expression(stat->expression2);
            ast["post"] = jsonify_expression(stat->expression3);
            ast["statement"] = jsonify_statement(stat->statement1);
        } else if (stat->statement_type == "for_expression") {
            ast["init"] = jsonify_expression(stat->expression1);
            ast["condition"] = jsonify_expression(stat->expression2);
            ast["post"] = jsonify_expression(stat->expression3);
            ast["statement"] = jsonify_statement(stat->statement1);
        } else if (stat->statement_type == "while" || stat->statement_type == "do") {
            ast["condition"] = jsonify_expression(stat->expression1);
            ast["statment"] = jsonify_statement(stat->statement1);
        } else if (stat->statement_type == "compound") {
            json items_json;
            for (auto item: stat->items) {
                items_json += jsonify_block_item(item);
            }
            ast["block_items"] = items_json;
        } else if (stat->statement_type == "return") {
            ast["expression"] = jsonify_expression(stat->expression1);
        } // else if stat->statment_type == "break" || stat->statement_type == "continue") {
        return ast;
    }
}

std::shared_ptr<Expression> parse_expression(std::list<std::string>& tokens) {
    auto exp = std::shared_ptr<Expression>(new Expression);
    if (tokens.front() == ";" || tokens.front() == ")") {
        exp->exp_type = "null";
        return exp;
    } else {
        exp->exp_type = "assignment";
        exp->expressions.push_back(parse_expression_assignment(tokens));

        while (tokens.front() == ",") {
            tokens.pop_front();
            exp->expressions.push_back(parse_expression_assignment(tokens));
        }
        return exp;
    }
}

json jsonify_expression(std::shared_ptr<Expression>& exp) {
    json ast;
    if (exp->exp_type == "null") {
        ast["type"] = exp->exp_type;
        return ast;
    } else { // if (exp->exp_type == "assignment") {
        ast += jsonify_expression_assignment(exp->expressions.front());
        auto expression = exp->expressions.begin();
        std::advance(expression, 1);
        for (int i=0; i<exp->expressions.size()-1;i++) {
            ast += ",";
            ast += jsonify_expression_assignment(*expression);
            std::advance(expression, 1);
        }
        return ast;
    }
}

std::shared_ptr<ExpressionAssignment> parse_expression_assignment(std::list<std::string>& tokens) {
    auto exp = std::shared_ptr<ExpressionAssignment>(new ExpressionAssignment);

    if (std::regex_match(tokens.front(), std::regex("[A-Za-z_]\\w*"))) {
        if (keywords.count(tokens.front())) {
            throw std::runtime_error("expected identifier (got '" + tokens.front() + "' )\n");
        }
        auto id = tokens.front();

        auto token = std::next(tokens.begin());

        auto t = *token;

        if (t=="="||t=="+="||t=="-="||t=="*="||t=="/="||t=="%="||t=="&="||t=="^="||t=="|="||t=="<<="||t==">>=") {
            exp->exp_type = "assignment";
            exp->assign_id = id;
            exp->assign_type = t;
            tokens.pop_front();
            tokens.pop_front();

            exp->assign_exp = parse_expression_assignment(tokens);

        } else {
            exp->exp_type = "conditional";
            exp->expression = parse_expression_conditional(tokens);
        }
        return exp;

    } else {
        exp->exp_type = "conditional";
        exp->expression = parse_expression_conditional(tokens);

        return exp;
    }
}

json jsonify_expression_assignment(std::shared_ptr<ExpressionAssignment>& exp) {
    if (exp->exp_type == "conditional") {
        return jsonify_expression_conditional(exp->expression);
    } else { // if (exp->exp_type == "assignment") {
        json ast = {{"type", exp->exp_type}};

        ast["id"] = exp->assign_id;
        ast["assign_type"] = exp->assign_type;
        ast["expression"] = jsonify_expression_assignment(exp->assign_exp);

        return ast;
    }
}

std::shared_ptr<ExpressionConditional> parse_expression_conditional(std::list<std::string>& tokens) {
    auto exp = std::shared_ptr<ExpressionConditional>(new ExpressionConditional);

    exp->exp_type = "logic_or";
    exp->condition = parse_expression_logic_or(tokens);

    if (tokens.front() == "?") {
        exp->exp_type = "conditional";

        tokens.pop_front();
        exp->exp_true = parse_expression(tokens);

        if (tokens.front() != ":") {
            throw std::runtime_error("expected ':' after '?'\n");
        }
        tokens.pop_front();
        exp->exp_false = parse_expression_conditional(tokens);
    }
    return exp;
}

json jsonify_expression_conditional(std::shared_ptr<ExpressionConditional>& exp) {
    if (exp->exp_type == "logic_or") {
        return jsonify_expression_logic_or(exp->condition);
    } else { // if (exp->exp_type == "conditional") {
        json ast = {{"type", exp->exp_type}};

        ast["condition"] = jsonify_expression_logic_or(exp->condition);
        ast["expression_true"] = jsonify_expression(exp->exp_true);
        ast["expression_false"] = jsonify_expression_conditional(exp->exp_false);

        return ast;
    }
}

std::shared_ptr<ExpressionLogicOr> parse_expression_logic_or(std::list<std::string>& tokens) {
    auto exp = std::shared_ptr<ExpressionLogicOr>(new ExpressionLogicOr);

    exp->expressions.push_back(parse_expression_logic_and(tokens));

    while (tokens.front() == "||") {
        tokens.pop_front();

        exp->expressions.push_back(parse_expression_logic_and(tokens));
    }
    return exp;
}

json jsonify_expression_logic_or(std::shared_ptr<ExpressionLogicOr>& exp) {
    if (exp->expressions.size() == 1) {
        return jsonify_expression_logic_and(exp->expressions.front());
    } else {
        json ast;

        json expressions_json;

        auto expression = exp->expressions.begin();
        expressions_json += jsonify_expression_logic_and(*expression);
        std::advance(expression, 1);

        for (int i=0; i<exp->expressions.size()-1; i++) {
            expressions_json += "||";
            expressions_json += jsonify_expression_logic_and(*expression);
            std::advance(expression, 1);
        }
        ast["expressions"] = expressions_json;

        return ast;
    }
}

std::shared_ptr<ExpressionLogicAnd> parse_expression_logic_and(std::list<std::string>& tokens) {
    auto exp = std::shared_ptr<ExpressionLogicAnd>(new ExpressionLogicAnd);

    exp->expressions.push_back(parse_expression_bitwise_or(tokens));

    while (tokens.front() == "&&") {
        tokens.pop_front();

        exp->expressions.push_back(parse_expression_bitwise_or(tokens));
    }
    return exp;
}

json jsonify_expression_logic_and(std::shared_ptr<ExpressionLogicAnd>& exp) {
    if (exp->expressions.size() == 1) {
        return jsonify_expression_bitwise_or(exp->expressions.front());
    } else {
        json ast;

        json expressions_json;

        auto expression = exp->expressions.begin();
        expressions_json += jsonify_expression_bitwise_or(*expression);
        std::advance(expression, 1);

        for (int i=0; i<exp->expressions.size()-1; i++) {
            expressions_json += "&&";
            expressions_json += jsonify_expression_bitwise_or(*expression);
            std::advance(expression, 1);
        }
        ast["expressions"] = expressions_json;

        return ast;
    }
}

std::shared_ptr<ExpressionBitwiseOr> parse_expression_bitwise_or(std::list<std::string>& tokens) {
    auto exp = std::shared_ptr<ExpressionBitwiseOr>(new ExpressionBitwiseOr);

    exp->expressions.push_back(parse_expression_bitwise_xor(tokens));

    while (tokens.front() == "|") {
        tokens.pop_front();

        exp->expressions.push_back(parse_expression_bitwise_xor(tokens));
    }

    return exp;
}

json jsonify_expression_bitwise_or(std::shared_ptr<ExpressionBitwiseOr>& exp) {
    if (exp->expressions.size() == 1) {
        return jsonify_expression_bitwise_xor(exp->expressions.front());
    } else {
        json ast;

        json expressions_json;

        auto expression = exp->expressions.begin();
        expressions_json += jsonify_expression_bitwise_xor(*expression);
        std::advance(expression, 1);

        for (int i=0; i<exp->expressions.size()-1; i++) {
            expressions_json += "|";
            expressions_json += jsonify_expression_bitwise_xor(*expression);
            std::advance(expression, 1);
        }
        ast["expressions"] = expressions_json;

        return ast;
    }
}

std::shared_ptr<ExpressionBitwiseXor> parse_expression_bitwise_xor(std::list<std::string>& tokens) {
    auto exp = std::shared_ptr<ExpressionBitwiseXor>(new ExpressionBitwiseXor);

    exp->expressions.push_back(parse_expression_bitwise_and(tokens));

    while (tokens.front() == "^") {
        tokens.pop_front();

        exp->expressions.push_back(parse_expression_bitwise_and(tokens));
    }

    return exp;
}

json jsonify_expression_bitwise_xor(std::shared_ptr<ExpressionBitwiseXor>& exp) {
    if (exp->expressions.size() == 1) {
        return jsonify_expression_bitwise_and(exp->expressions.front());
    } else {
        json ast;

        json expressions_json;

        auto expression = exp->expressions.begin();
        expressions_json += jsonify_expression_bitwise_and(*expression);
        std::advance(expression, 1);

        for (int i=0; i<exp->expressions.size()-1; i++) {
            expressions_json += "^";
            expressions_json += jsonify_expression_bitwise_and(*expression);
            std::advance(expression, 1);
        }
        ast["expressions"] = expressions_json;

        return ast;
    }
}

std::shared_ptr<ExpressionBitwiseAnd> parse_expression_bitwise_and(std::list<std::string>& tokens) {
    auto exp = std::shared_ptr<ExpressionBitwiseAnd>(new ExpressionBitwiseAnd);

    exp->expressions.push_back(parse_expression_equality(tokens));

    while (tokens.front() == "&") {
        tokens.pop_front();

        exp->expressions.push_back(parse_expression_equality(tokens));
    }

    return exp;
}

json jsonify_expression_bitwise_and(std::shared_ptr<ExpressionBitwiseAnd>& exp) {
    if (exp->expressions.size() == 1) {
        return jsonify_expression_equality(exp->expressions.front());
    } else {
        json ast;

        json expressions_json;

        auto expression = exp->expressions.begin();
        expressions_json += jsonify_expression_equality(*expression);
        std::advance(expression, 1);

        for (int i=0; i<exp->expressions.size()-1; i++) {
            expressions_json += "&";
            expressions_json += jsonify_expression_equality(*expression);
            std::advance(expression, 1);
        }
        ast["expressions"] = expressions_json;

        return ast;
    }
}

std::shared_ptr<ExpressionEquality> parse_expression_equality(std::list<std::string>& tokens) {
    auto exp = std::shared_ptr<ExpressionEquality>(new ExpressionEquality);

    exp->expressions.push_back(parse_expression_relational(tokens));

    while (tokens.front() == "=="  ||  tokens.front() == "!=") {
        exp->operators.push_back(tokens.front());
        tokens.pop_front();

        exp->expressions.push_back(parse_expression_relational(tokens));
    }

    return exp;
}

json jsonify_expression_equality(std::shared_ptr<ExpressionEquality>& exp) {
    if (exp->expressions.size() == 1) {
        return jsonify_expression_relational(exp->expressions.front());
    } else {
        json ast;

        auto expressions = exp->expressions.begin();
        auto op = exp->operators.begin();

        json expressions_json = {jsonify_expression_relational(*expressions)};
        std::advance(expressions, 1);
        for (int i=0; i<exp->operators.size(); i++) {
            expressions_json += *op;
            expressions_json += jsonify_expression_relational(*expressions);
            std::advance(expressions, 1);
            std::advance(op, 1);
        }
        ast["expressions"] = expressions_json;
        
        return ast;
    }
}

std::shared_ptr<ExpressionRelational> parse_expression_relational(std::list<std::string>& tokens) {
    auto exp = std::shared_ptr<ExpressionRelational>(new ExpressionRelational);

    exp->expressions.push_back(parse_expression_shift(tokens));

    while (tokens.front() == ">"  ||  tokens.front() == "<" ||
           tokens.front() == ">=" ||  tokens.front() == "<=") {
        exp->operators.push_back(tokens.front());
        tokens.pop_front();

        exp->expressions.push_back(parse_expression_shift(tokens));
    }

    return exp;
}

json jsonify_expression_relational(std::shared_ptr<ExpressionRelational>& exp) {
    if (exp->expressions.size() == 1) {
        return jsonify_expression_shift(exp->expressions.front());
    } else {
        json ast;

        auto expressions = exp->expressions.begin();
        auto op = exp->operators.begin();

        json expressions_json = {jsonify_expression_shift(*expressions)};
        std::advance(expressions, 1);
        for (int i=0; i<exp->operators.size(); i++) {
            expressions_json += *op;
            expressions_json += jsonify_expression_shift(*expressions);
            std::advance(expressions, 1);
            std::advance(op, 1);
        }
        ast["expressions"] = expressions_json;

        return ast;
    }
}

std::shared_ptr<ExpressionShift> parse_expression_shift(std::list<std::string>& tokens) {
    auto exp = std::shared_ptr<ExpressionShift>(new ExpressionShift);
    exp->expressions.push_back(parse_expression_add(tokens));

    while (tokens.front() == "<<" || tokens.front() == ">>") {
        exp->operators.push_back(tokens.front());
        tokens.pop_front();

        exp->expressions.push_back(parse_expression_add(tokens));
    }

    return exp;
}

json jsonify_expression_shift(std::shared_ptr<ExpressionShift>& exp) {
    if (exp->expressions.size() == 1) {
        return jsonify_expression_add(exp->expressions.front());
    } else {
        json ast;

        auto expr = exp->expressions.begin();
        auto op = exp->operators.begin();

        json expressions_json = {jsonify_expression_add(*expr)};
        std::advance(expr, 1);
        for (int i=0; i<exp->operators.size(); i++) {
            expressions_json += *op;
            expressions_json += jsonify_expression_add(*expr);
            std::advance(expr, 1);
            std::advance(op, 1);
        }
        ast["expressions"] = expressions_json;

        return ast;
    }
}

std::shared_ptr<ExpressionAdd> parse_expression_add(std::list<std::string>& tokens) {
    auto exp = std::shared_ptr<ExpressionAdd>(new ExpressionAdd);
    exp->expressions.push_back(parse_expression_mult(tokens));

    while (tokens.front() == "+" || tokens.front() == "-") {
        exp->operators.push_back(tokens.front());
        tokens.pop_front();

        exp->expressions.push_back(parse_expression_mult(tokens));
    }

    return exp;
}

json jsonify_expression_add(std::shared_ptr<ExpressionAdd>& exp) {
    if (exp->expressions.size() == 1) {
        return jsonify_expression_mult(exp->expressions.front());
    } else {
        json ast;
        auto exp_mult = exp->expressions.begin();
        auto op = exp->operators.begin();

        json expressions_json = {jsonify_expression_mult(*exp_mult)};
        std::advance(exp_mult, 1);
        for (int i=0; i<exp->operators.size(); i++) {
            expressions_json += *op;
            expressions_json += jsonify_expression_mult(*exp_mult);
            std::advance(exp_mult, 1);
            std::advance(op, 1);
        }
        ast["expressions"] = expressions_json;

        return ast;
    }
}

std::shared_ptr<ExpressionMult> parse_expression_mult(std::list<std::string>& tokens) {
    auto exp = std::shared_ptr<ExpressionMult>(new ExpressionMult);
    exp->expressions.push_back(parse_expression_unary(tokens));

    while (tokens.front() == "*" || tokens.front() == "/" || tokens.front() == "%") {
        exp->operators.push_back(tokens.front());
        tokens.pop_front();

        exp->expressions.push_back(parse_expression_unary(tokens));
    }
    return exp;
}

json jsonify_expression_mult(std::shared_ptr<ExpressionMult>& exp) {
    if (exp->expressions.size() == 1) {
        return jsonify_expression_unary(exp->expressions.front());
    } else {
        json ast;
        auto exp_unary = exp->expressions.begin();
        auto op = exp->operators.begin();

        json expressions_json = {jsonify_expression_unary(*exp_unary)};
        std::advance(exp_unary, 1);
        for (int i=0; i<exp->operators.size(); i++) {
            expressions_json += *op;
            expressions_json += jsonify_expression_unary(*exp_unary);
            std::advance(exp_unary, 1);
            std::advance(op, 1);
        }
        ast["expressions"] = expressions_json;

        return ast;
    }
}

std::shared_ptr<ExpressionUnary> parse_expression_unary(std::list<std::string>& tokens) {
    auto exp = std::shared_ptr<ExpressionUnary>(new ExpressionUnary);

    if (std::regex_match(tokens.front(), std::regex("[!~-]|"))) {
        exp->exp_type = "unary_op";
        exp->unaryop = tokens.front();
        tokens.pop_front();
        exp->unary_exp = parse_expression_unary(tokens);
    }else if (tokens.front() == "++" || tokens.front() == "--") {
        exp->exp_type = "prefix";
        exp->unaryop = tokens.front();
        tokens.pop_front();

        if (keywords.count(tokens.front()) ||
            !std::regex_match(tokens.front(), std::regex("[A-Za-z]\\w*"))) {
            throw std::runtime_error("invalid identifier: " + tokens.front() + "\n");
        }
        exp->prefix_id = tokens.front();
        tokens.pop_front();
    } else {
        exp->exp_type = "postfix";
        exp->postfix_exp = parse_expression_postfix(tokens);
    }
    return exp;
}

json jsonify_expression_unary(std::shared_ptr<ExpressionUnary>& exp) {
    if (exp->exp_type == "postfix") {
        return jsonify_expression_postfix(exp->postfix_exp);
    } else if (exp->exp_type == "prefix") {
        json ast = {
            {"operator", exp->unaryop},
            {"id", exp->prefix_id}
        };
        return ast;
    } else {
        json ast = {
            {"type", exp->exp_type},
            {"operator", exp->unaryop}
        };
        ast["expression"] = jsonify_expression_unary(exp->unary_exp);

        return ast;
    }
}

std::shared_ptr<ExpressionPostfix> parse_expression_postfix(std::list<std::string>& tokens) {
    auto exp = std::shared_ptr<ExpressionPostfix>(new ExpressionPostfix);

    if (tokens.front() == "(") {
        exp->exp_type = "bracket_exp";
        tokens.pop_front();

        exp->bracket_exp = parse_expression(tokens);

        if (tokens.front() != ")") {
            throw std::runtime_error("missing closing ')'");
        }
        return exp;
    } else if (std::regex_match(tokens.front(), std::regex("[A-Za-z]\\w*"))) {
        if (keywords.count(tokens.front())) {
            throw std::runtime_error("invalid identifier: " + tokens.front() + "\n");
        }
        exp->id = tokens.front();
        tokens.pop_front();

        if (tokens.front() == "(") {
            exp->exp_type = "function_call";
            tokens.pop_front();

            exp->args.push_back(parse_expression_assignment(tokens));
            while (tokens.front() == ",") {
                tokens.pop_front();
                exp->args.push_back(parse_expression_assignment(tokens));
            }
            if (tokens.front() != ")") {
                throw std::runtime_error("missing closing ')' after function call");
            }
            tokens.pop_front();

            return exp;
        } else if (tokens.front() == "++" || tokens.front() == "--") {
            exp->exp_type = "postfix";
            exp->postfix_op = tokens.front();
            tokens.pop_front();

            return exp;
        } else {
            exp->exp_type = "variable";
            return exp;
        }
    } else {
        exp->exp_type = "const";
        try {
            exp->value = std::stoi(tokens.front(), NULL, 0);
            tokens.pop_front();
        } catch (...) {
            throw std::runtime_error("invalid integer literal: " + tokens.front() + "\n");
        }
        return exp;
    }
}

json jsonify_expression_postfix(std::shared_ptr<ExpressionPostfix>& exp) {
    if (exp->exp_type == "const") {
        return exp->value;
    } else if (exp->exp_type == "variable") {
        json ast = {
            {"type", exp->exp_type},
            {"id", exp->id}
        };
        return ast;
    } else if(exp->exp_type == "postfix") {
        json ast = {
            {"type", exp->exp_type},
            {"operation", exp->postfix_op},
        };
        ast["expression"] = jsonify_expression_postfix(exp->postfix_exp);

        return ast;
    } else if (exp->exp_type == "bracket_exp") {
        json ast = {
            {"type", exp->exp_type}
        };
        ast["expression"] = jsonify_expression_postfix(exp->postfix_exp);

        return ast;
    } else { // if (exp->exp_type == "function_call") {
        json ast = {
            {"type", exp->exp_type},
            {"function_id", exp->id},
        };
        json args_json;
        for (auto arg: exp->args) {
            args_json += jsonify_expression_assignment(arg);
        }
        ast["arguments"] = args_json;

        return ast;
    }
}

#define parser
#endif