#ifndef WORD_HPP
#define WORD_HPP
    #include "Tag_enum.hpp"
    #include "Token.hpp"
    #include <string>

    class Word: public Token{
        public:
            std::string lexeme;
            Word(Tag t, std::string lex): Token{t}, lexeme{lex}{}
            std::string to_string(){return lexeme;}
            
    };
#endif