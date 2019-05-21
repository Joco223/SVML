#pragma once

#include <iostream>
#include <vector>
#include <algorithm>
#include <string>

#include "Lexer.h"

namespace Parser {

	struct definition {
		int first;
		std::string second;
		int line;
	};

	struct arg_def {
		Lexer::token def_type;
		std::string value;
	};

	struct instruction {
		int ins_type = -1;
		Lexer::token def_type; //Variable type or return type for functions
		Lexer::token identifier;
		std::vector<Lexer::token> expression;
		std::vector<std::vector<Lexer::token>> arguments; //Arguments for a function call
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

	/*
	Instruction types:
	1: Variable definition
	2: Function definition
	3: Variable change
	4: Function call
	5: If statement
	6: Else
	7: While
	8: Ret
	9: Struct //Not supported yet
	20: { //Removed when data tree is made
	21: } //Same
	*/

	tree_node* process(std::vector<Lexer::token>&, bool);
}
