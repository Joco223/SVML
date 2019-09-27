#include "error_handling.h"

namespace Error_handler {

	bool compilable = true;

	void error_out(std::string error) {
		std::cout << "\n\033[31m" << "ERROR" << "\033[0m - ";
		std::cout << error << '\n';
		compilable = false;
	}

	void error_part_out(std::string error, int pos, int line, int length, int file_index) {
		std::cout << "\n\033[31m" << "ERROR" << "\033[0m - ";
		std::cout << error << '\n';

		std::cout << Lexer::lines[file_index][line-1] << '\n';
		for(int i = 0; i < pos; i++) {
			std::cout << ' ';
		}
		std::cout << '^';
		for(int i = 0; i < length-1; i++) {
			std::cout << '~';
		}
		std::cout << '\n';

		compilable = false;
	}
};