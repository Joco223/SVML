#pragma once

#include <vector>
#include <string>
#include <iostream>

std::vector<std::pair<std::string, std::string>>* handle_args(int argc, char** argv) {
	std::vector<std::pair<std::string, std::string>>* arguments;

	bool has_input = false;

	for(int i = 1; i < argc; i++) {
		std::string argument = argv[i];

		if(argument == "-h") {
			std::cout << "Available arguments are:\n-i <input file>  - Input file for SVM.\n-d               - Debug mode, will output log to a file.\n-s <stack size>  - How many elements can the stack hold.\n-m <memory size> - How big is the program memory.\n-h               - Help about available parameters.";
			return nullptr;
		}else if(argument == "-i") {
			if(i == argc-1) {
				std::cout << "Argument without parameter provided: " << argv[i] << ", aborting.\n";
				return nullptr;
			}else{
				std::string parameter = argv[i+1];
				if(parameter == "-d" || parameter == "-s" || parameter == "-i" || parameter == "-m" || parameter == "-h") {
					std::cout << "Uknown parameter: " << argv[i+1] << " for argument -i, aborting.\n";
					return nullptr;
				}
				arguments->push_back({"input", parameter});
				i++;
				has_input = true;
			}
		}else if(argument == "-d") {
			arguments->push_back({"debug", "true"});
		}else if(argument == "-s") {
			if(i == argc-1) {
				std::cout << "Argument without parameter provided: " << argv[i] << ", aborting.\n";
				return nullptr;
			}else{
				std::string parameter = argv[i+1];
				if(parameter == "-d" || parameter == "-s" || parameter == "-i" || parameter == "-m" || parameter == "-h") {
					std::cout << "Uknown parameter: " << argv[i+1] << " for argument -s, aborting.\n";
					return nullptr;
				}
				arguments->push_back({"stack", argv[i+1]});
				i++;
			}
		}else if(argument == "-m") {
			if(i == argc-1) {
				std::cout << "Argument without parameter provided: " << argv[i] << ", aborting.\n";
				return nullptr;
			}else{
				std::string parameter = argv[i+1];
				if(parameter == "-d" || parameter == "-s" || parameter == "-i" || parameter == "-m" || parameter == "-h") {
					std::cout << "Uknown parameter: " << argv[i+1] << " for argument -m, aborting.\n";
					return nullptr;
				}
				arguments->push_back({"memory", argv[i+1]});
				i++;
			}
		}else{
			std::cout << "Unkwown argument: " << argument << " provided, aborting.\n";
			std::cout << "Available arguments are:\n-i <input file>  - Input file for SVM.\n-d               - Debug mode, will output log to a file.\n-s <stack size>  - How many elements can the stack hold.\n-m <memory size> - How big is the program memory.\n-h               - Help about available parameters.";
			return nullptr;
		}
	}

	if(!has_input) {
		std::cout << "No input file argument provided.\n";
		std::cout << "Available arguments are:\n-i <input file>  - Input file for SVM.\n-d               - Debug mode, will output log to a file.\n-s <stack size>  - How many elements can the stack hold.\n-m <memory size> - How big is the program memory.\n-h               - Help about available parameters.";
		return nullptr;
	}

	return arguments;
}