#ifndef STD_hpp
#define STD_hpp

#include <iostream>
#include <fstream>
#include <cstdarg>
#include <cstdio>
#include <cmath>
#include <memory>
#include <chrono>
#include <functional>
#include <algorithm>
#include <random>
#include <thread>
#include <atomic>
#include <mutex>

#include <vector>
#include <array>
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

typedef unsigned int uint;
typedef unsigned long long uint64;

typedef double dec;

typedef dec ticks;

typedef char * chars;
typedef void * void_ptr;

typedef FILE file;

use_std( hash );

use_std( exception );
use_std( runtime_error );

use_std( function );
use_std( shuffle );

use_std( default_random_engine );
use_std( chrono::system_clock );

use_std( istream );
use_std( ostream );
use_std( iostream );
use_std( ifstream );
use_std( ofstream );
use_std( getline );

use_std( cout );
use_std( endl );

// -- threads -- //

use_std( thread );
use_std( move );
use_std( atomic );
use_std( mutex );
use_std( lock_guard );

// -- containers -- //

use_std( initializer_list );

// -- strings -- //

use_std( string );

use_std_to_string( int );
use_std_to_string( uint );
use_std_to_string( uint64 );
use_std_to_string( dec );

// -- math std -- //

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
