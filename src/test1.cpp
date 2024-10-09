#include <iostream>
#include "test1.hpp"

MyClass::MyClass() : myVariable(0){
  std::cout << "Hello Class!";
}

void MyClass::myFunction(){
  std::cout << "Hello Function!";
}


int main (int argc, char *argv[]) {
  std::cout << "Hello test!";
  MyClass obj;
  obj.myFunction();
  return 0;
}
