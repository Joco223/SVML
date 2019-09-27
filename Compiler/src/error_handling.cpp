#include "error_handling.h"

namespace Error_handler {

	bool compilable = true;

	void error_out(std::string error) {
		std::cout << "\n\033[31m" << "ERROR" << "\033[0m - ";
		std::cout << error << '\n';
		compilable = false;
	}
};