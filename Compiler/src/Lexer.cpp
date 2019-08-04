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
				t = {tt_type,  line, chunk};
			}else if(chunk == "if") {
				t = {tt_if, line, "if"};
			}else if(chunk == "else") {
				t = {tt_else, line, "else"};
			}else if(chunk == "+" || chunk == "-" || chunk == "*" || chunk == "/" || chunk == "%") {
				t = {tt_arithmetic, line, chunk};
			}else if(chunk == "<" || chunk == ">" || chunk == "<=" || chunk == ">=" || chunk == "==" || chunk == "!=") {
				t = {tt_logic, line, chunk};
			}else if(chunk == "while") {
				t = {tt_while, line, "while"};
			}else if(chunk == "return") {
				t = {tt_return,  line, "return"};
			}else{
				if(is_int(chunk)) {
					t = {tt_value, line, chunk};
				}else{
					t = {tt_identifier, line, chunk};
				}
			}
			chunk = "";
			tokens.push_back(t);
		}
	}

	std::vector<token> process(std::string& file_path, bool debug) {
		std::string input = load_file(file_path);
		std::vector<token> tokens;
		if(input == "")
			return tokens;
		std::string chunk = "";
		int line = 1;
		auto start = std::chrono::high_resolution_clock::now();
		for(int i = 0; i < input.length(); i++) {
			if(input[i] == ';') {
				process_chunk(chunk, line, tokens);
				tokens.push_back({tt_eoi, line, ";"});
			}else if(input[i] == '{') {
				process_chunk(chunk, line, tokens);
				tokens.push_back({tt_ocb, line, "{"});
			}else if(input[i] == '}') {
				process_chunk(chunk, line, tokens);
				tokens.push_back({tt_ccb, line, "}"});
			}else if(input[i] == '(') {
				process_chunk(chunk, line, tokens);
				tokens.push_back({tt_ob, line, "("});
			}else if(input[i] == ')') {
				process_chunk(chunk, line, tokens);
				tokens.push_back({tt_cb, line, ")"});
			}else if(input[i] == '[') {
				process_chunk(chunk, line, tokens);
				tokens.push_back({tt_osb, line, "["});
			}else if(input[i] == ']') {
				process_chunk(chunk, line, tokens);
				tokens.push_back({tt_csb, line, "]"});
			}else if(input[i] == ',') {
				process_chunk(chunk, line, tokens);
				tokens.push_back({tt_comma, line, ","});
			}else if(input[i] == '.') {
				process_chunk(chunk, line, tokens);
				tokens.push_back({tt_point, line, "."});
			}else if(input[i] == '=') {
				if(input[i + 1] == '=') {
					process_chunk(chunk, line, tokens);
					tokens.push_back({tt_logic, line, "=="});
					i++;
				}else{
					process_chunk(chunk, line, tokens);
					tokens.push_back({tt_assign, line, "="});
				}
			}else if(input[i] == '+') {
				process_chunk(chunk, line, tokens);
				tokens.push_back({tt_arithmetic, line, "+"});
			}else if(input[i] == '-') {
				process_chunk(chunk, line, tokens);
				tokens.push_back({tt_arithmetic, line, "-"});
			}else if(input[i] == '*') {
				process_chunk(chunk, line, tokens);
				tokens.push_back({tt_arithmetic, line, "*"});
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
					tokens.push_back({tt_arithmetic, line, "/"});
				}		
			}else if(input[i] == '%') {
				process_chunk(chunk, line, tokens);
				tokens.push_back({tt_arithmetic, line, "%"});
			}else if(input[i] == '^') {
				process_chunk(chunk, line, tokens);
				tokens.push_back({tt_arithmetic, line, "^"});
			}else if(input[i] == '<') {
				if(input[i + 1] == '=') {
					process_chunk(chunk, line, tokens);
					tokens.push_back({tt_logic, line, "<="});
					i++;
				}else{
					process_chunk(chunk, line, tokens);
					tokens.push_back({tt_logic, line, "<"});
				}
			}else if(input[i] == '>') {
				if(input[i + 1] == '=') {
					process_chunk(chunk, line, tokens);
					tokens.push_back({tt_logic, line, ">="});
					i++;
				}else{
					process_chunk(chunk, line, tokens);
					tokens.push_back({tt_logic, line, ">"});
				}
			}else if(input[i] == '!' && input[i + 1] == '=') {
				process_chunk(chunk, line, tokens);
				tokens.push_back({tt_logic, line, "!="});
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
					std::vector<token> additional_tokens = process(additional_input, debug);
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

			if(i % 100 == 0 && debug) {
				auto end = std::chrono::high_resolution_clock::now();
				std::cout << "\rLexer: Processing file: " << file_path << " - [";
				float percent = (float)(i+1) / (float)input.length();
				int filled = ceil(percent * 20);
				int empty = 20 - filled;
				for(int j = 0; j < filled; j++)
					std::cout << '#';
				for(int j = 0; j < empty; j++)
					std::cout << '-';
				std::cout << "] - " << (percent*100) << "%, time left: " << (((double)std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()/(double)(i+1)) * (input.length()-i-1) / 1000) << "s.";
				for(int k = 0; k < 20; k++)
					std::cout  << ' ';
				std::cout << '\r';
				//start = std::chrono::high_resolution_clock::now();
			}	
		}
		if(debug) {
			std::cout << "\rLexer: Processing file: " << file_path << " - [";
			for(int j = 0; j < 20; j++)
				std::cout << '#';
			std::cout << "] - " << 100 << "%, time left: 0s.";
			for(int k = 0; k < 20; k++)
				std::cout  << ' ';
			std::cout << '\n';
		}
		return tokens;
	}
}
