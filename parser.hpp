#ifndef parser
#include <iostream>
#include <list>
#include <regex>
#include <string>

#include "json.hpp"

using json = nlohmann::json;

class UnaryOp {
    public:

    std::string op;

    UnaryOp(std::list<std::string>& tokens) {
        if (std::regex_match(tokens.front(), std::regex("[!~-]"))) {
            op = tokens.front();
            tokens.pop_front();
        } else {
            throw std::runtime_error("invalid operator: " + tokens.front() + "\n");
        }
    }
};

class Expression {
    public:

    int value;

    std::string exp_type;

    UnaryOp* unaryop;
    Expression* expression;

    Expression(std::list<std::string>& tokens) {
        if (!std::regex_match(tokens.front(), std::regex("[!~-]"))) {
            try {
                exp_type = "const";
                value = std::stoi(tokens.front(), 0);
                tokens.pop_front();
            } catch (...) {
                throw std::runtime_error("invalid integer literal: " + tokens.front() + "\n");
            }
        } else {
            unaryop = new UnaryOp(tokens);
            expression = new Expression(tokens);
        }
    }

    ~Expression() {
        std::cout << "expression destructor called\n";
        if (exp_type == "unary_op"){
            delete unaryop;
            delete expression;
        }
    }

    json jsonify() {
        json ast = {{"type", exp_type}};
        if (exp_type == "unary_op") {
            ast["expression"] = expression->jsonify();
        } else if (exp_type == "const"){
            ast["value"] = value;
        }

        return ast;
    }
};

class Statement {
    public:

    Expression* expression;
    std::string statement_type;

    Statement(std::list<std::string>& tokens) {
        if (tokens.front() != "return") {
            throw std::runtime_error("invalid statement: " + tokens.front() + "\n");
        }
        statement_type = tokens.front();
        tokens.pop_front();

        expression = new Expression(tokens);

        if (tokens.front() != ";") {
            throw std::runtime_error("expected ';'\n");
        }
        tokens.pop_front();
    }

    ~Statement() {
        std::cout << "Statement destructor called\n";
        delete expression;
    }

    json jsonify() {

        json ast = {{"type", statement_type}};
        // std::list<Expression>::iterator it = expressions.begin();

        // json expressions_json;
        // for (int i=0; i < expressions.size(); i++) {
        //     expressions_json["expression" + std::to_string(i)] = it->jsonify();
        //     std::advance(it, 1);
        // }
        ast["expression"] = expression->jsonify();
        return ast;
    }
};

class Function {
    public:
    std::string return_type;
    std::string id;
    std::list<Statement> statements;

    Function(std::list<std::string>& tokens) {
        if (tokens.front() != "int") {
            throw std::runtime_error("invalid return type: " + tokens.front() + "\n");
        }
        return_type = tokens.front();
        tokens.pop_front();
        if (!std::regex_match(tokens.front(), std::regex("[A-Za-z_]\\w*"))) {
            throw std::runtime_error("invalid function identifier: " + tokens.front() + "\n");
        }
        id = tokens.front();
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

        statements.push_back(Statement(tokens));

        if (tokens.front() != "}") {
            throw std::runtime_error("expected '}'\n");
        }
        tokens.pop_front();
    }

    json jsonify() {

        json ast = {
            {"identifier", id},
            {"return_type", return_type}
        };

        std::list<Statement>::iterator it = statements.begin();

        json statements_json;
        for (int i=0; i < statements.size(); i++) {
            statements_json["statement" + std::to_string(i)] = it->jsonify();
            std::advance(it, 1);
        }
        ast["statements"] = statements_json;

        return ast;
    }
};

class Program {
    public:

    std::list<Function> functions;

    Program(std::list<std::string>& tokens) {
        functions.push_back(Function(tokens));

        if (tokens.size() > 0) {
            throw std::runtime_error("unexpected tokens after function definition\n");
        }
    }

    json jsonify() {

        json ast;
        std::list<Function>::iterator it = functions.begin();

        for (int i=0; i < functions.size(); i++) {
            json a = it->jsonify();
            ast["function" + std::to_string(i)] = it->jsonify();
            std::advance(it, 1);
        }

        return ast;
    }
};

#define parser
#endif