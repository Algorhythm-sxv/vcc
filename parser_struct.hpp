#ifndef parser
#include <list>
#include <regex>
#include <string>

#include "json.hpp"
using json = nlohmann::json;

struct expression {
    int value;
    std::string exp_type;
    std::string unaryop;
    struct expression* expression;
};
typedef struct expression Expression;

struct statement {
    struct expression* expression;
    std::string statement_type;
};
typedef struct statement Statement;

struct function {
    std::string return_type;
    std::string id;
    std::list<struct statement> statements;
};
typedef struct function Function;

struct program {
    std::list<struct function> functions;
};
typedef struct program Program;

Expression parse_expression(std::list<std::string>& tokens) {
    Expression exp;

    if (!std::regex_match(tokens.front(), std::regex("[!~-]"))) {
        try {
            exp.exp_type = "const";
            exp.value = std::stoi(tokens.front(), 0);
            tokens.pop_front();
        } catch (...) {
            throw std::runtime_error("invalid integer literal: " + tokens.front() + "\n");
        }
    } else {
        exp.exp_type = "unary_op";
        exp.unaryop = tokens.front();
        tokens.pop_front();
        *exp.expression = parse_expression(tokens);
    }

    return exp;
}

json jsonify(Expression expression) {
    json ast = {{"type", expression.exp_type}};
    if (expression.exp_type == "unary_op") {
        ast["expression"] = jsonify(*expression.expression);
    } else if (expression.exp_type == "const") {
        ast["value"] = expression.value;
    }

    return ast;
}

Statement parse_statement(std::list<std::string>& tokens) {
    Statement stat;

    if (tokens.front() != "return") {
        throw std::runtime_error("invalid statement: " + tokens.front() + "\n");
    }
    stat.statement_type = tokens.front();
    tokens.pop_front();

    *stat.expression = parse_expression(tokens);

    if (tokens.front() != ";") {
        throw std::runtime_error("expected ';'\n");
    }
    tokens.pop_front();

    return stat;
}

json jsonify(Statement statement) {
    json ast = {{"type", statement.statement_type}};
    // std::list<Expression>::iterator it = expressions.begin();

    // json expressions_json;
    // for (int i=0; i < statement.expressions.size(); i++) {
    //     expressions_json["expression" + std::to_string(i)] = jsonify(*statement.expression));
    //     std::advance(it, 1);
    // }
    ast["expression"] = jsonify(*statement.expression);
    return ast;
}

Function parse_function(std::list<std::string>& tokens) {
    Function fun;
    if (tokens.front() != "int") {
        throw std::runtime_error("invalid return type: " + tokens.front() + "\n");
    }
    fun.return_type = tokens.front();
    tokens.pop_front();
    if (!std::regex_match(tokens.front(), std::regex("[A-Za-z_]\\w*"))) {
        throw std::runtime_error("invalid function identifier: " + tokens.front() + "\n");
    }
    fun.id = tokens.front();
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

    fun.statements.push_back(parse_statement(tokens));

    if (tokens.front() != "}") {
        throw std::runtime_error("expected '}'\n");
    }
    tokens.pop_front();

    return fun;
}

json jsonify(Function function) {
    json ast = {
        {"identifier", function.id},
        {"return_type", function.return_type}
    };

    std::list<Statement>::iterator it = function.statements.begin();

    json statements_json;
    for (int i=0; i < function.statements.size(); i++) {
        statements_json["statement" + std::to_string(i)] = jsonify(*it);
        std::advance(it, 1);
    }
    ast["statements"] = statements_json;

    return ast;
}

Program parse_program(std::list<std::string>& tokens) {
    Program prog;

    prog.functions.push_back(parse_function(tokens));

    if (tokens.size() > 0) {
        throw std::runtime_error("unexpected tokens after function definition\n");
    }

    return prog;
}

json jsonify(Program program) {
    json ast;
    std::list<Function>::iterator it = program.functions.begin();

    for (int i=0; i < program.functions.size(); i++) {
        ast["function" + std::to_string(i)] = jsonify(*it);
        std::advance(it, 1);
    }

    return ast;
}
#define parser
#endif //parser