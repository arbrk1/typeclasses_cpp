#include <iostream>

#define TC_COMPAT
#include "../tc_alt.hpp"

// an example of constrained instantiation

template<class> struct Foo;

TC_DEF(Good, class T, {});

template<class T> 
TC_INSTANCE(Good, Foo<T>, {
    TC_REQUIRE(Good<T>);  // constraint on an instance definition
    
    // DIFFERENCE
    // A possible alternative: Good<T> GoodT;
    // note that in contrast to the tc.hpp 
    // one defines a variable instead of a type alias
});

template<> TC_INSTANCE(Good, int, {});

int main() {
    // constraints on a function definition
    
    TC_REQUIRE(Good<Foo<int>>); // satisfied
    TC_REQUIRE(Good<Foo<Foo<int>>>); // satisfied
    // TC_REQUIRE(Good<Foo<double>>); // won't compile

    typedef Good<Foo<double>> ThisCompiles;
    
    // ThisCompiles NOT; // an explicit variable can be used
    // ThisCompiles(); // or the form which doesn't use a gratuitous identifier
}
