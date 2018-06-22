#include <map>
#include <string>

#include "parser.hpp"

std::map<std::string, Function> ast_functions;
void typecheck_program(Program&);
void typecheck_function(Function&);
void typecheck_block_item(BlockItem&, std::map<std::string, std::string>&);
void typecheck_declaration_list(DeclarationList&, std::map<std::string, std::string>&);
void typecheck_declaration(Declaration&, std::map<std::string, std::string>&);
void typecheck_statement(Statement&, std::map<std::string, std::string>);
std::string typecheck_expression(Expression&, std::map<std::string, std::string>);
std::string typecheck_get_compatible_type(std::string, std::string);

std::string typecheck_get_compatible_type(std::string type1, std::string type2) {
    if (type1 == type2) {
        return type1;
    } else {
        return types[type1] < types[type2]? type1: type2;
    }
}

void typecheck_program(Program& prog) {
    for (auto func: prog.functions) {
        ast_functions[func->id] = *func;
        typecheck_function(*func);
    }
}

void typecheck_function(Function& func) {
    std::map<std::string, std::string> local_types;
    for (auto item: func.items) {
        typecheck_block_item(*item, local_types);
    }
}

void typecheck_block_item(BlockItem& item, std::map<std::string, std::string>& local_types) {
    if (item.item_type == "declaration")  {
        typecheck_declaration_list(*item.declaration_list, local_types);
    } else { // if (item->item_type == "statement") {
        typecheck_statement(*item.statement, local_types);
    }
}

void typecheck_declaration_list(DeclarationList& declist, std::map<std::string, std::string>& local_types) {
    for (auto decl: declist.declarations) {
        local_types[decl->var_id] = declist.var_type;
        typecheck_declaration(*decl, local_types);
    }
}

void typecheck_declaration(Declaration& decl, std::map<std::string, std::string>& local_types) {
    if (decl.initialised) {
        typecheck_expression(*decl.init_exp, local_types);
    }
}

void typecheck_statement(Statement& stat, std::map<std::string, std::string> local_types) {
    auto type = stat.statement_type;
    if (type == "return") {
        typecheck_expression(*stat.expression1, local_types);
    } else if (type == "conditional") {
        typecheck_expression(*stat.expression1, local_types);
        typecheck_statement(*stat.statement1, local_types);
        typecheck_statement(*stat.statement2, local_types);
    } else if (type.find("for") == 0) {
        if (type == "for_declaration") {
            typecheck_block_item(*stat.items.front(), local_types);
        } else { // if (type == "for_expression") {
            typecheck_expression(*stat.expression1, local_types);
        }
        typecheck_expression(*stat.expression2, local_types);
        typecheck_expression(*stat.expression3, local_types);
        typecheck_statement(*stat.statement1, local_types);
    } else if (type == "while" || type == "do") {
        typecheck_expression(*stat.expression1, local_types);
        typecheck_statement(*stat.statement1, local_types);
    } else if (type == "compound") {
        for (auto item: stat.items) {
            typecheck_block_item(*item, local_types);
        }
    } else if (type == "expression") {
        typecheck_expression(*stat.expression1, local_types);
    } else if (type == "continue" || type == "break") {
    } else {
        throw std::runtime_error("typecheck: unimplemented statement type: " + type + "\n");
    }
}

std::string typecheck_expression(Expression& exp, std::map<std::string, std::string> local_types) {
    std::string return_type;
    switch (exp.exp_class) {
        case ExpClass::comma: {
            ExpressionComma& exp_comma = dynamic_cast<ExpressionComma&>(exp);

            if (exp_comma.exp_type == "assignment") {
                for (auto expression: exp_comma.expressions) {
                    return_type = typecheck_expression(*expression, local_types);
                }
                exp_comma.return_type = return_type;
            } else { // if (exp_comma.exp_type == "null") {
                exp_comma.return_type = "void";
            }
            return exp_comma.return_type;
        }
        case ExpClass::assignment: {
            ExpressionAssignment& exp_assign = dynamic_cast<ExpressionAssignment&>(exp);
            if (exp_assign.exp_type == "conditional") {
                exp_assign.return_type = typecheck_expression(*exp_assign.expression, local_types);
            } else { // if (exp_assign.exp_type == "assignment") {
                return_type = typecheck_expression(*exp_assign.assign_exp, local_types);
                exp_assign.return_type = typecheck_get_compatible_type(return_type, local_types[exp_assign.assign_id]);
            }
            return exp_assign.return_type;
        }
        case ExpClass::conditional: {
            ExpressionConditional& exp_cond = dynamic_cast<ExpressionConditional&>(exp);
            if (exp_cond.exp_type == "logic_or") {
                exp_cond.return_type = typecheck_expression(*exp_cond.condition, local_types);
            } else { // if (exp_cond.return_type == "conditional") {
                typecheck_expression(*exp_cond.condition, local_types);

                auto type1 = typecheck_expression(*exp_cond.exp_true, local_types);
                auto type2 = typecheck_expression(*exp_cond.exp_false, local_types);
                exp_cond.return_type = typecheck_get_compatible_type(type1, type2);
            }
            return exp_cond.return_type;
        }
        case ExpClass::logicor: {
            ExpressionLogicOr& exp_lor = dynamic_cast<ExpressionLogicOr&>(exp);
            if (exp_lor.expressions.size() == 1) {
                exp_lor.return_type = typecheck_expression(*exp_lor.expressions.front(), local_types);
            } else {
                exp_lor.return_type == "int";

                for (auto expression: exp_lor.expressions) {
                    exp_lor.operand_types.push_back(typecheck_expression(*expression, local_types));
                }
            }
            return exp_lor.return_type;
        }
        case ExpClass::logicand: {
            ExpressionLogicAnd& exp_land = dynamic_cast<ExpressionLogicAnd&>(exp);
            if (exp_land.expressions.size() == 1) {
                exp_land.return_type = typecheck_expression(*exp_land.expressions.front(), local_types);
            } else {
                exp_land.return_type == "int";

                for (auto expression: exp_land.expressions) {
                    exp_land.operand_types.push_back(typecheck_expression(*expression, local_types));
                }
            }
            return exp_land.return_type;
        }
        case ExpClass::bitwiseor: {
            ExpressionBitwiseOr& exp_bor = dynamic_cast<ExpressionBitwiseOr&>(exp);
            if (exp_bor.expressions.size() == 1) {
                exp_bor.return_type = typecheck_expression(*exp_bor.expressions.front(), local_types);
            } else {
                exp_bor.return_type = "int";

                for (auto expression: exp_bor.expressions) {
                    return_type = typecheck_expression(*expression, local_types);
                    if (return_type != "int") {
                        throw std::runtime_error("invalid operand for binary |: got '" + return_type + "' \n");
                    }
                    exp_bor.operand_types.push_back(return_type);
                }
            }
            return exp_bor.return_type;
        }
        case ExpClass::bitwisexor: {
            ExpressionBitwiseXor& exp_xor = dynamic_cast<ExpressionBitwiseXor&>(exp);
            if (exp_xor.expressions.size() == 1) {
                exp_xor.return_type = typecheck_expression(*exp_xor.expressions.front(), local_types);
            } else {
                exp_xor.return_type = "int";

                for (auto expression: exp_xor.expressions) {
                    return_type = typecheck_expression(*expression, local_types);
                    if (return_type != "int") {
                        throw std::runtime_error("invalid operand for binary ^: got '" + return_type + "' \n");
                    }
                    exp_xor.operand_types.push_back(return_type);
                }
            }
            return exp_xor.return_type;
        }
        case ExpClass::bitwiseand: {
            ExpressionBitwiseAnd& exp_band = dynamic_cast<ExpressionBitwiseAnd&>(exp);
            if (exp_band.expressions.size() == 1) {
                exp_band.return_type = typecheck_expression(*exp_band.expressions.front(), local_types);
            } else {
                exp_band.return_type = "int";

                for (auto expression: exp_band.expressions) {
                    return_type = typecheck_expression(*expression, local_types);
                    if (return_type != "int") {
                        throw std::runtime_error("invalid operand for binary |: got '" + return_type + "' \n");
                    }
                    exp_band.operand_types.push_back(return_type);
                }
            }
            return exp_band.return_type;
        }
        case ExpClass::equality: {
            ExpressionEquality& exp_eq = dynamic_cast<ExpressionEquality&>(exp);
            if (exp_eq.expressions.size() == 1) {
                exp_eq.return_type = typecheck_expression(*exp_eq.expressions.front(), local_types);
            } else {
                exp_eq.return_type = "int";

                
                auto expression = exp_eq.expressions.begin();
                return_type = typecheck_expression(**expression, local_types);
                if (return_type != "int") {
                    throw std::runtime_error("invalid operand for binary " + exp_eq.operators.front() + ": got '" + return_type + "' \n");
                }
                std::advance(expression, 1);
                
                for (auto op: exp_eq.operators) {
                    return_type = typecheck_expression(**expression, local_types);
                    if (return_type != "int") {
                        throw std::runtime_error("invalid operand for binary " + op + ": got '" + return_type + "' \n");
                    }
                    std::advance(expression, 1);
                }
            }
            return exp_eq.return_type;
        }
        case ExpClass::relational: {
            ExpressionRelational& exp_rel = dynamic_cast<ExpressionRelational&>(exp);
            if (exp_rel.expressions.size() == 1) {
                exp_rel.return_type = typecheck_expression(*exp_rel.expressions.front(), local_types);
            } else {
                exp_rel.return_type = "int";

                
                auto expression = exp_rel.expressions.begin();
                return_type = typecheck_expression(**expression, local_types);
                if (return_type != "int") {
                    throw std::runtime_error("invalid operand for binary " + exp_rel.operators.front() + ": got '" + return_type + "' \n");
                }
                std::advance(expression, 1);
                
                for (auto op: exp_rel.operators) {
                    return_type = typecheck_expression(**expression, local_types);
                    if (return_type != "int") {
                        throw std::runtime_error("invalid operand for binary " + op + ": got '" + return_type + "' \n");
                    }
                    std::advance(expression, 1);
                }
            }
            return exp_rel.return_type;
        }
        case ExpClass::shift: {
            ExpressionShift& exp_shift = dynamic_cast<ExpressionShift&>(exp);
            if (exp_shift.expressions.size() == 1) {
                exp_shift.return_type = typecheck_expression(*exp_shift.expressions.front(), local_types);
            } else {
                exp_shift.return_type = "int";

                
                auto expression = exp_shift.expressions.begin();
                return_type = typecheck_expression(**expression, local_types);
                if (return_type != "int") {
                    throw std::runtime_error("invalid operand for binary " + exp_shift.operators.front() + ": got '" + return_type + "' \n");
                }
                std::advance(expression, 1);
                
                for (auto op: exp_shift.operators) {
                    return_type = typecheck_expression(**expression, local_types);
                    if (return_type != "int") {
                        throw std::runtime_error("invalid operand for binary " + op + ": got '" + return_type + "' \n");
                    }
                    std::advance(expression, 1);
                }
            }
            return exp_shift.return_type;
        }
        case ExpClass::add: {
            ExpressionAdd& exp_add = dynamic_cast<ExpressionAdd&>(exp);
            if (exp_add.expressions.size() == 1) {
                exp_add.return_type = typecheck_expression(*exp_add.expressions.front(), local_types);
            } else {                
                auto expression = exp_add.expressions.begin();
                return_type = typecheck_expression(**expression, local_types);
                std::advance(expression, 1);
                
                for (auto op: exp_add.operators) {
                    auto new_type = typecheck_expression(**expression, local_types);
                    if (new_type != return_type) {
                        return_type = typecheck_get_compatible_type(new_type, return_type);
                    }
                    std::advance(expression, 1);
                }
                exp_add.return_type = return_type;
            }
            return exp_add.return_type;
        }
        case ExpClass::mult: {
            ExpressionMult& exp_mult = dynamic_cast<ExpressionMult&>(exp);
            if (exp_mult.expressions.size() == 1) {
                exp_mult.return_type = typecheck_expression(*exp_mult.expressions.front(), local_types);
            } else {                
                auto expression = exp_mult.expressions.begin();
                return_type = typecheck_expression(**expression, local_types);
                std::advance(expression, 1);
                
                for (auto op: exp_mult.operators) {
                    auto new_type = typecheck_expression(**expression, local_types);
                    if (new_type != return_type) {
                        return_type = typecheck_get_compatible_type(new_type, return_type);
                    }
                    std::advance(expression, 1);
                }
                exp_mult.return_type = return_type;
            }
            return exp_mult.return_type;
        }
        case ExpClass::unary: {
            ExpressionUnary& exp_unary = dynamic_cast<ExpressionUnary&>(exp);
            if (exp_unary.exp_type == "postfix") {
                exp_unary.return_type = typecheck_expression(*exp_unary.postfix_exp, local_types);
            } else if (exp_unary.exp_type == "prefix") {
                exp_unary.return_type = local_types[exp_unary.prefix_id];
            } else { // if (exp_unary.exp_type == "unary_op") {
                exp_unary.return_type = typecheck_expression(*exp_unary.unary_exp, local_types);
                if (exp_unary.unaryop == "~") {
                    if (exp_unary.return_type != "int") {
                        throw std::runtime_error("wrong type argument to bit-complement\n");
                    }
                }
            }
            return exp_unary.return_type;
        }
        case ExpClass::postfix: {
            ExpressionPostfix& exp_post = dynamic_cast<ExpressionPostfix&>(exp);
            if (exp_post.exp_type == "const_int") {
                exp_post.return_type = "int";
            } else if (exp_post.exp_type == "const_float") {
                exp_post.return_type = "float";
            } else if (exp_post.exp_type == "variable") {
                if (!local_types.count(exp_post.id)) {
                    throw std::runtime_error("indentifier '" + exp_post.id + "' not defined in this scope\n");
                }
                exp_post.return_type = local_types[exp_post.id];
            } else if (exp_post.exp_type == "postfix") {
                exp_post.return_type = typecheck_expression(*exp_post.postfix_exp, local_types);
                // check for valid types
            } else if (exp_post.exp_type == "bracket_exp") {
                exp_post.return_type = typecheck_expression(*exp_post.bracket_exp, local_types);
            } else { // if (exp_post.exp_type == "function_call") {
                if (!ast_functions.count(exp_post.id)) {
                    throw std::runtime_error("function '" + exp_post.id + "' not defined\n");
                }
                auto function = ast_functions[exp_post.id];
                exp_post.return_type = function.return_type;

                for (auto arg: exp_post.args) {
                    exp_post.operand_types.push_back(typecheck_expression(*arg, local_types));
                }
            }
            return exp_post.return_type;
        }
        default:
            throw std::runtime_error("typecheck: unimplemented expression type");
            return std::string("");
    }
}