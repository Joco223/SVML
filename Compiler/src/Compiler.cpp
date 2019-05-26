#include "Compiler.h"

namespace Compiler {

	std::vector<fn> functions;
	std::vector<r_ins> function_calls;

	int get_op_type(Lexer::token t) {
		if(t.name == "^")       {return 9;}
		else if(t.name == "*")  {return 8;}
		else if(t.name == "/")  {return 7;}
		else if(t.name == "+")  {return 6;}
		else if(t.name == "-")  {return 5;}
		else if(t.name == "%")  {return 4;}
		else if(t.name == "<")  {return 3;}
		else if(t.name == ">")  {return 3;}
		else if(t.name == "<=") {return 3;}
		else if(t.name == ">=") {return 3;}
		else if(t.name == "==") {return 3;}
		else if(t.name == "!=") {return 3;}
		else if(t.name == ")")  {return 2;}
		else if(t.name == "(")  {return 1;}
		else {return -1;}
	}

	std::vector<Lexer::token> parse_expression(std::vector<Lexer::token>& expression) {
		jci::Stack<Lexer::token> s;
		std::vector<Lexer::token> parsed;

		int e_index = 0;
		for(auto i : expression) {
			if(i.type != 4 && i.type != 5 && i.type != 6 && i.type != 12 && i.type != 13 && i.type != 14 && i.type != 15) {
				std::cout << "Invalid token: \"" << i.name << "\" in expression: \"";
				for(auto j : expression) {std::cout << j.name << ' ';}
				std::cout << '\b';
				std::cout << "\" on line: " << i.line << '\n';
				for(int j = 0; j < 34+i.name.length(); j++) {std::cout << '-';}
				for(int j = 0; j < e_index; j++) { for(int k = 0; k < expression[j].name.length()+1; k++) {std::cout << '-';}}
				for(int j = 0; j < i.name.length(); j++) {
					std::cout << '^';
				}
				parsed.clear();
				return parsed;
			}
			e_index++;
		}

		for(int i = 0; i < expression.size(); i++) {
			if(expression[i].type == 14 || expression[i].type == 15) {
				if(i < expression.size()-1) {
					if(expression[i].type == 15) {
						if(expression[i+1].type == 4) {	
							Lexer::token name = expression[i];
							i += 2;
							std::vector<std::vector<Lexer::token>> arguments;

							while(i < expression.size() && expression[i].type != 5) {
								std::vector<Lexer::token> argument;
								while(i < expression.size() && expression[i].type != 6 && expression[i].type != 5) {
									argument.push_back(expression[i]);
									i++;
								}
								arguments.push_back(argument);
								i++;
							}
							i++;

							r_ins ins;
							ins.type = 6;
							ins.identifier = name;
							std::vector<std::vector<binary_op>> arguments_ops;
							for(auto& i : arguments) { arguments_ops.push_back(evaluate_expression(i)); }
							ins.arguments_ops = arguments_ops;
							function_calls.push_back(ins);
							std::string a = "*" + std::to_string(function_calls.size()-1);
							parsed.push_back({15, expression[i].line, a});
						}else{
							parsed.push_back(expression[i]);
						}
					}else{
						parsed.push_back(expression[i]);
					}
				}else{
					parsed.push_back(expression[i]);
				}
			}else if(expression[i].type == 4) {
				s.push(expression[i]);
			}else if(expression[i].type == 5) {
				while(s.peek().type != 4) {
					parsed.push_back(s.peek());
					s.pop();
				}		
				s.pop();
			}else{
				int top;
				if(s.isEmpty()) {
					top = -1;
				}else{
					top = get_op_type(s.peek());
				} 
				int cur = get_op_type(expression[i]);

				if(cur > top || top == -1) {
					s.push(expression[i]);
				}else{
					while(cur < top) {
						if(!s.isEmpty()) {
							parsed.push_back(s.peek());
							s.pop();
							if(!s.isEmpty()) {
								top = get_op_type(s.peek());
							}else{
								top = -1;
							}
						}else{
							top = -1;
						}
					}
					s.push(expression[i]);
					
				}
			}
		}

		while(!s.isEmpty()) {
			parsed.push_back(s.peek());
			s.pop();
		}

		return parsed;
	}

	int get_op_type_e(std::string op) {
			 if(op == "!=") {return  1;}
		else if(op == "==") {return  2;}
		else if(op == ">=") {return  3;}
		else if(op == "<=") {return  4;}
		else if(op ==  ">") {return  5;}
		else if(op ==  "<") {return  6;}
		else if(op ==  "%") {return  7;}
		else if(op ==  "-") {return  8;}
		else if(op ==  "+") {return  9;}
		else if(op ==  "/") {return 10;}
		else if(op ==  "*") {return 11;}
		else if(op ==  "^") {return 12;}
		else                {return -1;}
	}

	

	std::vector<binary_op> evaluate_expression(std::vector<Lexer::token>& base_expression) {
		std::vector<Lexer::token> expression = parse_expression(base_expression);
		jci::Stack<Lexer::token> s;

		std::vector<binary_op> ops;

		int op_count = 0;
		for(int i = 0; i < expression.size(); i++) {
			if(expression[i].type == 14 || expression[i].type == 15) {
				s.push(expression[i]);				
			}else{
				std::string b = s.peek().name;
				s.pop();
				std::string a = s.peek().name;
				s.pop();
				s.push({14, 0, "#"+std::to_string(op_count)});
				op_count++;
				ops.push_back({a, b, get_op_type_e(expression[i].name)});
			}
		}

		if(ops.size() == 0) {
			ops.push_back({expression[0].name, "", -1});
		}

		return ops;
	}

	r_ins process_node(Parser::tree_node* node, std::vector<fn>& fns, r_ins* prev, fn* parent) {
		r_ins ins;
		ins.type = -1;
		ins.prev = prev;
		ins.parent = parent;
		switch(node->ins.ins_type) {
			case 1: //Variable definiton
				ins = {parent, prev, 1, node->ins.def_type, node->ins.identifier, evaluate_expression(node->ins.expression)};
				break;
			case 2: { //Function def
				fn n_fn;
				n_fn.def_type = node->ins.def_type;
				n_fn.identifier = node->ins.identifier;
				n_fn.def_arguments = node->ins.def_arguments;
				n_fn.start_pos = 0;
				for(auto& i : node->nodes) { n_fn.instructions.push_back(process_node(i, fns, nullptr, &n_fn)); }
				fns.push_back(n_fn);
				break; }
			case 3: //Variable change
				ins.type = 2;
				ins.identifier = node->ins.identifier;
				ins.expression_ops = evaluate_expression(node->ins.expression);
				break;
			case 4: { //Function call
				ins.type = 6;
				ins.identifier = node->ins.identifier;
				std::vector<std::vector<binary_op>> arguments_ops;
				for(auto& i : node->ins.arguments) { arguments_ops.push_back(evaluate_expression(i)); }
				ins.arguments_ops = arguments_ops;
				break; }
			case 5: //If statement
				ins.type = 3;
				ins.expression_ops = evaluate_expression(node->ins.expression);
				for(auto& i : node->nodes) { ins.child_instructions.push_back(process_node(i, fns, &ins, parent));}
				break;
			case 6: //Else statement
				ins.type = 4;
				for(auto& i : node->nodes) { ins.child_instructions.push_back(process_node(i, fns, &ins, parent));}
				break;
			case 7: //While statement
				ins.type = 5;
				ins.expression_ops = evaluate_expression(node->ins.expression);
				for(auto& i : node->nodes) { ins.child_instructions.push_back(process_node(i, fns, &ins, parent));}
				break;
			case 8: //Return
				ins.type = 7;
				ins.expression_ops = evaluate_expression(node->ins.expression);
				break;
		}
		return ins;
	}

	
	int all_var_count = 0;
	std::unordered_map<std::string, int> variables;
	std::vector<int> bytecode;

	void parse_operand(std::string operand, int op_count, std::vector<Parser::arg_def>* fn_vars = nullptr) {
		if(operand[0] == '#') {
			std::string tmp = operand.substr(1);
			bytecode.push_back(0x3);
			bytecode.push_back(op_count);
			bytecode.push_back(std::stoi(tmp)+all_var_count);		
		}else if(operand[0] == '*') {
			int index = std::stoi(operand.substr(1));
			process_instructions(function_calls[index], nullptr, fn_vars);
			bytecode.push_back(0x23);
			bytecode.push_back(op_count);
		}else if(fn_vars != nullptr) {
			bool found = false;
			for(int i = 0; i < fn_vars->size(); i++) {
				if(operand == fn_vars->at(i).value) {
					bytecode.push_back(0x20);
					bytecode.push_back(op_count);
					bytecode.push_back(i);
					found = true;
				}
			}
			if(!found) {
				bytecode.push_back(0x1);
				bytecode.push_back(op_count);
				bytecode.push_back(std::stoi(operand));
			}
		}else if(variables.count(operand) > 0) {
			bytecode.push_back(0x3);
			bytecode.push_back(op_count);
			bytecode.push_back(variables[operand]);
		}else{
			bytecode.push_back(0x1);
			bytecode.push_back(op_count);
			bytecode.push_back(std::stoi(operand));
		}
	}

	void handle_expression(std::vector<binary_op> exp, std::vector<Parser::arg_def>* fn_vars = nullptr) {
		int curr_var_count = 0;

		for(auto& i : exp) {
			if(i.op_type == -1) {
				parse_operand(i.a, 0, fn_vars);
			}else{	
				switch(i.op_type) {
					case 1:
						parse_operand(i.a, 0, fn_vars);
						parse_operand(i.b, 1, fn_vars);
						bytecode.push_back(0x15);
						bytecode.push_back(0);
						bytecode.push_back(1);
						bytecode.push_back(0x4);
						bytecode.push_back(0);
						bytecode.push_back(all_var_count+curr_var_count);
						break;
					case 2:
						parse_operand(i.a, 0, fn_vars);
						parse_operand(i.b, 1, fn_vars);
						bytecode.push_back(0x14);
						bytecode.push_back(0);
						bytecode.push_back(1);
						bytecode.push_back(0x4);
						bytecode.push_back(0);
						bytecode.push_back(all_var_count+curr_var_count);
						break;
					case 3:
						parse_operand(i.b, 0, fn_vars);
						parse_operand(i.a, 1, fn_vars);
						bytecode.push_back(0x17);
						bytecode.push_back(0);
						bytecode.push_back(1);
						bytecode.push_back(0x4);
						bytecode.push_back(0);
						bytecode.push_back(all_var_count+curr_var_count);
						break;
					case 4:
						parse_operand(i.a, 1, fn_vars);
						parse_operand(i.b, 0, fn_vars);
						bytecode.push_back(0x17);
						bytecode.push_back(0);
						bytecode.push_back(1);
						bytecode.push_back(0x4);
						bytecode.push_back(0);
						bytecode.push_back(all_var_count+curr_var_count);
						break;
					case 5:
						parse_operand(i.a, 0, fn_vars);
						parse_operand(i.b, 1, fn_vars);
						bytecode.push_back(0x16);
						bytecode.push_back(0);
						bytecode.push_back(1);
						bytecode.push_back(0x4);
						bytecode.push_back(0);
						bytecode.push_back(all_var_count+curr_var_count);
						break;
					case 6:
						parse_operand(i.a, 1, fn_vars);
						parse_operand(i.b, 0, fn_vars);
						bytecode.push_back(0x16);
						bytecode.push_back(0);
						bytecode.push_back(1);
						bytecode.push_back(0x4);
						bytecode.push_back(0);
						bytecode.push_back(all_var_count+curr_var_count);
						break;
					case 7:
						parse_operand(i.a, 0, fn_vars);
						parse_operand(i.b, 1, fn_vars);
						bytecode.push_back(0xB);
						bytecode.push_back(0);
						bytecode.push_back(1);
						bytecode.push_back(0x4);
						bytecode.push_back(0);
						bytecode.push_back(all_var_count+curr_var_count);
						break;
					case 8:
						parse_operand(i.a, 0, fn_vars);
						parse_operand(i.b, 1, fn_vars);
						bytecode.push_back(0x7);
						bytecode.push_back(0);
						bytecode.push_back(1);
						bytecode.push_back(0x4);
						bytecode.push_back(0);
						bytecode.push_back(all_var_count+curr_var_count);
						break;
					case 9:
						parse_operand(i.a, 0, fn_vars);
						parse_operand(i.b, 1, fn_vars);
						bytecode.push_back(0x6);
						bytecode.push_back(0);
						bytecode.push_back(1);
						bytecode.push_back(0x4);
						bytecode.push_back(0);
						bytecode.push_back(all_var_count+curr_var_count);
						break;
					case 10:
						parse_operand(i.a, 0, fn_vars);
						parse_operand(i.b, 1, fn_vars);
						bytecode.push_back(0xA);
						bytecode.push_back(0);
						bytecode.push_back(1);
						bytecode.push_back(0x4);
						bytecode.push_back(0);
						bytecode.push_back(all_var_count+curr_var_count);
						break;
					case 11:
						parse_operand(i.a, 0, fn_vars);
						parse_operand(i.b, 1, fn_vars);
						bytecode.push_back(0x8);
						bytecode.push_back(0);
						bytecode.push_back(1);
						bytecode.push_back(0x4);
						bytecode.push_back(0);
						bytecode.push_back(all_var_count+curr_var_count);
						break;
				}
			}
		curr_var_count++;
		}

		all_var_count += curr_var_count;
	}

	void handle_var_def(r_ins& ins, std::vector<Parser::arg_def>* fn_vars = nullptr) {
		handle_expression(ins.expression_ops, fn_vars);
		variables.insert({ins.identifier.name, all_var_count});
		bytecode.push_back(0x4);
		bytecode.push_back(0);
		bytecode.push_back(all_var_count);
		all_var_count++;
	}

	void handle_var_change(r_ins& ins, std::vector<Parser::arg_def>* fn_vars = nullptr) {
		handle_expression(ins.expression_ops, fn_vars);
		if(fn_vars != nullptr) {
			for(int i = 0; i < fn_vars->size(); i++) {
				if(ins.identifier.name == fn_vars->at(i).value) {
					bytecode.push_back(0x21);
					bytecode.push_back(0);
					bytecode.push_back(i);
				}
			}
		}else{
			bytecode.push_back(0x4);
			bytecode.push_back(0);
			bytecode.push_back(variables[ins.identifier.name]);
		}	
	}

	void handle_fun(r_ins& ins, std::vector<Parser::arg_def>* fn_vars = nullptr) {
		if(ins.identifier.name == "print") {
			handle_expression(ins.arguments_ops[0], fn_vars);
			bytecode.push_back(0x24);
			bytecode.push_back(0);
		}else if(ins.identifier.name == "printl") {
			bytecode.push_back(0x26);
		}else{
			bool found = false;
			int fn_counter = 0;
			for(auto& i : functions) {
				if(ins.identifier.name == i.identifier.name) {
					if(ins.arguments_ops.size() == i.def_arguments.size()) {
						found = true;

						for(auto& j : ins.arguments_ops) {
							handle_expression(j, fn_vars);
							bytecode.push_back(0x1E);
							bytecode.push_back(0);
						}

						bytecode.push_back(0x1F);
						bytecode.push_back(fn_counter);
					}else{
						std::cout << "Wrong number of arguments passed to function: " << ins.identifier.name << '\n';
					}
				}
				fn_counter++;
			}
			if(!found) {
				std::cout << "Function: " << ins.identifier.name << " has not been found.\n";
			}
		}
	}

	void handle_if(r_ins& ins, r_ins* next_ins = nullptr, std::vector<Parser::arg_def>* fn_vars = nullptr) {
		handle_expression(ins.expression_ops, fn_vars);
		bytecode.push_back(0x19);
		bytecode.push_back(0);
		int jmp_pos = bytecode.size();
		bytecode.push_back(0);

		for(int i = 0; i < ins.child_instructions.size(); i++) {
			if(i < ins.child_instructions.size()-1) {
				process_instructions(ins.child_instructions[i], &ins.child_instructions[i+1], fn_vars);
			}else{
				process_instructions(ins.child_instructions[i], nullptr, fn_vars);
			}
		}

		if(next_ins != nullptr) {
			if(next_ins->type == 4) {
				bytecode.push_back(0x18);
				int jmp_pos2 = bytecode.size();
				bytecode.push_back(0);

				bytecode[jmp_pos] = bytecode.size();

				for(int i = 0; i < next_ins->child_instructions.size(); i++) {
					if(i < next_ins->child_instructions.size()-1) {
						process_instructions(next_ins->child_instructions[i], &next_ins->child_instructions[i+1], fn_vars);
					}else{
						process_instructions(next_ins->child_instructions[i], nullptr, fn_vars);
					}
				}

				bytecode[jmp_pos2] = bytecode.size();
			}else{
				bytecode[jmp_pos] = bytecode.size();
			}
		}else{
			bytecode[jmp_pos] = bytecode.size();
		}
		
	}

	void handle_while(r_ins& ins, std::vector<Parser::arg_def>* fn_vars = nullptr) {
		int ret_pos = bytecode.size();
		handle_expression(ins.expression_ops, fn_vars);
		bytecode.push_back(0x19);
		bytecode.push_back(0);
		int jmp_pos = bytecode.size();
		bytecode.push_back(0);

		for(int i = 0; i < ins.child_instructions.size(); i++) {
			if(i < ins.child_instructions.size()-1) {
				process_instructions(ins.child_instructions[i], &ins.child_instructions[i+1], fn_vars);
			}else{
				process_instructions(ins.child_instructions[i], nullptr, fn_vars);
			}
		}

		bytecode.push_back(0x18);
		bytecode.push_back(ret_pos);

		bytecode[jmp_pos] = bytecode.size();
	}

	void handle_ret(r_ins& ins, std::vector<Parser::arg_def>* fn_vars = nullptr) {
		handle_expression(ins.expression_ops, fn_vars);
		bytecode.push_back(0x22);
	}

	void process_instructions(r_ins& ins, r_ins* next_ins, std::vector<Parser::arg_def>* fn_vars) {
		switch(ins.type) {
			case 1: //Variable definition
				handle_var_def(ins, fn_vars);
				break;
			case 2: //Variable change
				handle_var_change(ins, fn_vars);
				break;
			case 3: //If statement
				handle_if(ins, next_ins, fn_vars);
				break;
			case 4:
				//Nothing here since it chained with if statement
				break;
			case 5: //While statement
				handle_while(ins, fn_vars);
				break;
			case 6: //Function call
				handle_fun(ins, fn_vars);
				break;
			case 7: //Return
				handle_ret(ins, fn_vars);
				break;
			default:
				//for(auto& i : ins.child_instructions) {process_instructions(i);}

				for(int i = 0; i < ins.child_instructions.size(); i++) {
					if(i < ins.child_instructions.size()-1) {
						process_instructions(ins.child_instructions[i], &ins.child_instructions[i+1], fn_vars);
					}else{
						process_instructions(ins.child_instructions[i], nullptr, fn_vars);
					}
				}
		}
	}

	void compile(Parser::tree_node* code_tree) {
		r_ins parent_ins;
		
		for(auto& i : code_tree->nodes) {
			r_ins n_r_ins = process_node(i, functions, &parent_ins, nullptr);
			if(n_r_ins.type != -1) {
				parent_ins.child_instructions.push_back(n_r_ins);
			}
		}

		for(auto& i : functions) {
			i.start_pos = bytecode.size();
			for(auto& j : i.instructions) {
				process_instructions(j, nullptr, &i.def_arguments);
			}
		}

		int program_start = bytecode.size();

		process_instructions(parent_ins, nullptr, nullptr);

		auto output = std::fstream("output.svb", std::ios::out);
    	output << all_var_count << '\n';
		output << program_start << '\n';
		output << functions.size() << '\n';

		for(auto& i : functions) {
			output << i.start_pos << '\n';
		}

		for(auto& i : bytecode) {
			output << i << '\n';
		}

    	output.close();

		/*//For printing out the instruction tree
		int d = 0;
		print_r_ins(parent_ins, d);*/

		/*d = 0;
		for(auto& i : functions) {
			std::cout << "Function: " << i.identifier.name << '\n';
			std::cout << "Return type: " << i.def_type.name << '\n';
			std::cout << "Arguments: ";
			for(int j = 0; j < i.def_arguments.size(); j++) {
				if(j != 0) {std::cout << "           ";}
				std::cout << '#' << (j+1) << ' ' << i.def_arguments[j].def_type.name << ' ' << i.def_arguments[j].value << '\n';
			}
			d++;
			for(auto& j : i.instructions) { print_r_ins(j, d); }
			d--;
		}*/
	}

	bool var_exists(r_ins& ins, std::string& name) {
		r_ins* prev_s = ins.prev;

		while(prev_s != nullptr) {
			for(auto& i : prev_s->child_instructions) {
				if(i.type == 1) {
					if(i.identifier.name == name) {
						return true;
					}
				}
			}
			prev_s = prev_s->prev;
		}

		if(ins.parent != nullptr) {
			for(auto& i : ins.parent->instructions) {
				if(i.type == 1) {
					if(i.identifier.name == name) {
						return true;
					}
				}
			}
		}

		return false;
	}

	void handle_r_ins(r_ins& ins) {
		switch(ins.type) {
			case 1:
				handle_var_def(ins);
				break;
		}
	}
}

