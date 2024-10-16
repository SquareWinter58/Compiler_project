#include "Word.hpp"
#include <cstddef>
#include <functional>



// std::size_t Word_hash::operator()(const Word& word) const{
//     return std::hash<std::string>()(word.lexeme) ^ std::hash<int>()(word.tag);
// }

// bool Word_equal::operator()(const Word& lhs, const Word& rhs) const{
//     return lhs.lexeme == rhs.lexeme && lhs.tag == rhs.tag;
// }

// blunder not needed in the end.