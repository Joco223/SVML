#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <stack>

#include "Lexer.h"
#include "Parser.h"

namespace code_gen {
	struct instruction {
		unsigned char op_code;
		std::vector<unsigned int> args;
	};

	struct variable {
		std::string name;
		int type;
		int index;
	};

	struct scope {
		scope* prev = nullptr;
		std::vector<variable> variables;
		std::vector<scope> child_scopes;
	};

	struct function {
		std::string name;
		std::vector<instruction> instructions;
		scope* function_scope;
		int mem_count = 0;
	};

	std::vector<function> functions;
	std::vector<bool> used_registers = {false, false, false, false, false, false, false, false};

	void process_node(Parser::tree_node* node);
	void generate(Parser::tree_node* code_tree, std::string output_file_path);
}