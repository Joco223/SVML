#include <string>
#include <vector>
#include <iostream>
#include <iomanip>  
#include <chrono>

#include "Lexer.h"
#include "Parser.h"
#include "code_gen.h"
#include "cmd_arg_handler.h"
#include "error_handling.h"

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

		Error_handler::compilable = true;
		std::cout << std::fixed << std::setprecision(1);
		auto start = std::chrono::high_resolution_clock::now();
		std::vector<Lexer::token> tokens = Lexer::process(input_file, debug);
		Parser::tree_node* root = Parser::parse(tokens, debug, 0, tokens.size());
		code_gen::generate(root, output_file);
		auto end = std::chrono::high_resolution_clock::now();
		std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms\n";
	}

	return 0;
}
