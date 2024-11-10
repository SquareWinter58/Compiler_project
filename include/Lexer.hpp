#ifndef LEXER_HPP
#define LEXER_HPP
    
#include <fstream>
#include <memory>
#include <string>
#include <unordered_map>
#include "Token.hpp"
#include "Word.hpp"
#include <filesystem>
class Lexer{
        private:
            char peek;
            std::unordered_map<std::string, std::shared_ptr<Word>> words;
            std::shared_ptr<Token> eof_token;
            std::shared_ptr<Token> comment;
            std::shared_ptr<Token> multi_comment;
            std::filesystem::path file_path;
            std::ifstream file;
            void reserve(Word w);
            void get_next_char();
            bool get_next_char(char c);
        public:
            int line = 1;
            int character = 0;
            Lexer(std::string path);
            std::shared_ptr<Token>scan();
    };

#endif

