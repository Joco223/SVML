#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <stack>
#include <fstream>
#include <math.h>

#include "Lexer.h"
#include "Parser.h"
#include "error_handling.h"

namespace code_gen {
	struct instruction {
		unsigned char op_code;
		std::vector<unsigned int> args;
	};

	struct variable {
		std::string name;
		int type;
		int index; //Index where the variable is in memory
	};

	struct scope {
		scope* prev = nullptr;
		std::vector<variable> variables;
		std::vector<scope*> child_scopes;
	};

	struct function {
		std::string name;
		std::vector<instruction> instructions;
		scope* function_scope;
		int mem_count = 0;
	};

	void process_node(Parser::tree_node* node, std::vector<instruction>& instructions, scope* local_scope, int& mem_count);
	void generate(Parser::tree_node* code_tree, std::string output_file_path);
}