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

} // namespace axn
