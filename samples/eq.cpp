#include <iostream>
#include <utility>
#include <assert.h>
#include "../tc.hpp"

template<class T>
struct Eq {
    static bool equal(T const & a, T const & b) = delete;
};

template<> 
TC_INSTANCE(Eq<int>, {
    static bool equal(int const & a, int const & b) {
        return a == b;
    }
});
    
template<class A, class B> 
TC_INSTANCE(TC(Eq<std::pair<A, B>>), {
    // constrained instantiation
    TC_IMPL(Eq<A>) EA; 
    TC_IMPL(Eq<B>) EB;

    static bool equal(std::pair<A,B> const & pa, std::pair<A,B> const & pb) {
        return
            EA::equal(pa.first, pb.first) and
            EB::equal(pa.second, pb.second);
    }
});

int main() {
    TC_IMPL(Eq<std::pair<int, int>>) EII;
    TC_IMPL(Eq<std::pair<double, int>>) EDI;
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
    assert(tc_impl_t<Eq<decltype(px)>>::equal(px,py));
}
