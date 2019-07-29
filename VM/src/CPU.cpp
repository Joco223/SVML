#include "CPU.h"

CPU::CPU(std::vector<function> templates) 
	:
	halt(false)
{
	registers.resize(8);
	function_templates = templates;
	function_stack.push_back(function_templates[0]);
}

unsigned int CPU::get_mem_offset(unsigned int index, function& target_fn) {
	unsigned int final_offset = 0;
	for(unsigned int i = 0; i < index; i++) {
		final_offset += target_fn.offsets[i];
	}
	return final_offset;
}

void CPU::execute(instruction c_ins) {
	switch(c_ins.op_code) {
		case 0x0: //Halt
			halt = true;
			break;

		//Memory management instructions-------------------------------------------------------------------------------------------------------------
		case 0x1: //Set A to B | arg1 - target register, arg2 - value
			registers[c_ins.args[0]] = c_ins.args[1];
			break;
		case 0x2: //Read regA to regB | arg1 - target register, arg2 - source register
			registers[c_ins.args[0]] = registers[c_ins.args[1]];
			break;
		case 0x3: //Read from function memory | arg1 - targtet register, arg2 - variable index, arg3 - variable offset
			registers[c_ins.args[0]] = function_stack[fn_stack_top].memory[get_mem_offset(c_ins.args[1], function_stack[fn_stack_top]) + c_ins.args[2]];
			break;
		case 0x4: //Write to function memory | arg1 - targtet register, arg2 - variable index, arg3 - variable offset
			function_stack[fn_stack_top].memory[get_mem_offset(c_ins.args[1], function_stack[fn_stack_top]) + c_ins.args[2]] = registers[c_ins.args[0]];
			break;
		case 0x5: //Allocate memory | arg1 - new variable size
			function_stack[fn_stack_top].offsets.push_back(c_ins.args[0]);
			function_stack[fn_stack_top].memory.resize(function_stack[fn_stack_top].memory.size() + c_ins.args[0]);
			break;
		case 0x6: //Deallocate memory | arg1 - variable index
			function_stack[fn_stack_top].memory.erase(function_stack[fn_stack_top].memory.begin() + get_mem_offset(c_ins.args[0], function_stack[fn_stack_top]), function_stack[fn_stack_top].memory.begin() + get_mem_offset(c_ins.args[0], function_stack[fn_stack_top]) + function_stack[fn_stack_top].offsets[c_ins.args[0]]);
			function_stack[fn_stack_top].offsets.erase(function_stack[fn_stack_top].offsets.begin() + c_ins.args[0]);
			break;

		//Arithmetic instructions--------------------------------------------------------------------------------------------------------------------
		case 0x7: //Addition
			registers[c_ins.args[0]] = registers[c_ins.args[0]] + registers[c_ins.args[1]];
			break;
		case 0x8: //Subtraction
			registers[c_ins.args[0]] = registers[c_ins.args[0]] - registers[c_ins.args[1]];
			break;
		case 0x9: //Multiplication
			registers[c_ins.args[0]] = registers[c_ins.args[0]] * registers[c_ins.args[1]];
			break;
		case 0xA: //Intiger divison (unsigned)
			registers[c_ins.args[0]] = registers[c_ins.args[0]] / registers[c_ins.args[1]];
			break;
		case 0xB: //Intiger divison (signed)
			registers[c_ins.args[0]] = static_cast<int>(registers[c_ins.args[0]]) / static_cast<int>(registers[c_ins.args[1]]);
			break;
		case 0xC: //Modulo
			registers[c_ins.args[0]] = registers[c_ins.args[0]] % registers[c_ins.args[1]];
			break;
		case 0xD: //Increment the value in a register
			registers[c_ins.args[0]]++;
			break;
		case 0xE: //Decrement the value in a register
			registers[c_ins.args[0]]--;
			break;
		case 0xF: //Clear a register
			registers[c_ins.args[0]] = 0;
			break;

		//Bitwise instructions-----------------------------------------------------------------------------------------------------------------------
		case 0x10: //Bitwise and
			registers[c_ins.args[0]] = registers[c_ins.args[0]] & registers[c_ins.args[1]];
			break;
		case 0x11: //Bitwise or
			registers[c_ins.args[0]] = registers[c_ins.args[0]] | registers[c_ins.args[1]];
			break;
		case 0x12: //Bitwise not
			registers[c_ins.args[0]] = registers[c_ins.args[0]] ^ 1;
			break;
		case 0x13: //Bitwise shift right
			registers[c_ins.args[0]] = registers[c_ins.args[0]] >> registers[c_ins.args[1]];
			break;
		case 0x14: //Bitwise shift left
			registers[c_ins.args[0]] = registers[c_ins.args[0]] << registers[c_ins.args[1]];
			break;

		//Logical instructions-----------------------------------------------------------------------------------------------------------------------
		case 0x15: //A == B
			registers[c_ins.args[0]] = registers[c_ins.args[0]] == registers[c_ins.args[1]];
			break;
		case 0x16: //A != B
			registers[c_ins.args[0]] = registers[c_ins.args[0]] != registers[c_ins.args[1]];
			break;
		case 0x17: //A > B
			registers[c_ins.args[0]] = registers[c_ins.args[0]] > registers[c_ins.args[1]];
			break;
		case 0x18: //A >= B
			registers[c_ins.args[0]] =registers[c_ins.args[0]] >= registers[c_ins.args[1]];
			break;
		

		//Flow control instructions------------------------------------------------------------------------------------------------------------------
		case 0x19: //Jump
			function_stack[fn_stack_top].program_counter = c_ins.args[0];
			break;
		case 0x1A: //Jump if the value in a register is equal to 0
			if(registers[c_ins.args[0]] == 0) {
				function_stack[fn_stack_top].program_counter = c_ins.args[1];
			}else{
				function_stack[fn_stack_top].program_counter++;
			}
			break;
		case 0x1B: //Jump if the value in a register is equal to 1
			if(registers[c_ins.args[0]] == 1) {
				function_stack[fn_stack_top].program_counter = c_ins.args[1];
			}else{
				function_stack[fn_stack_top].program_counter++;
			}
			break;
			
		//Function stack control---------------------------------------------------------------------------------------------------------------------
		case 0x1C: {//Function call
			function new_fn = function_templates[c_ins.args[0]];
			unsigned int new_offset = 0;
			for(int i = 1; i < c_ins.args.size(); i++) {
				unsigned int size = function_stack[fn_stack_top].offsets[c_ins.args[i]];
				new_fn.offsets.push_back(size);
				
				new_fn.memory.resize(new_fn.memory.size() + size);
				for(int j = 0; j < size; j++) {
					new_fn.memory[new_offset + j] = function_stack[fn_stack_top].memory[get_mem_offset(c_ins.args[i], function_stack[fn_stack_top]) + j];
				}
				new_offset += size;
			}
			function_stack.push_back(new_fn);
			fn_stack_top++;
			break; }
		case 0x1D: {//Return
			int size = function_stack[fn_stack_top].offsets[c_ins.args[0]];
			int new_index = function_stack[fn_stack_top-1].offsets.size()-1;
			function_stack[fn_stack_top-1].offsets.push_back(size);
			for(int i = 0; i < size; i++) {
				function_stack[fn_stack_top-1].memory[get_mem_offset(new_index, function_stack[fn_stack_top-1]) + i] = function_stack[fn_stack_top].memory[get_mem_offset(c_ins.args[0], function_stack[fn_stack_top]) + i];
			}
			function_stack.erase(function_stack.end());
			fn_stack_top--;
			break; }

		//I/O instructions---------------------------------------------------------------------------------------------------------------------------
		case 0x24: //Put a value from a register into console (debug)
			std::cout << registers[c_ins.args[0]];
			break;

		//Default error------------------------------------------------------------------------------------------------------------------------------
		default:
			std::cout << "error " << function_stack[fn_stack_top].program_counter << ' ' << function_stack[fn_stack_top].instructions[function_stack[fn_stack_top].program_counter].op_code << '\n';
			//std::cerr << "Unknown instruction: 0x" << std::hex << (int)instructions[program_counter] << '\n';
			halt = true;
			break;
	}
}

void CPU::tick() {
	if(!halt) {	
		execute(function_stack[fn_stack_top].instructions[function_stack[fn_stack_top].program_counter]);
	}
}
