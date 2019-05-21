#include "Parser.h"

namespace Parser {

	instruction handle_var_def(Lexer::token& token, int& index, std::vector<Lexer::token>& tokens) {
		instruction ins(-1);

		Lexer::token identifier = tokens[++index];
		if(identifier.type == 15) { //Check the identifier
			Lexer::token op = tokens[++index];
			if(op.type == 7) { //Check if the variable is initialized
				Lexer::token next = tokens[++index];
				std::vector<Lexer::token> expression;
				while(next.type != 1) {

					if(next.type == 4 || next.type == 5 || next.type == 12 || next.type == 13 || next.type == 14 || next.type == 15) { //Check if its a valid expression character
						expression.push_back(next);
					}else{
						std::cout << "Invalid token: " << next.type << " " << next.name << " at: " << token.name << " " << identifier.name << " on line: " << identifier.line << ".\n";
						return ins;
						break;
					}

					next = tokens[++index];
				}

				ins.ins_type = 1;
				ins.def_type = token;
				ins.identifier = identifier;
				ins.expression = expression;

				/*std::cout << "-----\n";
				std::cout << "Variable definition: \n";
				std::cout << "Type: " << token.name << ".\n";
				std::cout << "Identifier: " << identifier.name << ".\n";
				std::cout << "Value: " << expression << ".\n-----\n";*/
			}else if(op.type == 4) { //Handle function definition
				Lexer::token next = tokens[++index];

				/*std::cout << "-----\n";
				std::cout << "Function definition:\n";
				std::cout << "Type: " << type.name << ".\n";
				std::cout << "Identifier: " << identifier.name << ".\n";*/
				Lexer::token arg_identifier;
				std::vector<arg_def> arguments;

				while(next.type == 15 || next.type == 8 || next.type == 6) {
					Lexer::token old_next = next;
					if(next.type == 8) {
						Lexer::token arg_identifier = tokens[++index];
						//std::cout << "Argument: " << next.name << " " << arg_identifier.name << ".\n";
						arguments.push_back({next, arg_identifier.name});
					}else if(next.type != 6){
						std::cout << "Unknown token: " << next.name << " on: " << token.name << " " << identifier.name << ".\n";
						return ins;
					}
					next = tokens[++index];
				}

				ins.ins_type = 2;
				ins.def_type = token;
				ins.identifier = identifier;
				ins.def_arguments = arguments;

				//std::cout << "-----\n";
			}else{
				std::cout << "Variable cannot be left unitialized on: " << token.name << " " << identifier.name << ".\n";
			}
		}else{
			std::cout << "Improper identifier on: " << token.name << " " << identifier.name << ".\n";
		}

		return ins;
	}

	instruction handle_vc_and_fnc(Lexer::token& token, int& index, std::vector<Lexer::token>& tokens) {
		instruction ins(-1);

		Lexer::token assign = tokens[++index];
		if(assign.type == 7) {
			Lexer::token next = tokens[++index];
			std::vector<Lexer::token> expression;
			while(next.type != 1) {

				if(next.type == 4 || next.type == 5 || next.type == 12 || next.type == 13 || next.type == 14 || next.type == 15) { //Check if its a valid expression character
					expression.push_back(next);
				}else{
					std::cout << "Invalid token: " << next.type << " " << next.name << " at: " << token.name << ".\n";
					return ins;
				}

				next = tokens[++index];
			}

			ins.ins_type = 3;
			ins.identifier = token;
			ins.expression = expression;

			/*std::cout << "-----\n";
			std::cout << "Variable change: \n";
			std::cout << "Identifier: " << token.name << ".\n";
			std::cout << "Value: " << expression << ".\n-----\n";*/
		}else if(assign.type == 4) {
			Lexer::token next = tokens[++index];

			/*std::cout << "-----\n";
			std::cout << "Function call:\n";
			std::cout << "Identifier: " << token.name << ".\n";
			std::cout << "Arguments: \n";*/

			ins.identifier = token;

			while(next.type != 5) {
				std::vector<Lexer::token> expression;
				while(next.type != 6) {

					if(next.type == 5) {break;}

					if(next.type == 4 || next.type == 5 || next.type == 12 || next.type == 13 || next.type == 14 || next.type == 15) { //Check if its a valid expression character
						expression.push_back(next);
					}else{
						std::cout << "Invalid token: " << next.type << " " << next.name << " at: " << token.name << ".\n";
						return ins;
					}

					next = tokens[++index];
				}

				if(next.type == 6) {
					next = tokens[++index];
				}

				ins.arguments.push_back(expression);
				//std::cout << "Argument: " << expression << ".\n";
			}

			ins.ins_type = 4;

			//std::cout << "-----\n";

		}else{
			std::cout << "No assignement operator or open bracket on: " << token.name << " " << assign.name << ".\n";
		}

		return ins;
	}

	instruction handle_if(Lexer::token& token, int& index, std::vector<Lexer::token>& tokens) {
		instruction ins(-1);

		Lexer::token ob = tokens[++index];
		if(ob.type == 4) {
			Lexer::token next = tokens[++index];
			std::vector<Lexer::token> expression;
			while(next.type != 5) {

				if(next.type == 4 || next.type == 5 || next.type == 12 || next.type == 13 || next.type == 14 || next.type == 15) { //Check if its a valid expression character
					expression.push_back(next);
				}else{
					std::cout << "Invalid token: " << next.type << " " << next.name << " at: " << token.name << ".\n";
					return ins;
				}

				next = tokens[++index];
			}

			ins.ins_type = 5;
			ins.expression = expression;

			/*std::cout << "-----\n";
			std::cout << "If statement:\n";
			std::cout << "Condition: " << expression << ".\n";
			std::cout << "-----\n";*/
		}else{
			std::cout << "No open bracket on: " << token.name << " " << ob.name << ".\n";
		}

		return ins;
	}

	instruction handle_else(Lexer::token& token, int& index, std::vector<Lexer::token>& tokens) {
		instruction ins(-1);

		Lexer::token ob = tokens[++index];
		if(ob.type == 4) {
			Lexer::token next = tokens[++index];
			std::vector<Lexer::token> expression;
			while(next.type != 5) {
				if(next.type == 4 || next.type == 5 || next.type == 12 || next.type == 13 || next.type == 14 || next.type == 15) { //Check if its a valid expression character
					expression.push_back(next);
				}else{
					std::cout << "Invalid token: " << next.type << " " << next.name << " at: " << token.name << ".\n";
					return ins;
				}

				next = tokens[++index];
			}

			ins.ins_type = 7;
			ins.expression = expression;

			/*std::cout << "-----\n";
			std::cout << "If statement:\n";
			std::cout << "Condition: " << expression << ".\n";
			std::cout << "-----\n";*/
		}else{
			std::cout << "No open bracket on: " << token.name << " " << ob.name << ".\n";
		}

		return ins;
	}

	instruction handle_ret(Lexer::token& token, int& index, std::vector<Lexer::token>& tokens) {
		instruction ins(-1);

		Lexer::token next = tokens[++index];
		std::vector<Lexer::token> expression;
		while(next.type != 1) {
			if(next.type == 4 || next.type == 5 || next.type == 12 || next.type == 13 || next.type == 14 || next.type == 15) { //Check if its a valid expression character
				expression.push_back(next);
			}else{
				std::cout << "Invalid token: " << next.type << " " << next.name << " at: " << token.name << ".\n";
				return ins;
			}

			next = tokens[++index];
		}

		ins.ins_type = 8;
		ins.expression = expression;
		return ins;
	}

	instruction handle_struct(Lexer::token& token, int& index, std::vector<Lexer::token>& tokens) {
		instruction ins(-1);

		Lexer::token identifier = tokens[++index];
		if(identifier.type == 15) {
			ins.ins_type = 9;
			ins.identifier = identifier;
		}else{
			std::cout << "Improper identifier on: " << token.name << " " << identifier.name << ".\n";
		}
		return ins;
	}

	void print_node(tree_node* node, int& depth) {
		for(int i = 0; i < depth; i++) 
			std::cout << "---";

		switch(node->ins.ins_type) {
			case -1: std::cout << "Code start:\n";          break;
			case  1: std::cout << "Variable definition.\n";  break;
			case  2: std::cout << "Function definition.\n"; break;
			case  3: std::cout << "Variable change.\n";     break;
			case  4: std::cout << "Function call.\n";       break;
			case  5: std::cout << "If statement.\n";        break;
			case  6: std::cout << "Else statement.\n";      break;
			case  7: std::cout << "While loop.\n";          break;
			case  8: std::cout << "Return statement.\n";    break;
			case  9: std::cout << "Struct definition.\n";   break;
		}

		if(node->ins.ins_type != -1)
			depth++;

		for(int i = 0; i < node->nodes.size(); i++)
			print_node(node->nodes[i], depth);

		if(node->ins.ins_type != -1) 
			depth--;
	}

	void check_defs(tree_node* node, bool& not_success) {
		std::vector<definition> defs;
		//0 - Variable definition
		//1 - Function definition

		int index = 0;
		for(tree_node* i : node->nodes) {
			int j_index = 0;
			for(auto& j : defs) {
				if(j_index != index) {
					if(i->ins.ins_type == 1 && j.first == 0 && i->ins.identifier.name == j.second) {
						not_success = true;
						std::cout << "Error, double variable definition of variable: " << j.second << ", on line: " << i->ins.identifier.line << ".\n";
						return;
					}else if(i->ins.ins_type == 2 && j.first == 1 && i->ins.identifier.name == j.second) {
						not_success = true;
						std::cout << "Error, double function definition of variable: " << j.second << ", on line: " << i->ins.identifier.line << ".\n";
						return;
					}
				}
				j_index++;
			}


			if(i->ins.ins_type == 1) {
				defs.push_back({0, i->ins.identifier.name, i->ins.identifier.line});
			}else if(i->ins.ins_type == 2) {
				defs.push_back({1, i->ins.identifier.name, i->ins.identifier.line});
			}
			index++;
		}
	}

	tree_node* process(std::vector<Lexer::token>& tokens, bool debug) {
		std::vector<instruction> instructions;
		for(int i = 0; i < tokens.size(); i++) {
			Lexer::token current = tokens[i];
			if(current.type != -1) {
				if(current.type == 8) {
					instruction ins = handle_var_def(current, i, tokens);
					if(ins.ins_type == -1) {
						break;
					}else{
						instructions.push_back(ins);
					}
				}else if(current.type == 15) {
					instruction ins = handle_vc_and_fnc(current, i, tokens); //Variable change and function call
					if(ins.ins_type == -1) {
						break;
					}else{
						instructions.push_back(ins);
					}
				}else if(current.type == 16) {
					instruction ins = handle_else(current, i, tokens);
					if(ins.ins_type == -1) {
						break;
					}else{
						instructions.push_back(ins);
					}
				}else if(current.type == 10) {
					instruction ins = handle_if(current, i, tokens);
					if(ins.ins_type == -1) {
						break;
					}else{
						instructions.push_back(ins);
					}
				}else if(current.type == 11) {
					instruction ins(6); //else
					instructions.push_back(ins);
				}else if(current.type == 9) {
					instruction ins = handle_ret(current, i, tokens);
					if(ins.ins_type == -1) {
						break;
					}else{
						instructions.push_back(ins);
					}
				}else if(current.type == 17) {
					instruction ins = handle_struct(current, i, tokens);
					if(ins.ins_type == -1) {
						break;
					}else{
						instructions.push_back(ins);
					}
				}else if(current.type == 2) {
					instruction ins(20); // {
					instructions.push_back(ins);
				}else if(current.type == 3) {
					instruction ins(21); // }
					instructions.push_back(ins);
				}
			}
		}

		//Turn the vector into a tree data structure

		tree_node* current = new tree_node;
		bool dont_compile = false;

		for(int i = 0; i < instructions.size(); i++) {
			if(instructions[i].ins_type == 20) {
				current = current->nodes[current->nodes.size()-1];
			}else if(instructions[i].ins_type == 21) {
				if(current->prev == nullptr) {
					std::cout << "Extra } found. Please remove excess.\n";
					dont_compile = true;
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
		}

		check_defs(current, dont_compile);

		if(!dont_compile) {
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
