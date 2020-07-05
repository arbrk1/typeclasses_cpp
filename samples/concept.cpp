#include <iostream>
#include "../tc_concept.hpp"

template<class T>
struct Foo {};

template<class T> requires Instance<Foo<T>>
void foo(T t) {
    std::cout << "I am Foo!" << std::endl;
}

template<class T>
void foo(T t) {
    std::cout << "I am not Foo!" << std::endl;
}

template<> TC_INSTANCE(Foo<int>, {});


int main() {
    int x = 0;
    double y = 0;

    foo(x);
    foo(y);
}
