#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <type_traits>
#include "../tc.hpp"

// This example shows how to select overload on the basis
// of whether some type belongs to some typeclass or not.
//
// This is a conceptless analogue of show_concept.cpp, using explicit SFINAE 
// and a lot of magic to make it work as intended...
//
// Changes are marked with ##NEW##.


template<class T>
struct Show {
    static std::string show(T const &x) = delete;
};

// Show int
template<>
TC_INSTANCE(Show<int>, {
    static void requirements () {} // ##NEW##

    static std::string show(int const & x) {
        return ("int"+std::to_string(x));
    }
});

struct Foo{};

// Show Foo
template<>
TC_INSTANCE(Show<Foo>, {
    static void requirements () {} // ##NEW##

    static std::string show(Foo const & x) {
        return "Foo";
    }
});



// Show a => Show [a]
template<class T>
TC_INSTANCE(Show<std::vector<T>>, {
    template<class P = _tc_impl_<Show<T>>, class Q = typename P::type>
    static void requirements () {} // ##NEW##

    static std::string show(std::vector<T> const & xs) {
        std::string res = "[";

        if (xs.size() > 0) {
            for (size_t i = 0; i < xs.size() - 1; i++) {
                res += tc_impl_t<Show<T>>::show(xs[i]);
                res += ",";
            }

            res += tc_impl_t<Show<T>>::show(xs[xs.size()-1]);
        }

        res += "]";

        return res;
    }
});


// print Show instances ##NEW## (now a function instead of an operator)
template<class T,
    class P = _tc_impl_<Show<T>>,
    class Q = typename P::type,
    class = decltype(Q::requirements())  // !!!!???!!!!
>
std::ostream & operator_out(std::ostream & os, T const & x) {
    std::operator<<(os, Q::show(x));
    return os;
}

// ##NEW##
template<class T, class... Dummy>  // less specific (a dirty hack...)
std::ostream & operator_out(std::ostream & os, T const & x, Dummy...) {
    std::operator<<(os, "<UNSHOWABLE>");
    return os;
}

// ##NEW##
template<class T> // another dirty hack (operator<< can't be defined with template varargs)
std::ostream & operator<<(std::ostream & os, T const &x) {
    return operator_out(os, x);
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
TC_INSTANCE(Show<DynShow>, {
    static void requirements () {} // ##NEW##

    static std::string show(DynShow const & x) {
        return x.show_me();
    }
});


template<class T>
struct DynShowWrapper: DynShow {
    T self;

    std::string show_me() const {
        return tc_impl_t<Show<T>>::show(self);
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
TC_INSTANCE(Show<std::unique_ptr<T>>, {
    static void requirements () {} // ##NEW##

    static std::string show(std::unique_ptr<T> const & ptr) {
        return tc_impl_t<Show<T>>::show(*ptr);
    }
});


int main() {
    std::cout << 123 << std::endl;   // old int overload (more specific than template overload)
    std::cout << 123. << std::endl;  // old double overload
    std::cout << Foo() << std::endl; // new and the only overload
    std::cout << std::vector<int>{1,2,3} << std::endl;  // new Show instance
    
    // Unshowable 
    std::cout << std::vector<double>{1,2,3} << std::endl;

    // And now enter the dynamic Show trait: 
    std::cout << to_show(3) << std::endl;

    // Now we demonstrate an analogue of rusty Vec<Box<dyn Show>>.
    std::vector<std::unique_ptr<DynShow>> some_showables;

    some_showables.push_back(to_show(1));
    some_showables.push_back(to_show(Foo()));
    some_showables.push_back(to_show(2));

    
    std::cout << some_showables << std::endl;
}

