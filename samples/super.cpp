#include "../tc.hpp"

// an example of constrained definition

template<class T> 
struct Foo {};

template<class T>
struct Bar { TC_REQUIRE(Foo<T>); };

struct Quuz {};

template<> TC_INSTANCE(Foo<Quuz>, {}); // ### TRY TO REMOVE ### 
template<> TC_INSTANCE(Bar<Quuz>, {});


int main() {
    TC_IMPL(Bar<Quuz>) BarQuuz; 
    // Does not compile if the Foo<Quuz> instance is removed. 
}


