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

} // namespace axn
