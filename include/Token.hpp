#ifndef TOKEN_HPP
#define TOKEN_HPP
#include "Tag_enum.hpp"
#include <cstddef>
#include <functional>
#include <string>
class Token{
    public:
        Tag tag;
        std::string tag_lexeme;
        // this serves to map the tag to the operator like >=, *, > , < etc..
        Token(Tag t):tag{t}{tag_lexeme = tag_to_symbol.find(tag)->second;}
        // and is used later to emit three adress code.
        Token(){}
        std::string to_string(){return tag_lexeme;}
        virtual ~Token(){}
};

class token_hash{
    std::size_t operator()(const Token& tok) const{
        return std::hash<int>()(static_cast<int>(tok.tag));
    }
};

#endif