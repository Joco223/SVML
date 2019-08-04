#include <string>
#include <vector>
#include <iostream>
#include <iomanip>  

#include "Lexer.h"
#include "Parser.h"
//#include "code_gen.h"
#include "cmd_arg_handler.h"

int main(int argc, char** argv) {
	std::vector<std::pair<std::string, std::string>> arguments = handle_args(argc, argv);

	if(arguments.size() > 0) {
		bool debug = false;
		std::string input_file;
		std::string output_file;

		for(auto& i : arguments) {
			if(i.first == "debug")  {debug = true;}
			if(i.first == "input")  {input_file = i.second;}
			if(i.first == "output") {output_file = i.second;}
		}

		std::ios::sync_with_stdio(false);
		std::cout << std::fixed << std::setprecision(1);
		std::vector<Lexer::token> tokens = Lexer::process(input_file, debug);
		Parser::parse(tokens, debug);
		
		/*if(tokens.size() == 0) return -1;
		std::cout << '\n';
		Parser::tree_node* code_tree = Parser::process(tokens, debug);
		if(code_tree == nullptr) return -1;	
		std::cout << '\n';
		code_gen::generate(code_tree, output_file);*/
	}

	return 0;
}
