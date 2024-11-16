#ifndef FLOAT_HPP
#define FLOAT_HPP
#include "Token.hpp"
#include <string>
class Decimal: public Token{
    public:
        float value;
        Decimal(float v): Token(Tag::DECIMAL), value{v} {}
        std::string to_string(){return std::to_string(value);}
};
#endif