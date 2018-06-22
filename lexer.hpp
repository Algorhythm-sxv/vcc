#include <fstream>
#include <iostream>
#include <list>
#include <regex>
#include <string>

std::list<std::string> lex(std::ifstream& file) {
    std::list<std::string> token_regexes = {
                                            // Keywords and grouping
                                            "int[^\\w]",                        // int keyword
                                            "float[^\\w]",                      // float keyword
                                            "return[^\\w]",                     // return keyword
                                            "if[^\\w]",                         // if keyword
                                            "else[^\\w]",                       // else keyword
                                            "for[^\\w]",                        // for keyword    
                                            "while[^\\w]",                      // while keyword
                                            "do[^\\w]",                         // do keyword
                                            "break[^\\w]",                      // break keyword
                                            "continue[^\\w]",                   // continue keyword
                                            "\\{", "\\}",                       // braces

                                            // Operators (in order of precedence)
                                            "\\(", "\\)",                       // brackets
                                            "\\+\\+[\\S\\s]",                   // postfix increment
                                            "--[\\S\\s]",                       // postfix decrement
                                        
                                        //  "\\+\\+[\\S\\s]",                   // prefix increment
                                        //  "--[\\S\\s]",                       // prefix decrement
                                            "![^=]",                            // logical NOT
                                            "~[^=]",                            // bitwise NOT
                                        //  "\\+[^=]",                          // unary plus (repeated below)
                                        //  "-[^=]",                            // negation (repeated below)

                                            "\\*[^=]",                          // multiplication
                                            "/[^=]",                            // division
                                            "%[^=]",                            // modulo

                                            "\\+[^=+]",                         // addition
                                            "-[^=-]",                            // subtraction

                                            "<<[^=]",                           // left shift
                                            ">>[^=]",                           // right shift

                                            ">=[\\S\\s]",                       // greater or equal comparison
                                            "<=[\\S\\s]",                       // lesser or equal comparison
                                            ">[^=>]",                           // greater comparison
                                            "<[^=<]",                           // lesser comparison

                                            "!=[\\S\\s]",                       // not equal comparison
                                            "==[\\S\\s]",                       // equal comparison

                                            "&[^&=]",                           // bitwise AND

                                            "\\^[^=]",                          // bitwise XOR

                                            "\\|[^\\|=]",                       // bitwise OR

                                            "&&[\\S\\s]",                       // logical AND

                                            "\\|\\|[\\S\\s]",                   // logical OR

                                            "=[^=]",                            // assignment
                                            "\\+=[\\S\\s]",                     // compound addition assignment
                                            "-=[\\S\\s]",                       // compound subtraction assignment
                                            "\\*=[\\S\\s]",                     // compound multiplication assignment
                                            "/=[\\S\\s]",                       // compound division assignment
                                            "%=[\\S\\s]",                       // compound modulo assignment
                                            "&=[\\S\\s]",                       // compound bitwise AND assignment
                                            "^=[\\S\\s]",                       // compound bitwise XOR assignment
                                            "\\|=[\\S\\s]",                     // compound bitwise OR assignment
                                            "<<=[\\S\\s]",                      // compound left shift assignment
                                            ">>=[\\S\\s]",                      // compound right shift assignment

                                            ",",                                // comma

                                            // Other
                                            ";",                                // semicolon
                                            ":",                                // colon
                                            "\\?",                              // question mark
                                            "[A-Za-z_]\\w*[^\\w]",              // identifiers
                                            "[0-9]*\\.[0-9]*[^0-9]",            // decimal float literals
                                            // add sci notation float literals (0.3e10)
                                            // add hex float literals (0x0.3e10)
                                            "0[xX][0-9a-fA-f]+[^0-9a-fA-F]",    // hex literals
                                            "0[^xX0-7]|0[0-7]+[^0-7.]",         // octal and zero literals
                                            "[1-9][0-9]*[^0-9.]"                // decimal literals
    };        
    std::list<std::string> tokens = {};
    std::string candidate;
    char c;
    while (file.peek() != EOF) {
        c = file.peek();
        if (std::regex_match(std::string(1, c), std::regex("\\s+"))) {
            if (candidate != "") {
                tokens.push_back(candidate);
                candidate = "";
            }
            file.get(c);
            continue;
        }
        
        bool regex_matched = false;
        for (std::string re : token_regexes) {
            try {
                if (std::regex_match(candidate + std::string(1,c), std::regex(re))) {
                    regex_matched = true;
                    if (candidate.length() >= 1) {
                        tokens.push_back(candidate);
                        candidate = "";
                    } else {
                        tokens.push_back(std::string(1, c));
                        file.get(c);
                    }
                    break;
                }
            } catch (std::regex_error) {
                throw std::runtime_error("bad regex in lexer: " + re + "\n");
            }
        }
        if (!regex_matched) {
            candidate += c;
            file.get(c);
        }
    }
    file.close();

    return tokens;
}