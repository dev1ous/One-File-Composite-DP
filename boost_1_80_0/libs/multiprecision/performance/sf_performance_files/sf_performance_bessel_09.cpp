///////////////////////////////////////////////////////////////
//  Copyright 2011 John Maddock. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt

#include "../sf_performance.hpp"

void bessel_tests_09()
{
#ifdef TEST_MPFR_CLASS
   time_proc("Bessel Functions (50 digit precision)", "mpfr_class", test_bessel<mpfr_class>, mpfr_buildopt_tls_p() ? 3 : 1);
#endif
}
