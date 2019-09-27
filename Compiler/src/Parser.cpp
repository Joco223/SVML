#include "Parser.h"

namespace Parser {
	void print_info(std::chrono::high_resolution_clock::time_point start_t, const int i, const int end) {
		std::chrono::high_resolution_clock::time_point end_t = std::chrono::high_resolution_clock::now();
		std::cout << "\rParser: Processing tokens - [";
		float percent = (float)(i+1) / (float)end;
		int filled = ceil(percent * 20);
		int empty = 20 - filled;
		for(int j = 0; j < filled; j++)
			std::cout << '#';
		for(int j = 0; j < empty; j++)
			std::cout << '-';
		std::cout << "] - " << (percent*100) << "%, time left: " << ((((double)std::chrono::duration_cast<std::chrono::milliseconds>(end_t - start_t).count()/(double)(i+1)) * (end-i-1))/1000) << "s.";
		for(int k = 0; k < 20; k++)
			std::cout  << ' ';
		std::cout << '\r';
	}

	int get_type(Lexer::token type) {
		if(type.name == "void") {
			return t_void;
		}else if(type.name == "int") {
			return t_int;
		}else if(type.name == "bool") {
			return t_bool;
		}else{
			Error_handler::error_out("Unknown type: " + type.name + " on line: " + std::to_string(type.line));
			return -1;
		}
	}

	instruction match_pattern(const std::vector<Lexer::token>& tokens, int& i);

	instruction process_uninit_var_def(const std::vector<Lexer::token>& tokens, const int match_start) {
		instruction uninit_var_def;
		uninit_var_def.ins_type   = it_variable_definition;
		uninit_var_def.def_type   = get_type(tokens[match_start]);
		uninit_var_def.identifier = tokens[match_start + 1].name;
		uninit_var_def.line       = tokens[match_start + 1].line;
		return uninit_var_def;
	}

	instruction process_init_var_def(const std::vector<Lexer::token>& tokens, const int match_start, const int match_end) {
		instruction init_var_def;
		init_var_def.ins_type   = it_variable_definition;
		init_var_def.def_type   = get_type(tokens[match_start]);
		init_var_def.identifier = tokens[match_start + 1].name;
		init_var_def.line       = tokens[match_start + 1].line;

		std::vector<std::variant<Lexer::token, instruction>> expression;
		for(int i = match_start + 3; i <= match_end - 1; i++) {		
			instruction nested_instruction = match_pattern(tokens, i);
			if(nested_instruction.ins_type == -1) {			
				expression.push_back(tokens[i]);
			}else{
				expression.push_back(nested_instruction);
			}
		}
		init_var_def.expressions.push_back(expression);

		return init_var_def;
	}

	instruction process_fn_def(const std::vector<Lexer::token>& tokens, const int match_start, const int match_end) {
		instruction fn_def;
		fn_def.ins_type   = it_function_definition;
		fn_def.def_type   = get_type(tokens[match_start]);
		fn_def.identifier = tokens[match_start + 1].name;
		fn_def.line       = tokens[match_start + 1].line;

		for(int i = match_start + 3; i <= match_end - 1; i++) {
			
			arg_def argument;
			if(tokens[i].type != Lexer::tt_type) {
				Error_handler::error_out("Unknown type: " + tokens[i].name + " on line: " + std::to_string(tokens[i].line));
				break;
			}else{
				argument.arg_type = get_type(tokens[i]);
			}
			i++;

			if(i > match_end - 1) {
				break;
			}else{
				if(tokens[i].type != Lexer::tt_identifier) {
					Error_handler::error_out("Invalid identifier: " + tokens[i].name + " on line: " + std::to_string(tokens[i].line));
					break;
				}else{
					argument.name = tokens[i].name;
				}
			}
			
			i++;
			fn_def.def_arguments.push_back(argument);
		}
		return fn_def;
	}

	instruction process_var_change(const std::vector<Lexer::token>& tokens, const int match_start, const int match_end) {
		instruction var_change;
		var_change.ins_type   = it_variable_change;
		var_change.identifier = tokens[match_start].name;
		var_change.line       = tokens[match_start + 1].line;

		std::vector<std::variant<Lexer::token, instruction>> expression;
		for(int i = match_start + 2; i <= match_end - 1; i++) {
			instruction nested_instruction = match_pattern(tokens, i);
			if(nested_instruction.ins_type == -1) {
				expression.push_back(tokens[i]);
			}else{
				expression.push_back(nested_instruction);
			}
		}
		var_change.expressions.push_back(expression);

		return var_change;
	}

	instruction process_fn_call(const std::vector<Lexer::token>& tokens, const int match_start, const int match_end) {
		instruction fn_call;
		fn_call.ins_type   = it_function_call;
		fn_call.identifier = tokens[match_start].name; 
		fn_call.line       = tokens[match_start + 1].line;

		for(int i = match_start + 2; i <= match_end; i++) {
			std::vector<std::variant<Lexer::token, instruction>> expression;
			while(tokens[i].type != Lexer::tt_comma) {
				if(i >= match_end)
					break;

				instruction nested_instruction = match_pattern(tokens, i);
				if(nested_instruction.ins_type == -1){
					expression.push_back(tokens[i]);
				}else{
					expression.push_back(nested_instruction);
				}

				i++;
			}
			fn_call.expressions.push_back(expression);
		}

		return fn_call;
	}

	instruction process_if_stat(const std::vector<Lexer::token>& tokens, const int match_start, const int match_end) {
		instruction if_stat;
		if_stat.ins_type = it_if_statement;

		std::vector<std::variant<Lexer::token, instruction>> expression;
		for(int i = match_start + 2; i < match_end; i++) {
			instruction nested_instruction = match_pattern(tokens, i);
			if(nested_instruction.ins_type == -1){
				expression.push_back(tokens[i]);
			}else{
				expression.push_back(nested_instruction);
			}
		}
		if_stat.expressions.push_back(expression);

		return if_stat;
	}

	instruction process_else_stat() {
		instruction else_stat;
		else_stat.ins_type = it_else;
		return else_stat;
	}

	instruction process_while_stat(const std::vector<Lexer::token>& tokens, const int match_start, const int match_end) {
		instruction while_stat;
		while_stat.ins_type = it_while;

		std::vector<std::variant<Lexer::token, instruction>> expression;
		for(int i = match_start + 2; i < match_end; i++) {
			instruction nested_instruction = match_pattern(tokens, i);
			if(nested_instruction.ins_type == -1){
				expression.push_back(tokens[i]);
			}else{
				expression.push_back(nested_instruction);
			}
		}
		while_stat.expressions.push_back(expression);

		return while_stat;
	}

	instruction process_return_stat(const std::vector<Lexer::token>& tokens, const int match_start, const int match_end) {
		instruction return_stat;
		return_stat.ins_type = it_return;

		std::vector<std::variant<Lexer::token, instruction>> expression;
		for(int i = match_start + 1; i < match_end; i++) {
			instruction nested_instruction = match_pattern(tokens, i);
			if(nested_instruction.ins_type == -1){
				expression.push_back(tokens[i]);
			}else{
				expression.push_back(nested_instruction);
			}
		}
		return_stat.expressions.push_back(expression);

		return return_stat;
	}

	instruction process_match(const std::vector<Lexer::token>& tokens, const int match_start, const int match_end, const int match_type) {
		instruction invalid_instruction;
		switch(match_type) {
			case p_uninit_var_def: return process_uninit_var_def(tokens, match_start);            break;
			case p_init_var_def:   return process_init_var_def(tokens, match_start, match_end);   break;
			case p_fn_def:         return process_fn_def(tokens, match_start, match_end);         break;
			case p_var_change:     return process_var_change(tokens, match_start, match_end);     break;
			case p_fn_call:        return process_fn_call(tokens, match_start, match_end);        break;
			case p_if_stat:        return process_if_stat(tokens, match_start, match_end);        break;
			case p_else_stat:      return process_else_stat();                                    break;
			case p_while_stat:     return process_while_stat(tokens, match_start, match_end);     break;
			case p_return_stat:    return process_return_stat(tokens, match_start, match_end);    break;
			default: return invalid_instruction;
		}
	}

	instruction match_pattern(const std::vector<Lexer::token>& tokens, int& i) {
		instruction new_instruction;
		int pattern_type = 0;
		for(auto& pattern : patterns) {
			if(tokens[i].type == pattern[0].key_matches[0]) {
				bool final_match = false;
				int offset = 1;
				int index = 1;

				while(true) {
					if(offset >= pattern.size())
						break;
					final_match = std::find(pattern[offset].key_matches.begin(), pattern[offset].key_matches.end(), tokens[i + index].type) != pattern[offset].key_matches.end();;
					if(!final_match) {
						if(pattern[offset].repeatable) {
							if(offset < pattern.size()-1) {
								offset++;
								final_match = std::find(pattern[offset].key_matches.begin(), pattern[offset].key_matches.end(), tokens[i + index].type) != pattern[offset].key_matches.end();;
								if(!final_match) {
									break;
								}
							}else{
								final_match = true;
								break;
							}
						}else{
							break;
						}
					}
					if(!pattern[offset].repeatable)
						offset++;
					index++;
				}

				if(final_match) {

					new_instruction = process_match(tokens, i, i + index - 1, pattern_type);
					i = i + index-1;
					break;
				}
			}
			pattern_type++;
		}
		return new_instruction;
	}

	void print_instruction(instruction& ins) {
		switch(ins.ins_type) {
			case it_variable_definition: {
				std::cout << "---Variable definition---\n";
				std::cout << "Variable type: " << ins.def_type << '\n';
				std::cout << "Variable name: " << ins.identifier << '\n';
				std::cout << "Variable expression:\n";
				for(auto& i : ins.expressions[0]) {	
					if(i.index() == 0) {
						std::cout << std::get<Lexer::token>(i).name << ' ';
					}else{
						print_instruction(std::get<instruction>(i));
					}
				}
				std::cout << '\n';
				break; }
			case it_function_definition: {
				std::cout << "---Function definition---\n";
				std::cout << "Function return type: " << ins.def_type << '\n';
				std::cout << "Function name: " << ins.identifier << '\n';
				std::cout << "Function definitnion arguments:\n";
				int count = 0;
				for(auto& i : ins.def_arguments) {
					std::cout << "Argument type: " << i.arg_type << '\n';
					std::cout << "Argument name: " << i.name << '\n';
					count++;
				}
				std::cout << '\n';
				break; }
			case it_variable_change: {
				std::cout << "---Variable change---\n";
				std::cout << "Changed variable name: " << ins.identifier << '\n';
				std::cout << "Changed variable expression:\n";
				for(auto& i : ins.expressions[0]) {	
					if(i.index() == 0) {
						std::cout << std::get<Lexer::token>(i).name << ' ';
					}else{
						print_instruction(std::get<instruction>(i));
					}
				}
				std::cout << '\n';
				break; }
			case it_function_call: {
				std::cout << "---Function call---\n";
				std::cout << "Function name: " << ins.identifier << '\n';
				std::cout << "Arguments:\n";
				int arg_count = 1;
				for(auto& i : ins.expressions) {
					std::cout << "Argument #" << arg_count << ": ";
					for(auto& j : i) {	
						if(j.index() == 0) {
							std::cout << std::get<Lexer::token>(j).name << ' ';
						}else{
							print_instruction(std::get<instruction>(j));
						}
					}
					arg_count++;
					std::cout << '\n';
				}
				break; }
			case it_if_statement: {
				std::cout << "---If statement---\n";
				std::cout << "Expression:\n";
				for(auto& i : ins.expressions[0]) {	
					if(i.index() == 0) {
						std::cout << std::get<Lexer::token>(i).name << ' ';
					}else{
						print_instruction(std::get<instruction>(i));
					}
				}
				std::cout << '\n';
				break; }
			case it_else: {
				std::cout << "---Else statement---\n";
				break; }
			case it_while: {
				std::cout << "---While statement---\n";
				std::cout << "Expression:\n";
				for(auto& i : ins.expressions[0]) {			
					if(i.index() == 0) {
						std::cout << std::get<Lexer::token>(i).name << ' ';
					}else{
						print_instruction(std::get<instruction>(i));
					}
				}
				std::cout << '\n';
				break; }
			case it_return: {
				std::cout << "---Return statement---\n";
				std::cout << "Expression:\n";
				for(auto& i : ins.expressions[0]) {
					if(i.index() == 0) {
						std::cout << std::get<Lexer::token>(i).name << ' ';
					}else{
						print_instruction(std::get<instruction>(i));
					}
				}
				std::cout << '\n';
				break; }
		}
	}

	tree_node* parse(const std::vector<Lexer::token>& tokens, bool debug, const int start, const int end) {
		std::chrono::high_resolution_clock::time_point start_t = std::chrono::high_resolution_clock::now();

		std::vector<instruction> instructions;

		for(int i = start; i < end; i++) {
			instruction new_instruction;
			if(tokens[i].type == Lexer::tt_ocb) {
				new_instruction.ins_type = it_ocb;
				instructions.push_back(new_instruction);
				continue;
			}else if(tokens[i].type == Lexer::tt_ccb){
				new_instruction.ins_type = it_ccb;
				instructions.push_back(new_instruction);
				continue;
			}

			new_instruction = match_pattern(tokens, i);		

			if(new_instruction.ins_type != -1)
				instructions.push_back(new_instruction);
		}

		tree_node* code_root = new tree_node;
		tree_node* current_node = code_root;

		for(int i = 0; i < instructions.size(); i++) {
			if(instructions[i].ins_type == it_ocb) {
				current_node = current_node->nodes[current_node->nodes.size()-1];
			}else if(instructions[i].ins_type == it_ccb) {
				if(current_node->prev == nullptr) {
					break;
				}else{
					current_node = current_node->prev;
				}
			}else{
				tree_node* tmp = new tree_node;
				tmp->ins = instructions[i];
				tmp->prev = current_node;
				current_node->nodes.push_back(tmp);
			}
		}

		return code_root;
	}
}