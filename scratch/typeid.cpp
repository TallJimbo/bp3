#include <typeinfo>
#include <iostream>

int main() {
    std::cout << "int: '" << typeid(int).name() << "'\n";
    std::cout << "int const: '" << typeid(int).name() << "'\n";
    std::cout << "int *: '" << typeid(int*).name() << "'\n";
    std::cout << "int * const: '" << typeid(int*).name() << "'\n";
    std::cout << "int const *: '" << typeid(int const *).name() << "'\n";
    std::cout << "int const * const: '" << typeid(int const * const).name() << "'\n";
    std::cout << "int &: '" << typeid(int&).name() << "'\n";
    std::cout << "int const &: '" << typeid(int const &).name() << "'\n";
    std::cout << "int &&: '" << typeid(int&&).name() << "'\n";
    std::cout << "int const &&: '" << typeid(int const &&).name() << "'\n";
}
