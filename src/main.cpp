
#include "Decimal.hpp"
#include "Lexer.hpp"
#include "Num.hpp"
#include "Tag_enum.hpp"
#include "Token.hpp"
#include "Word.hpp"
#include <cstdlib>
#include <iostream>
#include <memory>
#include <vector>
int main(int argc, char* argv[]){
    using std::cout;
    std::vector<std::shared_ptr<Token>> token_vector;
    Lexer lexer{"/home/squarewinter/school/programming/third_year/Compilers/Compiler_project/src/file.sb"};
    while (1) {
        auto token = lexer.scan();
        token_vector.push_back(token);
        if (auto w = std::dynamic_pointer_cast<Word>(token)){
            cout << w->lexeme << '\n';
        }
        else if (auto n = std::dynamic_pointer_cast<Number>(token)) {
            cout << n->value << '\n';
        }
        else if (auto d = std::dynamic_pointer_cast<Decimal>(token)){
            cout << d->value << '\n';
        }
        else cout << token->tag << '\n';
        if (token->tag == Tag::EOF_) break;
    }
    // while (true) {
    //     //std::cout << "entered\n";
    //     auto token = lexer.scan();
    //     if (token->tag == Tag::EOF_) break;
    //     std::cout << token->tag << '\n';
    //     token_vector.push_back(token);
    //     // else if (token->tag == Tag::COMMENT || token->tag == Tag::MULTILINE_COMMENT){
    //     //     std::cout << "\n\nfound a comment cool\n\n";
    //     // }
    //     // else{
    //     //     if (auto w = std::dynamic_pointer_cast<Word>(token)){
    //     //         std::cout << w->lexeme << '\n';
    //     //     }
    //     //     else
    //     //         std::cout << token->tag << '\n';
    //     //     }
    // }
    // for (auto i: token_vector){
    //     if (auto w = std::dynamic_pointer_cast<Word>(i))
    //         std::cout << "This is mega cool, word lexeme: " << w->lexeme << '\n';
    //     if (auto てすと = std::dynamic_pointer_cast<Number>(i))
    //         std::cout << "これも格好いいだね、日本語を書きます" << てすと->value << '\n';
    // }
    
    return EXIT_SUCCESS;
}