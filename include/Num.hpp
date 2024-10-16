#ifndef NUMBER_HPP
#define NUMBER_HPP
#include "Tag_enum.hpp"
#include "Token.hpp"
class Number: public Token{
    public:
        int value;
        Number(int v):Token(Tag::NUMBER), value{v} {}
};
#endif