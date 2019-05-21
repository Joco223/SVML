#pragma once

#include "Parser.h"
#include "Lexer.h"
#include "Stack.h"

#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <fstream>

namespace Compiler {

	struct binary_op {
		std::string a;
		std::string b;
		int op_type;
		/*
		Operation types: (Going from leat priority)
		-1 -    - Unary op (Assingement)
		1  - != - Different
		2  - == - Equals
		3  - >= - Bigger or equal
		4  - <= - Smaller or equal
		5  - >  - Bigger
		6  - <  - Smaller
		7  - %  - Modulo
		8  - -  - Subtraction
		9  - +  - Addition
		10 - /  - Division
		11 - *  - Multiplication
		12 - ^  - Power 
		*/
	};

	struct r_ins;

	struct fn {
		Lexer::token def_type; //Return type
		Lexer::token identifier;
		std::vector<Parser::arg_def> def_arguments;
		std::vector<r_ins> instructions;
	};

	struct r_ins { //Refined instruction
		fn* parent;
		r_ins* prev;
		int type;
		Lexer::token def_type; //For variable definitions
		Lexer::token identifier;
		std::vector<binary_op> expression_ops; //For variable definitions and changes
		std::vector<std::vector<binary_op>> arguments_ops; //For function calls
		std::vector<r_ins> child_instructions; //For If, Else and While statements

		/*
		Refined Instruction types:
		1 - Variable definition
		2 - Variable change
		3 - If statement
		4 - Else statement
		5 - While statement
		6 - Function call
		7 - Retun
		*/
	};

	void process_instructions(r_ins& ins);
	void handle_if(r_ins& ins);
	int get_op_type(std::vector<Lexer::token>& expression);
	std::vector<Lexer::token> parse_expression(std::vector<Lexer::token>& expression);
	std::vector<binary_op> evaluate_expression(std::vector<Lexer::token>& expression);
	void compile(Parser::tree_node* code_tree);
}
