#ifndef parser
#include <list>
#include <regex>
#include <string>

#include "json.hpp"

using json = nlohmann::json;

// class declarations
class Program;
class Function;
class Statement;
class Expression;
class ExpressionLogicAnd;
class ExpressionEquality;
class ExpressionRelational;
class ExpressionAddSub;
class Term; // sets operator precedence for binary +/-
class Factor; // sets operator precedence for binary * & /, unary !/~/-

// function prototypes
// Program parse_program(std::list<std::string>& tokens);
std::shared_ptr<Function> parse_function(std::list<std::string>& tokens);
std::shared_ptr<Statement> parse_statement(std::list<std::string>& tokens);
std::shared_ptr<Expression> parse_expression(std::list<std::string>& tokens);
std::shared_ptr<ExpressionLogicAnd> parse_expression_logic_and(std::list<std::string>& tokens);
std::shared_ptr<ExpressionEquality> parse_expression_equality(std::list<std::string>& tokens);
std::shared_ptr<ExpressionRelational> parse_expression_relational(std::list<std::string>& tokens);
std::shared_ptr<ExpressionAddSub> parse_expression_add_sub(std::list<std::string>& tokens);
std::shared_ptr<Term> parse_term(std::list<std::string>& tokens);
std::shared_ptr<Factor> parse_factor(std::list<std::string>& tokens);

json jsonify_program(Program prog);
json jsonify_function(std::shared_ptr<Function> fun);
json jsonify_statement(std::shared_ptr<Statement> stat);
json jsonify_expression(std::shared_ptr<Expression> exp);
json jsonify_expression_logic_and(std::shared_ptr<ExpressionLogicAnd> exp);
json jsonify_expression_equality(std::shared_ptr<ExpressionEquality> exp);
json jsonify_expression_relational(std::shared_ptr<ExpressionRelational> exp);
json jsonify_expression_add_sub(std::shared_ptr<ExpressionAddSub> exp);
json jsonify_term(std::shared_ptr<Term> term);
json jsonify_factor(std::shared_ptr<Factor> fac);

class Program {
    public:
    std::list<std::shared_ptr<Function>> functions;
};

class Function {
    public:
    std::string return_type;
    std::string id;
    std::list<std::shared_ptr<Statement>> statements;
};

class Statement {
    public:
    std::string statement_type;
    std::shared_ptr<Expression> expression;
};

class Expression {
    public:
    std::list<std::shared_ptr<ExpressionLogicAnd>> expressions;
};

class ExpressionLogicAnd {
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
    std::string unaryop;
    std::shared_ptr<Expression> expression;
    std::shared_ptr<Factor> factor;
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
json jsonify_program(Program prog) {

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

    fun->statements.push_back(parse_statement(tokens));

    if (tokens.front() != "}") {
        throw std::runtime_error("expected '}'\n");
    }
    tokens.pop_front();

    return fun;
}

json jsonify_function(std::shared_ptr<Function> fun) {

    json ast = {
        {"identifier", fun->id},
        {"return_type", fun->return_type}
    };

    auto it = fun->statements.begin();

    json statements_json;
    for (int i=0; i < fun->statements.size(); i++) {
        statements_json["statement" + std::to_string(i)] = jsonify_statement(*it);
        std::advance(it, 1);
    }
    ast["statements"] = statements_json;

    return ast;
}

std::shared_ptr<Statement> parse_statement(std::list<std::string>& tokens) {
    auto stat = std::shared_ptr<Statement>(new Statement);
    if (tokens.front() != "return") {
        throw std::runtime_error("invalid statement: " + tokens.front() + "\n");
    }
    stat->statement_type = tokens.front();
    tokens.pop_front();

    stat->expression = parse_expression(tokens);

    if (tokens.front() != ";") {
        throw std::runtime_error("expected ';'\n");
    }
    tokens.pop_front();

    return stat;
}

json jsonify_statement(std::shared_ptr<Statement> stat) {

    json ast = {{"type", stat->statement_type}};
    // std::list<Expression>::iterator it = expressions.begin();

    // json expressions_json;
    // for (int i=0; i < expressions.size(); i++) {
    //     expressions_json["expression" + std::to_string(i)] = it->jsonify();
    //     std::advance(it, 1);
    // }
    ast["expression"] = jsonify_expression(stat->expression);
    return ast;
}

std::shared_ptr<Expression> parse_expression(std::list<std::string>& tokens) {
    auto exp = std::shared_ptr<Expression>(new Expression);

    exp->expressions.push_back(parse_expression_logic_and(tokens));

    while (tokens.front() == "||") {
        tokens.pop_front();

        exp->expressions.push_back(parse_expression_logic_and(tokens));
    }

    return exp;
}

json jsonify_expression(std::shared_ptr<Expression> exp) {
    json ast;

    if (exp->expressions.size() == 1) {
        auto expression = exp->expressions.front();
        ast["expression"] = jsonify_expression_logic_and(expression);
    } else {
        json expressions_json;

        auto expression = exp->expressions.begin();
        expressions_json += jsonify_expression_logic_and(*expression);
        std::advance(expression, 1);

        for (int i=0; i<exp->expressions.size()-1; i++) {
            expressions_json += "||";
            expressions_json += jsonify_expression_logic_and(*expression);
            std::advance(expression, 1);
        }

    }
    return ast;
}

std::shared_ptr<ExpressionLogicAnd> parse_expression_logic_and(std::list<std::string>& tokens) {
    auto exp = std::shared_ptr<ExpressionLogicAnd>(new ExpressionLogicAnd);

    exp->expressions.push_back(parse_expression_equality(tokens));

    while (tokens.front() == "&&") {
        tokens.pop_front();

        exp->expressions.push_back(parse_expression_equality(tokens));
    }

    return exp;
}

json jsonify_expression_logic_and(std::shared_ptr<ExpressionLogicAnd> exp) {
    json ast;

    if (exp->expressions.size() == 1) {
        auto expression = exp->expressions.front();
        ast["expression"] = jsonify_expression_equality(expression);
    } else {
        json expressions_json;

        auto expression = exp->expressions.begin();
        expressions_json += jsonify_expression_equality(*expression);
        std::advance(expression, 1);

        for (int i=0; i<exp->expressions.size()-1; i++) {
            expressions_json += "&&";
            expressions_json += jsonify_expression_equality(*expression);
            std::advance(expression, 1);
        }

        ast["expressions"] = expressions_json;
    }

    return ast;
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

json jsonify_expression_equality(std::shared_ptr<ExpressionEquality> exp) {
    json ast;

    if (exp->expressions.size() == 1) {
        auto expression = exp->expressions.front();
        ast["expression"] = jsonify_expression_relational(expression);
    } else {
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
    }

    return ast;
}

std::shared_ptr<ExpressionRelational> parse_expression_relational(std::list<std::string>& tokens) {
    auto exp = std::shared_ptr<ExpressionRelational>(new ExpressionRelational);

    exp->expressions.push_back(parse_expression_add_sub(tokens));

    while (tokens.front() == ">"  ||  tokens.front() == "<" ||
           tokens.front() == ">=" ||  tokens.front() == "<=") {
        exp->operators.push_back(tokens.front());
        tokens.pop_front();

        exp->expressions.push_back(parse_expression_add_sub(tokens));
    }

    return exp;
}

json jsonify_expression_relational(std::shared_ptr<ExpressionRelational> exp) {
    json ast;

    if (exp->expressions.size() == 1) {
        auto expression = exp->expressions.front();
        ast["expression"] = jsonify_expression_add_sub(expression);
    } else {
        auto expressions = exp->expressions.begin();
        auto op = exp->operators.begin();

        json expressions_json = {jsonify_expression_add_sub(*expressions)};
        std::advance(expressions, 1);
        for (int i=0; i<exp->operators.size(); i++) {
            expressions_json += *op;
            expressions_json += jsonify_expression_add_sub(*expressions);
            std::advance(expressions, 1);
            std::advance(op, 1);
        }

        ast["expressions"] = expressions_json;
    }

    return ast;
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

json jsonify_expression_add_sub(std::shared_ptr<ExpressionAddSub> exp) {
    json ast;

    if (exp->terms.size() == 1) {
        auto term = exp->terms.front();
        ast["term"] = jsonify_term(term);
    } else {
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
    }

    return ast;
}

std::shared_ptr<Term> parse_term(std::list<std::string>& tokens) {
    auto term = std::shared_ptr<Term>(new Term);
    term->factors.push_back(parse_factor(tokens));

    while (tokens.front() == "*" || tokens.front() == "/") {
        term->operators.push_back(tokens.front());
        tokens.pop_front();

        term->factors.push_back(parse_factor(tokens));
    }

    return term;
}

json jsonify_term(std::shared_ptr<Term> term) {
    json ast;

    if (term->factors.size() == 1) {
        auto factor = term->factors.front();
        ast["factor"] = jsonify_factor(factor);
    } else {
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
    }

    return ast;
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
    } else if (std::regex_match(tokens.front(), std::regex("[!~-]"))) {
        fac->factor_type = "unary_op";
        fac->unaryop = tokens.front();
        tokens.pop_front();
        fac->factor = parse_factor(tokens);
    } else {
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

json jsonify_factor(std::shared_ptr<Factor> fac) {
    json ast = {{"type", fac->factor_type}};

    if (fac->factor_type == "bracket_expression") {
        ast["expression"] = jsonify_expression(fac->expression);
    } else if (fac->factor_type == "unary_op") {
        ast["operation"] = fac->unaryop;
        ast["factor"] = jsonify_factor(fac->factor);
    } else {
        ast["value"] = fac->value;
    }

    return ast;
}

#define parser
#endif