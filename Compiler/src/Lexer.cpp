#include "Lexer.h"

namespace Lexer {

	const std::string load_file(std::string& file_path) {
		std::ifstream f(file_path.c_str());

		if(!f.good()) {
			Error_handler::error_out("Error opening file: " + file_path + ".");
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

	void process_chunk(std::string& chunk, int line, std::vector<token>& tokens, int column, std::string& path, int file_index) {
		token t = {-1, line, column, file_index, path, ""};
		if(chunk != "") {
			if(matches_type(chunk)) {
				t = {tt_type, line, column, file_index, path, chunk};
			}else if(chunk == "if") {
				t = {tt_if, line, column, file_index, path, "if"};
			}else if(chunk == "else") {
				t = {tt_else, line, column, file_index, path, "else"};
			}else if(chunk == "+" || chunk == "-" || chunk == "*" || chunk == "/" || chunk == "%") {
				t = {tt_arithmetic, line, column, file_index, path, chunk};
			}else if(chunk == "<" || chunk == ">" || chunk == "<=" || chunk == ">=" || chunk == "==" || chunk == "!=") {
				t = {tt_logic, line, column, file_index, path, chunk};
			}else if(chunk == "while") {
				t = {tt_while, line, column, file_index, path, "while"};
			}else if(chunk == "return") {
				t = {tt_return,  line, column, file_index, path, "return"};
			}else{
				if(is_int(chunk)) {
					t = {tt_value, line, column, file_index, path, chunk};
				}else{
					t = {tt_identifier, line, column, file_index, path, chunk};
				}
			}
			chunk = "";
			tokens.push_back(t);
		}
	}

	std::vector<std::vector<std::string>> lines;
	int file_count = 0;

	std::vector<token> process(std::string& file_path, bool debug) {
		const std::string input = load_file(file_path);
		std::vector<token> tokens;
		if(input == "")
			return tokens;
		std::string chunk = "";
		int line = 1;
		auto start = std::chrono::high_resolution_clock::now();
		tokens.reserve(input.length());
		int since_last_nl = 0;
		std::string current_line = "";
		int current_file = file_count;
		file_count++;
		lines.resize(lines.size()+1);
		for(int i = 0; i < input.length(); i++) {

			if(input[i] != '\n') {
				if(input[i] == '\t') {
					std::string tmp = " ";
					current_line += tmp;
				}else{
					current_line.push_back(input[i]);
				}	
			}else{	
				lines[current_file].push_back(current_line);
				current_line = "";
			}

			if(input[i] == ';') {
				process_chunk(chunk, line, tokens, i - since_last_nl - chunk.length(), file_path, current_file);
				tokens.push_back({tt_eoi, line, i - since_last_nl, current_file, file_path, ";"});
			}else if(input[i] == '{') {
				process_chunk(chunk, line, tokens, i - since_last_nl - chunk.length(), file_path, current_file);
				tokens.push_back({tt_ocb, line, i - since_last_nl, current_file, file_path, "{"});
			}else if(input[i] == '}') {
				process_chunk(chunk, line, tokens, i - since_last_nl - chunk.length(), file_path, current_file);
				tokens.push_back({tt_ccb, line, i - since_last_nl, current_file, file_path, "}"});
			}else if(input[i] == '(') {
				process_chunk(chunk, line, tokens, i - since_last_nl - chunk.length(), file_path, current_file);
				tokens.push_back({tt_ob, line, i - since_last_nl, current_file, file_path, "("});
			}else if(input[i] == ')') {
				process_chunk(chunk, line, tokens, i - since_last_nl - chunk.length(), file_path, current_file);
				tokens.push_back({tt_cb, line, i - since_last_nl, current_file, file_path, ")"});
			}else if(input[i] == '[') {
				process_chunk(chunk, line, tokens, i - since_last_nl - chunk.length(), file_path, current_file);
				tokens.push_back({tt_osb, line, i - since_last_nl, current_file, file_path, "["});
			}else if(input[i] == ']') {
				process_chunk(chunk, line, tokens, i - since_last_nl - chunk.length(), file_path, current_file);
				tokens.push_back({tt_csb, line, i - since_last_nl, current_file, file_path, "]"});
			}else if(input[i] == ',') {
				process_chunk(chunk, line, tokens, i - since_last_nl - chunk.length(), file_path, current_file);
				tokens.push_back({tt_comma, line, i - since_last_nl, current_file, file_path, ","});
			}else if(input[i] == '.') {
				process_chunk(chunk, line, tokens, i - since_last_nl - chunk.length(), file_path, current_file);
				tokens.push_back({tt_point, line, i - since_last_nl, current_file, file_path, "."});
			}else if(input[i] == '=') {
				if(input[i + 1] == '=') {
					process_chunk(chunk, line, tokens, i - since_last_nl - chunk.length(), file_path, current_file);
					tokens.push_back({tt_logic, line, i - since_last_nl, current_file, file_path, "=="});
					i++;
				}else{
					process_chunk(chunk, line, tokens, i - since_last_nl - chunk.length(), file_path, current_file);
					tokens.push_back({tt_assign, line, i - since_last_nl, current_file, file_path, "="});
				}
			}else if(input[i] == '+') {
				process_chunk(chunk, line, tokens, i - since_last_nl - chunk.length(), file_path, current_file);
				tokens.push_back({tt_arithmetic, line, i - since_last_nl, current_file, file_path, "+"});
			}else if(input[i] == '-') {
				process_chunk(chunk, line, tokens, i - since_last_nl - chunk.length(), file_path, current_file);
				tokens.push_back({tt_arithmetic, line, i - since_last_nl, current_file, file_path, "-"});
			}else if(input[i] == '*') {
				process_chunk(chunk, line, tokens, i - since_last_nl - chunk.length(), file_path, current_file);
				tokens.push_back({tt_arithmetic, line, i - since_last_nl, current_file, file_path, "*"});
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
					process_chunk(chunk, line, tokens, i - since_last_nl - chunk.length(), file_path, current_file);
					tokens.push_back({tt_arithmetic, line, i - since_last_nl, current_file, file_path, "/"});
				}		
			}else if(input[i] == '%') {
				process_chunk(chunk, line, tokens, i - since_last_nl - chunk.length(), file_path, current_file);
				tokens.push_back({tt_arithmetic, line, i - since_last_nl, current_file, file_path, "%"});
			}else if(input[i] == '^') {
				process_chunk(chunk, line, tokens, i - since_last_nl - chunk.length(), file_path, current_file);
				tokens.push_back({tt_arithmetic, line, i - since_last_nl, current_file, file_path, "^"});
			}else if(input[i] == '<') {
				if(input[i + 1] == '=') {
					process_chunk(chunk, line, tokens, i - since_last_nl - chunk.length(), file_path, current_file);
					tokens.push_back({tt_logic, line, i - since_last_nl, current_file, file_path, "<="});
					i++;
				}else{
					process_chunk(chunk, line, tokens, i - since_last_nl - chunk.length(), file_path, current_file);
					tokens.push_back({tt_logic, line, i - since_last_nl, current_file, file_path, "<"});
				}
			}else if(input[i] == '>') {
				if(input[i + 1] == '=') {
					process_chunk(chunk, line, tokens, i - since_last_nl - chunk.length(), file_path, current_file);
					tokens.push_back({tt_logic, line, i - since_last_nl, current_file, file_path, ">="});
					i++;
				}else{
					process_chunk(chunk, line, tokens, i - since_last_nl - chunk.length(), file_path, current_file);
					tokens.push_back({tt_logic, line, i - since_last_nl, current_file, file_path, ">"});
				}
			}else if(input[i] == '!' && input[i + 1] == '=') {
				process_chunk(chunk, line, tokens, i - since_last_nl - chunk.length(), file_path, current_file);
				tokens.push_back({tt_logic, line, i - since_last_nl, current_file, file_path, "!="});
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
					Error_handler::error_out("Invalid macro: @" + use + " on line: " + std::to_string(line)); 
					tokens.clear();
					return tokens;
				}
			}else if(input[i] == ' ') {
				process_chunk(chunk, line, tokens, i - since_last_nl - chunk.length(), file_path, current_file);
			}else if(input[i] == '\n') {
				since_last_nl = i+1;
				line++;
			}else if(input[i] != '\t' && input[i] != '\n') {
				chunk = chunk + input[i];
			}else if(input[i] != '\t') {
				std::string tmp = "Invalid token: ";
				tmp.push_back(input[i]);
				Error_handler::error_part_out(tmp + " on: " + file_path + ":" + std::to_string(i-since_last_nl-1) + ":" + std::to_string(line), i-since_last_nl-1, line, 1, current_file);
				tokens.clear();
				return tokens;
			}

			if(i % 300 == 0 && debug) {
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
