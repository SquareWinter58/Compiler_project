#ifndef SYMBOLS_HEADER
#define SYMBOLS_HEADER

#include "Tag_enum.hpp"
#include "Token.hpp"
#include "Word.hpp"
#include <optional>
#include <sstream>
#include <string>
#include <unordered_map>
#include <variant>

struct type_test{
    int width;

};


enum class types{
    // If you change this, be sure to change the type_to_value, numeric and max defined under.
    Int, Float, Char, Bool, NULLTYPE
};

const std::unordered_map<types, int> type_to_value{{types::Int,4}, {types::Float, 8}, {types::Char, 1}, {types::Bool, 1}};


// class Type_wrapper: public Word{
//     public:
//         types type;
//         int width;
//         Type_wrapper(std::string name, Tag tag, types tp, int w): Word{tag, name},type{tp},width{w}{}
//         static bool numeric(types type){
//             switch (type) {
//                 case types::Int:
//                 case types::Float:
//                 case types::Char:
//                     return true;
//                 default:
//                     return false;
//             }
//         }

//         static types max(types t1, types t2){
//             if (!numeric(t1) || !numeric(t2)) return types::NULLTYPE;
//             else if (t1 == types::Float || t2 == types::Float) return types::Float;
//             else if (t1 == types::Int || t2 == types::Int) return types::Int;
//             else return types::Char;
//         }

// };


// struct basic_types{
//   Type_wrapper Int{"int", Tag::INT, types::Int, 4};
//   Type_wrapper Float{"float", Tag::FLOAT, types::Float, 8};
//   Type_wrapper Char{"char", Tag::CHAR, types::Char, 1};
//   Type_wrapper Bool{"bool", Tag::CHAR, types::Char, 1};  
// };

class  Array;

using Type = std::variant<types, Array, int>;
using Op_type = std::optional<types>;
class Array{
    public:
            // Word, just holds the lexeme, the lexeme for array is [] 
        std::unique_ptr<Type> array_type;
        int array_length;
        int array_size; //in bytes
        Array(int len, types tp): array_type{tp}, array_length{len}, array_size{array_length*type_to_value.find(array_type)->second}{}
        // std::string to_string(){
        //     std::ostringstream os;
        //     os << "[" << array_length << "]" << array_type.to_string();
        //     return os.str();
        // }
};

std::string type_to_string(Type t){
    auto get_type = [](types t){
        switch (t) {
            case types::Int:
                return "int";
            case types::Float:
                return "float";
            case types::Char:
                return "char";
            case types::Bool:
                return "bool";
        }
    };
    if (std::holds_alternative<types>(t)){
        return get_type(std::get<types>(t));
    }
    else if (std::holds_alternative<Array>(t)){
        Array arr = std::get<Array>(t);
        return "[" + std::to_string(arr.array_length) + ']' + get_type(arr.array_type);
    }
}

bool numeric(types type){
    switch (type) {
        case types::Int:
        case types::Float:
        case types::Char:
            return true;
        default:
            return false;
    }
}
Op_type max(Type t1, Type t2){
    // return the empty optional, since an array is not a comprable type
    if (std::holds_alternative<Array>(t1) || std::holds_alternative<Array>(t2)) return {};
    // this takes care of tow incompatible types, basically if any one is bool
    else if (!numeric(std::get<types>(t1)) || !numeric(std::get<types>(t2))) return {};
    else{
        types type1 = std::get<types>(t1), type2 = std::get<types>(t2);
        if (type1 == types::Float || type2 == types::Float) return types::Float;
        if (type1 == types::Int || type2 == types::Int) return types::Int;
        return types::Char;
    }
}
#endif