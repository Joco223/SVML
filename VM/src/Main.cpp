#include <vector>
#include <string>
#include <fstream>

#include "CPU.h"
#include "cmd_arg_handler.h"

int main(int argc, char** argv) {

	std::vector<std::pair<std::string, std::string>> arguments = handle_args(argc, argv);

	if(arguments.size() > 0) {
		std::string input_file;

		for(auto& i : arguments) {
			if(i.first == "input")  {input_file = i.second;}
		}

		std::vector<CPU::function> functions;

		std::ifstream input(input_file);
		bool first = true;
		int starting_function = 0;
		int function_count = 0;

		for(std::string line; getline(input, line);) {
			if(first) {
				starting_function = std::stoi(line);
				first = false;
			}else{
				if(line == "-----") {
					CPU::function new_func;
					new_func.id = function_count;
					new_func.program_counter = 0;
					function_count++;
					for(line; getline(input, line);) {
						if(line == "-----") {
							functions.push_back(new_func);
							break;
						}else{
							if(line == "---") {
								CPU::instruction new_instruction;
								bool i_first = true;
								for(line; getline(input, line);) {

									if(line == "---") {
										new_func.instructions.push_back(new_instruction);
										break;
									}else{
										if(i_first) {
											new_instruction.op_code = std::stoi(line);
											i_first = false;
										}else{
											new_instruction.args.push_back(std::stoi(line));
										}
									}
								}
							}
						}
					}
				}
			}
		}

		CPU cpu(functions, starting_function);

		while(!cpu.halt) {
			cpu.tick();
		}
	}

	return 0;
}
