#ifndef STRING_LITERAL_HPP
#define STRING_LITERAL_HPP
#include "Tag_enum.hpp"
#include "Token.hpp"
#include <string>
class String_literal: public Token{
    public:
        std::string value;
        String_literal(const std::string& v):Token(Tag::STRING_LITERAL), value{v}{}
};
#endif