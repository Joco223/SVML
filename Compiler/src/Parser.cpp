#include "Parser.h"

namespace Parser {
	void parse(std::vector<Lexer::token>& tokens) {
		std::vector<int> tokens_to_ignore;
		for(int i = 0; i < tokens.size(); i++) {
			if(std::find(tokens_to_ignore.begin(), tokens_to_ignore.end(), i) == tokens_to_ignore.end()) {
				int pattern_type = 0;
				for(auto& j : patterns) {
					std::vector<bool> truth_table;
					int offset = 0;
					int index = 0;
					while(true) {
						if(offset >= j.size()) {
							break;
						}				
						bool matches = false;
						for(int k = 0; k < j[offset].key_matches.size(); k++) {
							if(tokens[i + index].type == j[offset].key_matches[k]) {
								matches = true;
								break;
							}
						}
						truth_table.push_back(matches);
						if(!matches) {
							if(j[offset].repeatable) {
								offset++;
								bool matches_f = false;
								for(int k = 0; k < j[offset].key_matches.size(); k++) {
									if(tokens[i + index].type == j[offset].key_matches[k]) {
										matches_f = true;
										break;
									}
								}
								truth_table[truth_table.size()-1] = true;
								truth_table.push_back(matches_f);
								if(!matches_f) {
									break;
								}
							}else{
								break;
							}
						}else{
							
						}
						if(!j[offset].repeatable)
							offset++;
						index++;
					}

					if(std::find(truth_table.begin(), truth_table.end(), false) == truth_table.end()) {
						if(pattern_type != 5 && pattern_type != 6) {
							for(int j = i; j < i+index-1; j++)
								tokens_to_ignore.push_back(j);
						}else{
							for(int j = i; j < i+index-2; j++)
								tokens_to_ignore.push_back(j);
						}
						if(pattern_type == 0)
							std::cout << "Uninitialized variable definition found on index: " << i << " and ending on index: " << (i+index-1) << '\n';
						if(pattern_type == 1)
							std::cout << "Initialized variable definition found on index: " << i << " and ending on index: " << (i+index-1) << '\n';
						if(pattern_type == 2)
							std::cout << "Function definition found on index: " << i << " and ending on index: " << (i+index-1) << '\n';
						if(pattern_type == 3)
							std::cout << "Variable change found on index: " << i << " and ending on index: " << (i+index-1) << '\n';
						if(pattern_type == 4)
							std::cout << "Function call found on index: " << i << " and ending on index: " << (i+index-1) << '\n';
						if(pattern_type == 5)
							std::cout << "If statement found on index: " << i << " and ending on index: " << (i+index-2) << '\n';
						if(pattern_type == 6)
							std::cout << "While statement found on index: " << i << " and ending on index: " << (i+index-2) << '\n';
						if(pattern_type == 7)
							std::cout << "Return statement found on index: " << i << " and ending on index: " << (i+index-1) << '\n';
					}
					pattern_type++;
				}
			}
		}
	}
}