#include "axn/tests/Tests.hpp"

#include "axn/system/Utility.hpp"

namespace axn
{

test( "for_range tests" )
{
    int count;

    count = 0;
    for_range( i, 32 ) { count += 1; }

    check( count == 32 );

    count = 0;
    for_range( i, 5 ) { count += i; }

    check( count == 10 );

    count = 0;
    for_range( x, 5 ) { count += x; }

    check( count == 10 );
}

test( "while_max tests" )
{
    int count = 0;

    while_max( true, 10 ) { check( ( count++ ) < 10 ); }
    while_max( true, 10 ) { check( ( count++ ) < 20 ); }
    while_max( true, 10 ) { check( ( count++ ) < 30 ); }

    check( count == 30 );
}

test( "and_bool tests" )
{
    and_bool b;
    check( b );
    b = true;
    check( b );
    b = false;
    check( !b );
    b = true;
    check( !b );
}

test( "or_bool tests" )
{
    or_bool b;
    check( !b );
    b = false;
    check( !b );
    b = true;
    check( b );
    b = false;
    check( b );
}

test( "swap tests" )
{
    section( "int" )
    {
        int x = 3;
        int y = 5;

        swap_values( x, y );

        check( x == 5 );
        check( y == 3 );
    }

    section( "dec" )
    {
        dec x = 3.3;
        dec y = 5.5;

        swap_values( x, y );

        check( x == 5.5 );
        check( y == 3.3 );
    }

    section( "struct" )
    {
        struct structest { structest( int in_x ) { i = in_x; } int i = 0; };

        structest x = structest( 3 );
        structest y = structest( 5 );

        swap_values( x, y );

        check( x.i == 5 );
        check( y.i == 3 );
    }

    section( "if" )
    {
        int x = 3;
        int y = 5;

        swap_values_if( true, x, y );

        check( x == 5 );
        check( y == 3 );

        swap_values_if( false, x, y );

        check( x == 5 );
        check( y == 3 );
    }
}

test( "math tests" )
{
    section( "divisible" )
    {
        check( is_divisible( 5.0, 1.0 ) );
        check( is_divisible( 4.0, 2.0 ) );
        check( is_divisible( 3.0, 1.5 ) );

        check_not( is_divisible( 3.0, 1.6 ) );

        check( is_divisible( 3.0, 3.0 ) );
        check( is_divisible( 4.2, 4.2 ) );

        check_not( is_divisible( 0.0, 0.0 ) );
        check_not( is_divisible( 0.0, 1.0 ) );
        check_not( is_divisible( 1.0, 0.0 ) );
    }

    section( "prime" )
    {
        check_not( is_prime( 0 ) );
        check_not( is_prime( 1 ) );
        check( is_prime( 2 ) );
        check( is_prime( 3 ) );
        check_not( is_prime( 4 ) );
        check( is_prime( 5 ) );
        check_not( is_prime( 6 ) );
        check( is_prime( 7 ) );
        check_not( is_prime( 8 ) );
        check_not( is_prime( 9 ) );

        check_not( is_prime( 10 ) );
        check( is_prime( 11 ) );
        check_not( is_prime( 12 ) );
        check( is_prime( 13 ) );
        check_not( is_prime( 14 ) );
        check_not( is_prime( 15 ) );
        check_not( is_prime( 16 ) );
        check( is_prime( 17 ) );
        check_not( is_prime( 18 ) );
        check( is_prime( 19 ) );
        check_not( is_prime( 20 ) );

        check( is_prime( 101 ) );
        check_not( is_prime( 102 ) );
        check( is_prime( 103 ) );

        check_not( is_prime( 1.1 ) );
        check_not( is_prime( 2.1 ) );
        check_not( is_prime( 3.1 ) );
    }

    section( "composite" )
    {
        check_not( is_composite( 0 ) );
        check_not( is_composite( 1 ) );
        check_not( is_composite( 2 ) );
        check_not( is_composite( 3 ) );
        check( is_composite( 4 ) );
        check_not( is_composite( 5 ) );
        check( is_composite( 6 ) );
        check_not( is_composite( 7 ) );
        check( is_composite( 8 ) );
        check( is_composite( 9 ) );

        check_not( is_composite( 1.1 ) );
        check_not( is_composite( 2.1 ) );
        check_not( is_composite( 3.1 ) );
    }

    section( "pythagorean" )
    {
        check( pythagorean( 0.0, 0.0 ) == 0.0 );
        check( pythagorean( 3.0, 4.0 ) == 5.0 );
        check( pythagorean( 1.0, 1.0 ) == sqrt( 2.0 ) );
        check( pythagorean( 1.0, 2.0 ) == sqrt( 5.0 ) );
        check( pythagorean( 5.0, 12.0 ) == 13.0 );
        check( pythagorean( 7.0, 24.0 ) == 25.0 );
        check( pythagorean( 20.0, 21.0 ) == 29.0 );
    }

    section( "additorial" )
    {
        check( additorial( 0 ) == 0 );
        check( additorial( 1 ) == 1 );
        check( additorial( 2 ) == 3 );
        check( additorial( 3 ) == 6 );
        check( additorial( 4 ) == 10 );
        check( additorial( 5 ) == 15 );
        check( additorial( 6 ) == 21 );
        check( additorial( 7 ) == 28 );
        check( additorial( 8 ) == 36 );
        check( additorial( 9 ) == 45 );
    }

    section( "factorial" )
    {
        check( factorial( 0 ) == 1 );
        check( factorial( 1 ) == 1 );
        check( factorial( 2 ) == 2 );
        check( factorial( 3 ) == 6 );
        check( factorial( 4 ) == 24 );
        check( factorial( 5 ) == 120 );
        check( factorial( 6 ) == 720 );
        check( factorial( 7 ) == 5040 );
        check( factorial( 8 ) == 40320 );
        check( factorial( 9 ) == 362880 );
    }

    section( "fibonacci" )
    {
        check( fibonacci( 0 ) == 1 );
        check( fibonacci( 1 ) == 1 );
        check( fibonacci( 2 ) == 2 );
        check( fibonacci( 3 ) == 3 );
        check( fibonacci( 4 ) == 5 );
        check( fibonacci( 5 ) == 8 );
        check( fibonacci( 6 ) == 13 );
        check( fibonacci( 7 ) == 21 );
        check( fibonacci( 8 ) == 34 );
        check( fibonacci( 9 ) == 55 );
    }

    section( "pascal" )
    {
        check( pascal_triangle( 0, 0 ) == 1 );

        check( pascal_triangle( 1, 0 ) == 1 );
        check( pascal_triangle( 1, 1 ) == 1 );

        check( pascal_triangle( 2, 0 ) == 1 );
        check( pascal_triangle( 2, 1 ) == 2 );
        check( pascal_triangle( 2, 2 ) == 1 );

        check( pascal_triangle( 3, 0 ) == 1 );
        check( pascal_triangle( 3, 1 ) == 3 );
        check( pascal_triangle( 3, 2 ) == 3 );
        check( pascal_triangle( 3, 3 ) == 1 );

        check( pascal_triangle( 4, 0 ) == 1 );
        check( pascal_triangle( 4, 1 ) == 4 );
        check( pascal_triangle( 4, 2 ) == 6 );
        check( pascal_triangle( 4, 3 ) == 4 );
        check( pascal_triangle( 4, 4 ) == 1 );

        check( pascal_triangle( 5, 0 ) == 1 );
        check( pascal_triangle( 5, 1 ) == 5 );
        check( pascal_triangle( 5, 2 ) == 10 );
        check( pascal_triangle( 5, 3 ) == 10 );
        check( pascal_triangle( 5, 4 ) == 5 );
        check( pascal_triangle( 5, 5 ) == 1 );

        check( pascal_triangle( 6, 0 ) == 1 );
        check( pascal_triangle( 6, 1 ) == 6 );
        check( pascal_triangle( 6, 2 ) == 15 );
        check( pascal_triangle( 6, 3 ) == 20 );
        check( pascal_triangle( 6, 4 ) == 15 );
        check( pascal_triangle( 6, 5 ) == 6 );
        check( pascal_triangle( 6, 6 ) == 1 );

        check( pascal_triangle( 7, 0 ) == 1 );
        check( pascal_triangle( 7, 1 ) == 7 );
        check( pascal_triangle( 7, 2 ) == 21 );
        check( pascal_triangle( 7, 3 ) == 35 );
        check( pascal_triangle( 7, 4 ) == 35 );
        check( pascal_triangle( 7, 5 ) == 21 );
        check( pascal_triangle( 7, 6 ) == 7 );
        check( pascal_triangle( 7, 7 ) == 1 );

        check( pascal_triangle( 8, 0 ) == 1 );
        check( pascal_triangle( 8, 1 ) == 8 );
        check( pascal_triangle( 8, 2 ) == 28 );
        check( pascal_triangle( 8, 3 ) == 56 );
        check( pascal_triangle( 8, 4 ) == 70 );
        check( pascal_triangle( 8, 5 ) == 56 );
        check( pascal_triangle( 8, 6 ) == 28 );
        check( pascal_triangle( 8, 7 ) == 8 );
        check( pascal_triangle( 8, 8 ) == 1 );
    }
}

test( "mod tests" )
{
    // truncated (default), takes the sign of x like fmod
    check( mod( 7.0, 4.0 ) == 3.0 );
    check( mod( -7.0, 4.0 ) == -3.0 );
    check( mod( 7.0, -4.0 ) == 3.0 );
    check( mod( -7.0, -4.0 ) == -3.0 );
    check( mod( 8.0, 4.0 ) == 0.0 );

    // floored, takes the sign of y
    check( mod( 7.0, 4.0, true ) == 3.0 );
    check( mod( -7.0, 4.0, true ) == 1.0 );
    check( mod( 7.0, -4.0, true ) == -1.0 );
    check( mod( -7.0, -4.0, true ) == -3.0 );
    check( mod( -8.0, 4.0, true ) == 0.0 );
    check( mod( -1.0, 4.0, true ) == 3.0 );

    // ints
    check( mod( -7, 4 ) == -3 );
    check( mod( -7, 4, true ) == 1 );
}

test( "Countdown randomize tests" )
{
    Countdown countdown( 10.0 );

    for_range( i, 50 )
    {
        countdown.randomize( );

        // somewhere within the duration, which stays the same
        check( countdown.remaining( ) >= 0.0 );
        check( countdown.remaining( ) <= 10.0 );
        check( countdown.duration( ) == 10.0 );
    }

    Countdown empty;
    empty.randomize( );
    check( empty.completed( ) );
}

test( "map tests" )
{
    omap<int, int> ordered;
    umap<int, int> unordered;

    // operator[ ] adds a default value for a new key, then works like at( )
    ordered[ 3 ] = 7;
    unordered[ 3 ] = 7;

    check( ordered.contains( 3 ) );
    check( ordered.at( 3 ) == 7 );
    check( ordered[ 3 ] == 7 );
    check( ordered.size( ) == 1 );

    check( unordered.contains( 3 ) );
    check( unordered.at( 3 ) == 7 );
    check( unordered[ 3 ] == 7 );
    check( unordered.size( ) == 1 );

    // insert doesn't replace an existing value, same as the std containers
    check( ordered.insert( 4, 9 ) == 9 );
    check( ordered.insert( 4, 1 ) == 9 );
    check( unordered.insert( 4, 9 ) == 9 );
    check( unordered.insert( 4, 1 ) == 9 );

    check( ordered.size( ) == 2 );
    check( unordered.size( ) == 2 );

    ordered.clear( );
    unordered.clear( );

    check( ordered.empty( ) );
    check( unordered.empty( ) );
    check_not( ordered.contains( 3 ) );
    check_not( unordered.contains( 3 ) );
}

test( "queue tests" )
{
    queue<int> q;

    // push gives back the element it just added, not the front
    q.push( 1 );
    q.push( 2 ) = 5;

    check( q.front( ) == 1 );
    check( q.back( ) == 5 );
    check( q.pop( ) == 1 );
    check( q.pop( ) == 5 );
}

test( "Assert message tests" )
{
    // asserts are disabled while tests run, so failing ones only record their message
    check( string( Assert( false, "value %d is %s", 5, "bad" ) ) == "value 5 is bad" );
    check( string( Assert( false, "plain message" ) ) == "plain message" );
    check( string( Assert( true, "value %d", 5 ) ) == "" );
}

} // namespace axn
