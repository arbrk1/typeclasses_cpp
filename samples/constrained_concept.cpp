#include <iostream>
#include "../tc_concept.hpp"

// an example of constrained instantiation

template<class> struct Foo;

template<class T> struct Good{};

template<class T> requires Instance<Good<T>> // constraint on an instance
TC_INSTANCE(Good<Foo<T>>, {});

template<> TC_INSTANCE(Good<int>, {});

int main() {
    // constraints on a function definition
    
    TC_REQUIRE(Good<Foo<int>>); // satisfied
    TC_REQUIRE(Good<Foo<Foo<int>>>); // satisfied
    // TC_REQUIRE(Good<Foo<double>>); // won't compile
    // TC_IMPL(Good<double>) WontCompileAlso;
    
    // TC_IMPL(Good<Foo<double>>) Now_DOES_NOT_compile;
}
