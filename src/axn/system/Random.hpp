#ifndef Random_hpp
#define Random_hpp

#include "axn/system/Clock.hpp"
#include "axn/system/Utility.hpp"
#include "axn/graphics/Color.hpp"
#include "axn/geometry/Planc.hpp"
#include "axn/geometry/Angle.hpp"
#include "axn/geometry/Point.hpp"
#include "axn/geometry/Vector.hpp"
#include "axn/geometry/Polygon.hpp"

namespace axn
{

class Random
{

public:

    // todo add more
    enum distribution_type
    {
        UNIFORM,
    };

    #define DISTRIBUTION_IN distribution_type in_distribution = UNIFORM

private:

    Random( ) { }

    static uint seed_value;
    static default_random_engine engine;

    static uint random_int( );

public:

    static uint seed( ) { return( seed_value ); }

    static error seed( uint in_seed );
    static error seed( string cref in_seed ) { return( seed( stoi( in_seed ) ) ); }
    static error seed_milliseconds( ) { return( seed( milliseconds( ) ) ); }

    static error reset( ) { return( seed( seed( ) ) ); }

    static bool rbool( dec in_true_odds = 0.5 ) { return( rdec( 1.0 ) <= in_true_odds ); }

    static int rint( int in_bound, DISTRIBUTION_IN ) { return( rint( 0, in_bound, in_distribution ) ); }
    static int rint( int in_bound1, int in_bound2, DISTRIBUTION_IN ) { return( ( random_int( ) % ( uint( abs( in_bound1 - in_bound2 ) ) + 1 ) ) + min( in_bound1, in_bound2 ) ); }

    static dec rdec( DISTRIBUTION_IN ) { return( rdec( 0.0, 1.0, in_distribution ) ); }
    static dec rdec( dec in_bound, DISTRIBUTION_IN ) { return( rdec( 0.0, in_bound, in_distribution ) ); }
    static dec rdec( dec in_bound1, dec in_bound2, DISTRIBUTION_IN ) { return( ( ( dec( random_int( ) ) / dec( RAND_MAX ) ) * ( in_bound1 - in_bound2 ) ) + in_bound2 ); }

    static Planc rPlanc( Planc cref in_bound, DISTRIBUTION_IN ) { return( rPlanc( P0, in_bound, in_distribution ) ); }
    static Planc rPlanc( Planc cref in_bound1, Planc cref in_bound2, DISTRIBUTION_IN ) { return( Planc( rdec( in_bound1, in_bound2, in_distribution ) ) ); }

    static Angle rAngle( DISTRIBUTION_IN ) { return( rAngle( A0, TAU, in_distribution ) ); }
    static Angle rAngle( Angle cref in_bound, DISTRIBUTION_IN ) { return( rAngle( A0, in_bound, in_distribution ) ); }
    static Angle rAngle( Angle cref in_bound1, Angle cref in_bound2, DISTRIBUTION_IN ) { return( Angle( rdec( in_bound1, in_bound2, in_distribution ) ) ); }

    static Color rColor( bool in_include_alpha = false ) { return( Color::rgb( rdec( 1.0 ), rdec( 1.0 ), rdec( 1.0 ), in_include_alpha ? rdec( 1.0 ) : 1.0 ) ); }

    typeT static int rint( Span<T1> cref in_span, DISTRIBUTION_IN ) { return( rint( in_span.min( ), in_span.max( ), in_distribution ) ); }
    typeT static dec rdec( Span<T1> cref in_span, DISTRIBUTION_IN ) { return( rdec( in_span.min( ), in_span.max( ), in_distribution ) ); }
    typeT static Planc rPlanc( Span<T1> cref in_span, DISTRIBUTION_IN ) { return( rPlanc( in_span.min( ), in_span.max( ), in_distribution ) ); }
    typeT static Angle rAngle( Span<T1> cref in_span, DISTRIBUTION_IN ) { return( rAngle( in_span.min( ), in_span.max( ), in_distribution ) ); }

    typeT static void negate( T1 ref in_x, dec in_odds = 0.5 ) { if( rbool( in_odds ) ) { in_x = -in_x; } }
    typeT static T1 negated( T1 cref in_x, dec in_odds = 0.5 ) { return( rbool( in_odds ) ? -in_x : in_x ); }

    typeT static void shuffle( varray<T1> ref in_varray ) { default_random_engine engine = default_random_engine{ }; engine.seed( seed( ) ); std::shuffle( in_varray.begin( ), in_varray.end( ), engine ); }
    typeT static varray<T1> shuffled( varray<T1> cref in_varray ) { varray<T1> vs = in_varray; shuffle( vs ); return( vs ); }

    typeT static T1 cref at( varray<T1> cref in_varray, DISTRIBUTION_IN ) { Assert( in_varray.size( ) ); return( in_varray[ rint( in_varray.size( ) - 1 ) ] ); }

    static Point rPointCircle( Point cref in_center, Planc cref in_distance ) { return( VectorA( rAngle( ), ( sqrt( rdec( 1.0 ) ) * in_distance ), in_center ).destination( ) ); }

    static Point rPointTriangle( Polygon cref in_bounds )
    {
        Assert( in_bounds.sides( ) == 3 );

        Point cref p1 = in_bounds.points( )[ 0 ];
        Point cref p2 = in_bounds.points( )[ 1 ];
        Point cref p3 = in_bounds.points( )[ 2 ];

        dec r1 = Random::rdec( );
        dec r2 = Random::rdec( );

        dec a = r1;
        dec b = r2 - r1;
        dec c = one_minus( r2 );

        Planc x = ( a * p1.x( ) ) + ( b * p2.x( ) ) + ( c * p3.x( ) );
        Planc y = ( a * p1.y( ) ) + ( b * p2.y( ) ) + ( c * p3.y( ) );

        return( Point( x, y ) );
    }

    static Point rPointPolygon( Polygon cref in_polygon )
    {
        varray<Polygon> triangles = in_polygon.triangles( );
        varray<dec> div( triangles.size( ) );

        return_if( is_zero( triangles.size( ) ), in_polygon.bounds_center( ) );

        double area = in_polygon.area( );

        for_range( i, triangles.size( ) )
        {
            Polygon triangle = triangles[ i ];

            double div_area = triangle.area( ) / area;

            if( i )
            {
                div[ i ] = div_area + div[ i - 1 ];
            }
            else
            {
                div[ i ] = div_area;
            }
        }

        Assert( is_one( div.back( ) ) );

        dec r = Random::rdec( );

        uint index = 0;
        while( ( index < div.size( ) ) && ( div[ index ] < r ) ) { index++; }

        return( rPointTriangle( triangles[ index ] ) );
    }

};

} // namespace axn

#endif /* Random_hpp */
