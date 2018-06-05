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
    std::list<std::string> token_regexes = {"int[^\\w]",
                                            "return[^\\w]",
                                            "\\{", "\\}",
                                            "\\(", "\\)", 
                                            ";",
                                            "-",
                                            "!",
                                            "~",
                                            "\\+",
                                            "\\*",
                                            "/",
                                            "[A-Za-z_]\\w*[^\\w]",   // identifiers
                                            "0[xX][0-9a-fA-f]+[^0-9a-fA-F]",    // hex literals
                                            "0[0-7]+[^0-7]",          // octal literals
                                            "[1-9][0-9]*[^0-9]"};        // decimal literals

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