#include <cassert>
#include <vector>
#include <iostream>

int main(){
    using std::vector;
    vector<int> integer_vector;
    integer_vector.push_back(42);
    auto b = integer_vector.begin();
    assert(b == integer_vector.end());
    std::cout << "If i am smart enough i should get to this line\n";
}