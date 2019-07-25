#pragma once

#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include <variant>

#include "Lexer.h"

namespace Parser {

	struct instruction;

	typedef std::vector<std::variant<Lexer::token, instruction>> expression_type;

	enum definition_type {
		def_variable = 1,
		def_function
	};

	struct definition {
		int type;
		std::string name;
		int line;
	};

	struct arg_def {
		Lexer::token def_type;
		std::string identifier;
	};

	struct instruction {
		int ins_type = -1;
		Lexer::token def_type; //Variable type or return type for functions
		Lexer::token identifier;
		expression_type expression;
		std::vector<expression_type> arguments; //Arguments for a function call
		std::vector<arg_def> def_arguments; //Arguments when defining a function

		instruction(int ins_type_) {
			ins_type = ins_type_;
		}
	};

	struct tree_node {
		tree_node* prev = nullptr;
		instruction ins = instruction(-1);
		std::vector<tree_node*> nodes;

		tree_node() {
			prev = nullptr;
			ins = instruction(-1);
		}
	};
	
	enum instruction_type {
		it_variable_definition = 1,
		it_function_definition,
		it_variable_change,
		it_function_call,
		it_if_statement,
		it_else,
		it_while,
		it_return,
		it_ocb,
		it_ccb
	};

	void print_error(std::string);
	tree_node* process(std::vector<Lexer::token>&, bool);
}
