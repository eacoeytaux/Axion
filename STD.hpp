#ifndef STD_hpp
#define STD_hpp

#include "OS.hpp"

// #define _USE_MATH_DEFINES
#include <cmath>
#include <iostream>
#include <cstdarg>
#include <memory>
#include <functional>
#include <queue>
#include <stack>
#include <list>
#include <set>
#include <map>
#include <unordered_set>
#include <unordered_map>
#include <vector>

#define use_std( s ) using std::s;

#define use_std_double( s ) \
    template <typename T>   \
    T s( const T & t ) { return std::s( (double)t ); }
#define use_std_double2( s ) \
    template <typename T>    \
    T s( const T & t1, const T & t2 ) { return std::s( (double)t1, (double)t2 ); }

namespace axn
{

use_std( exception );
use_std( runtime_error );

use_std( function );

use_std( string );

use_std( cout );
use_std( endl );

// -- math std --

use_std_double( abs );
use_std_double( round );
use_std_double( floor );
use_std_double( ceil );

use_std_double( sqrt );

use_std_double( sin );
use_std_double( cos );
use_std_double( tan );

use_std_double( asin );
use_std_double( acos );
use_std_double( atan );
use_std_double2( atan2 );

// -- constants --

const double SQRT_2 = sqrt( 2 );

const double INFINITY_POSITIVE = INFINITY;
const double INFINITY_NEGATIVE = -INFINITY;

typedef unsigned int uint;

} // namespace axn

#endif /* STD_hpp */
