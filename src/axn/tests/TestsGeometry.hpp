#include "axn/tests/Tests.hpp"

#include "axn/geometry/Geometry.hpp"

namespace axn
{

test( "Circle tests" )
{
    const Circle unit_circle = Circle( 1.0, ORIGIN );

    check( unit_circle.area( ) == PI );
    check( unit_circle.circumference( ) == TAU );

    check( unit_circle.intersects( Line( Point( 10.0, 1.0 ), Point( -10.0, 1.0 ) ), true ) );
    check_not( unit_circle.intersects( Line( Point( 10.0, 1.0 ), Point( -10.0, 1.0 ) ), false ) );

    check( unit_circle.intersects( Line( Point( 10.0, 0.9 ), Point( -10.0, 0.9 ) ) ) );
    check( unit_circle.intersects( Line( Point( 10.0, 9.5 ), Point( -10.0, -9.5 ) ) ) );
    check_not( unit_circle.intersects( Line( Point( 10.0, 1.1 ), Point( -10.0, 1.1 ) ) ) );

    check( unit_circle.intersection(
        Line( Point( 10.0, 0.0 ), Point( -10.0, 0.0 ) ) ) ==
        Line( Point( 1.0, 0.0 ), Point( -1.0, 0.0 ) ) );

    check( unit_circle.intersection(
        Line( Point( 10.0, 10.0 ), Point( -10.0, -10.0 ) ) ) ==
        Line( Point( sqrt( 0.5 ), sqrt( 0.5 ) ), Point( -sqrt( 0.5 ), -sqrt( 0.5 ) ) ) );

    check( unit_circle.intersection(
        Line( Point( 10.0, 0.5 ), Point( -10.0, 0.5 ) ) ) ==
        Line( Point( sqrt( 0.75 ), 0.5 ), Point( -sqrt( 0.75 ), 0.5 ) ) );
}

test( "Angle comparison tests" )
{
    const dec ROUNDING = 0.00001; // smaller than DEFAULT_EPSILON, like floating point rounding
    const dec REAL = 0.001; // bigger than DEFAULT_EPSILON, a real difference

    // a rounding sized difference counts as equal whichever side the Angle is on
    check( Angle( numbers::RIGHT + ROUNDING ) == RIGHT );
    check_not( Angle( numbers::RIGHT + ROUNDING ) > RIGHT );
    check_not( ( numbers::RIGHT + ROUNDING ) > RIGHT );
    check( ( numbers::RIGHT + ROUNDING ) == RIGHT );
    check( ( numbers::RIGHT + ROUNDING ) <= RIGHT );

    // a real difference still counts
    check( Angle( numbers::RIGHT + REAL ) > RIGHT );
    check( ( numbers::RIGHT + REAL ) > RIGHT );
    check( ( numbers::RIGHT - REAL ) < RIGHT );

    // abs( ) keeps it an Angle so comparing the result allows for rounding too
    check_not( abs( Angle( -( numbers::RIGHT + ROUNDING ) ) ) > RIGHT );
    check( abs( Angle( -( numbers::RIGHT + REAL ) ) ) > RIGHT );
    check( abs( Angle( -0.5 ) ) == Angle( 0.5 ) );
}

} // namespace axn
