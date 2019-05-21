#include "Lexer.h"

namespace Lexer {

	std::string load_file(std::string& file_path) {
		std::ifstream f(file_path.c_str());
		std::string content((std::istreambuf_iterator<char>(f)),
							(std::istreambuf_iterator<char>()));

		//content.erase(std::remove(content.begin(), content.end(), '\n'), content.end());

		return content;
	};

	void rs_start(std::string& input) {
		while(input[0] == ' ') {
			input.erase(input.begin());
		}
	}

	void rs_end(std::string& input) {
		while(input[input.length()] == ' ') {
			input.erase(input.end());
		}
	}

	void clean(std::string& input) {
		rs_start(input);
		rs_end(input);
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

	token process_chunk(bool& was_ass, std::string& chunk, int line) {
		token t = {-1, line, ""};
		clean(chunk);
		if(chunk != "") {
			if(matches_type(chunk)) {
				t = {8,  line, chunk};
			}else if(chunk == "if") {
				t = {10, line, "if"};
			}else if(chunk == "else") {
				t = {11, line, "else"};
			}else if(chunk == "+" || chunk == "-" || chunk == "*" || chunk == "/" || chunk == "%" || chunk == "^") {
				t = {12, line, chunk};
			}else if(chunk == "<" || chunk == ">" || chunk == "<=" || chunk == ">=" || chunk == "==" || chunk == "!=") {
				t = {13, line, chunk};
			}else if(chunk == "while") {
				t = {16, line, "while"};
			}else if(chunk == "ret") {
				t = {9,  line, "ret"};
			}else if(chunk == "struct") {
				t = {17, line, "struct"};
			}else{
				if(is_int(chunk) || chunk == "true" || chunk == "false") {
					t = {14, line, chunk};
				}else{
					t = {15, line, chunk};
				}
			}
			chunk = "";
			return t;
		}else{
			return t;
		}
	}

	std::vector<token> process(std::string& input) {
		std::vector<token> tokens;
		std::string chunk = "";
		bool was_ass = false;
		int line = 1;
		for(int i = 0; i < input.length(); i++) {
			if(input[i] == ';') {
				tokens.push_back(process_chunk(was_ass, chunk, line));
				tokens.push_back({1, line, ";"});
				was_ass = false;
			}else if(input[i] == '{') {
				tokens.push_back(process_chunk(was_ass, chunk, line));
				tokens.push_back({2, line, "{"});
			}else if(input[i] == '}') {
				tokens.push_back(process_chunk(was_ass, chunk, line));
				tokens.push_back({3, line, "}"});
			}else if(input[i] == '(') {
				tokens.push_back(process_chunk(was_ass, chunk, line));
				tokens.push_back({4, line, "("});
			}else if(input[i] == ')') {
				tokens.push_back(process_chunk(was_ass, chunk, line));
				tokens.push_back({5, line, ")"});
			}else if(input[i] == ',') {
				tokens.push_back(process_chunk(was_ass, chunk, line));
				tokens.push_back({6, line, ","});
			}else if(input[i] == '=') {
				tokens.push_back(process_chunk(was_ass, chunk, line));
				tokens.push_back({7, line, "="});
				was_ass = true;
			}else if(input[i] == ' ') {
				if(chunk != "") {
					tokens.push_back(process_chunk(was_ass, chunk, line));
				}	
			}else if(input[i] != '\t' && input[i] != '\n') {
				chunk = chunk + input[i];
			}else if(input[i] == '\n') {
				line++;
			}else{
				if(input[i] != '\t' && input[i] != ' ') {
					std::cout << "Invalid token: <" << input[i]<< "> on line: " << line << '\n';
					tokens.clear();
					return tokens;
				}	
			}
		}

		tokens.erase(std::remove_if(tokens.begin(), tokens.end(), [](const token& x) { return x.type == -1; }), tokens.end());

		return tokens;
	}
}
