#pragma once

#include <vector>
#include <string>
#include <variant>
#include <algorithm>
#include <math.h>
#include <chrono>

#include "Lexer.h"
#include "error_handling.h"

namespace Parser {
	enum types {
		t_void = 1,
		t_int,
		t_bool
	};

	const std::vector<std::string> types = {"void, int, bool"};

	enum instruction_type {
		it_variable_definition = 1,
		it_function_definition,
		it_variable_change,
		it_function_call,
		it_if_statement,
		it_else,
		it_while,
		it_return,
		it_ocb, //These two are temporary instructions in AST gen
		it_ccb
	};

	struct arg_def {
		int arg_type;
		std::string name;
	};

	struct instruction_error_info {
		int line, identifier_pos, identifier_length, file_index;
		std::string path;
	};

	struct instruction {
		int ins_type = -1;
		int def_type;
		instruction_error_info ins_err;
		std::string identifier;
		std::vector<std::vector<std::variant<Lexer::token, instruction>>> expressions;
		std::vector<arg_def> def_arguments;
	};

	struct tree_node {
		tree_node* prev = nullptr;
		instruction ins;
		std::vector<tree_node*> nodes;
	};

	struct pattern_key {
		std::vector<int> key_matches;
		bool repeatable;
	};

	enum pattern_types {
		p_uninit_var_def = 0,
		p_init_var_def,
		p_fn_def,
		p_var_change,
		p_fn_call,
		p_if_stat,
		p_else_stat,
		p_while_stat,
		p_return_stat
	};

	const std::vector<pattern_key> uninit_var_def = {{{Lexer::tt_type}, false}, {{Lexer::tt_identifier}, false}, {{Lexer::tt_eoi}, false}};
	const std::vector<pattern_key> init_var_def   = {{{Lexer::tt_type}, false}, {{Lexer::tt_identifier}, false}, {{Lexer::tt_assign}, false}, {{Lexer::tt_arithmetic, Lexer::tt_logic, Lexer::tt_identifier, Lexer::tt_value, Lexer::tt_ob, Lexer::tt_cb, Lexer::tt_comma}, true}, {{Lexer::tt_eoi}, false}};
	const std::vector<pattern_key> fn_def         = {{{Lexer::tt_type}, false}, {{Lexer::tt_identifier}, false}, {{Lexer::tt_ob}, false}, {{Lexer::tt_type, Lexer::tt_identifier, Lexer::tt_comma}, true}, {{Lexer::tt_cb}, false}};
	const std::vector<pattern_key> var_change     = {{{Lexer::tt_identifier}, false}, {{Lexer::tt_assign}, false}, {{Lexer::tt_arithmetic, Lexer::tt_logic, Lexer::tt_identifier, Lexer::tt_value, Lexer::tt_ob, Lexer::tt_cb, Lexer::tt_comma}, true}, {{Lexer::tt_eoi}, false}};
	const std::vector<pattern_key> fn_call        = {{{Lexer::tt_identifier}, false}, {{Lexer::tt_ob}, false}, {{Lexer::tt_arithmetic, Lexer::tt_logic, Lexer::tt_identifier, Lexer::tt_value, Lexer::tt_ob, Lexer::tt_cb, Lexer::tt_comma}, true}};
	const std::vector<pattern_key> if_stat        = {{{Lexer::tt_if}, false}, {{Lexer::tt_ob}, false}, {{Lexer::tt_arithmetic, Lexer::tt_logic, Lexer::tt_identifier, Lexer::tt_value, Lexer::tt_ob, Lexer::tt_cb, Lexer::tt_comma}, true}};
	const std::vector<pattern_key> else_stat      = {{{Lexer::tt_else}, false}};
	const std::vector<pattern_key> while_stat     = {{{Lexer::tt_while}, false}, {{Lexer::tt_ob}, false}, {{Lexer::tt_arithmetic, Lexer::tt_logic, Lexer::tt_identifier, Lexer::tt_value, Lexer::tt_ob, Lexer::tt_cb, Lexer::tt_comma}, true}};
	const std::vector<pattern_key> return_stat    = {{{Lexer::tt_return}, false}, {{Lexer::tt_arithmetic, Lexer::tt_logic, Lexer::tt_identifier, Lexer::tt_value, Lexer::tt_ob, Lexer::tt_cb, Lexer::tt_comma}, true}, {{Lexer::tt_eoi}, false}};

	const std::vector<std::vector<pattern_key>> patterns = {uninit_var_def, init_var_def, fn_def, var_change, fn_call, if_stat, else_stat, while_stat, return_stat};

	tree_node* parse(const std::vector<Lexer::token>&, bool, const int, const int);
}