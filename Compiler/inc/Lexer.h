#pragma once

#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <math.h>
#include <chrono>

namespace Lexer {

	struct token {
		int type, line;
		std::string name;
	};
	
	enum token_types {
		tt_eoi = 1,    //End of instruction (semicolon)
		tt_ocb,        //Open curly bracket
		tt_ccb,        //Closed curly bracket
		tt_ob,         //Open bracket
		tt_cb,         //Closed bracket
		tt_osb,        //Open square bracket
		tt_csb,        //Closed square braket
		tt_comma,
		tt_point,
		tt_assign,
		tt_type,       //int, void, Array, bool
		tt_value,
		tt_identifier,
		tt_struct,
		tt_arithmetic, //+, -, *, /, %
		tt_logic,      //<, >, <=, >=, ==, !=
		tt_return,
		tt_if,
		tt_else,
		tt_while
	};

	static const std::vector<std::string> types = {"int", "void", "bool"};

	const std::string load_file(std::string&);
	std::vector<token> process(std::string&, bool);

}
