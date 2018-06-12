// <program> ::= <function>
// <function> ::= "int" <id> "(" ")" "{" { <block-item> } "}"
// <block-item> ::= <statement> | <declaration>
// <declaration> ::= "int" <id> [ = <exp> ] ";"
// <statement> ::= "return" <exp> ";"
//               | <exp> ";"
//               | "if" "(" <exp> ")" <statement> [ "else" <statement> ]
//               | "{" { <block-item> } "}"

// <exp> ::= <assignment-exp> { "," <assignment-exp> }
// <assignment-exp> ::= <id> "=" <exp> | <conditional-exp>
// <conditional-exp> ::= <logical-or-exp> "?" <exp> ":" <conditional-exp>
// <logical-or-exp> ::= <logical-and-exp> { "||" <logical-and-exp> }
// <logical-and-exp> ::= <equality-exp> { "&&" <equality-exp> }
// <equality-exp> ::= <relational-exp> { ("!=" | "==") <relational-exp> }
// <relational-exp> ::= <additive-exp> { ("<" | ">" | "<=" | ">=") <additive-exp> }
// <additive-exp> ::= <term> { ("+" | "-") <term> }
// <term> ::= <factor> { ("*" | "/") <factor> }
// <factor> ::= "(" <exp> ")" | <unary_op> <factor> | <int> | <id>
// <unary_op> ::= "!" | "~" | "-"

#ifndef parser
#include <list>
#include <regex>
#include <string>

#include "json.hpp"

using json = nlohmann::json;

// class declarations
class Program;
class Function;
class BlockItem;
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
class ExpressionAddSub;
class Term; // sets operator precedence for binary +/-
class Factor; // sets operator precedence for binary * & /, unary !/~/-

// function prototypes
// Program parse_program(std::list<std::string>& tokens);
std::shared_ptr<Function> parse_function(std::list<std::string>& tokens);
std::shared_ptr<BlockItem> parse_block_item(std::list<std::string>& tokens);
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
std::shared_ptr<ExpressionAddSub> parse_expression_add_sub(std::list<std::string>& tokens);
std::shared_ptr<Term> parse_term(std::list<std::string>& tokens);
std::shared_ptr<Factor> parse_factor(std::list<std::string>& tokens);

json jsonify_program(Program& prog);
json jsonify_function(std::shared_ptr<Function>& fun);
json jsonify_block_item(std::shared_ptr<BlockItem>& item);
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
json jsonify_expression_add_sub(std::shared_ptr<ExpressionAddSub>& exp);
json jsonify_term(std::shared_ptr<Term>& term);
json jsonify_factor(std::shared_ptr<Factor>& fac);

class Program {
    public:
    std::list<std::shared_ptr<Function>> functions;
};

class Function {
    public:
    std::string return_type;
    std::string id;
    std::list<std::shared_ptr<BlockItem>> items;
};

class BlockItem {
    public:
    std::string item_type;
    std::string var_type;
    std::string var_id;
    bool initialised = false;
    std::shared_ptr<Expression> init_exp;
    std::shared_ptr<Statement> statement;
};

class Statement {
    public:
    std::string statement_type;
    std::shared_ptr<Expression> expression;
    std::shared_ptr<Statement> if_statement;
    std::shared_ptr<Statement> else_statement;
    std::list<std::shared_ptr<BlockItem>> items;
};

class Expression {
    public:
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
    std::list<std::shared_ptr<ExpressionAddSub>> expressions;
    std::list<std::string> operators;
};

class ExpressionAddSub {
    public:
    std::list<std::shared_ptr<Term>> terms;
    std::list<std::string> operators;
};

class Term {
    public:
    std::list<std::shared_ptr<Factor>> factors;
    std::list<std::string> operators;
};

class Factor {
    public:
    std::string factor_type;
    std::shared_ptr<Expression> expression;
    std::string unaryop;
    std::shared_ptr<Factor> factor;
    std::string id;
    int value;
};

Program parse_program(std::list<std::string> tokens) {
    Program prog;
    prog.functions.push_back(std::shared_ptr<Function>(parse_function(tokens)));

    if (tokens.size() > 0) {
        throw std::runtime_error("unexpected tokens after function definition\n");
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
        throw std::runtime_error("expected '(' after function identifier\n");
    }
    tokens.pop_front();

    // argument parsing goes here
    if (tokens.front() != ")") {
        throw std::runtime_error("expected ')'\n");
    }
    tokens.pop_front();

    if (tokens.front() != "{") {
        throw std::runtime_error("expected '{'\n");
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
        item->item_type = "variable_declaration";
        item->var_type = tokens.front();
        tokens.pop_front();

        item->var_id = tokens.front();
        tokens.pop_front();

        if (tokens.front() == "=") {
            item->initialised = true;
            tokens.pop_front();
            item->init_exp = parse_expression(tokens);
        }
        if (tokens.front() != ";") {
            throw std::runtime_error("expected ';'\n");
        }
        tokens.pop_front();
    } else {
        item->item_type = "statement";
        item->statement = parse_statement(tokens);
    }
    return item;
}

json jsonify_block_item(std::shared_ptr<BlockItem>& item) {
    if (item->item_type == "statement") {
        return jsonify_statement(item->statement);
    } else { // if (item->item_type == "variable_declaration") {
        json ast = {
            {"type", item->item_type}, 
            {"var_type", item->var_type},
            {"var_id", item->var_id},
            {"initialised", item->initialised}
        };
        if (item->initialised) {
            ast["init_expression"] = jsonify_expression(item->init_exp);
        }
        return ast;
    }
}

std::shared_ptr<Statement> parse_statement(std::list<std::string>& tokens) {
    auto stat = std::shared_ptr<Statement>(new Statement);
    if (tokens.front() == "return") {
        stat->statement_type = "return";
        tokens.pop_front();

        stat->expression = parse_expression(tokens);

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

        stat->expression = parse_expression(tokens);

        if (tokens.front() != ")") {
            throw std::runtime_error("expected ')' in if statement\n");
        }
        tokens.pop_front();

        stat->if_statement = parse_statement(tokens);

        if (tokens.front() == "else") {
            tokens.pop_front();
            stat->else_statement = parse_statement(tokens);
        }
    } else if (tokens.front() == "else") {
        throw std::runtime_error("'else' statement has no parent 'if' statement\n");
    } else if (tokens.front() == "{") {
        stat->statement_type = "compound";
        tokens.pop_front();
        while (tokens.front() != "}") {
            stat->items.push_back(parse_block_item(tokens));
        }
        tokens.pop_front();
    } else {
        stat->statement_type = "expression";
        stat->expression = parse_expression(tokens);
        if (tokens.front() != ";") {
            throw std::runtime_error("expected ';'\n");
        }
        tokens.pop_front();
    }
    return stat;
}

json jsonify_statement(std::shared_ptr<Statement>& stat) {
    if (stat->statement_type == "expression") {
        return jsonify_expression(stat->expression);
    } else {
        json ast = {{"type", stat->statement_type}};

        if (stat->statement_type == "conditional") {
            ast["condition"] = jsonify_expression(stat->expression);
            ast["if_statement"] = jsonify_statement(stat->if_statement);
            ast["else_statement"] = jsonify_statement(stat->else_statement);
        } else if (stat->statement_type == "compound") {
            json items_json;
            for (auto item: stat->items) {
                items_json += jsonify_block_item(item);
            }
            ast["block_items"] = items_json;
        } else { // if (stat->statement_type == "return") {
            ast["expression"] = jsonify_expression(stat->expression);
        }
        return ast;
    }
}

std::shared_ptr<Expression> parse_expression(std::list<std::string>& tokens) {
    auto exp = std::shared_ptr<Expression>(new Expression);

    exp->expressions.push_back(parse_expression_assignment(tokens));

    while (tokens.front() == ",") {
        tokens.pop_front();
        exp->expressions.push_back(parse_expression_assignment(tokens));
    }
    return exp;
}

json jsonify_expression(std::shared_ptr<Expression>& exp) {
    json ast;
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

std::shared_ptr<ExpressionAssignment> parse_expression_assignment(std::list<std::string>& tokens) {
    auto exp = std::shared_ptr<ExpressionAssignment>(new ExpressionAssignment);

    if (std::regex_match(tokens.front(), std::regex("[A-Za-z_]\\w*"))) {
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
    exp->expressions.push_back(parse_expression_add_sub(tokens));

    while (tokens.front() == "<<" || tokens.front() == ">>") {
        exp->operators.push_back(tokens.front());
        tokens.pop_front();

        exp->expressions.push_back(parse_expression_add_sub(tokens));
    }

    return exp;
}

json jsonify_expression_shift(std::shared_ptr<ExpressionShift>& exp) {
    if (exp->expressions.size() == 1) {
        return jsonify_expression_add_sub(exp->expressions.front());
    } else {
        json ast;

        auto expr = exp->expressions.begin();
        auto op = exp->operators.begin();

        json expressions_json = {jsonify_expression_add_sub(*expr)};
        std::advance(expr, 1);
        for (int i=0; i<exp->operators.size(); i++) {
            expressions_json += *op;
            expressions_json += jsonify_expression_add_sub(*expr);
            std::advance(expr, 1);
            std::advance(op, 1);
        }
        ast["expressions"] = expressions_json;

        return ast;
    }
}

std::shared_ptr<ExpressionAddSub> parse_expression_add_sub(std::list<std::string>& tokens) {
    auto exp = std::shared_ptr<ExpressionAddSub>(new ExpressionAddSub);
    exp->terms.push_back(parse_term(tokens));

    while (tokens.front() == "+" || tokens.front() == "-") {
        exp->operators.push_back(tokens.front());
        tokens.pop_front();

        exp->terms.push_back(parse_term(tokens));
    }

    return exp;
}

json jsonify_expression_add_sub(std::shared_ptr<ExpressionAddSub>& exp) {
    if (exp->terms.size() == 1) {
        return jsonify_term(exp->terms.front());
    } else {
        json ast;
        auto term = exp->terms.begin();
        auto op = exp->operators.begin();

        json terms_json = {jsonify_term(*term)};
        std::advance(term, 1);
        for (int i=0; i<exp->operators.size(); i++) {
            terms_json += *op;
            terms_json += jsonify_term(*term);
            std::advance(term, 1);
            std::advance(op, 1);
        }
        ast["terms"] = terms_json;

        return ast;
    }
}

std::shared_ptr<Term> parse_term(std::list<std::string>& tokens) {
    auto term = std::shared_ptr<Term>(new Term);
    term->factors.push_back(parse_factor(tokens));

    while (tokens.front() == "*" || tokens.front() == "/" || tokens.front() == "%") {
        term->operators.push_back(tokens.front());
        tokens.pop_front();

        term->factors.push_back(parse_factor(tokens));
    }
    return term;
}

json jsonify_term(std::shared_ptr<Term>& term) {
    if (term->factors.size() == 1) {
        return jsonify_factor(term->factors.front());
    } else {
        json ast;
        auto fac = term->factors.begin();
        auto op = term->operators.begin();

        json factors_json = {jsonify_factor(*fac)};
        std::advance(fac, 1);
        for (int i=0; i<term->operators.size(); i++) {
            factors_json += *op;
            factors_json += jsonify_factor(*fac);
            std::advance(fac, 1);
            std::advance(op, 1);
        }
        ast["factors"] = factors_json;

        return ast;
    }
}

std::shared_ptr<Factor> parse_factor(std::list<std::string>& tokens) {
    auto fac = std::shared_ptr<Factor>(new Factor);

    if (tokens.front() == "(") {
        fac->factor_type = "bracket_expression";
        tokens.pop_front();
        fac->expression = parse_expression(tokens);
        if (tokens.front() != ")") {
            throw std::runtime_error("missing ')' after '('\n");
        }
        tokens.pop_front();
    } else if (std::regex_match(tokens.front(), std::regex("[!~-]|\\+\\+|--"))) {
        fac->factor_type = "unary_op";
        fac->unaryop = tokens.front();
        tokens.pop_front();
        fac->factor = parse_factor(tokens);
    } else if (std::regex_match(tokens.front(), std::regex("[A-Za-z_][\\w]*"))) {
        auto id = tokens.front();
        tokens.pop_front();
        if (tokens.front() == "++" || tokens.front() == "--") {
            fac->factor_type = "unary_op";
            fac->unaryop = tokens.front();
            tokens.pop_front();

            tokens.push_front(id);
            fac->factor = parse_factor(tokens);
        } else {
            fac->factor_type = "variable";
            fac->id = id;
        }
    } else {
        if (tokens.front() == "+") {
            tokens.pop_front();
        }
        try {
            fac->factor_type = "const";
            fac->value = std::stoi(tokens.front(), NULL, 0);
            tokens.pop_front();
        } catch (...) {
            throw std::runtime_error("invalid integer literal: " + tokens.front() + "\n");
        }
    }
    return fac;
}

json jsonify_factor(std::shared_ptr<Factor>& fac) {
    json ast = {{"type", fac->factor_type}};

    if (fac->factor_type == "bracket_expression") {
        ast["expression"] = jsonify_expression(fac->expression);
    } else if (fac->factor_type == "unary_op") {
        ast["operation"] = fac->unaryop;
        ast["factor"] = jsonify_factor(fac->factor);
    } else if (fac->factor_type == "const") {
        ast["value"] = fac->value;
    } else {
        ast["id"] = fac->id;
    }
    return ast;
}

#define parser
#endif