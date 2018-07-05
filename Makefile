all: compiler.exe

compiler.exe: compiler.cpp parser.hpp lexer.hpp codegen.hpp typechecker.hpp
	g++ -g -I "C:\Program Files\boost_1_67_0" -o $@ $<