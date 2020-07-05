#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include "../tc_alt.hpp"

TC_DEF(Show, class T, {
    static std::string show(T const &) = delete;
});

// Show int
template<>
TC_INSTANCE(Show, int, {
    static std::string show(int const & x) {
        return ("int"+std::to_string(x));
    }
});

struct Foo{};

// Show Foo
template<>
TC_INSTANCE(Show, Foo, {
    static std::string show(Foo const & x) {
        return "Foo";
    }
});


// Show a => Show [a]
template<class T>
TC_INSTANCE(Show, std::vector<T>, {
    static std::string show(std::vector<T> const & xs) {
        std::string res = "[";

        if (xs.size() > 0) {
            for (size_t i = 0; i < xs.size() - 1; i++) {
                res += Show<T>::show(xs[i]);
                res += ",";
            }

            res += Show<T>::show(xs[xs.size()-1]);
        }

        res += "]";

        return res;
    }
});


// print Show instances
template<class T>
std::ostream & operator<<(std::ostream & os, T const & x) {
    // we are using std::operator<< explicitly to avoid ambigous overload
    std::operator<<(os,Show<T>::show(x));
    return os;
}


// Emulation of rust-like dynamic traits.
// 
// Unfortunately, I do not know if an adequate analogue of
// full-blown haskell-like existential types is possible.

// If the typeclass being dynamized has more than one type parameter
// then the DynClass may be a template having one parameter less.
struct DynShow {
    virtual std::string show_me() const = 0;
    virtual ~DynShow() {}  // Do not forget this! We usually hide DynShow
                           // behind some smart pointer.
};

// We have to provide some Show instance to DynShow values.
template<>
TC_INSTANCE(Show, DynShow, {
    static std::string show(DynShow const & x) {
        return x.show_me();
    }
});


template<class T>
struct DynShowWrapper: DynShow {
    T self;

    std::string show_me() const {
        return Show<T>::show(self);
    }

    DynShowWrapper(T x): self(x) {}
};

// type-inferring constructor
template<class T>
std::unique_ptr<DynShow> to_show(T x) {
    return std::make_unique<DynShowWrapper<T>>(x);
}

// and an instance to support boxed Show values:
// in rust this would be: impl<T: Show+?Sized> Show for Box<T> 
template<class T>
TC_INSTANCE(Show, std::unique_ptr<T>, {
    static std::string show(std::unique_ptr<T> const & ptr) {
        return Show<T>::show(*ptr);
    }
});


int main() {
    std::cout << 123 << std::endl;   // old int overload (more specific)
    std::cout << 123. << std::endl;  // old double overload
    std::cout << Foo() << std::endl; // new and the only overload
    std::cout << std::vector<int>{1,2,3} << std::endl;  // new Show instance
    // std::cout << std::vector<double>{1,2,3} << std::endl;  // won't compile

    // And now enter the dynamic Show trait: 
    std::cout << to_show(3) << std::endl;

    // Now we demonstrate an analogue of rusty Vec<Box<dyn Show>>.
    std::vector<std::unique_ptr<DynShow>> some_showables;

    some_showables.push_back(to_show(1));
    some_showables.push_back(to_show(Foo()));
    some_showables.push_back(to_show(2));

    
    std::cout << some_showables << std::endl;
}

