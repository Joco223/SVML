#pragma once

#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>

namespace Lexer {

	struct token {
		int type, line;
		std::string name;
	};

	/*
	Token types:
	 1: LEXER_EOI - ;
	 2: LEXER_OCB - {
	 3: LEXER CCB - }
	 4: LEXER_OB - (
	 5: LEXER_CB - )
	 6: LEXER_COMMA - ,
	 7: LEXER_ASSIGNEMENT - =
	 8: LEXER_TYPE - uint8, int8, uint16, int16, bool
	 9: LEXER_RETURN - ret
	10: LEXER_IF - if
	11: LEXER_ELSE - else
	12: LEXER_ARITHMETIC - +, -, *, /, %, ^
	13: LEXER_LOGIC - <, >, <=, >=, ==, !=
	14: LEXER_VALUE - Any number
	15: LEXER_IDENT - Identifier
	16: LEXER_WHILE - while
	*/

	static const std::vector<std::string> types = {"var"};

	std::string load_file(std::string&);
	std::vector<token> process(std::string&);

}
