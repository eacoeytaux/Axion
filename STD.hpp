#ifndef STD_hpp
#define STD_hpp

#include <iostream>
#include <cstdarg>
#include <cmath>
#include <memory>
#include <chrono>
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

use_std( cout );
use_std( endl );

use_std( exception );
use_std( runtime_error );

use_std( function );

use_std( string );

use_std( chrono::system_clock );

// -- math std --

use_std_dec( abs );
use_std_dec( round );
use_std_dec( floor );
use_std_dec( ceil );

use_std_dec( sqrt );
use_std_dec( log );

use_std_dec( sin );
use_std_dec( cos );
use_std_dec( tan );

use_std_dec( asin );
use_std_dec( acos );
use_std_dec( atan );
use_std_dec2( atan2 );

// -- constants --

const dec ZERO = 0.0;
const dec HALF = 0.5;
const dec ONE = 1.0;
const dec TWO = 2.0;
const dec THREE = 3.0;
const dec FOUR = 4.0;
const dec FIVE = 5.0;
const dec SIX = 6.0;
const dec SEVEN = 7.0;
const dec EIGTH = 8.0;
const dec NINE = 9.0;
const dec TEN = 10.0;

const dec TAU = 6.283185307179586476925;
const dec PI = 3.141592653589793236926;
const dec RIGHT_ANGLE = 1.570796326794896618463;
const dec RIGHT_ANGLE_0 = RIGHT_ANGLE * ZERO;
const dec RIGHT_ANGLE_1 = RIGHT_ANGLE * ONE;
const dec RIGHT_ANGLE_2 = RIGHT_ANGLE * TWO;
const dec RIGHT_ANGLE_3 = RIGHT_ANGLE * THREE;

const dec INFINITY_POSITIVE = INFINITY;
const dec INFINITY_NEGATIVE = -INFINITY;

} // namespace axn

#endif /* STD_hpp */
