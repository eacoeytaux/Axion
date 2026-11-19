#ifndef Tests_hpp
#define Tests_hpp

#include "axn/AXN.hpp"

#include "axn/tests/Catch.hpp"

#undef test
#define test( ... )               TEST_CASE( __VA_ARGS__ )

#undef section
#define section( ... )            SECTION( __VA_ARGS__ )

#undef check
#undef check_not
#define check( ... )              CHECK( __VA_ARGS__ )
#define check_not( ... )          CHECK_FALSE( __VA_ARGS__ )

#undef check_throws
#undef check_throws_not
#define check_throws( ... )       CHECK_THROWS( __VA_ARGS__ )
#define check_throws_not( ... )   CHECK_NOTHROW( __VA_ARGS__ )

#undef require
#undef require_not
#define require( ... )            REQUIRE( __VA_ARGS__ )
#define require_not( ... )        REQUIRE_FALSE( __VA_ARGS__ )

#undef require_throws
#undef require_throws_not
#define require_throws( ... )     REQUIRE_THROWS( __VA_ARGS__ )
#define require_throws_not( ... ) REQUIRE_NOTHROW( __VA_ARGS__ )

#endif /* Tests_hpp */
