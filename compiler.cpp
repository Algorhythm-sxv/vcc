#include <fstream>
#include <iostream>

#include "lexer.hpp"
#include "parser.hpp"
#include "codegen.hpp"

int main(int argc, char* argv[]) {
    std::ifstream file;
    file.open(argv[1]);

    std::list<std::string> tokens = lex(file);

    try {
        auto prog = parse_program(tokens);

        json ast = jsonify_program(prog);
        std::cout << ast.dump(4) << "\n";

        std::filebuf fb;
        fb.open("out.s", std::ios::out);
        std::ostream asm_out(&fb);
        asm_out << codegen_x86(prog);
        fb.close();
    } catch (const std::runtime_error& e) {
        std::cout << "Error: " << e.what();
        exit;
    }
}