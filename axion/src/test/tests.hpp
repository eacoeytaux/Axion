#ifndef AxionTests_hpp
#define AxionTests_hpp

#include "Catch.hpp"

#include "axn.hpp"
#include "Geometry.hpp"

#define REQUIRE_EQ( a, b ) REQUIRE( a == b )
#define REQUIRE_NOT_EQ( a, b ) REQUIRE( a != b )
#define REQUIRE_FLOAT_EQ( a, b ) REQUIRE( float_eq( a, b ) )
#define REQUIRE_FLOAT_NOT_EQ( a, b ) REQUIRE( !float_eq( a, b ) )
#define REQUIRE_FLOAT_GE( a, b ) REQUIRE( float_ge( a, b ) )
#define REQUIRE_FLOAT_GT( a, b ) REQUIRE( float_gt( a, b ) )
#define REQUIRE_FLOAT_LE( a, b ) REQUIRE( float_le( a, b ) )
#define REQUIRE_FLOAT_LT( a, b ) REQUIRE( float_lt( a, b ) )

AXN_NAMESPACES
namespace axn {

inline void TEST_COORDINATE_DISTANCE( const Coordinate & c1, const Coordinate & c2 ) {
    REQUIRE_FLOAT_EQ( c1.distance( c2 ), sqrt( pow<float>( c1.x( ) - c2.x( ), 2 ) +
                                               pow<float>( c1.y( ) - c2.y( ), 2 ) ) );
}

inline void TEST_COORDINATE( const Coordinate & coordinate, const planc x, const planc y,
                             const Quadrant quad, const Axis axis ) {
    REQUIRE_FLOAT_EQ( coordinate.x( ), x );
    REQUIRE_FLOAT_EQ( coordinate.y( ), y );
    REQUIRE( coordinate.in_quadrant( quad ) );
    REQUIRE( coordinate.on_axis( axis ) );
    TEST_COORDINATE_DISTANCE( coordinate, Coordinate( 0.f, 0.f ) );
    TEST_COORDINATE_DISTANCE( coordinate, Coordinate( 1.f, 0.f ) );
    TEST_COORDINATE_DISTANCE( coordinate, Coordinate( 1.f, 1.f ) );
    TEST_COORDINATE_DISTANCE( coordinate, Coordinate( 0.f, 1.f ) );
    TEST_COORDINATE_DISTANCE( coordinate, Coordinate( -1.f, 1.f ) );
    TEST_COORDINATE_DISTANCE( coordinate, Coordinate( -1.f, 0.f ) );
    TEST_COORDINATE_DISTANCE( coordinate, Coordinate( -1.f, -1.f ) );
    TEST_COORDINATE_DISTANCE( coordinate, Coordinate( 0.f, -1.f ) );
    TEST_COORDINATE_DISTANCE( coordinate, Coordinate( 1.f, -1.f ) );
}

inline void TEST_LINE( const Line & line, const Coordinate & c1, const Coordinate & c2,
                       const Angle & a, const planc length ) {
    REQUIRE_EQ( line.c1( ), c1 );
    REQUIRE_EQ( line.c2( ), c2 );
    REQUIRE_EQ( line.angle( ), a );
    REQUIRE_FLOAT_EQ( line.length( ), length );
}

inline void TEST_SHAPE( const Shape & shape, uint sides, const planc area,
                        const Coordinate & upper_bound, const Coordinate & lower_bound ) {
    REQUIRE_EQ( shape.coordinates( ).size( ), sides );
    REQUIRE_EQ( shape.lines( ).size( ), sides );
    REQUIRE_FLOAT_EQ( shape.area( ), area );
    REQUIRE_FLOAT_EQ( shape.upper_bound_x( ), upper_bound.x( ) );
    REQUIRE_FLOAT_EQ( shape.upper_bound_y( ), upper_bound.y( ) );
    REQUIRE_FLOAT_EQ( shape.lower_bound_x( ), lower_bound.x( ) );
    REQUIRE_FLOAT_EQ( shape.lower_bound_y( ), lower_bound.y( ) );
}

//inline void TEST_CIRCLE( const Circle & circle, const Coordinate & center,
//                         const planc radius ) {
//    TEST_SHAPE( circle, Circle::PRECISION, radius * radius * PI,
//                center + Vector( radius, radius ), center - Vector( radius, radius ) );
//    REQUIRE_EQ( circle.center( ), center );
//    REQUIRE_FLOAT_EQ( circle.radius( ), radius );
//}

} // namespace axn

#endif /* AxionTests_hpp */
