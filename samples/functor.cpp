#include <iostream>
#include <utility>
#include <vector>
#include "../tc.hpp"

// C++ has HKTs and C++11 facilitates their use with the ability to fix some
// template arguments:
template<class T> using Vec = std::vector<T>;

// T has the kind * -> *
template<template<class> class T>
struct Functor {
    template<class A, class F>
    static auto fmap(T<A> xs, F f) -> T<decltype(f(std::declval<A>()))> 
    = delete;
};


template<> 
TC_INSTANCE(Functor<Vec>, {    
    template<class A, class F>
    static auto fmap(Vec<A> xs, F f) -> Vec<decltype(f(std::declval<A>()))> {
        decltype(fmap(xs,f)) res;

        for (auto x : xs) {
            res.push_back(f(x));
        }
            
        return res;
    }
});


int main() {
    Vec<int> foo{1,2,3,4,5};

    TC_IMPL(Functor<Vec>) FV;

    for (auto x: FV::fmap(foo, [](int x) { return x*x; })) {
        std::cout << x << std::endl;
    }
}


