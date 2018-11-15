#include <iostream>
#include "../tc_alt.hpp"

// This file illustrates one of the most useful typeclass features: 
// support of default methods.
//
// In contrast with a C-with-classes style of abstract-class interfaces
// our typeclass implementation implicitly uses a sort of CRTP so 
// the methods are resolved statically with no overhead of virtual calls.

TC_DEF(Foo, class T, {
    static int foo(int x) {
        return Foo<T>::bar(x) + 1;
    }

    static int bar(int x) {
        return Foo<T>::foo(x) - 1;
    }
});

struct Bar; struct Baz; struct Quuz;

template<>
TC_INSTANCE(Foo, Bar, {
    static int foo(int x) {
        return x;
    }
})

template<>
TC_INSTANCE(Foo, Baz, {
    static int bar(int x) {
        return x-1;
    }
})


// for optimization purposes one may define something larger than 
// a minimal set of required methods
template<>
TC_INSTANCE(Foo, Quuz, {
    static int foo(int x) {
        return x;
    }

    static int bar(int x) {
        return x-1;
    }
})


int main() {
    std::cout << Foo<Bar>::foo(1) << " "
              << Foo<Bar>::bar(1) << std::endl;
    
    std::cout << Foo<Baz>::foo(1) << " "
              << Foo<Baz>::bar(1) << std::endl;
    
    std::cout << Foo<Quuz>::foo(1) << " "
              << Foo<Quuz>::bar(1) << std::endl;
}
