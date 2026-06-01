#ifndef STD_hpp
#define STD_hpp

#include <iostream>
#include <fstream>
#include <cstdarg>
#include <cmath>
#include <memory>
#include <chrono>
#include <functional>
#include <algorithm>
#include <random>

#include <vector>
#include <stack>
#include <queue>
#include <list>
#include <set>
#include <map>
#include <unordered_set>
#include <unordered_map>

#include "axn/system/OS.hpp"
#include "axn/system/Defines.hpp"

namespace axn
{

typedef void * void_ptr;
typedef char * chars;
typedef double dec;
typedef unsigned int uint;

use_std( hash );

use_std( cout );
use_std( endl );

use_std( istream );
use_std( ostream );
use_std( iostream );
use_std( ifstream );
use_std( ofstream );

use_std( string );

use_std( exception );
use_std( runtime_error );

use_std( array );
use_std( function );
use_std( shuffle );

use_std( default_random_engine );
use_std( chrono::system_clock );

// -- math std --

use_std_func( abs );
use_std_func( round );
use_std_func( floor );
use_std_func( ceil );

use_std_func( log );

use_std_func( sqrt );
use_std_func2( pow );

use_std_func( sin );
use_std_func( cos );
use_std_func( tan );

use_std_func( asin );
use_std_func( acos );
use_std_func( atan );
use_std_func2( atan2 );

} // namespace axn

#endif /* STD_hpp */
