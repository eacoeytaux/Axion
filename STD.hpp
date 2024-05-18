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

typedef double dec;
typedef unsigned int uint;

#define use_std( s ) using std::s;

#define use_std_dec( s )  \
    template <typename T> \
    T s( const T & t ) { return std::s( (dec)t ); }
#define use_std_dec2( s ) \
    template <typename T> \
    T s( const T & t1, const T & t2 ) { return std::s( (dec)t1, (dec)t2 ); }

namespace axn
{

use_std( exception );
use_std( runtime_error );

use_std( function );

use_std( string );

use_std( cout );
use_std( endl );

// -- math std --

use_std_dec( abs );
use_std_dec( round );
use_std_dec( floor );
use_std_dec( ceil );

use_std_dec( sqrt );

use_std_dec( sin );
use_std_dec( cos );
use_std_dec( tan );

use_std_dec( asin );
use_std_dec( acos );
use_std_dec( atan );
use_std_dec2( atan2 );

// -- constants --

const dec SQRT_2 = sqrt( 2 );

const dec INFINITY_POSITIVE = INFINITY;
const dec INFINITY_NEGATIVE = -INFINITY;

} // namespace axn

#endif /* STD_hpp */
