#include "code_gen.h"

namespace code_gen {

	void print_error(std::string error) {
		std::cout << "\033[31m" << "ERROR" << "\033[0m - ";
		std::cout << error << '\n';
	}

	int find_free_reg() {
		int index = -1;
		for(int i = 0; i < 8; i++) {
			if(!used_registers[i])
				index = i;
		}
		return index;
	}

	int find_variable(scope* local_scope, std::string target) {
		int mem_pos = -1;
		scope* curr_scope = local_scope;
		while(curr_scope != nullptr) {
			for(int i = 0;  < curr_scope->variables.size(); i++) {
				if(curr_scope->variables[i].name == target) {
					mem_pos = curr_scope->variables[i].index;
					break;
				}
			}
			if(mem_pos != -1)
				break;
			curr_scope = local_scope.prev;
		}
		return mem_pos;
	}

	int op_importance(Lexer::token t) {
		     if(t.name == "^")  {return 6;}
		else if(t.name == "*")  {return 5;}
		else if(t.name == "/")  {return 5;}
		else if(t.name == "+")  {return 4;}
		else if(t.name == "-")  {return 4;}
		else if(t.name == "%")  {return 3;}
		else if(t.name == "<")  {return 2;}
		else if(t.name == ">")  {return 2;}
		else if(t.name == "<=") {return 2;}
		else if(t.name == ">=") {return 2;}
		else if(t.name == "==") {return 2;}
		else if(t.name == "!=") {return 2;}
		else if(t.name == ")")  {return 1;}
		else if(t.name == "(")  {return 1;}
		else {return -1;}
	}

	std::vector<Parser::expression_type> infix_to_postfix(std::vector<Parser::expression_type>& expression) {
		std::vector<Parser::expression_type> postfix_expression;
		std::stack<Parser::expression_type> op_stack;

		for(auto& i : expression) {
			if(i.index() == 0) {
				Lexer::token current = std::get<0>(i);
				if(current.type == Lexer::lexer_identifier || current.type == Lexer::lexer_value) {
					postfix_expression.push_back(i);
				}else if(current.type == Lexer::lexer_ob) {
					op_stack.push(i);
				}else if(current.type == Lexer::lexer_cb) {
					while(!op_stack.empty()) {
						if(op_stack.top().index() == 0) {
							if(std::get<0>(op_stack.top()).type == Lexer::lexer_ob) {
								break;
							}
						}
						postfix_expression.push_back(op_stack.top());
						op_stack.pop();
					}
					if(op_stack.top().index() == 0) {
						if(std::get<0>(op_stack.top()).type == Lexer::lexer_ob) {
							op_stack.pop();
						}
					}
				}else{
					while(!op_stack.empty()) {
						if(op_stack.top().index() == 0) {
							if(std::get<0>(op_stack.top()).type == Lexer::lexer_arithmetic || 
							   std::get<0>(op_stack.top()).type == Lexer::lexer_logic      ||
							   std::get<0>(op_stack.top()).type == Lexer::lexer_ob         ||
							   std::get<0>(op_stack.top()).type == Lexer::lexer_cb) {
								if(op_importance(std::get<0>(op_stack.top())) > op_importance(current)) {
									break;
								}
							}
						}
						postfix_expression.push_back(op_stack.top());
						op_stack.pop();
					}
					op_stack.push(i);
				}
			}else{
				postfix_expression.push_back(i);
			}
		}

		while(!op_stack.empty()) {
			postfix_expression.push_back(op_stack.top());
			op_stack.pop();
		}

		return postfix_expression;
	}

	int handle_expression(Parser::tree_node* node, std::vector<instruction>& instructions, scope* local_scope, int& mem_count) {
		std::vector<Parser::expression_type> postfix_expression = infix_to_postfix(node.ins.expression);
		std::stack<std::variant<Lexer::token, instruction, int, std::string>> execution_stack; //Variable/direct value, function call, intermediate register, memory_location

		for(int i = 0; i < postfix_expression.size(); i++) {
			if(postfix_expression[i].index() == 1) {
				execution_stack.push(postfix_expression[i]);
			}else{
				if(std::get<0>(postfix_expression[i]).type == Lexer::lexer_identifier || std::get<0>(postfix_expression[i]).type == Lexer::lexer_value) {
					execution_stack.push(postfix_expression[i]);
				}else{
					std::variant<Lexer::token, instruction, int, int> a = execution_stack.top();
					execution_stack.pop();
					std::variant<Lexer::token, instruction, int, int> b = execution_stack.top();
					execution_stack.pop();

					int a_used = -1;
					int mem_index_a = -1;

					int b_used = 0;
					int mem_index_b = -1;

					if(a.index() == 1) {
						handle_function_call(std::get<1>(a));
						int free_reg = find_free_reg();
						if(free_reg == -1) {
							mem_index_a = mem_count;
							instructions.push_back({0x5, {1}}); //Allocate memory to store value from first reg
							instructions.push_back({0x4, {0, mem_index_a, 0}}); //Store the reg value
							instructions.push_back({0x3, {0, mem_index_a-1, 0}}); //Read the return value
							a_used = 0;	
							mem_count++;
						}else{
							instructions.push_back({0x3, {free_reg, mem_count, 0}});
							a_used = free_reg;
						}
					}else if(a.index() == 0) {
						int free_reg = find_free_reg();
						if(free_reg == -1) {
							mem_index_a = mem_count;
							instructions.push_back({0x5, {1}}); //Allocate memory to store value from first reg
							instructions.push_back({0x4, {0, mem_index_a, 0}}); //Store the reg value
							if(std::get<0>(postfix_expression[i]).type == Lexer::lexer_value) {
								instructions.push_back({0x1, {0, std::stoi(std::get<0>(postfix_expression[i]).name)}});
							}else{
								int mem_pos = find_variable(local_scope, std::get<0>(postfix_expression[i]).name);
								if(mem_pos == -1) {
									print_error("Unknown variable used: " + std::get<0>(postfix_expression[i]).name + " - on line: " + std::get<0>(postfix_expression[i]).line);
									return -1;
								}
								instructions.push_back({0x3, {0, mem_pos, 0}});
							}
							a_used = 0;
							mem_count++;
						}else{
							if(std::get<0>(postfix_expression[i]).type == Lexer::lexer_value) {
								instructions.push_back({0x1, {free_reg, std::stoi(std::get<0>(postfix_expression[i]).name)}});
							}else{
								int mem_pos = find_variable(local_scope, std::get<0>(postfix_expression[i]).name);
								if(mem_pos == -1) {
									print_error("Unknown variable used: " + std::get<0>(postfix_expression[i]).name + " - on line: " + std::get<0>(postfix_expression[i]).line);
									return -1;
								}
								instructions.push_back({0x3, {free_reg, mem_pos, 0}});
							}
							a_used = free_reg;
						}
					}else if(a.index() == 2) {
						int free_reg = find_free_reg();
						if(free_reg == -1) {
							mem_index_a = mem_count;
							instructions.push_back({0x5, {1}}); //Allocate memory to store value from first reg
							instructions.push_back({0x4, {0, mem_index_a, 0}}); //Store the reg value
							instructions.push_back({0x2, {0, std::get<2>(a)}}); //Load the value from another register
							a_used = 0;	
							mem_count++;
						}else{
							instructions.push_back({0x2, {free_reg, std::get<2>(a)}});
							a_used = free_reg;
						}
					}else{
						int free_reg = find_free_reg();
						if(free_reg == -1) {
							mem_index_a = mem_count;
							instructions.push_back({0x5, {1}}); //Allocate memory to store value from first reg
							instructions.push_back({0x4, {0, mem_index_a, 0}}); //Store the reg value
							instructions.push_back({0x3, {0, std::stoi(std::get<3>(a))}}); //Load the value from a memory location
							a_used = 0;	
							instructions.push_back({0x6, {std::stoi(std::get<3>(a))}});
						}else{
							instructions.push_back({0x3, {free_reg, std::stoi(std::get<3>(a))}});
							a_used = free_reg;
							instructions.push_back({0x6, {std::stoi(std::get<3>(a))}});
							mem_count--;
						}
					}

					used_registers[a_used] = true;

					if(b.index() == 1) {
						handle_function_call(std::get<1>(b));
						int free_reg = find_free_reg();
						if(free_reg == -1) {
							mem_index_b = mem_count;
							instructions.push_back({0x5, {1}}); //Allocate memory to store value from first reg
							instructions.push_back({0x4, {1, mem_index_b, 0}}); //Store the reg value
							instructions.push_back({0x3, {1, mem_index_b-1, 0}}); //Read the return value
							b_used = 1;
							mem_count++;
						}else{
							b_used = free_reg;
							while(b_used == a_used)
								b_used++;

							instructions.push_back({0x3, {b_used, mem_coun, 0}});
						}
					}else if(b.index() == 0) {
						int free_reg = find_free_reg();
						if(free_reg == -1) {
							mem_index_b = mem_count;
							instructions.push_back({0x5, {1}}); //Allocate memory to store value from first reg
							instructions.push_back({0x4, {1, mem_index_b, 0}}); //Store the reg value
							if(std::get<0>(postfix_expression[i]).type == Lexer::lexer_value) {
								instructions.push_back({0x1, {1, std::stoi(std::get<0>(postfix_expression[i]).name)}});
							}else{
								int mem_pos = find_variable(local_scope, std::get<0>(postfix_expression[i]).name);
								if(mem_pos == -1) {
									print_error("Unknown variable used: " + std::get<0>(postfix_expression[i]).name + " - on line: " + std::get<0>(postfix_expression[i]).line);
									return -1;
								}
								instructions.push_back({0x3, {1, mem_pos, 0}});
							}
							b_used = 1;
							mem_count++;
						}else{
							b_used = free_reg;
							while(b_used == a_used)
								b_used++;

							if(std::get<0>(postfix_expression[i]).type == Lexer::lexer_value) {
								instructions.push_back({0x1, {b_used, std::stoi(std::get<0>(postfix_expression[i]).name)}});
							}else{
								int mem_pos = find_variable(local_scope, std::get<0>(postfix_expression[i]).name);
								if(mem_pos == -1) {
									print_error("Unknown variable used: " + std::get<0>(postfix_expression[i]).name + " - on line: " + std::get<0>(postfix_expression[i]).line);
									return -1;
								}
								instructions.push_back({0x3, {b_used, mem_pos, 0}});
							}
						}

					}else if(b.index() == 2) {
						int free_reg = find_free_reg();
						if(free_reg == -1) {
							mem_index_b = mem_count;
							instructions.push_back({0x5, {1}}); //Allocate memory to store value from first reg
							instructions.push_back({0x4, {1, mem_index_b, 0}}); //Store the reg value
							instructions.push_back({0x2, {1, std::get<2>(b)}}); //Load the value from another register
							b_used = 0;	
							mem_count++;
						}else{
							b_used = free_reg;
							while(b_used == a_used)
								b_used++;

							instructions.push_back({0x2, {b_used, std::get<2>(b)}});
						}
					}else{
						int free_reg = find_free_reg();
						if(free_reg == -1) {
							mem_index_b = mem_count;
							instructions.push_back({0x5, {1}}); //Allocate memory to store value from first reg
							instructions.push_back({0x4, {1, mem_index_b, 0}}); //Store the reg value
							instructions.push_back({0x3, {1, std::stoi(std::get<3>(b))}}); //Load the value from a memory location
							b_used = 0;	
							instructions.push_back({0x6, {std::stoi(std::get<3>(b))}});
						}else{
							b_used = free_reg;
							while(b_used == a_used)
								b_used++;

							instructions.push_back({0x3, {b_used, std::stoi(std::get<3>(b))}});
							instructions.push_back({0x6, {std::stoi(std::get<3>(b))}});
							mem_count--;
						}
					}

					used_registers[b_used] = true;
					std::string operator = std::get<0>(postfix_expression[i]).name;
					result_reg = a_used;

					if(operator == "+") {
						instructions.push_back({0x7, {a_used, b_used}});
					}else if(operator == "-") {
						instructions.push_back({0x8, {a_used, b_used}});
					}else if(operator == "*") {
						instructions.push_back({0x9, {a_used, b_used}});
					}else if(operator == "/") {
						instructions.push_back({0xA, {a_used, b_used}});
					}else if(operator == "%") {
						instructions.push_back({0xC, {a_used, b_used}});
					}else if(operator == "<") {
						instructions.push_back({0x17, {a_used, b_used}});
					}else if(operator == ">") {
						instructions.push_back({0x17, {b_used, a_used}});
						result_reg = b_used;
					}else if(operator == "<=") {
						instructions.push_back({0x18, {a_used, b_used}});
					}else if(operator == ">=") {
						instructions.push_back({0x18, {b_used, a_used}});
						result_reg = b_used;
					}else if(operator == "==") {
						instructions.push_back({0x15, {a_used, b_used}});
					}else if(operator == "!=") {
						instructions.push_back({0x16, {a_used, b_used}});
					}

					if(mem_index_a != -1) {
						if(result_reg == a_used) {
							instructions.push_back({0x5, {1}});
							instructions.push_back({0x4, {result_reg, mem_count, 0}});
							execution_stack.push(std::to_string(mem_count));
							mem_count++;
							used_registers[a_used] = false;
							used_registers[b_used] = false;
						}
						
						instructions.push_back({0x3, {a_used, mem_index_a, 0}});
						instructions.push_back({0x6, {mem_index_a}});
						mem_count--;
						used_registers[a_used] = true;
					}

					if(mem_index_b != -1) {
						if(result_reg == b_used) {
							instructions.push_back({0x5, {1}});
							instructions.push_back({0x4, {result_reg, mem_count, 0}});
							execution_stack.push(std::to_string(mem_count));
							mem_count++;
							used_registers[a_used] = false;
							used_registers[b_used] = false;
						}
						
						instructions.push_back({0x3, {b_used, mem_index_b, 0}});
						instructions.push_back({0x6, {mem_index_a}});
						mem_count--;
						used_registers[b_used] = true;
					}

					if(mem_index_a == -1 && result_reg == a_used) {
						execution_stack.push(a_used);
						used_registers[b_used] = false;
					}

					if(mem_index_b == -1 && result_reg == b_used) {
						execution_stack.push(b_used);
						used_registers[a_used] = false;
					}
				}
			}
		}

		instructions.push_back({0x5, {1}});
		instructions.push_back((0x4, {result_reg, mem_count, 0}));
		used_registers[reg_to_clean] = false;
		mem_count++;
		return mem_count-1;
	}

	std::vector<instruction> process_variable_definition(Parser::tree_node* node, scope* local_scope, int& mem_count) {
		std::vector<instruction> new_instructions;

		new_instructions.push_back({0x6, {1}});

		handle_expression(node, new_instructions, local_scope, int& mem_count);
	}

	void process_function_definition(Parser::tree_node* node) {
		function new_function;
		new_function.name = node.ins.identifier.name;

		for(auto i : node.nodes) {
			std::vector<instruction> new_instructions = process_node(i, new_function.scope, new_function.mem_count);
			for(auto j : new_instructions) {
				new_function.instructions.push_back(j);
			}
		}
		functions.push_back(new_function);
	}

	std::vector<instruction> process_node(Parser::tree_node* node, scope* local_scope) {
		std::vector<instruction> new_instructions;
		switch(node.ins.type) {
			case Parser::it_variable_definition: process+variable_definition(node, function_scope) break;
			case Parser::it_function_definition: process_function_definition(node); break;
			case Parser::it_variable_change: break;
			case Parser::it_function_call: break;
			case Parser::it_if_statement: break;
			case Parser::it_else: break;
			case Parser::it_return: break;
		}
		return new_instructions;
	}

	void generate(Parser::tree_node* code_tree, std::string output_file_path) {

	}
}