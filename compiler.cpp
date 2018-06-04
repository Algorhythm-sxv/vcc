#include <fstream>
#include <iostream>

#include "lexer.hpp"
#include "parser.hpp"
#include "codegen.hpp"

int main(int argc, char* argv[]) {
    std::ifstream file;
    file.open(argv[1]);

    std::list<std::string> tokens = lex(file);

    Program prog(tokens);

    json ast = prog.jsonify();
    std::cout << ast.dump(4);

    std::filebuf fb;
    fb.open("out.s", std::ios::out);
    std::ostream asm_out(&fb);
    asm_out << codegen_x86(prog);
    fb.close();
}