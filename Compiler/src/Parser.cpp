#include "Parser.h"

namespace Parser {
	
	void print_error(std::string error) {
		std::cout << "\033[31m" << "ERROR" << "\033[0m - ";
		std::cout << error << '\n';
	}

	std::vector<expression_type> handle_expression(int& index, std::vector<Lexer::token>& tokens, int delimiter, int op_delimiter = 0) {
		Lexer::token next = tokens[++index];
		std::vector<expression_type> expression;
		
		while(next.type != delimiter && next.type != op_delimiter) {
			if(next.type == Lexer::lexer_identifier && tokens[index + 1].type == Lexer::lexer_ob) { //Handle if expression has a function call
				instruction fc(-1);
				fc.identifier = next;
				fc.ins_type = it_function_call;
				
				index++;
				next = tokens[++index];

				while(next.type != Lexer::lexer_cb) {
					std::vector<expression_type> new_expression = handle_expression(--index, tokens, Lexer::lexer_comma, Lexer::lexer_cb);
					fc.arguments.push_back(new_expression);
					if(tokens[index].type == Lexer::lexer_cb)
						break;
					next = tokens[++index];
				}
				expression.push_back(fc);
			}else if(next.type == Lexer::lexer_ob      	  || next.type == Lexer::lexer_cb         || 
					 next.type == Lexer::lexer_arithmetic || next.type == Lexer::lexer_logic      || 
					 next.type == Lexer::lexer_value      || next.type == Lexer::lexer_identifier) { //Check if its a valid expression character
				expression.push_back(next);
			}else{
				print_error("Invalid token: " + next.name + " - on line " + std::to_string(next.line));
				expression.clear();
				break;
			}
			next = tokens[++index];
		}

		return expression;
	}

	instruction handle_var_def(Lexer::token& token, int& index, std::vector<Lexer::token>& tokens) {
		instruction ins(-1);

		Lexer::token identifier = tokens[++index];
		if(identifier.type == Lexer::lexer_identifier) { //Check the identifier

			

			Lexer::token op = tokens[++index];
			if(op.type == Lexer::lexer_assign) { //Check if the variable is initialized
				ins.def_type = token;
				ins.identifier = identifier;
				ins.expression = handle_expression(index, tokens, Lexer::lexer_eoi);
				if(ins.expression.size() == 0)
					return ins;
				ins.ins_type = it_variable_definition;
			}else if(op.type == Lexer::lexer_ob) { //Handle function definition
				Lexer::token next = tokens[++index];
				
				std::vector<arg_def> arguments;
				
				while(true) {
					if(next.type == Lexer::lexer_cb && arguments.size() == 0)
						break;

					if(next.type == Lexer::lexer_comma || next.type == Lexer::lexer_cb) {
						if(tokens[index - 1].type == Lexer::lexer_identifier && tokens[index - 2].type == Lexer::lexer_type) {
							arguments.push_back({tokens[index - 2], tokens[index - 1].name});
						}else{
							print_error("Invalid function definition argument: " + tokens[index - 2].name + " " + tokens[index - 1].name + " - on line " + std::to_string(next.line));
							return ins;
						}
					}

					if(next.type == Lexer::lexer_cb)
						break;

					next = tokens[++index];

					if(next.type == Lexer::lexer_cb && tokens[index - 1].type == Lexer::lexer_comma) {
						print_error("Invalid comma in function definition - on line " + std::to_string(next.line));
						return ins;
					}
				}

				ins.ins_type = it_function_definition;
				ins.def_type = token;
				ins.identifier = identifier;
				ins.def_arguments = arguments;
			}else if(op.type == Lexer::lexer_eoi) { //Handle uninitialized variable
				ins.ins_type = it_variable_definition;
				ins.def_type = token;
				ins.identifier = identifier;
				ins.expression.clear();
			}else{
				print_error("Invalid varialbe/function declaration: " + op.name + " - on line " + std::to_string(op.line));
			}
		}else{
			print_error("Invalid identifier: " + identifier.name + " - on line " + std::to_string(identifier.line));
		}

		return ins;
	}

	instruction handle_vc_and_fnc(Lexer::token& token, int& index, std::vector<Lexer::token>& tokens) { //Variable change and function call
		instruction ins(-1);

		Lexer::token assign = tokens[++index];
		if(assign.type == Lexer::lexer_assign) { //Variable change
			ins.identifier = token;
			ins.expression = handle_expression(index, tokens, Lexer::lexer_eoi);
			if(ins.expression.size() == 0)
				return ins;
			ins.ins_type = it_variable_change;
		}else if(assign.type == Lexer::lexer_ob) { //Function call
			Lexer::token next = tokens[++index];

			while(next.type != Lexer::lexer_cb) {
				std::vector<expression_type> new_expression = handle_expression(index, tokens, Lexer::lexer_comma, Lexer::lexer_cb);
				if(new_expression.size() == 0)
					return ins;
				ins.arguments.push_back(new_expression);
			}

			ins.identifier = token;
			ins.ins_type = it_function_call;
		}else{
			print_error("Unknown operation on: " + token.name + " - on line " + std::to_string(token.line));
		}

		return ins;
	}

	instruction handle_if(Lexer::token& token, int& index, std::vector<Lexer::token>& tokens) {
		instruction ins(-1);

		Lexer::token ob = tokens[++index];
		if(ob.type == Lexer::lexer_ob) {
			ins.expression = handle_expression(index, tokens, Lexer::lexer_cb);
			if(ins.expression.size() == 0)
				return ins;
			ins.ins_type = it_if_statement;
		}else{
			print_error("No open bracket on if - on line " + std::to_string(token.line));
		}

		return ins;
	}

	instruction handle_while(Lexer::token& token, int& index, std::vector<Lexer::token>& tokens) {
		instruction ins(-1);

		Lexer::token ob = tokens[++index];
		if(ob.type == Lexer::lexer_ob) {
			ins.expression = handle_expression(index, tokens, Lexer::lexer_cb);
			if(ins.expression.size() == 0) 
				return ins;
			ins.ins_type = it_while;
		}else{
			print_error("No open bracket on: " + token.name + " " + ob.name + " - on line " + std::to_string(token.line));
		}

		return ins;
	}

	instruction handle_return(Lexer::token& token, int& index, std::vector<Lexer::token>& tokens) {
		instruction ins(-1);
		ins.expression = handle_expression(index, tokens, Lexer::lexer_eoi);
		if(ins.expression.size() == 0)
			return ins;
		ins.ins_type = it_return;
		return ins;
	}

	void print_node(tree_node* node, int& depth) {
		for(int i = 0; i < depth; i++) 
			std::cout << "---";

		switch(node->ins.ins_type) {
			case -1: 					  std::cout << "Code start:\n";          break;
			case  it_variable_definition: std::cout << "Variable definition.\n"; break;
			case  it_function_definition: std::cout << "Function definition.\n"; break;
			case  it_variable_change: 	  std::cout << "Variable change.\n";     break;
			case  it_function_call: 	  std::cout << "Function call.\n";       break;
			case  it_if_statement: 		  std::cout << "If statement.\n";        break;
			case  it_else: 				  std::cout << "Else statement.\n";      break;
			case  it_while: 			  std::cout << "While loop.\n";          break;
			case  it_return: 			  std::cout << "Return statement.\n";    break;
		}

		if(node->ins.ins_type != -1)
			depth++;

		for(int i = 0; i < node->nodes.size(); i++)
			print_node(node->nodes[i], depth);

		if(node->ins.ins_type != -1) 
			depth--;
	}

	bool check_defs(tree_node* node) {
		std::vector<definition> defs;

		int index = 0;
		for(tree_node* i : node->nodes) {
			int j_index = 0;
			for(auto& j : defs) {
				if(j_index != index) {
					if(i->ins.ins_type == it_variable_definition && j.type == def_variable && i->ins.identifier.name == j.name) {
						print_error("Double variable definition of variable: " + j.name + " - on line: " + std::to_string(i->ins.identifier.line));
						return false;
					}else if(i->ins.ins_type == it_function_definition && j.type == def_function && i->ins.identifier.name == j.name) {
						print_error("Double function definition of function: " + j.name + " - on line: " + std::to_string(i->ins.identifier.line));
						return false;
					}
				}
				j_index++;
			}


			if(i->ins.ins_type == it_variable_change) {
				defs.push_back({def_variable, i->ins.identifier.name, i->ins.identifier.line});
			}else if(i->ins.ins_type == it_function_definition) {
				defs.push_back({def_function, i->ins.identifier.name, i->ins.identifier.line});
			}
			index++;
		}

		return true;
	}

	tree_node* process(std::vector<Lexer::token>& tokens, bool debug) {
		std::vector<instruction> instructions;
		for(int i = 0; i < tokens.size(); i++) {
			Lexer::token current = tokens[i];
			instruction ins(-1);

			switch(current.type) {
				case Lexer::lexer_type:       ins = handle_var_def(current, i, tokens);    break;
				case Lexer::lexer_identifier: ins = handle_vc_and_fnc(current, i, tokens); break;
				case Lexer::lexer_while:	  ins = handle_while(current, i, tokens);      break;
				case Lexer::lexer_if:		  ins = handle_if(current, i, tokens);         break;
				case Lexer::lexer_return:	  ins = handle_return(current, i, tokens);     break;
				case Lexer::lexer_else:		  ins.ins_type = it_else;                      break;
				case Lexer::lexer_ocb:		  ins.ins_type = it_ocb;                       break;
				case Lexer::lexer_ccb:		  ins.ins_type = it_ccb;                       break;
			}

			if(ins.ins_type != -1)
				instructions.push_back(ins);

			std::cout << "\rParser: Processing tokens - [";
			float percent = (float)i / (float)tokens.size();
			int filled = ceil(percent * 20);
			int empty = 20 - filled;
			for(int j = 0; j < filled; j++)
				std::cout << '#';
			for(int j = 0; j < empty; j++)
				std::cout << '-';
			std::cout << "] - " << round(percent)*100 << "%/100%\r";
		}

		std::cout << '\n';

		//Turn the vector into a tree data structure

		tree_node* current = new tree_node;
		bool compile = true;

		for(int i = 0; i < instructions.size(); i++) {
			if(instructions[i].ins_type == it_ocb) {
				current = current->nodes[current->nodes.size()-1];
			}else if(instructions[i].ins_type == it_ccb) {
				if(current->prev == nullptr) {
					print_error("Extra } found.");
					compile = false;
					break;
				}else{
					current = current->prev;
				}
			}else{
				tree_node* tmp = new tree_node;
				tmp->ins = instructions[i];
				tmp->prev = current;
				current->nodes.push_back(tmp);
			}

			std::cout << "\rParser: Building instruction tree - [";
			float percent = (float)(i+1) / (float)instructions.size();
			int filled = ceil(percent * 20);
			int empty = 20 - filled;
			for(int j = 0; j < filled; j++)
				std::cout << '#';
			for(int j = 0; j < empty; j++)
				std::cout << '-';
			std::cout << "] - " << round(percent)*100 << "%/100%\r";
		}

		if(compile)
			compile = check_defs(current);

		if(compile) {
			if(debug) {
				int depth = 0;
				print_node(current, depth);
			}
			return current;
		}else{
			return nullptr;
		}	
	}
}
