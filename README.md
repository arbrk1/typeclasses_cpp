# Better typeclasses/traits for C++

This repository is an experiment with defining a rather complete 
[typeclass system](https://en.wikipedia.org/wiki/Type_class) inside 
the C++ language. 

## Introduction

Typical uses of typeclasses are following:

1. a solution to the "expression problem" (concerned with extending
   a datatype both with new operations and new variants without touching 
   existing code)
2. a rich interface (an interface with a small set of required methods and 
   a great supply of default methods defined in terms of the required ones 
   or in terms of each another)
3. a compiler-checked logical constraint on a type or a tuple of types
4. a dynamic-dispatch mechanism (e.g. existential types in Haskell or 
   dynamic traits in Rust)
5. function overloading (using type inference to determine a 
   corresponding typeclass instance)

Typeclasses in the mainstream languages are rare (despite their general 
usefulness): the only two languages with builtin support for typeclasses 
seem to be Haskell and Rust (and some aspects of typeclasses can be 
emulated in several other languages).

It appears that typeclasses 
(at least with respect to _all_ the usecases above) 
can be easily described by C++ templates. Futhermore a typeclass system
supporting all the points above
can be implemented using only Standard C++98. Our implementation consists 
of the [tc.hpp](./tc.hpp) header-only library and some usage examples in the 
[samples](./samples/) directory.

The full C++98 implementation is very short: it consists 
of a `struct` declaration and three one-line variadic macros. 

The C++11 implementation has in addition 
a single type alias (which can be used in place of one of the previously 
mentioned C++98 macros), a single short struct definition and a single 
one-line macro for forcing compile-time constraints.

## Existing implementations (of which existence I know)

This is a (very likely incomplete) list of C++ typeclass implementations 
which can be easily googled.

### "Naive" translation

E.g. <http://www.nauths.fr/en/2015/04/26/cpp-typeclasses.html> or 
<http://blog.shaynefletcher.org/2016/10/haskell-type-classes-in-ocaml-and-c.html>.

A typeclass is modelled by a simple templated `struct`:

``` c++
template<class T>
struct Monoid {
    static T empty();
    static T append(T, T);
};
```

A typeclass instance is defined by the template specialization:

``` c++
template<>
struct Monoid<int> {
    static int empty() { return 0; }
    static int append(int x, int y) { return x+y; }
};
```

The main drawback is that no part of the definition of 
`template<class T> struct Monoid` is present in any of 
its specializations. So all the code in that definition 
describes only how the types for which the typeclass 
is not implemented behave. 

The problem is that we have no _simple_  way of defining default method 
implementations or enforcing logical constraints like "all instances 
of the typeclass A must also be instances of the typeclass B".


Because the general typeclass template definition is concerned 
with the case of "type T is not an instance of the typeclass",
a much better version of the typeclass definition would be simply

``` c++
template<class T> struct Monoid;
```

This line of thought gets us to [the following](#comparison-with-the-naive-version).


### "Less naive" translation

A blogpost <https://functionalcpp.wordpress.com/2013/08/16/type-classes/> has 
a little more elaborate definition using an explicit static boolean 
to distinguish between the types which implement the typeclass and the 
types which do not.

The problem with the inability to define default method implementations 
still persists.


### C++ concepts

Sometimes it is said that C++ concepts have lots in common with 
typeclasses. Unfortunately it's only partially true: of the 
five usecases listed in [the introduction](#introduction) the C++ concepts 
provide only the third one: they group types into semantic categories 
so that a semantic category membership can be compile-time checked.

Some of the other typeclass features were present in a previous 
iteration of the concept system (so called "C++0x concepts") but 
currently we (do not yet) have only "Concepts Lite".



## This implementation

Our implementation solves the issue that the template specializations 
override the original definition by never specializing the definition 
of a typeclass. Instead all typeclass instances are stored as 
specializations of a separate type (this type is same for all 
instances of all typeclasses). This allows us to define the default 
methods of a typeclass with a CRTP-like pattern.



Below we describe the structure and the workings of the `tc.hpp` header.


### Defining typeclasses

To define a typeclass one uses a templated `struct` with some static 
methods:

``` c++
template<class T>
struct Foo {
    static void foo() = delete;
};
```

Default implementations (even mutually dependent) can be used:

``` c++
template<class T>
struct Foo {
    // default typeclass methods are a sort of CRTP

    static int foo(int x) {
        TC_IMPL(Foo<T>) FooT; // this syntax is described below
        return FooT::bar(x) + 1;
    }

    static int bar(int x) {
        TC_IMPL(Foo<T>) FooT; 
        return FooT::foo(x) - 1;
    }
};
```

For more info on default methods see [default.cpp](./samples/default.cpp).


### Extending typeclasses with types

All typeclass instances are tracked by a single type `_tc_impl_`. 
It's defined as

``` c++
template<class T> struct _tc_impl_;
```

Adding a type `Bar` to the typeclass `Foo` is as simple as

``` c++
template<>
struct _tc_impl_< Foo<Bar> > {
    typedef struct: Foo<Bar> { /* method implementations */ } type;
};
```

To facilitate such definitions a macro is introduced:

``` c++
#define TC_INSTANCE(tc, body...) \
    struct _tc_impl_< tc > { typedef struct: tc body type; };
```

If the `tc` parameter has commas (e.g. `Show< pair<int,int> >` or 
`Foo<Bar, Baz>`) a small helper variadic macro can be used to wrap 
the parameter:

``` c++
#define TC(x...) x
```

It should be noted that even in the case variadic macros are not supported 
a helpful set of macros can be defined:

``` c++
#define TC_INSTANCE_BEGIN(tc) \
    struct _tc_impl_< tc > { typedef struct: tc
#define TC_INSTANCE_END type; };
#define COMMA ,
```

And now instead of

``` c++
template<>
TC_INSTANCE(TC(Foo<Bar,Baz>), { 
    static void foo() { /* definition */ } 
})
```

we must write

``` c++
template<>
TC_INSTANCE_BEGIN(Foo<Bar COMMA Baz>) {
    static void foo() { /* definition */ } 
} TC_INSTANCE_END
```

### Using typeclasses

The `tc.hpp` provides two ways of calling methods of a typeclass. 

The first way is to use the `TC_IMPL` macro defined as

``` c++
#define TC_IMPL(tc...) typedef typename _tc_impl_< tc >::type
```

For example:

``` c++
TC_IMPL(Foo<Bar,Baz>) FBB;

FBB::foo();
```

The second (less verbose) way is to use a C++11-only templated 
type alias

``` c++
template<class T> using tc_impl_t = typename _tc_impl_<T>::type;
```

It is used as follows:

``` c++
tc_impl_t<Foo<Bar,Baz>>::foo();
```

It can be used in conjunction with `decltype` to get a sort of 
poor man's type inference.

Fortunately, typeclasses play rather well with SFINAE (see 
[show.cpp](./samples/show.cpp) where a templated overload of `operator<<` 
is used to infer a correct typeclass instance) so in some cases 
one doesn't need to specify any types at all.


### Constraining implementations

The mechanism of C++ template instantiation provides an automatic 
checking of typeclass instance "real" dependencies (required by 
typeclass method implementations). E.g. with the following definition

``` c++
template<class T>
TC_INSTANCE(Foo<Bar<T>>, {
    void foo() {
        tc_impl_t<Foo<T>>::foo();
    }
})
```

an attempt to use `tc_impl_t<Foo<Bar<Baz>>>::foo()` will fail 
if `Baz` doesn't implement the `Foo` typeclass.

But sometimes typeclasses are used as a sort of marker (e.g. Rust 
marker traits like `Clone`): a fact that some relation holds 
between some types. Frequently such typeclasses have no methods so 
the mechanism described above is useless.

We use a rather dumb solution: a `static_assert` checking if 
a static `type` field is present on the specific `_tc_impl_` 
specialization.

``` c++
template<class T> struct _tc_dummy_ { static bool const value = true; T t; };
#define TC_REQUIRE(tc...) \
    static_assert( _tc_dummy_<tc_impl_t<tc>>::value, "unreachable" );
```

It's very likely that a more beautiful solution exists.

Also note that such a constraint can be placed using C++98-only constructs:

``` c++
// a constrained typeclass instance
TC_INSTANCE(Foo<Bar<T>>, {
    TC_IMPL(Foo<T>) FooBar; 
    // instead of TC_REQUIRE(Foo<T>)

    /* some methods */
})

// a constrained function
void foo() {
    TC_IMPL(Foo<Baz>) FooBaz;
    FooBaz();
    // two lines instead of a single one: TC_REQUIRE(Foo<Baz>)

    /* some code */
}
```

For an example see the [constrained.cpp](./samples/constrained.cpp) file.


### Existential types (aka typeclass-based dynamic dispatch)

Existential types are a way of describing dynamic dispatch in terms of 
a typeclass system. Simply stated, if we have a (one-parametric, 
for the sake of simplicity) typeclass `C` then we have a 
type `(exists a. C a => a)` which is a supertype of any instance of 
the typeclass `C`. We'll use a shorter (Rust-like) notation `dyn C` below.

To describe such a type we must define two utility types. 
Suppose we have a typeclass

``` c++
template<class T> 
struct Foo {
    // we can dispatch only through a reference or a pointer
    static void foo(T const &x, int y) = delete;
};
```

Now we define an abstract type corresponding to an existential 
`dyn Foo`

``` c++
struct DynFoo {
    // now the first argument becomes "this"
    virtual void foo_self(int y) const = 0;
    virtual ~DynFoo() {}
};

template<>
TC_INSTANCE(Foo<DynFoo>, {
    static void foo(DynFoo const &x, int y) {
        x.foo_self(y);
    }
})
```
and a generic type to wrap objects of different types

``` c++
template<class T>
struct DynFooWrapper: DynFoo {
    T self;

    void foo_self(int y) const {
        tc_impl_t<Foo<T>>::foo(self, y);
    }

    DynFooWrapper(T x): self(x) {}
};
```

The final step is to transform `DynFooWrapper<T>` values into an 
instances of `DynFoo`. This can be done by the means of boxing:

``` c++
template<T>
std::unique_ptr<DynFoo> to_foo(T x) {
    return std::make_unique<DynFooWrapper<T>>(x);
}
```

Finally, it's convenient to define a `Foo` instance for boxed values:

``` c++
template<T>
TC_INSTANCE(Foo<std::unique_ptr<T>>, {
    static void foo(std::unique_ptr<T> const &x, int y) {
        tc_impl_t<Foo<T>>::foo(*x, y);
    }
})
```

This last step is the only step which must be explicitly done in Rust 
(and all these steps are done implicitly in Haskell). So consider 
this only as a proof-of-concept demonstration that it is possible 
(albeit a little cumbersome) to adapt C++ inheritance-based dispatching 
to a typeclass system.

An example of existential types can be seen in 
the [show.cpp](./samples/show.cpp) file.


## Comparison with the "naive" version

At the cost of slightly complicating the macros the "naive" version 
described [above](#naive-translation) could be endowed with the 
same features as the [tc.hpp](./tc.hpp) implementation.

See the [tc_alt.hpp](./tc_alt.hpp) header and the 
[alt_samples](./alt_samples/) directory. Significant differences in 
the usage are marked by a `// DIFFERENCE` comment.



