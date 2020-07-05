#ifndef _TC_CONCEPT_HPP_
#define _TC_CONCEPT_HPP_

#include "tc.hpp"

template<class TC>
concept Instance = _tc_dummy_<tc_impl_t<TC>>::value;



#endif // _TC_CONCEPT_HPP_
