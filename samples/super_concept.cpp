#include "../tc_concept.hpp"

// an example of constrained definition

template<class T> 
struct Foo {};

template<class T> requires Instance<Foo<T>>
struct Bar {};

struct Quuz {};

template<> TC_INSTANCE(Foo<Quuz>, {}); // ### TRY TO REMOVE ### 
template<> TC_INSTANCE(Bar<Quuz>, {});


int main() {
    TC_IMPL(Bar<Quuz>) BarQuuz; 
    // Does not compile if the Foo<Quuz> instance is removed. 
}


