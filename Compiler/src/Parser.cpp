#include "Parser.h"

namespace Parser {
	void parse(std::vector<Lexer::token>& tokens) {
		int removed_tokens = 0;
		int original_size = tokens.size();
		for(int i = 0; i < tokens.size(); i++) {
			int pattern_type = 0;
			for(auto& j : patterns) {
				std::vector<bool> truth_table;
				int offset = 0;
				int index = 0;
				while(true) {
					if(offset >= j.size())
						break;				
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
							if(offset >= j.size()) {
								truth_table[truth_table.size()-1] = true;
								break;
							}			
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
					/* 
					if(pattern_type == 0)
						std::cout << "Uninitialized variable definition found on index: " << (i+removed_tokens) << " and ending on index: " << ((i+removed_tokens)+index-1) << '\n';
					if(pattern_type == 1)
						std::cout << "Initialized variable definition found on index: " << (i+removed_tokens) << " and ending on index: " << ((i+removed_tokens)+index-1) << '\n';
					if(pattern_type == 2)
						std::cout << "Function definition found on index: " << (i+removed_tokens) << " and ending on index: " << ((i+removed_tokens)+index-1) << '\n';
					if(pattern_type == 3)
						std::cout << "Variable change found on index: " << (i+removed_tokens) << " and ending on index: " << ((i+removed_tokens)+index-1) << '\n';
					if(pattern_type == 4)
						std::cout << "Function call found on index: " << (i+removed_tokens) << " and ending on index: " << ((i+removed_tokens)+index-1) << '\n';
					if(pattern_type == 5)
						std::cout << "If statement found on index: " << (i+removed_tokens) << " and ending on index: " << ((i+removed_tokens)+index-2) << '\n';
					if(pattern_type == 6)
						std::cout << "While statement found on index: " << (i+removed_tokens) << " and ending on index: " << ((i+removed_tokens)+index-2) << '\n';
					if(pattern_type == 7)
						std::cout << "Return statement found on index: " << (i+removed_tokens) << " and ending on index: " << ((i+removed_tokens)+index-1) << '\n';
					*/
					if(pattern_type != 5 && pattern_type != 6) {
						tokens.erase(tokens.begin()+i, tokens.begin()+index+i-1);
						removed_tokens += index-1;
					}else{
						tokens.erase(tokens.begin()+i, tokens.begin()+index+i-2);
						removed_tokens += index-2;
					}
					break;
				}
				pattern_type++;
			}

			std::cout << "\rParser: Processing tokens - [";
			float percent = (float)(i+removed_tokens+1) / (float)original_size;
			int filled = ceil(percent * 20);
			int empty = 20 - filled;
			for(int j = 0; j < filled; j++)
				std::cout << '#';
			for(int j = 0; j < empty; j++)
				std::cout << '-';
			std::cout << "] - " << round(percent*100) << "%\r";
		}
	}
}