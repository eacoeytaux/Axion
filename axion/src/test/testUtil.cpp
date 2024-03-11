#include "tests.hpp"

TEST_CASE( "for_range tests", "prove that for_range works" ) {
    int count;

    count = 0;
    for_range( 32 ) count++;
    REQUIRE( count == 32 );

    count = 0;
    for_range( i, 5 ) count += i;
    REQUIRE( count == 10 );

    count = 0;
    for_range( x, 5 ) count += x;
    REQUIRE( count == 10 );
}

TEST_CASE( "float tests", "prove that float_eq,gt,ge,lt,le works" ) {
    SECTION( "float_eq" ) {
        REQUIRE( float_eq( 1.f, 1.f ) );
        REQUIRE( float_eq( 1.f, 0.999999999f ) );
        REQUIRE( !float_eq( 1.f, 0.999f ) );
    }

    SECTION( "float_gt" ) {
        REQUIRE( float_gt( 1.f, 0.f ) );
        REQUIRE( !float_gt( 1.f, 1.f ) );
        REQUIRE( !float_gt( 1.f, 0.999999999f ) );
        REQUIRE( float_gt( 1.f, 0.999f ) );
        REQUIRE( !float_gt( 1.f, 2.f ) );
    }

    SECTION( "float_ge" ) {
        REQUIRE( float_ge( 1.f, 0.f ) );
        REQUIRE( float_ge( 1.f, 1.f ) );
        REQUIRE( float_ge( 1.f, 0.999999999f ) );
        REQUIRE( float_ge( 1.f, 0.999f ) );
        REQUIRE( !float_ge( 1.f, 2.f ) );
    }

    SECTION( "float_lt" ) {
        REQUIRE( !float_lt( 1.f, 0.f ) );
        REQUIRE( !float_lt( 1.f, 1.f ) );
        REQUIRE( !float_lt( 1.f, 0.999999999f ) );
        REQUIRE( !float_lt( 1.f, 0.999f ) );
        REQUIRE( float_lt( 1.f, 2.f ) );
    }

    SECTION( "float_le" ) {
        REQUIRE( !float_le( 1.f, 0.f ) );
        REQUIRE( float_le( 1.f, 1.f ) );
        REQUIRE( float_le( 1.f, 0.999999999f ) );
        REQUIRE( !float_le( 1.f, 0.999f ) );
        REQUIRE( float_le( 1.f, 2.f ) );
    }
}

TEST_CASE( "random tests", "pseudo-prove that random works" ) {
    const int iterations = 1000;
    const int low = 0;
    const int high = 100;

    SECTION( "seed" ) {
        Random::seed( 0 );
        int a = Random::rng_int( low, high );
        int b = Random::rng_int( low, high );
        REQUIRE( a != b ); // should rarely be two rngs repeated
        Random::seed( 1 );
        int c = Random::rng_int( low, high );
        REQUIRE( a != c ); // first rng with different seeds should not match
        Random::seed( 0 );
        int d = Random::rng_int( low, high );
        REQUIRE( a == d ); // first rng with same seeds should match
        Random::seed( );
    }

    SECTION( "bool" ) {
        REQUIRE( Random::rng_bool( 1.f ) );
        REQUIRE( !Random::rng_bool( 0.f ) );
        REQUIRE( Random::rng_bool( 2.f ) );
        REQUIRE( Random::rng_bool( 1.5f ) );
        REQUIRE_THROWS( Random::rng_bool( -1.f ) );

        bool variation = false;
        bool has_false = false;
        bool has_true = false;
        int last_b = false;
        for_range( iterations ) {
            bool b = Random::rng_bool( );
            if( last_b )
                variation |= ( b != last_b );
            has_false |= !b;
            has_true |= b;
            last_b = b;
        }
        REQUIRE( variation );
        REQUIRE( has_false );
        REQUIRE( has_true );
    }

    SECTION( "int" ) {
        bool variation = false;
        int last_r = 0;
        for_range( iterations ) {
            REQUIRE( Random::rng_int( i, i ) == i );
            int r = Random::rng_int( low, high );
            if( last_r )
                variation |= ( r != last_r );
            REQUIRE( r >= low );
            REQUIRE( r <= high );
            last_r = r;
        }
        REQUIRE( variation );
    }

    SECTION( "float" ) {
        const float float_low = low - 0.25f;
        const float float_high = high + 0.25f;
        bool variation = false;
        bool nonint = false;
        float last_f = 0;
        for_range( iterations ) {
            REQUIRE( Random::rng_float( i, i ) == i );
            float f = Random::rng_float( float_low, float_high );
            if( last_f )
                variation |= ( f != last_f );
            nonint |= ( f != (int)f );
            REQUIRE( f >= float_low );
            REQUIRE( f <= float_high );
            last_f = f;
        }
        REQUIRE( variation );
        REQUIRE( nonint );
    }
}

TEST_CASE( "swap tests", "prove that swap works" ) {
    SECTION( "int" ) {
        int x = 5;
        int y = 3;
        swap_values( x, y );
        REQUIRE( x == 3 );
        REQUIRE( y == 5 );
    }

    SECTION( "float" ) {
        float x = 5;
        float y = 3;
        swap_values( x, y );
        REQUIRE( x == 3 );
        REQUIRE( y == 5 );
    }

    SECTION( "struct" ) {
        struct test {
            test( int x ) : mx( x ) { };
            int mx = 0;
        };
        test x = test( 5 );
        test y = test( 3 );
        swap_values( x, y );
        REQUIRE( x.mx == 3 );
        REQUIRE( y.mx == 5 );
    }
}
