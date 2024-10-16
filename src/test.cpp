#include <iostream>

namespace my_enum {
    enum tokens_class{
        TEST = 256, IF, ELSE, WHILE, FOR, WHY
    };
}

int main(int argc, char* argv[]){
    using namespace std;
    
    cout<< "I wonder if this will work: " << my_enum::tokens_class::WHILE << '\n';
}