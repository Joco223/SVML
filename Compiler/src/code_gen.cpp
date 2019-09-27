#include "code_gen.h"

namespace code_gen {

	std::vector<function> functions;
	std::vector<bool> used_registers = {false, false, false, false, false, false, false, false};

	int find_free_reg() {
		int index = -1;
		for(int i = 0; i < 8; i++) {
			if(used_registers[i] == false) {
				index = i;
				break;
			}
		}
		return index;
	}

	int find_variable(scope* local_scope, std::string& target) {
		int mem_pos = -1;
		scope* curr_scope = local_scope;
		while(curr_scope != nullptr) {
			for(int i = 0; i < curr_scope->variables.size(); i++) {
				if(curr_scope->variables[i].name == target) {
					mem_pos = curr_scope->variables[i].index;
					break;
				}
			}
			if(mem_pos != -1)
				break;
			curr_scope = curr_scope->prev;
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

	std::vector<std::variant<Lexer::token, Parser::instruction>> infix_to_postfix(std::vector<std::variant<Lexer::token, Parser::instruction>>& expression) {
		std::vector<std::variant<Lexer::token, Parser::instruction>> postfix_expression;
		std::stack<std::variant<Lexer::token, Parser::instruction>> op_stack;

		for(auto& i : expression) {
			if(i.index() == 0) {
				Lexer::token current = std::get<0>(i);
				if(current.type == Lexer::tt_identifier || current.type == Lexer::tt_value) {
					postfix_expression.push_back(i);
				}else if(current.type == Lexer::tt_ob) {
					op_stack.push(i);
				}else if(current.type == Lexer::tt_cb) {
					while(!op_stack.empty()) {
						if(op_stack.top().index() == 0) {
							if(std::get<0>(op_stack.top()).type == Lexer::tt_ob) {
								break;
							}
						}
						postfix_expression.push_back(op_stack.top());
						op_stack.pop();
					}
					if(op_stack.top().index() == 0) {
						if(std::get<0>(op_stack.top()).type == Lexer::tt_ob) {
							op_stack.pop();
						}
					}
				}else{
					while(!op_stack.empty()) {
						if(op_stack.top().index() == 0) {
							if(std::get<0>(op_stack.top()).type == Lexer::tt_arithmetic || 
							   std::get<0>(op_stack.top()).type == Lexer::tt_logic      ||
							   std::get<0>(op_stack.top()).type == Lexer::tt_ob         ||
							   std::get<0>(op_stack.top()).type == Lexer::tt_cb) {
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

	int handle_expression(std::vector<std::variant<Lexer::token, Parser::instruction>>& base_expression, std::vector<instruction>& instructions, scope* local_scope, int& mem_count);

	void handle_function_call(Parser::instruction& ins, std::vector<instruction>& instructions, scope* local_scope, int& mem_count) {
		std::vector<unsigned int> argument_mem_locations;

		int index = -1;
		for(int i = 0; i < functions.size(); i++) { //Find the function
			if(ins.identifier == functions[i].name) {
				index = i;
				break;
			}
		}

		if(index == -1) { //Function not found
			Error_handler::error_out("Unknown function called: " + ins.identifier + " - on line: " + std::to_string(ins.line));
			return;
		}else{ //Function found
			argument_mem_locations.push_back(index); //Push the function index
			for(auto& i : ins.expressions) { //Handle all the arguments
				argument_mem_locations.push_back(handle_expression(i, instructions, local_scope, mem_count));
			}
			instructions.push_back({0x1C, argument_mem_locations});
			return;
		}
	}

	int handle_expression(std::vector<std::variant<Lexer::token, Parser::instruction>>& base_expression, std::vector<instruction>& instructions, scope* local_scope, int& mem_count) {
		std::vector<std::variant<Lexer::token, Parser::instruction>> postfix_expression = infix_to_postfix(base_expression);
		std::stack<std::variant<Lexer::token, Parser::instruction, int, std::string>> execution_stack; //Variable/direct value, function call, intermediate register, memory_location
		//String is used here just to have a different type instead of two ints
		int result_reg = -1;

		for(int i = 0; i < postfix_expression.size(); i++) {
			if(postfix_expression[i].index() == 1) {
				execution_stack.push(std::get<1>(postfix_expression[i]));
			}else{
				if(std::get<0>(postfix_expression[i]).type == Lexer::tt_identifier || std::get<0>(postfix_expression[i]).type == Lexer::tt_value) {
					execution_stack.push(std::get<0>(postfix_expression[i]));
				}else{
					std::variant<Lexer::token, Parser::instruction, int, std::string> a = execution_stack.top();
					execution_stack.pop();
					std::variant<Lexer::token, Parser::instruction, int, std::string> b = execution_stack.top();
					execution_stack.pop();

					int a_used = -1;
					int mem_index_a = -1;

					int b_used = 0;
					int mem_index_b = -1;

					if(a.index() == 1) {
						handle_function_call(std::get<1>(a), instructions, local_scope, mem_count);
						int free_reg = find_free_reg();
						if(free_reg == -1) {
							mem_index_a = mem_count;
							instructions.push_back({0x5, {1}}); //Allocate memory to store value from first reg
							instructions.push_back({0x4, {0, (unsigned int)mem_index_a, 0}}); //Store the reg value
							instructions.push_back({0x3, {0, (unsigned int)(mem_index_a-1), 0}}); //Read the return value
							a_used = 0;	
							mem_count++;
						}else{
							instructions.push_back({0x3, {(unsigned int)free_reg, (unsigned int)mem_count, 0}});
							a_used = free_reg;
						}
					}else if(a.index() == 0) {
						int free_reg = find_free_reg();
						if(free_reg == -1) {
							mem_index_a = mem_count;
							instructions.push_back({0x5, {1}}); //Allocate memory to store value from first reg
							instructions.push_back({0x4, {0, (unsigned int)mem_index_a, 0}}); //Store the reg value
							if(std::get<0>(a).type == Lexer::tt_value) {
								instructions.push_back({0x1, {0, (unsigned int)std::stoi(std::get<0>(a).name)}});
							}else{
								int mem_pos = find_variable(local_scope, std::get<0>(a).name);
								if(mem_pos == -1) {
									Error_handler::error_out("Unknown variable used: " + std::get<0>(a).name + " - on line: " + std::to_string(std::get<0>(a).line));
									return -1;
								}
								instructions.push_back({0x3, {0, (unsigned int)mem_pos, 0}});
							}
							a_used = 0;
							mem_count++;
						}else{
							if(std::get<0>(a).type == Lexer::tt_value) {
								instructions.push_back({0x1, {(unsigned int)free_reg, (unsigned int)std::stoi(std::get<0>(a).name)}});
							}else{
								int mem_pos = find_variable(local_scope, std::get<0>(a).name);
								if(mem_pos == -1) {
									Error_handler::error_out("Unknown variable used: " + std::get<0>(a).name + " - on line: " + std::to_string(std::get<0>(a).line));
									return -1;
								}
								instructions.push_back({0x3, {(unsigned int)free_reg, (unsigned int)mem_pos, 0}});
							}
							a_used = free_reg;
						}
					}else if(a.index() == 2) {
						int free_reg = find_free_reg();
						if(free_reg == -1) {
							mem_index_a = mem_count;
							instructions.push_back({0x5, {1}}); //Allocate memory to store value from first reg
							instructions.push_back({0x4, {0, (unsigned int)mem_index_a, 0}}); //Store the reg value
							instructions.push_back({0x2, {0, (unsigned int)std::get<2>(a)}}); //Load the value from another register
							a_used = 0;	
							mem_count++;
						}else{
							instructions.push_back({0x2, {(unsigned int)free_reg, (unsigned int)std::get<2>(a)}});
							a_used = free_reg;
						}
					}else{
						int free_reg = find_free_reg();
						if(free_reg == -1) {
							mem_index_a = mem_count;
							instructions.push_back({0x5, {1}}); //Allocate memory to store value from first reg
							instructions.push_back({0x4, {0, (unsigned int)mem_index_a, 0}}); //Store the reg value
							instructions.push_back({0x3, {0, (unsigned int)std::stoi(std::get<3>(a)), 0}}); //Load the value from a memory location
							a_used = 0;	
							instructions.push_back({0x6, {(unsigned int)std::stoi(std::get<3>(a))}});
						}else{
							instructions.push_back({0x3, {(unsigned int)free_reg, (unsigned int)std::stoi(std::get<3>(a)), 0}});
							a_used = free_reg;
							instructions.push_back({0x6, {(unsigned int)std::stoi(std::get<3>(a))}});
							mem_count--;
						}
					}

					used_registers[a_used] = true;

					if(b.index() == 1) {
						handle_function_call(std::get<1>(b), instructions, local_scope, mem_count);
						int free_reg = find_free_reg();
						if(free_reg == -1) {
							mem_index_b = mem_count;
							instructions.push_back({0x5, {1}}); //Allocate memory to store value from first reg
							instructions.push_back({0x4, {1, (unsigned int)mem_index_b, 0}}); //Store the reg value
							instructions.push_back({0x3, {1, (unsigned int)mem_index_b-1, 0}}); //Read the return value
							b_used = 1;
							mem_count++;
						}else{
							b_used = free_reg;
							while(b_used == a_used)
								b_used++;

							instructions.push_back({0x3, {(unsigned int)b_used, (unsigned int)mem_count, 0}});
						}
					}else if(b.index() == 0) {
						int free_reg = find_free_reg();
						if(free_reg == -1) {
							mem_index_b = mem_count;
							instructions.push_back({0x5, {1}}); //Allocate memory to store value from first reg
							instructions.push_back({0x4, {1, (unsigned int)mem_index_b, 0}}); //Store the reg value
							if(std::get<0>(b).type == Lexer::tt_value) {
								instructions.push_back({0x1, {1, (unsigned int)std::stoi(std::get<0>(b).name)}});
							}else{
								int mem_pos = find_variable(local_scope, std::get<0>(b).name);
								if(mem_pos == -1) {
									Error_handler::error_out("Unknown variable used: " + std::get<0>(b).name + " - on line: " + std::to_string(std::get<0>(b).line));
									return -1;
								}
								instructions.push_back({0x3, {1, (unsigned int)mem_pos, 0}});
							}
							b_used = 1;
							mem_count++;
						}else{
							b_used = free_reg;
							while(b_used == a_used)
								b_used++;

							if(std::get<0>(b).type == Lexer::tt_value) {
								instructions.push_back({0x1, {(unsigned int)b_used, (unsigned int)std::stoi(std::get<0>(b).name)}});
							}else{
								int mem_pos = find_variable(local_scope, std::get<0>(b).name);
								if(mem_pos == -1) {
									Error_handler::error_out("Unknown variable used: " + std::get<0>(b).name + " - on line: " + std::to_string(std::get<0>(b).line));
									return -1;
								}
								instructions.push_back({0x3, {(unsigned int)b_used, (unsigned int)mem_pos, 0}});
							}
						}

					}else if(b.index() == 2) {
						int free_reg = find_free_reg();
						if(free_reg == -1) {
							mem_index_b = mem_count;
							instructions.push_back({0x5, {1}}); //Allocate memory to store value from first reg
							instructions.push_back({0x4, {1, (unsigned int)mem_index_b, 0}}); //Store the reg value
							instructions.push_back({0x2, {1, (unsigned int)std::get<2>(b)}}); //Load the value from another register
							b_used = 0;	
							mem_count++;
						}else{
							b_used = free_reg;
							while(b_used == a_used)
								b_used++;

							instructions.push_back({0x2, {(unsigned int)b_used, (unsigned int)std::get<2>(b)}});
						}
					}else{
						int free_reg = find_free_reg();
						if(free_reg == -1) {
							mem_index_b = mem_count;
							instructions.push_back({0x5, {1}}); //Allocate memory to store value from first reg
							instructions.push_back({0x4, {1, (unsigned int)mem_index_b, 0}}); //Store the reg value
							instructions.push_back({0x3, {1, (unsigned int)std::stoi(std::get<3>(b)), 0}}); //Load the value from a memory location
							b_used = 0;	
							instructions.push_back({0x6, {(unsigned int)std::stoi(std::get<3>(b))}});
						}else{
							b_used = free_reg;
							while(b_used == a_used)
								b_used++;

							instructions.push_back({0x3, {(unsigned int)b_used, (unsigned int)std::stoi(std::get<3>(b)), 0}});
							instructions.push_back({0x6, {(unsigned int)std::stoi(std::get<3>(b))}});
							mem_count--;
						}
					}

					used_registers[b_used] = true;
					std::string op = std::get<0>(postfix_expression[i]).name;
					result_reg = a_used;

					if(op == "+") {
						instructions.push_back({0x7, {(unsigned int)a_used, (unsigned int)b_used}});
					}else if(op == "-") {
						instructions.push_back({0x8, {(unsigned int)a_used, (unsigned int)b_used}});
					}else if(op == "*") {
						instructions.push_back({0x9, {(unsigned int)a_used, (unsigned int)b_used}});
					}else if(op == "/") {
						instructions.push_back({0xA, {(unsigned int)a_used, (unsigned int)b_used}});
					}else if(op == "%") {
						instructions.push_back({0xC, {(unsigned int)a_used, (unsigned int)b_used}});
					}else if(op == "<") {
						instructions.push_back({0x17, {(unsigned int)a_used, (unsigned int)b_used}});
					}else if(op == ">") {
						instructions.push_back({0x17, {(unsigned int)b_used, (unsigned int)a_used}});
						result_reg = b_used;
					}else if(op == "<=") {
						instructions.push_back({0x18, {(unsigned int)a_used, (unsigned int)b_used}});
					}else if(op == ">=") {
						instructions.push_back({0x18, {(unsigned int)b_used, (unsigned int)a_used}});
						result_reg = b_used;
					}else if(op == "==") {
						instructions.push_back({0x15, {(unsigned int)a_used, (unsigned int)b_used}});
					}else if(op == "!=") {
						instructions.push_back({0x16, {(unsigned int)a_used, (unsigned int)b_used}});
					}

					if(mem_index_a != -1) {
						if(result_reg == a_used) {
							instructions.push_back({0x5, {1}});
							instructions.push_back({0x4, {(unsigned int)result_reg, (unsigned int)mem_count, 0}});
							execution_stack.push(std::to_string(mem_count));
							mem_count++;
							used_registers[a_used] = false;
							used_registers[b_used] = false;
						}
						
						instructions.push_back({0x3, {(unsigned int)a_used, (unsigned int)mem_index_a, 0}});
						instructions.push_back({0x6, {(unsigned int)mem_index_a}});
						mem_count--;
						used_registers[a_used] = true;
					}

					if(mem_index_b != -1) {
						if(result_reg == b_used) {
							instructions.push_back({0x5, {1}});
							instructions.push_back({0x4, {(unsigned int)result_reg, (unsigned int)mem_count, 0}});
							execution_stack.push(std::to_string(mem_count));
							mem_count++;
							used_registers[a_used] = false;
							used_registers[b_used] = false;
						}
						
						instructions.push_back({0x3, {(unsigned int)b_used, (unsigned int)mem_index_b, 0}});
						instructions.push_back({0x6, {(unsigned int)mem_index_a}});
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

		if(execution_stack.size() == 1) {
			std::variant<Lexer::token, Parser::instruction, int, std::string> a = execution_stack.top();
			execution_stack.pop();

			int a_used = 0;
			int mem_index_a = -1;

			if(a.index() == 1) {
				handle_function_call(std::get<1>(a), instructions, local_scope, mem_count);
				int free_reg = find_free_reg();
				if(free_reg == -1) {
					mem_index_a = mem_count;
					instructions.push_back({0x5, {1}}); //Allocate memory to store value from first reg
					instructions.push_back({0x4, {0, (unsigned int)mem_index_a, 0}}); //Store the reg value
					instructions.push_back({0x3, {0, (unsigned int)mem_index_a-1, 0}}); //Read the return value
					a_used = 0;	
					mem_count++;
				}else{
					instructions.push_back({0x3, {(unsigned int)free_reg, (unsigned int)mem_count, 0}});
					a_used = free_reg;
				}
			}else if(a.index() == 0) {
				int free_reg = find_free_reg();
				if(free_reg == -1) {
					mem_index_a = mem_count;
					instructions.push_back({0x5, {1}}); //Allocate memory to store value from first reg
					instructions.push_back({0x4, {0, (unsigned int)mem_index_a, 0}}); //Store the reg value
					if(std::get<0>(a).type == Lexer::tt_value) {
						instructions.push_back({0x1, {0, (unsigned int)std::stoi(std::get<0>(a).name)}});
					}else{
						int mem_pos = find_variable(local_scope, std::get<0>(a).name);
						if(mem_pos == -1) {
							Error_handler::error_out("Unknown variable used: " + std::get<0>(a).name + " - on line: " + std::to_string(std::get<0>(a).line));
							return -1;
						}
						instructions.push_back({0x3, {0, (unsigned int)mem_pos, 0}});
					}
					a_used = 0;
					mem_count++;
				}else{
					if(std::get<0>(a).type == Lexer::tt_value) {
						instructions.push_back({0x1, {(unsigned int)free_reg, (unsigned int)std::stoi(std::get<0>(a).name)}});
					}else{
						int mem_pos = find_variable(local_scope, std::get<0>(a).name);
						if(mem_pos == -1) {
							Error_handler::error_out("Unknown variable used: " + std::get<0>(a).name + " - on line: " + std::to_string(std::get<0>(a).line));
							return -1;
						}
						instructions.push_back({0x3, {(unsigned int)free_reg, (unsigned int)mem_pos, 0}});
					}
					a_used = free_reg;
				}
			}else if(a.index() == 2) {
				int free_reg = find_free_reg();
				if(free_reg == -1) {
					mem_index_a = mem_count;
					instructions.push_back({0x5, {1}}); //Allocate memory to store value from first reg
					instructions.push_back({0x4, {0, (unsigned int)mem_index_a, 0}}); //Store the reg value
					instructions.push_back({0x2, {0, (unsigned int)std::get<2>(a)}}); //Load the value from another register
					a_used = 0;	
					mem_count++;
				}else{
					instructions.push_back({0x2, {(unsigned int)free_reg, (unsigned int)std::get<2>(a)}});
					a_used = free_reg;
				}
			}else{
				int free_reg = find_free_reg();
				if(free_reg == -1) {
					mem_index_a = mem_count;
					instructions.push_back({0x5, {1}}); //Allocate memory to store value from first reg
					instructions.push_back({0x4, {0, (unsigned int)mem_index_a, 0}}); //Store the reg value
					instructions.push_back({0x3, {0, (unsigned int)std::stoi(std::get<3>(a)), 0}}); //Load the value from a memory location
					a_used = 0;	
					instructions.push_back({0x6, {(unsigned int)std::stoi(std::get<3>(a))}});
				}else{
					instructions.push_back({0x3, {(unsigned int)free_reg, (unsigned int)std::stoi(std::get<3>(a)), 0}});
					a_used = free_reg;
					instructions.push_back({0x6, {(unsigned int)std::stoi(std::get<3>(a))}});
					mem_count--;
				}
			}

			result_reg = a_used;

			instructions.push_back({0x5, {1}});
			instructions.push_back({0x4, {(unsigned int)result_reg, (unsigned int)mem_count, 0}});
			used_registers[result_reg] = false;
			mem_count++;

			if(mem_index_a != -1) {			
				instructions.push_back({0x3, {(unsigned int)a_used, (unsigned int)mem_index_a, 0}});
				instructions.push_back({0x6, {(unsigned int)mem_index_a}});
				mem_count--;
				used_registers[a_used] = true;
			}
		}else{
			instructions.push_back({0x5, {1}});
			instructions.push_back({0x4, {(unsigned int)result_reg, (unsigned int)mem_count, 0}});
			used_registers[result_reg] = false;
			mem_count++;
		}
		return mem_count-1;
	}

	void process_variable_definition(Parser::tree_node* node, std::vector<instruction>& instructions, scope* local_scope, int& mem_count) {
		int var_mem_location = handle_expression(node->ins.expressions[0], instructions, local_scope, mem_count);
		local_scope->variables.push_back({node->ins.identifier, 0, var_mem_location});
	}

	void process_function_definition(Parser::tree_node* node) {
		function new_function;
		new_function.name = node->ins.identifier;
		new_function.function_scope = new scope;

		for(auto& i : node->ins.def_arguments) { //Handle function arguments
			new_function.function_scope->variables.push_back({i.name, 0, new_function.mem_count});
			new_function.mem_count++;
		}

		for(auto& i : node->nodes) {
			process_node(i, new_function.instructions, new_function.function_scope, new_function.mem_count);
		}
		functions.push_back(new_function);
	}

	void process_variable_change(Parser::tree_node* node, std::vector<instruction>& instructions, scope* local_scope, int& mem_count) {
		int old_var_mem_location = find_variable(local_scope, node->ins.identifier);

		int var_mem_location = handle_expression(node->ins.expressions[0], instructions, local_scope, mem_count);
		int free_reg = find_free_reg();
		if(free_reg == -1) {
			instructions.push_back({0x5, {1}}); //Allocate memory to store value from first reg
			instructions.push_back({0x4, {0, (unsigned int)mem_count, 0}}); //Store the reg value
			instructions.push_back({0x3, {0, (unsigned int)var_mem_location, 0}}); //Load the value from a memory location
			instructions.push_back({0x4, {0, (unsigned int)old_var_mem_location, 0}}); //Write to variable memory location
			instructions.push_back({0x3, {0, (unsigned int)mem_count, 0}}); //Restore the old value in register
			instructions.push_back({0x6, {(unsigned int)mem_count}}); //Deallocate the temporary value
		}else{
			instructions.push_back({0x3, {(unsigned int)free_reg, (unsigned int)var_mem_location, 0}});
			instructions.push_back({0x4, {(unsigned int)free_reg, (unsigned int)old_var_mem_location, 0}});
		}
		instructions.push_back({0x6, {(unsigned int)var_mem_location}});
	}

	void process_if_statement(Parser::tree_node* node, std::vector<instruction>& instructions, scope* local_scope, int& mem_count) {
		int condition_mem_location = handle_expression(node->ins.expressions[0], instructions, local_scope, mem_count);
		
		int free_reg = find_free_reg();
		if(free_reg == -1) {
			instructions.push_back({0x5, {1}}); //Allocate memory to store value from first reg
			instructions.push_back({0x4, {0, (unsigned int)mem_count, 0}}); //Store the reg value
			instructions.push_back({0x3, {0, (unsigned int)condition_mem_location, 0}}); //Load the value from a memory location
			free_reg = 0;
		}else{
			instructions.push_back({0x3, {(unsigned int)free_reg, (unsigned int)condition_mem_location, 0}}); //Load the value from a memory location
		}
		int ins_to_adjust = instructions.size(); //Jump instruction to adjust later
		instructions.push_back({0x1A, {(unsigned int)free_reg, 0}});

		for(auto& i : node->nodes) { //Process everything inside the if statement
			scope* if_scope = new scope;
			if_scope->prev = local_scope;
			local_scope->child_scopes.push_back(if_scope);
			process_node(i, instructions, local_scope->child_scopes[local_scope->child_scopes.size()-1], mem_count);
		}

		instructions[ins_to_adjust].args[1] = instructions.size();
		instructions.push_back({0x19, {(unsigned int)instructions.size()}}); //Add a jump in case there is an else statement
	}

	void process_else(Parser::tree_node* node, std::vector<instruction>& instructions, scope* local_scope, int& mem_count) {
		int ins_to_adjust = instructions.size()-1;

		for(auto& i : node->nodes) { //Process everything inside the if statement
			local_scope->child_scopes.push_back(new scope);
			process_node(i, instructions, local_scope->child_scopes[local_scope->child_scopes.size()-1], mem_count);
		}

		instructions[ins_to_adjust].args[0] = instructions.size();
	}

	void process_return(Parser::tree_node* node, std::vector<instruction>& instructions, scope* local_scope, int& mem_count) {
		int return_value_mem_location = handle_expression(node->ins.expressions[0], instructions, local_scope, mem_count);
		instructions.push_back({0x1D, {(unsigned int)return_value_mem_location}});
	}

	void process_while(Parser::tree_node* node, std::vector<instruction>& instructions, scope* local_scope, int& mem_count) {
		int check_again_mem_location = instructions.size()-1;
		int condition_mem_location = handle_expression(node->ins.expressions[0], instructions, local_scope, mem_count);

		int free_reg = find_free_reg();
		if(free_reg == -1) {
			instructions.push_back({0x5, {1}}); //Allocate memory to store value from first reg
			instructions.push_back({0x4, {0, (unsigned int)mem_count, 0}}); //Store the reg value
			instructions.push_back({0x3, {0, (unsigned int)condition_mem_location, 0}}); //Load the value from a memory location
			free_reg = 0;
		}else{
			instructions.push_back({0x3, {(unsigned int)free_reg, (unsigned int)condition_mem_location, 0}}); //Load the value from a memory location
		}
		int ins_to_adjust = instructions.size(); //Jump instruction to adjust later
		instructions.push_back({0x1A, {(unsigned int)free_reg, 0}});

		for(auto& i : node->nodes) { //Process everything inside the while loop
			scope* if_scope = new scope;
			if_scope->prev = local_scope;
			local_scope->child_scopes.push_back(if_scope);
			process_node(i, instructions, local_scope->child_scopes[local_scope->child_scopes.size()-1], mem_count);
		}

		instructions.push_back({0x19, {(unsigned int)check_again_mem_location}}); //return to the check
		instructions[ins_to_adjust].args[1] = instructions.size();	
	}

	void process_node(Parser::tree_node* node, std::vector<instruction>& instructions, scope* local_scope, int& mem_count) {
		switch(node->ins.ins_type) {
			case Parser::it_variable_definition: process_variable_definition(node, instructions, local_scope, mem_count); break;
			case Parser::it_function_definition: process_function_definition(node); break;
			case Parser::it_variable_change: process_variable_change(node, instructions, local_scope, mem_count);break;
			case Parser::it_function_call: {
				handle_function_call(node->ins, instructions, local_scope, mem_count);
				instructions.push_back({0x6, {(unsigned int)mem_count}}); //Deallocate the function return variable (Function calls outside of an expression are treated as void)
				break; }
			case Parser::it_if_statement: process_if_statement(node, instructions, local_scope, mem_count); break;
			case Parser::it_else: process_else(node, instructions, local_scope, mem_count);  break;
			case Parser::it_return: process_return(node, instructions, local_scope, mem_count); break;
			case Parser::it_while: process_while(node, instructions, local_scope, mem_count); break;
		}
	}

	void generate(Parser::tree_node* code_tree, std::string output_file_path) {
		for(auto& i : code_tree->nodes) {
			if(i->ins.ins_type != Parser::it_function_definition) {
				Error_handler::error_out("Non function definition instruction found: " + std::to_string(i->ins.ins_type) + " - on line: " + std::to_string(i->ins.line));
			}else{
				process_function_definition(i);
			}
		}

		int main_function_index = -1;
		int function_count = functions.size();

		for(int i = 0; i < functions.size(); i++) {
			if(functions[i].name == "main") {
				main_function_index = i;
				break;
			}
		}

		if(main_function_index == -1) {
			Error_handler::error_out("No >main< function found, aborting compilation.");
			return;
		}

		
		if(Error_handler::compilable) {
			std::ofstream output_file;
			output_file.open(output_file_path, std::ios::binary);

			output_file.write((char*)&main_function_index, sizeof(main_function_index)); 
			output_file.write((char*)&function_count, sizeof(function_count));

			for(auto& i : functions) {
				int instruction_count = i.instructions.size();
				output_file.write((char*)&instruction_count, sizeof(instruction_count));

				for(auto& j : i.instructions) {
					output_file.write((char*)&j.op_code, sizeof(j.op_code));
					unsigned char argument_count = j.args.size();
					output_file.write((char*)&argument_count, sizeof(argument_count));
					for(auto& k : j.args) {
						output_file.write((char*)&k, sizeof(k));
					}
				}
			}
			std::cout << "\nProgram sucessfully compiled!\n";
			output_file.close();
		}
	}
}