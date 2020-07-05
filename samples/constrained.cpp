#include <iostream>
#include "../tc.hpp"

// an example of constrained instantiation

template<class> struct Foo;

template<class T> struct Good{};

template<class T> 
TC_INSTANCE(Good<Foo<T>>, {
    TC_REQUIRE(Good<T>);  // constraint on an instance
    
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
    // TC_IMPL(Good<double>) WontCompileAlso;
    
    TC_IMPL(Good<Foo<double>>) ThisCompiles;
    // TC_IMPL typedef can't be reliably used to put a constraint 
    // on a function definition because the type 
    //
    //     _tc_impl_<Good<Foo<T>>>::type 
    //
    // is declared for any T.
    //
    // But we can use the fact that the definition of this type 
    // is erroneous for some values of T:

    // ThisCompiles NOT; // an explicit variable can be used with TC_IMPL
    // ThisCompiles(); // or the form which doesn't use a gratuitous identifier
}
