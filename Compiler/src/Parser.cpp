#include "Parser.h"

namespace Parser {
	void parse(const std::vector<Lexer::token>& tokens, bool debug) {
		auto start = std::chrono::high_resolution_clock::now();

		for(int i = 0; i < tokens.size(); i++) {
			if(tokens[i].type == Lexer::tt_ocb || tokens[i].type == Lexer::tt_ccb)
				i++;
			int pattern_type = 0;
			for(auto& pattern : patterns) {
				if(tokens[i].type == pattern[0].key_matches[0]) {
					bool final_match = false;
					int offset = 1;
					int index = 1;
					while(true) {
						if(offset >= pattern.size())
							break;				
						final_match = std::find(pattern[offset].key_matches.begin(), pattern[offset].key_matches.end(), tokens[i + index].type) != pattern[offset].key_matches.end();;
						if(!final_match) {					
							if(pattern[offset].repeatable) {		
								if(offset < pattern.size()-1) {
									offset++;
									final_match = std::find(pattern[offset].key_matches.begin(), pattern[offset].key_matches.end(), tokens[i + index].type) != pattern[offset].key_matches.end();;
									if(!final_match) {
										break;
									}
								}else{
									final_match = true;
									break;
								}			
							}else{
								break;
							}
						}
						if(!pattern[offset].repeatable)
							offset++;
						index++;
					}

					if(final_match) {
						/*
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
						*/
						if(pattern_type != 5 && pattern_type != 6) { //Everything else
							i = i+index-1;
						}else{ //For if and while
							i = i+index-2;
						}
						break;
					}		
				}
				pattern_type++;
			}

			if(i % 100 == 0 && debug) {
				auto end = std::chrono::high_resolution_clock::now();
				std::cout << "\rParser: Processing tokens - [";
				float percent = (float)(i+1) / (float)tokens.size();
				int filled = ceil(percent * 20);
				int empty = 20 - filled;
				for(int j = 0; j < filled; j++)
					std::cout << '#';
				for(int j = 0; j < empty; j++)
					std::cout << '-';
				std::cout << "] - " << (percent*100) << "%, time left: " << ((((double)std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()/(double)(i+1)) * (tokens.size()-i-1))/1000) << "s.";
				for(int k = 0; k < 20; k++)
					std::cout  << ' ';
				std::cout << '\r';
			}	
		}

		if(debug) {
			std::cout << "\rParser: Processing tokens - [";
			for(int j = 0; j < 20; j++)
				std::cout << '#';
			std::cout << "] - " << 100 << "%, time left: 0s.";
			for(int k = 0; k < 20; k++)
				std::cout  << ' ';
			std::cout << '\n';
		}
		
	}
}