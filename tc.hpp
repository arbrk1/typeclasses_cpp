// --------------------------------- //
//    Static typeclasses for C++     //
// --------------------------------- //
// copyright: arbrk1@gmail.com, 2018 //
// license: MIT                      //
// ================================= //

#ifndef _TC_HPP_
#define _TC_HPP_

// Minumum requirements for the macros below: C++98 with variadic macros

template<class T> struct _tc_impl_;

#define TC_INSTANCE(tc, body...) \
    struct _tc_impl_< tc > { typedef struct: tc body type; };

#define TC_IMPL(tc...) typedef typename _tc_impl_< tc >::type

// wrap the first argument to the TC_INSTANCE macro if it contains commas
#define TC(x...) x
// a somewhat uglier alternative is to #define COMMA ,


// Usage example:
// 
// To define a typeclass:
//
// template<class T>
// struct MyClass {
//     some static methods
//
//     static void foo() { default implementation }
// OR
//     static void foo() = delete;  // no default implementation
// };
//
// 
// To define an instance of a typeclass:
//
// template<args>
// TC_INSTANCE(TC(MyClass< MyType<args> >), {
//     static void foo() { instance implementation }
// })
//
//
// To use an instance of a typeclass:
// 
// TC_IMPL(MyClass< MyType<args> >) some_name;
// some_name::foo();
//

#if __cplusplus >= 201103L

// an alternative for the TC_IMPL macro
template<class T> using tc_impl_t = typename _tc_impl_<T>::type;

// a mechanism for putting constraints on instantiations or implementations
template<class T> struct _tc_dummy_ { static bool const value = true; T t; };
#define TC_REQUIRE(tc...) \
    static_assert( _tc_dummy_<tc_impl_t<tc>>::value, "unreachable" );

#endif // c++11


#endif // _TC_HPP_
