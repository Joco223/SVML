#pragma once

#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <math.h>

namespace Lexer {

	struct token {
		int type, line;
		std::string name;
	};
	
	enum token_types {
		lexer_eoi = 1,    //End of instruction
		lexer_ocb,        //Open curly bracket
		lexer_ccb,        //Closed curly bracket
		lexer_ob,         //Open bracket
		lexer_cb,         //Closed bracket
		lexer_osb,        //Open square bracket
		lexer_csb,        //Closed square braket
		lexer_comma,
		lexer_point,
		lexer_assign,
		lexer_type,       //int, void, Array, bool
		lexer_value,
		lexer_identifier,
		lexer_struct,
		lexer_arithmetic, //+, -, *, /, %
		lexer_logic,      //<, >, <=, >=, ==, !=
		lexer_return,
		lexer_if,
		lexer_else,
		lexer_while
	};

	static const std::vector<std::string> types = {"int", "void", "Array", "bool"};

	std::string load_file(std::string&);
	std::vector<token> process(std::string&);

}
