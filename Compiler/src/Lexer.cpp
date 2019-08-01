#include "Lexer.h"

namespace Lexer {

	std::string load_file(std::string& file_path) {
		std::ifstream f(file_path.c_str());

		if(!f.good()) {
			std::cout << "Error opening file: " << file_path << ", ignoring.\n";
			return "";
		}
		
		std::string content((std::istreambuf_iterator<char>(f)), (std::istreambuf_iterator<char>()));
		
		f.close();

		return content;
	};
	void clean(std::string& input) {
		while(input[0] == ' ') {
			input.erase(input.begin());
		}
		while(input[input.length()] == ' ') {
			input.erase(input.end());
		}
	}

	bool is_int(const std::string& s) {
		if(s.empty() || ((!isdigit(s[0])) && (s[0] != '-') && (s[0] != '+'))) return false;

		char* p;
		strtol(s.c_str(), &p, 10);

		return (*p == 0);
	}

	bool matches_type(std::string type) {
		for(auto& i : types) {
			if(type == i) {
				return true;
			}
		}
		return false;
	}

	void process_chunk(std::string& chunk, int line, std::vector<token>& tokens) {
		token t = {-1, line, ""};
		if(chunk != "") {
			if(matches_type(chunk)) {
				t = {lexer_type,  line, chunk};
			}else if(chunk == "if") {
				t = {lexer_if, line, "if"};
			}else if(chunk == "else") {
				t = {lexer_else, line, "else"};
			}else if(chunk == "+" || chunk == "-" || chunk == "*" || chunk == "/" || chunk == "%") {
				t = {lexer_arithmetic, line, chunk};
			}else if(chunk == "<" || chunk == ">" || chunk == "<=" || chunk == ">=" || chunk == "==" || chunk == "!=") {
				t = {lexer_logic, line, chunk};
			}else if(chunk == "while") {
				t = {lexer_while, line, "while"};
			}else if(chunk == "return") {
				t = {lexer_return,  line, "return"};
			}else{
				if(is_int(chunk)) {
					t = {lexer_value, line, chunk};
				}else{
					t = {lexer_identifier, line, chunk};
				}
			}
			chunk = "";
			tokens.push_back(t);
		}
	}

	std::vector<token> process(std::string& file_path) {
		std::string input = load_file(file_path);
		std::vector<token> tokens;
		if(input == "")
			return tokens;
		std::string chunk = "";
		int line = 1;
		for(int i = 0; i < input.length(); i++) {
			if(input[i] == ';') {
				process_chunk(chunk, line, tokens);
				tokens.push_back({lexer_eoi, line, ";"});
			}else if(input[i] == '{') {
				process_chunk(chunk, line, tokens);
				tokens.push_back({lexer_ocb, line, "{"});
			}else if(input[i] == '}') {
				process_chunk(chunk, line, tokens);
				tokens.push_back({lexer_ccb, line, "}"});
			}else if(input[i] == '(') {
				process_chunk(chunk, line, tokens);
				tokens.push_back({lexer_ob, line, "("});
			}else if(input[i] == ')') {
				process_chunk(chunk, line, tokens);
				tokens.push_back({lexer_cb, line, ")"});
			}else if(input[i] == '[') {
				process_chunk(chunk, line, tokens);
				tokens.push_back({lexer_osb, line, "["});
			}else if(input[i] == ']') {
				process_chunk(chunk, line, tokens);
				tokens.push_back({lexer_csb, line, "]"});
			}else if(input[i] == ',') {
				process_chunk(chunk, line, tokens);
				tokens.push_back({lexer_comma, line, ","});
			}else if(input[i] == '.') {
				process_chunk(chunk, line, tokens);
				tokens.push_back({lexer_point, line, "."});
			}else if(input[i] == '=') {
				if(input[i + 1] == '=') {
					process_chunk(chunk, line, tokens);
					tokens.push_back({lexer_logic, line, "=="});
					i++;
				}else{
					process_chunk(chunk, line, tokens);
					tokens.push_back({lexer_assign, line, "="});
				}
			}else if(input[i] == '+') {
				process_chunk(chunk, line, tokens);
				tokens.push_back({lexer_arithmetic, line, "+"});
			}else if(input[i] == '-') {
				process_chunk(chunk, line, tokens);
				tokens.push_back({lexer_arithmetic, line, "-"});
			}else if(input[i] == '*') {
				process_chunk(chunk, line, tokens);
				tokens.push_back({lexer_arithmetic, line, "*"});
			}else if(input[i] == '/') {
				if(input[i + 1] == '/') {
					while(i < input.length()-1) {
						if(input[i + 1] != '\n') {
							i++;
						}else{
							break;
						}	
					}
				}else{
					process_chunk(chunk, line, tokens);
					tokens.push_back({lexer_arithmetic, line, "/"});
				}		
			}else if(input[i] == '%') {
				process_chunk(chunk, line, tokens);
				tokens.push_back({lexer_arithmetic, line, "%"});
			}else if(input[i] == '^') {
				process_chunk(chunk, line, tokens);
				tokens.push_back({lexer_arithmetic, line, "^"});
			}else if(input[i] == '<') {
				if(input[i + 1] == '=') {
					process_chunk(chunk, line, tokens);
					tokens.push_back({lexer_logic, line, "<="});
					i++;
				}else{
					process_chunk(chunk, line, tokens);
					tokens.push_back({lexer_logic, line, "<"});
				}
			}else if(input[i] == '>') {
				if(input[i + 1] == '=') {
					process_chunk(chunk, line, tokens);
					tokens.push_back({lexer_logic, line, ">="});
					i++;
				}else{
					process_chunk(chunk, line, tokens);
					tokens.push_back({lexer_logic, line, ">"});
				}
			}else if(input[i] == '!' && input[i + 1] == '=') {
				process_chunk(chunk, line, tokens);
				tokens.push_back({lexer_logic, line, "!="});
			}else if(input[i] == '@') {
				std::string use = "";
				i++;
				while(i < input.length()-1) {
					if(input[i] != ' ') {
						use = use + input[i];
						i++;
					}else{
						i++;
						break;
					}	
				}
				if(use == "use") {
					std::string additional_input = "";
					while(i < input.length()-1) {
						if(input[i] != ';') {
							additional_input = additional_input + input[i];
							i++;
						}else{
							break;
						}	
					}
					std::vector<token> additional_tokens = process(additional_input);
					for(auto& i : additional_tokens)
						tokens.push_back(i);
				}else{
					std::cout << "Invalid macro: @" << use << " on line: " << line << '\n';
					tokens.clear();
					return tokens;
				}
			}else if(input[i] == ' ') {
				process_chunk(chunk, line, tokens);
			}else if(input[i] == '\n') {
				line++;
			}else if(input[i] != '\t' && input[i] != '\n') {
				chunk = chunk + input[i];
			}else if(input[i] != '\t') {
				std::cout << "Invalid token: <" << input[i] << "> on line: " << line << '\n';
				tokens.clear();
				return tokens;
			}

			std::cout << "\rLexer: Processing file: " << file_path << " - [";
			float percent = (float)i / (float)input.length();
			int filled = ceil(percent) * 20;
			int empty = 20 - filled;
			for(int j = 0; j < filled; j++)
				std::cout << '#';
			for(int j = 0; j < empty; j++)
				std::cout << '-';
			std::cout << "] - " << round(percent)*100 << "%/100%\r";
		}

		return tokens;
	}
}
