#pragma once

#include <iostream>
#include <vector>
#include <string>

class CPU {
public:
	struct instruction {
		unsigned char op_code;
		std::vector<unsigned int> args;
	};
	

	struct function {
		int program_counter, id; //Local program counter for reach instruction and id used for calling functions

		std::vector<instruction> instructions; //Local function instructions
		std::vector<unsigned int> memory; //Local function memory
		std::vector<unsigned int> offsets; //Offsets from each variable to the start of the next one
	};
	
	CPU(std::vector<function>); //Constructor is supplied with function templates
	void tick();
private:
	bool halt;
	
	std::vector<unsigned int> registers; //Int registers
	std::vector<float> f_registers; //Float registers
	
	std::vector<function> function_templates; //Function templates for calling functions
	std::vector<function> function_stack; //Stack of functions that CPU is working with
	unsigned int fn_stack_top = 0;
	
	unsigned int get_mem_offset(unsigned int index, function& target_fn);
	void execute(instruction c_ins);
};
