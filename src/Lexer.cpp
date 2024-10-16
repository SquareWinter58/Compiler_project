
#include "Lexer.hpp"
#include "Decimal.hpp"
#include "Num.hpp"
#include "String_literal.hpp"
#include "Tag_enum.hpp"
#include "Token.hpp"
#include "Word.hpp"
#include <cctype>
#include <memory>
#include <ostream>
#include <sstream>
#include <stdexcept>
#include <string>

void Lexer::reserve(Word w){words.insert({w.lexeme, std::make_shared<Word>(w)});}//{words.insert({w.lexeme, w});}

Lexer::Lexer(std::string path): file_path{path}, eof_token{std::make_shared<Token>(Tag::EOF_)}, comment{std::make_shared<Token>(Tag::COMMENT)}, multi_comment{std::make_shared<Token>(Tag::MULTILINE_COMMENT)}{
    using namespace std;
    reserve({Tag::TRUE, "true"s});
    reserve({Tag::FALSE, "false"s});
    reserve({Tag::WHILE, "while"});
    reserve({Tag::FOR, "for"});
    reserve({Tag::IF, "if"});
    reserve({Tag::ELSE, "else"});
    reserve({Tag::RETURN, "return"});
    file.open(file_path);
    if (!file.is_open()){throw std::runtime_error{"Filed to open file"};}
    peek = '\0';
}

void Lexer::get_next_char(){
    file.get(peek);
}
bool Lexer::get_next_char(char c){
    file.get(peek);
    if (peek == c) return true;
    peek = ' ';
    return false;
}

std::shared_ptr<Token> Lexer::scan(){
    using std::shared_ptr;
    using shptr = std::shared_ptr<Token>;
    using std::string;

    // make peak take in one character from the file
    for(;; get_next_char()){
        if (file.eof()) return eof_token;
        else if (peek == '\0') continue;
        if (peek == ' ' || peek == '\t') continue;
        else if (peek == '\n') ++line;
        else break;
    }
    
    if (std::isalpha(peek)){
        // for now this is used to check alphabetic characters, 
        // it can be used to check alphabetic characters based on locale, but this implies the use of w_char_t which is discouraged.
        // Utf-8 has some unique characteristics that can help determine if characters are a part of BMP, by looking at certain bits
        // refer to chapters 2 and 3 of the following document once you get a chance
        // https://www.rfc-editor.org/rfc/rfc3629
        string lexeme{};
        while (std::isalnum(peek)) {
            lexeme += peek;
            get_next_char();
        }
        auto it = words.find(lexeme);
        if (it != words.end()){
            return it->second;
        }
        // cannot use shrptr here because it's type is already set to Token.
        auto w = std::shared_ptr<Word>{new Word{Tag::IDENTIFIER, lexeme}};
        words.insert({lexeme, w});
        return w;
    }
    else if (std::isdigit(peek)){
        int val{};
        while (std::isdigit(peek)){
            val = val*10 + (peek-'0');
            get_next_char();
        }
        if (peek == '.'){
            float float_value{};
            int devisor{10};
            get_next_char();
            if (!std::isdigit(peek)){
                std::ostringstream error;
                error << "Error on line: " << line << "float number must contain at least one digit after '.'\n";
            }
            // since the current character stored in peek is '.', this advences the peek by one.
            while (std::isdigit(peek)) {
                float_value += ((float)(peek-'0'))/devisor;
                devisor *= 10;
                get_next_char();
            }
            return shptr{new Decimal{val+float_value}};
        }
        else return shptr{new Number{val}};
    }
    else if (peek == '\'' || peek == '\"') {
        const char symbol{peek};
        string literal{""};
        while (!get_next_char(symbol)) {
            if (file.eof()){
                std::ostringstream error;
                error << "The string literal on line " << line << " Was not closed before EOF was reached\n";
                throw std::runtime_error{error.str()};
            }
            literal += peek;
        }
        get_next_char();
        return shptr{new String_literal{literal}};
    }
    switch (peek) {
        case '=':
            if (get_next_char('=')) {get_next_char(); return shptr{ new Token{Tag::EQUAL}};} else {get_next_char(); return shptr{ new Token{Tag::ASSIGN}};}
        case '>':
            if (get_next_char('=')) {get_next_char(); return shptr{new Token{Tag::MORE_THAN_EQUAL}};} else {get_next_char(); return shptr{new Token{Tag::MORE_THAN}};}
        case '<':
            if (get_next_char('=')) {get_next_char(); return shptr{new Token{Tag::LESS_THEN_EQUAL}};} else {get_next_char(); return shptr{new Token{Tag::LESS_THAN}};}
        case '!':
            if (get_next_char('=')) {get_next_char(); return shptr{new Token{Tag::NOT_EQUAL}};} else {get_next_char(); return shptr{new Token{Tag::NOT}};}
        
        case '/':
            {   // scoped because of tidineds.
                get_next_char();
                // the same method as before won't work, since the get_next_char() changes the state of peek, so the else if will
                // "ireperably" move the character forward, meaning the if and else if wont be testing the same character
                // Sidenote, this is thankfully not something i figured out in debuging
                if (peek == '/') {while (!get_next_char('\n')); return comment;}
                else if (peek == '*'){
                    char prev = '*';
                    while (1){
                        if (file.eof()){
                            throw std::runtime_error{"Ran out of characters before the multiline comment ended\n"};
                        }
                        get_next_char();
                                                        // this primes peek for the next function call
                        if (prev == '*' && peek == '/') { get_next_char(); return multi_comment;}
                        prev = peek;
                    }
                }
                else {get_next_char(); return shptr{new Token{Tag::DIV}};}
            }
        case '&':
        case '|':
           // scoped because of the variable declaration
            { 
                char prev = peek;
                get_next_char();
                if (prev == peek){
                    if (peek == '&') {get_next_char(); return shptr{new Token{Tag::AND}};} else {get_next_char(); return shptr{new Token{Tag::OR}};}
                }
                else{
                    std::ostringstream error;
                    error << "Line: " << line << ": Language does not support bitwise operations\n";
                    throw std::runtime_error{error.str()};
                }
            }
        case '(': 
            {get_next_char(); return shptr{new Token{Tag::PAREN_OPEN}};}
        case ')':
            {get_next_char(); return shptr{new Token{Tag::PAREN_CLOSED}};}
        case '{':
            {get_next_char(); return shptr{new Token{Tag::BRACKET_OPEN}};}
        case '}':
            {get_next_char(); return shptr{new Token{Tag::BRACKET_CLOSED}};}
        case '[':
            {get_next_char(); return shptr{new Token{Tag::SQUARE_BRACKET_OPEN}};}
        case ']':
            {get_next_char(); return shptr{new Token{Tag::SQUARE_BRACKET_CLOSED}};}
        case '.':
            {get_next_char(); return shptr{new Token{Tag::DOT}};}
        case '+':
            {get_next_char(); return shptr{new Token{Tag::ADD}};}
        case '-':
            {get_next_char(); return shptr{new Token{Tag::SUB}};}
        case '*':
            {get_next_char(); return shptr{new Token{Tag::MULT}};}
    }
    throw std::runtime_error{"Illegal character/token encountered\n"};
}