#include <iostream>
#include "../tc.hpp"

// an example of constrained instantiation

template<class> struct Foo;

template<class T> struct Good{};

template<class T> 
TC_INSTANCE(Good<Foo<T>>, {
    TC_REQUIRE(Good<T>);  // constraint on an instance definition
    
    // TC_IMPL(Good<T>) GoodT;
    // A possible alternative: TC_IMPL(Good<T>) GoodT;
    //
    // But TC_REQUIRE is better than TC_IMPL because it doesn't need 
    // arbitrary identifiers like GoodT.
});

template<> TC_INSTANCE(Good<int>, {});

int main() {
    // constraints on a function definition
    
    TC_REQUIRE(Good<Foo<int>>); // satisfied
    TC_REQUIRE(Good<Foo<Foo<int>>>); // satisfied
    // TC_REQUIRE(Good<Foo<double>>); // won't compile

    TC_IMPL(Good<Foo<double>>) ThisCompiles;
    // TC_IMPL can't be used to put a constraint on a function definition
    // because of lazy template instantiation (ThisCompiles gets instantiated 
    // only if you use some of its methods but there are none to use...)
    
    // ThisCompiles NOT; // but an explicit variable can be used with TC_IMPL
    // ThisCompiles(); // the form which doesn't use a gratuitous identifier
}
