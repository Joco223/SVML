#pragma once

#include <vector>
#include <string>
#include <iostream>

std::vector<std::pair<std::string, std::string>> handle_args(int argc, char** argv) {
	std::vector<std::pair<std::string, std::string>> arguments;

	bool has_input = false;

	for(int i = 1; i < argc; i++) {
		std::string argument = argv[i];

		if(argument[0] != '/') {
			if(argument == "-h") {
				std::cout << "Available arguments are:\n-i <input file>  - Input file for SVM.\n-d               - Debug mode, will output log to a file.\n-h               - Help about available parameters.";
				arguments.clear();
				return arguments;
			}else if(argument == "-i") {
				if(i == argc-1) {
					std::cout << "Argument without parameter provided: " << argv[i] << ", aborting.";
					arguments.clear();
					return arguments;
				}else{
					std::string parameter = argv[i+1];
					if(parameter == "-d" || parameter == "-i" || parameter == "-h") {
						std::cout << "Uknown parameter: " << argv[i+1] << " for argument -i, aborting.";
						arguments.clear();
						return arguments;
					}
					arguments.push_back({"input", parameter});
					i++;
					has_input = true;
				}
			}else if(argument == "-d") {
				arguments.push_back({"debug", "true"});
			}else{
				std::cout << "Unkwown argument: " << argument << " provided, aborting.\n";
				std::cout << "Available arguments are:\n-i <input file>  - Input file for SVM.\n-d               - Debug mode, will output log to a file.\n-h               - Help about available parameters.";
				arguments.clear();
				return arguments;
			}
		}
	}

	if(!has_input) {
		std::cout << "No input file argument provided.\n";
		std::cout << "Available arguments are:\n-i <input file>  - Input file for SVM.\n-d               - Debug mode, will output log to a file.\n-h               - Help about available parameters.";
		arguments.clear();
		return arguments;
	}

	return arguments;
}