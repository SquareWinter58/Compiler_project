#ifndef FLOAT_HPP
#define FLOAT_HPP
#include "Token.hpp"
class Decimal: public Token{
    public:
    float value;
    Decimal(float v): Token(Tag::DECIMAL), value{v} {}
};
#endif