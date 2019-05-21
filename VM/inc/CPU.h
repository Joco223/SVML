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

	jci::Stack<unsigned int> stack;
	std::vector<unsigned int> tempStack;
	std::vector<int> registers;
	
	unsigned int program_counter;
	std::vector<int> variables;

	std::vector<std::string> log;

	void execute();
	int handle_arg(int);
public:
	std::vector<int> instructions;
	
	CPU(bool, int, std::vector<int>);
	bool isHalted();
	void tick();
};