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

	enum byte_codes {
		bc_set = 1,
		bc_read_reg,
		bc_read_mem,
		bc_write_mem,
		bc_alloc,
		bc_dealloc,
		bc_add,
		bc_sub,
		bc_mult,
		bc_div_u,
		bc_div_s,
		bc_mod,
		bc_inc,
		bc_dec,
		bc_clr,
		bc_and,
		bc_or,
		bc_not,
		bc_bsr,
		bc_bsl,
		bc_equ,
		bc_diff,
		bc_bigger,
		bc_bigger_equ,
		bc_jmp,
		bc_jmp_z,
		bc_jmp_o,
		bc_call,
		bc_ret,
		bc_out
	};

	void process_node(Parser::tree_node* node, std::vector<instruction>& instructions, scope* local_scope, int& mem_count);
	void generate(Parser::tree_node* code_tree, std::string output_file_path);
}