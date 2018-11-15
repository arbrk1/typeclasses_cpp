// An alternative to the tc.hpp
// arbrk1@gmail.com, 2018

#ifndef _TC_ALT_HPP_
#define _TC_ALT_HPP_


#define TC_DEF(name, params, body...) \
    template< params > struct name; \
    template< params > struct _tc_methods_##name body;

#define TC_INSTANCE(name, params, body...) \
    struct name< params > : _tc_methods_##name< params > body;

#define PARAMS(args...) args

#ifdef TC_COMPAT
// compatibility layer with the tc.hpp
#define TC_IMPL(tc...) typedef tc
template<class T> using tc_impl_t = T;
template<class T> struct _tc_dummy_ { static bool const value = true; T t; };
#define TC_REQUIRE(tc...) \
    static_assert( _tc_dummy_<tc_impl_t<tc>>::value, "unreachable" );
#endif // TC_COMPAT

#endif // _TC_ALT_HPP_
