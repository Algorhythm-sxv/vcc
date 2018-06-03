#include <fstream>
#include <iostream>

#include "lexer.hpp"
#include "parser_struct.hpp"
#include "codegen.hpp"

int main(int argc, char* argv[]) {
    std::ifstream file;
    file.open(argv[1]);

    std::list<std::string> tokens = lex(file);

    Program prog = parse_program(tokens);

    json ast = jsonify(prog);
    std::cout << ast.dump(4);

    // std::filebuf fb;
    // fb.open("out.s", std::ios::out);
    // std::ostream asm_out(&fb);
    // asm_out << codegen_x86(prog);
}