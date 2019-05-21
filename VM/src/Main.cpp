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
		int tot_vars = 0;
		std::vector<int> instructions;

		for(std::string line; getline(input, line);) {
			if(first) {
				tot_vars = std::stoi(line);
				first = false;
			}else{
				instructions.push_back(std::stoi(line));
			}
		}

		CPU cpu(false, tot_vars, instructions);

		while(!cpu.isHalted()) {
			cpu.tick();
		}
	//}
	
	return 0;
}