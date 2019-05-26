#include "CPU.h"

CPU::CPU(bool debug_, int var_size, std::vector<int> ins, int program_start) 
	:
	halt(false),
	debug(debug_),
	overflow(false),
	underflow(false),
	program_counter(program_start)
{
	registers.resize(8); //8 general purpose registers
	variables.resize(var_size);
	instructions = ins;
}

bool CPU::isHalted() {return halt;}

int CPU::handle_arg(int arg) {
	if(arg < 0) {
		return arg*-1 + 1;
	}else{
		return arg;
	}
}

//Execute the next instruction which is loded in the OPcode register
void CPU::execute() {
	int arg1 = instructions[program_counter+1];
	if(program_counter+1 < instructions.size());
	int arg2 = instructions[program_counter+2];
	int arg3 = 0;
	int arg4 = 0;
	
	switch(instructions[program_counter]) {
		case 0x0: //Halt
			std::cerr << "\nThe CPU has been halted.\n";
			halt = true;
			if(debug) {
				std::ofstream logFile("output.log");
				for(auto& i : log) {
					logFile << i;
				}
				logFile.close();
			}
			break;

		//Memory management instructions-------------------------------------------------------------------------------------------------------------
		case 0x1: //Set A to B
			registers[arg1] = arg2;
			program_counter += 3;
			break;
		case 0x2: //Read regA to regB
			registers[arg1] = registers[arg2];
			program_counter += 3;
			break;
		case 0x3: //Read a variable
			registers[arg1] = variables[arg2];
			program_counter += 3;
			break;
		case 0x4: //Write to a variable
			variables[arg2] = registers[arg1];
			program_counter += 3;
			break;

		//Arithmetic instructions--------------------------------------------------------------------------------------------------------------------
		case 0x6: //Addition
			if(registers[arg2] + registers[arg1] > std::numeric_limits<unsigned int>::max()) {overflow = true;}
			registers[arg1] = registers[arg1] + registers[arg2];
			program_counter += 3;
			break;
		case 0x7: //Subtraction
			if(registers[arg1] - registers[arg2] < 0) {underflow = true;}
			registers[arg1] = registers[arg1] - registers[arg2];
			program_counter += 3;
			break;
		case 0x8: //Multiplication
			if(registers[arg1] * registers[arg2] > std::numeric_limits<unsigned int>::max()) {overflow = true;}
			registers[arg1] = registers[arg1] * registers[arg2];
			program_counter += 3;
			break;
		case 0xA: //Intiger divison
			if(registers[arg2] == 0) {std::cerr << "Error, can't divide by zero. Error memory location: " << program_counter << '\n'; halt = true; break;}
			registers[arg1] = registers[arg1] / registers[arg2];
			program_counter += 3;
			break;
		case 0xB: //Modulo
			registers[arg1] = registers[arg1] % registers[arg2];
			program_counter += 3;
			break;
		case 0xC: //Increment the value in a register
			if(registers[arg1] + 1 > std::numeric_limits<unsigned int>::max()) {overflow = true;}
			registers[arg1]++;
			program_counter += 2;
			break;
		case 0xD: //Decrement the value in a register
			if(registers[arg1] - 1 < 0) {underflow = true;}
			registers[arg1]--;
			program_counter += 2;
			break;
		case 0xE: //Clear a register
			registers[arg1] = 0;
			program_counter += 2;
			break;

		//Bitwise instructions-----------------------------------------------------------------------------------------------------------------------
		case 0xF: //Bitwise and
			registers[arg1] = registers[arg1] & registers[arg2];
			program_counter += 2;
			break;
		case 0x10: //Bitwise or
			registers[arg1] = registers[arg1] | registers[arg2];
			program_counter += 2;
			break;
		case 0x11: //Bitwise not
			registers[arg1] = registers[arg1] ^ 1;
			program_counter += 2;
			break;
		case 0x12: //Bitwise shift right
			registers[arg1] = registers[arg1] >> registers[arg2];
			program_counter += 2;
			break;
		case 0x13: //Bitwise shift left
			registers[arg1] = registers[arg1] << registers[arg2];
			program_counter += 2;
			break;

		//Logical instructions-----------------------------------------------------------------------------------------------------------------------
		case 0x14: //A == B
			registers[arg1] = registers[arg1] == registers[arg2];
			program_counter += 3;
			break;
		case 0x15: //A != B
			registers[arg1] = registers[arg1] != registers[arg2];
			program_counter += 3;
			break;
		case 0x16: //A > B
			registers[arg1] = registers[arg1] > registers[arg2];
			program_counter += 3;
			break;

		case 0x17: //A >= B
			registers[arg1] =registers[arg1] >= registers[arg2];
			program_counter += 3;
			break;
		

		//Flow control instructions------------------------------------------------------------------------------------------------------------------
		case 0x18: //Jump
			program_counter = arg1;
			break;
		case 0x19: //Jump if the value in a register is equal to 0
			if(registers[arg1] == 0) {
				program_counter = arg2;
				std::cout << std::hex << instructions[program_counter] << '\n';
			}else{
				program_counter += 3;
			}
			break;
		case 0x1A: //Jump if the value in a register is equal to 1
			if(registers[arg1] == 1) {
				program_counter = arg2;
			}else{
				program_counter += 3;
			}
			break;
		case 0x1B: //Jump if overflow flag is on
			if(overflow) {
				program_counter = arg1;
				overflow = false;
			}else{
				program_counter += 2;
			}
			break;
		case 0x1C: //Jump if udnerflowS flag is on
			if(underflow) {
				program_counter = arg1;
				underflow = false;
			}else{
				program_counter += 2;
			}
			break;
		case 0x1D: //Reset the flags
			overflow = false;
			underflow = false;
			program_counter++;
			break;

		//Stack operation instructions---------------------------------------------------------------------------------------------------------------
		case 0x1E:  //Push parameter
			tmp_parameters.push_back(registers[arg1]);
			program_counter += 2;
			break;
		
		case 0x1F: { //Push function call
			function_call tmp;
			for(auto i : tmp_parameters) {tmp.parameters.push_back(i);}
			tmp_parameters.clear();
			tmp.return_pos = program_counter + 2;
			fn_stack.push_back(tmp);
			program_counter = fn_positions[arg1];
			reg_stack.push_back(registers);
			break; }

		case 0x20: //Read parameter
			registers[arg1] = fn_stack[fn_stack.size()-1].parameters[arg2];
			program_counter += 3;
			break;
		
		case 0x21: //Write parameter
			fn_stack[fn_stack.size()-1].parameters[arg2] = registers[arg1];
			program_counter += 3;
			break;

		case 0x22: {//Return
			int tmp = registers[0];
			program_counter = fn_stack[fn_stack.size()-1].return_pos;
			fn_stack.pop_back();
			registers = reg_stack[reg_stack.size()-1];
			reg_stack.pop_back();
			return_reg = tmp;
			break; }

		case 0x23:
			registers[arg1] = return_reg;
			return_reg = 0;
			program_counter += 2;
			break;
		
		/*case 0x1E: //Push contents of A to the temp stack
			tempStack.push_back(registers[arg1]);
			program_counter += 2;
			break;
		case 0x1F: //Push contents of A to the stack
			stack.push(registers[arg1]);
			program_counter += 2;
			break;
		case 0x20: //Pop top of the stack and store it in a register
			if(!stack.isEmpty()) {
				registers[arg1] = stack.peek();
				stack.pop();
				program_counter += 2;
			}else{
				std::cerr << "Error, can't pop from stack when the stack is empty. Error memory location: " << program_counter << '\n';
				halt = true;
			}
			break; 
		case 0x21: //Push next instruction memory location to the stack and jump to A
			if(stack.getLength() + tempStack.size() + 1 <= 2048) {
				stack.push(arg1);
				stack.push(program_counter + 3);
				for(int i = 0; i < tempStack.size(); ++i) {stack.push(tempStack[i]);}
				tempStack.clear();
				program_counter = arg2;
			}else{
				std::cerr << "Error, can't push new element to the stack when stack is full. Error memory location: " << program_counter << '\n';
				halt = true;
			}
			break;
		case 0x22: //Pop top of the stack and jump to it
			if(!stack.isEmpty()) {
				program_counter = stack.peek();
				stack.pop();	
				registers[stack.peek()] = registers[arg1];
				stack.pop();
			}else{
				std::cerr << "Error, can't pop from stack when the stack is empty. Error memory location: " << program_counter << '\n';
				halt = true;
			}
			break;*/

		//I/O instructions---------------------------------------------------------------------------------------------------------------------------
		case 0x24: //Print a single signed int to the console
			//std::cout << registers[arg1];
			program_counter += 2;
			break;
		case 0x25: //Print a single character to the console
			std::cout << (char)(handle_arg(arg4));
			program_counter++;
			break;
		case 0x26:
			std::cout << '\n';
			program_counter++;
			break;
		/*case 0x26: { //Takes a line of input and stores it from a memory address onwards
			std::string tmp = "";
			std::getline(std::cin, tmp);
			u16 memoryLocation = decodeParamOut(addTwoU8(arg1, arg2));
			for(u16 i = 0; i < tmp.length(); ++i) {memory[memoryLocation + i] = (int)tmp[i];}
			program_counter++;
			break; }*/

		//Default error------------------------------------------------------------------------------------------------------------------------------
		default:
			std::cout << "error " << program_counter << ' ' << instructions.size()-1 << '\n';
			//std::cerr << "Unknown instruction: 0x" << std::hex << (int)instructions[program_counter] << '\n';
			halt = true;
			break;
	}
}

//If the CPU is not halted, load the next instruction and execute it
void CPU::tick() {
	if(program_counter >= instructions.size()-1) {halt = true;}
	if(!halt) {	
		//std::cout << std::dec << program_counter << ' ' << std::hex << instructions[program_counter] << '\n';
		/*if(debug) { //Logging to a file if debug mode is on
			std::string tmp = "Program Counter: " + std::to_string(program_counter);
			tmp = tmp + ", memory at Program Counter: " + std::to_string((u16)memory[program_counter]);
			tmp = tmp + ", RA: " + std::to_string((u16)registers[0]);
			tmp = tmp + ", RB: " + std::to_string((u16)registers[1]);
			tmp = tmp + ", RC: " + std::to_string((u16)registers[2]);
			tmp = tmp + ", RD: " + std::to_string((u16)registers[3]);
			tmp = tmp + ", RE: " + std::to_string((u16)registers[4]);
			tmp = tmp + ", RF: " + std::to_string((u16)registers[5]);
			tmp = tmp + ", RG: " + std::to_string((u16)registers[6]);
			tmp = tmp + ", RH: " + std::to_string((u16)registers[7]);
			tmp = tmp + ", OVERFLOW: " + std::to_string((u16)overflow);
			tmp = tmp + ", UNDERFLOW: " + std::to_string((u16)underflow) + "\n";
			log.push_back(tmp);
		}*/
		execute();
	}
}