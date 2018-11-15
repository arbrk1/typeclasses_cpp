#include <iostream>
#include <utility>
#include <assert.h>
#include "../tc_alt.hpp"

TC_DEF(Eq, class T, {
    static bool equal(T const & a, T const & b) = delete;
});

template<> 
TC_INSTANCE(Eq, int, {
    static bool equal(int const & a, int const & b) {
        return a == b;
    }
});
    
template<class A, class B> 
TC_INSTANCE(Eq, PARAMS(std::pair<A, B>), {
    // DIFFERENCE
    // constrained instantiation (note the absence of typedefs!)
    Eq<A> EA; 
    Eq<B> EB;

    static bool equal(std::pair<A,B> const & pa, std::pair<A,B> const & pb) {
        return
            Eq<A>::equal(pa.first, pb.first) and
            Eq<B>::equal(pa.second, pb.second);
    }
});

int main() {
    typedef Eq<std::pair<int, int>>  EII;
    typedef Eq<std::pair<double, int>> EDI;
    // We don't have an Eq<pair<double,int>> implementation
    // but the code _does_ compile if we do not _use_ this implementation.
    //
    // To put a typeclass constraint on a function definition use
    // the TC_REQUIRE macro (cf. constrained.cpp).

    using PII = std::pair<int,int>;
    using P_II_II = std::pair<PII,PII>;

    std::pair<int,int> pa{1,2}, pb{1,2};
    std::pair<int,double> pc{1,2}, pd{1,2};
    P_II_II px{{1,2},{3,4}}, py{{1,2},{3,4}};

    assert(EII::equal(pa,pb));

    // assert(EDI::equal(pc,pd)); // won't compile

    // c++11-style usage
    assert(Eq<decltype(px)>::equal(px,py));
}
