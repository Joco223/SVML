#pragma once

#include <iostream>
#include <string>

#include "Lexer.h"

namespace Error_handler {

	extern bool compilable;

	void error_out(std::string error);
	void error_part_out(std::string error, int pos, int line, int length, int file_index);
};