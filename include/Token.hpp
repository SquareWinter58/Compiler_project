#ifndef TOKEN_HPP
#define TOKEN_HPP
#include "Tag_enum.hpp"
class Token{
    public:
        int tag;
        Token(int t):tag{t}{}
        virtual ~Token(){}
};
#endif