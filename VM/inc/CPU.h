#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <limits>

#include "Stack.h"

typedef unsigned char u8;

class CPU {
private:
	bool halt;
	bool overflow;
	bool underflow;
	bool debug;

	struct function_call {
		std::vector<int> parameters;
		int return_pos;
	};

	std::vector<function_call> fn_stack;
	std::vector<int> tmp_parameters;
	
	
	std::vector<int> registers;

	std::vector<std::vector<int>> reg_stack;
	int return_reg = 0;
	
	unsigned int program_counter;
	std::vector<int> variables;

	std::vector<std::string> log;

	void execute();
	int handle_arg(int);
public:
	std::vector<int> instructions;
	std::vector<int> fn_positions;
	
	CPU(bool, int, std::vector<int>, int);
	bool isHalted();
	void tick();
};