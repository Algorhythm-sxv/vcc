/* tokens:
 * keywords:
 *      int
 *      return
 * unary operators:
 *      -
 *      !
 *      ~
 * other:
 *      {}
 *      ()
 *      ;
 *      identifiers ([A-Za-z]\w*)
 *      integer literals ([0-9]+)
 */
 
#include <fstream>
#include <iostream>
#include <list>
#include <regex>
#include <string>

std::list<std::string> lex(std::ifstream& file) {
    std::list<std::string> token_regexes = {"int[^\\w]",                        // int keyword
                                            "return[^\\w]",                     // return keyword
                                            "\\{", "\\}",                       // braces
                                            "\\(", "\\)",                       // brackets
                                            "!=[\\S\\s]",                       // not equal comparison
                                            "==[\\S\\s]",                       // equal comparison
                                            ">=[\\S\\s]",                       // greater or equal comparison
                                            "<=[\\S\\s]",                       // lesser or equal comparison
                                            ">[^=>]",                           // greater comparison
                                            "<[^=<]",                           // lesser comparison
                                            "\\|[^\\|]",                        // bitwise OR
                                            "\\^",                              // bitwise XOR
                                            "&[^&]",                            // bitwise AND
                                            "\\|\\|[\\S\\s]",                   // logical OR
                                            "&&[\\S\\s]",                       // logical AND
                                            ";",                                // semicolon
                                            "<<[\\S\\s]",                       // left shift
                                            ">>[\\S\\S]",                       // right shift
                                            "\\+[^=]",                          // addition
                                            "\\*[^=]",                          // multiplication
                                            "/[^=]",                            // division
                                            "%",                                // modulo
                                            "-[^=]",                            // subtraction / negation
                                            "![^=]",                            // logical NOT
                                            "~[^=]",                            // bitwise NOT
                                            "[A-Za-z_]\\w*[^\\w]",              // identifiers
                                            "0[xX][0-9a-fA-f]+[^0-9a-fA-F]",    // hex literals
                                            "0[^xX0-7]|0[0-7]+[^0-7]",          // octal and zero literals
                                            "[1-9][0-9]*[^0-9]"                 // decimal literals
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
        }
        if (!regex_matched) {
            candidate += c;
            file.get(c);
        }
    }
    file.close();

    return tokens;
}