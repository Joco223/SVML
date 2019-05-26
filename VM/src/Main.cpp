#include <vector>
#include <string>
#include <fstream>

#include "CPU.h"
#include "cmd_arg_handler.h"

int main(int argc, char** argv) {

	//std::vector<std::pair<std::string, std::string>>* arguments = handle_args(argc, argv);

	//if(arguments != nullptr) {
		/*bool debug = false;
		int stack_size = 128;
		int mem_size = 65535; //32M
		std::string input_file;

		for(auto& i : *arguments) {
			if(i.first == "debug")  {debug = true;}
			if(i.first == "stack")  {stack_size = std::stoi(i.second);}
			if(i.first == "memory") {mem_size = std::stoi(i.second);}
			if(i.first == "input")  {input_file = i.second;}
		}*/

		std::ifstream input(argv[1]);
		bool first = true;
		bool second = true;
		int tot_vars = 0;
		std::vector<int> instructions;
		int program_start = 0;
		int curr_line = 0;
		int fn_end = 0;
		std::vector<int> fn_positions;

		for(std::string line; getline(input, line);) {
			if(curr_line == 0) {
				tot_vars = std::stoi(line);
				first = false;
			}else if(curr_line == 1) {
				program_start = std::stoi(line);
				second = false;
			}else if(curr_line == 2) {
				fn_end = std::stoi(line) + curr_line;
			}else if(curr_line <= fn_end) {
				fn_positions.push_back(std::stoi(line));
			}else{
				instructions.push_back(std::stoi(line));
			}
			curr_line++;
		}

		CPU cpu(false, tot_vars, instructions, program_start);
		cpu.fn_positions = fn_positions;

		while(!cpu.isHalted()) {
			cpu.tick();
		}
	//}
	
	return 0;
}