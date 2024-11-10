#include "Decimal.hpp"
#include "Lexer.hpp"
#include "Num.hpp"
#include "Tag_enum.hpp"
#include "Token.hpp"
#include "Word.hpp"
#include <cassert>
#include <vector>
#include <iostream>

template <typename T>
using shptr = std::shared_ptr<T>;
unsigned look;
Token current_token;


void match(Tag t){
    if (look == t)
        // move one the stream
        return;
}
template <typename T>
class Vector: public std::vector<T>{
    using type = std::vector<T>::iterator;
    private:
        type current = this->end();
        int flag{0};
    public:
        using std::vector<T>::vector;
        auto move(){
            if (this->size()){
                if (!flag){
                    current = this->begin();
                    flag = 1;
                }
                else {
                    if (current != this->end())
                        ++current;
                }
            }
            return current;
        }    
};

int main(int argc, char* argv[]){
    using std::cout;
    using std::vector;
    Lexer lexer{"/home/squarewinter/school/programming/third_year/Compilers/Compiler_project/src/file.sb"};
    //cout << "testing testing 123\n";
    vector<shptr<Token>> token_vector;
    // while(1){
    //     auto token = lexer.scan();
    //     if (token->tag != Tag::COMMENT && token->tag != Tag::MULTILINE_COMMENT)
    //         token_vector.push_back(token);
    //     if (token->tag == Tag::EOF_) break;
    // }
    // cout << "The length of the vectore is " << token_vector.size() << '\n';
    Vector<int> test_vec{1,2,3,4,5};
    auto elem = test_vec.move();
    cout << test_vec[1] << ' ' << *(++test_vec.begin()) << ' ' << *(++elem) << '\n';
    //elem = test_vec.move();
    //cout << "trying to move with functoin " << *elem << '\n';
    while (elem != test_vec.end()){
        cout << *elem << '\n';
        elem = test_vec.move();
    }
    cout << "This should return bogus memory adress " << *elem << '\n'; 
    elem = test_vec.move();
    cout << "This should work as well " << *elem << '\n';
    test_vec.push_back(42);
    elem = test_vec.move();

    cout << *elem << '\n';
    
    // if (elem == test_vec.end())
    //     cout << "Im very smart\n";
    // test_vec.push_back(5);

    // while (elem != test_vec.end()) {
    //     cout << *elem.base() << "\n";
    //     elem = test_vec.move();
    // }
    // for (int i{0}; i < 100; ++i)
    //     elem = test_vec.move();
    // if (elem == test_vec.end())
    //     cout << "this makes sense\n";
    // test_vec.push_back(42);
    // elem = test_vec.move();
    // if (elem != test_vec.end())
    //     cout << "this is working as expected " << *elem << '\n';

    // vector<int> integer_vector{1,2,3,4,5};
    // cout << integer_vector[0] << ' ' << *integer_vector.begin() << '\n'

}
