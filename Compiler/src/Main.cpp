#include <string>
#include <vector>
#include <iostream>

#include "Lexer.h"
#include "Parser.h"
#include "Compiler.h"
#include "cmd_arg_handler.h"

int main(int argc, char** argv) {
	std::vector<std::pair<std::string, std::string>> arguments = handle_args(argc, argv);

	if(arguments.size() > 0) {
		bool debug = false;
		std::string input_file;

		for(auto& i : arguments) {
			if(i.first == "debug")  {debug = true;}
			if(i.first == "input")  {input_file = i.second;}
		}

		std::vector<Lexer::token> tokens = Lexer::process(input_file);
		if(tokens.size() == 0) return -1;
		Parser::tree_node* code_tree = Parser::process(tokens, debug);
		Compiler::compile(code_tree);
	}

	/*//"1=2*(3^4-5)^(6+7*8)-9"
	//a b c d ^ e - f g h * + ^ * + i -

	std::vector<Lexer::token> expression = {{14, 0, "1"}, {12, 0, "+"}, {14, 0, "2"}, {12, 0, "*"}, {4, 0, "("},
											{14, 0, "3"}, {12, 0, "^"}, {14, 0, "4"}, {12, 0, "-"}, {14, 0, "5"},
											{5, 0, ")"},  {12, 0, "^"}, {4, 0, "("}, {14, 0, "6"}, {12, 0, "+"},
											{14, 0, "7"}, {12, 0, "*"}, {14, 0, "8"}, {5, 0, ")"}, {12, 0, "-"},
											{14, 0, "9"}, {13, 0, ">"}, {14, 0, "2"}};

	std::vector<Lexer::token> t = Compiler::parse_expression(expression);
	Compiler::evaluate_expression(t);*/

	return 0;
}
