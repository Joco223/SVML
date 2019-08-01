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

		std::ifstream input;
		input.open(input_file, std::ios::binary);

		if(!input.good()) {
			std::cout << "Error, can't open file: " << input_file << '\n';
			return -1;
		}

		int starting_function = 0;
		int function_count = 0;

		input.read((char*)&starting_function, sizeof(starting_function));
		input.read((char*)&function_count, sizeof(function_count));

		for(int i = 0; i < function_count; i++) {
			CPU::function new_function;
			new_function.id = i;
			new_function.program_counter = 0;
			int instruction_count = 0;
			input.read((char*)&instruction_count, sizeof(instruction_count));
			for(int j = 0; j < instruction_count; j++) {
				CPU::instruction new_instruction;
				input.read((char*)&new_instruction.op_code, sizeof(new_instruction.op_code));
				unsigned char argument_count = 0;
				input.read((char*)&argument_count, sizeof(argument_count));
				for(unsigned char k = 0; k < argument_count; k++) {
					unsigned int argument = 0;
					input.read((char*)&argument, sizeof(argument));
					new_instruction.args.push_back(argument);
				}
				new_function.instructions.push_back(new_instruction);
			}
			functions.push_back(new_function);
		}

		CPU cpu(functions, starting_function);

		while(!cpu.halt) {
			cpu.tick();
		}
	}

	return 0;
}
