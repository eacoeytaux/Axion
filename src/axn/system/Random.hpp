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

class Random final
{

public:

    enum distribution_type
    {
        UNIFORM,
        SKEW_LO,
        SKEW_MID,
        SKEW_HI,
    };

    #define DISTRIBUTION_IN distribution_type in_distribution = UNIFORM

private:

    static default_random_engine engine;

    static uint seed_value;

    static uint next( );

    static dec spread( dec in_bound1, dec in_bound2, DISTRIBUTION_IN ) { return( min( in_bound1, in_bound2 ) + ( fraction( in_distribution ) * abs( in_bound1 - in_bound2 ) ) ); }

    static declmp fraction( DISTRIBUTION_IN )
    {
        dec f1 = ( dec( next( ) ) / dec( RAND_MAX ) );
        dec f2 = ( dec( next( ) ) / dec( RAND_MAX ) );

        return_if( ( in_distribution == UNIFORM ), f1 );
        return_if( ( in_distribution == SKEW_LO ), ( f1 * f2 ) );
        return_if( ( in_distribution == SKEW_MID ), half( f1 + f2 ) );
        return_if( ( in_distribution == SKEW_HI ), one_minus( f1 * f2 ) );

        return( f1 );
    }

    Random( ) { }

public:

    static Error reset( ) { return( seed( seed( ) ) ); }

    static uint seed( ) { return( seed_value ); }

    static Error seed( uint in_seed );
    static Error seed( string cref in_seed ) { return( seed( stoi( in_seed ) ) ); }

    static Error rseed( ) { return( seed( next( ) ) ); }

    static bool rbool( declmp in_true_odds = 0.5 ) { return( rdec( 1.0 ) <= in_true_odds ); }

    static int rint( int in_bound, DISTRIBUTION_IN ) { return( rint( 0, in_bound, in_distribution ) ); }
    static int rint( int in_bound1, int in_bound2, DISTRIBUTION_IN ) { uint range = ( uint( abs( in_bound1 - in_bound2 ) ) + 1 ); return_if( ( in_distribution == UNIFORM ), ( ( next( ) % range ) + min( in_bound1, in_bound2 ) ) ); return( min( in_bound1, in_bound2 ) + int( min( uint( fraction( in_distribution ) * dec( range ) ), ( range - 1 ) ) ) ); }

    static dec rdec( DISTRIBUTION_IN ) { return( rdec( 0.0, 1.0, in_distribution ) ); }
    static dec rdec( dec in_bound, DISTRIBUTION_IN ) { return( rdec( 0.0, in_bound, in_distribution ) ); }
    static dec rdec( dec in_bound1, dec in_bound2, DISTRIBUTION_IN ) { return_if( ( in_distribution != UNIFORM ), spread( in_bound1, in_bound2, in_distribution ) ); return( ( ( dec( next( ) ) / dec( RAND_MAX ) ) * ( in_bound1 - in_bound2 ) ) + in_bound2 ); }

    static ticks rticks( ticks in_bound, DISTRIBUTION_IN ) { return( rticks( 0.0, in_bound, in_distribution ) ); }
    static ticks rticks( ticks in_bound1, ticks in_bound2, DISTRIBUTION_IN ) { return_if( ( in_distribution != UNIFORM ), ticks( spread( in_bound1, in_bound2, in_distribution ) ) ); return( ( ( ticks( next( ) ) / ticks( RAND_MAX ) ) * ( in_bound1 - in_bound2 ) ) + in_bound2 ); }

    static Planc rPlanc( Planc cref in_bound, DISTRIBUTION_IN ) { return( rPlanc( P0, in_bound, in_distribution ) ); }
    static Planc rPlanc( Planc cref in_bound1, Planc cref in_bound2, DISTRIBUTION_IN ) { return( Planc( rdec( in_bound1, in_bound2, in_distribution ) ) ); }

    static Angle rAngle( DISTRIBUTION_IN ) { return( rAngle( A0, TAU, in_distribution ) ); }
    static Angle rAngle( Angle cref in_bound, DISTRIBUTION_IN ) { return( rAngle( A0, in_bound, in_distribution ) ); }
    static Angle rAngle( Angle cref in_bound1, Angle cref in_bound2, DISTRIBUTION_IN ) { return( Angle( rdec( in_bound1, in_bound2, in_distribution ) ) ); }

    static Color rColor( bool in_include_alpha = false ) { return( Color::rgb( rdec( 1.0 ), rdec( 1.0 ), rdec( 1.0 ), in_include_alpha ? rdec( 1.0 ) : 1.0 ) ); }

    typeT static int rint( span<T1> cref in_span, DISTRIBUTION_IN ) { return( rint( in_span.min( ), in_span.max( ), in_distribution ) ); }
    typeT static dec rdec( span<T1> cref in_span, DISTRIBUTION_IN ) { return( rdec( in_span.min( ), in_span.max( ), in_distribution ) ); }
    typeT static ticks rticks( span<T1> cref in_span, DISTRIBUTION_IN ) { return( rticks( in_span.min( ), in_span.max( ), in_distribution ) ); }
    typeT static Planc rPlanc( span<T1> cref in_span, DISTRIBUTION_IN ) { return( rPlanc( in_span.min( ), in_span.max( ), in_distribution ) ); }
    typeT static Angle rAngle( span<T1> cref in_span, DISTRIBUTION_IN ) { return( rAngle( in_span.min( ), in_span.max( ), in_distribution ) ); }

    static Point rPointCircle( Point cref in_center, Planc cref in_distance ) { return( VectorA( rAngle( ), ( sqrt( rdec( 1.0 ) ) * in_distance ), in_center ).destination( ) ); }
    static Point rPointTriangle( Polygon cref in_bounds ) { Assert( in_bounds.sides( ) == 3 ); dec r1 = rdec( ); dec r2 = rdec( ); dec lo = min( r1, r2 ); dec hi = max( r1, r2 ); varray<Point> cref pts = in_bounds.points( ); return( Point( ( lo * pts[ 0 ].x( ) ) + ( ( hi - lo ) * pts[ 1 ].x( ) ) + ( one_minus( hi ) * pts[ 2 ].x( ) ), ( lo * pts[ 0 ].y( ) ) + ( ( hi - lo ) * pts[ 1 ].y( ) ) + ( one_minus( hi ) * pts[ 2 ].y( ) ) ) ); } // C! two random cuts of 0 to 1, in order: the three pieces are the point's weights, so every point is equally likely
    static Point rPointPolygon( Polygon cref in_poly ) { varray<Polygon> triangles = in_poly.triangles( ); dec total_area = 0.0; varray<dec> cumulative_area( triangles.size( ) ); for_range( i, triangles.size( ) ) { total_area += dec( triangles[ i ].area( ) ); cumulative_area[ i ] = total_area; } return_if( !is_pos( total_area ), in_poly.bounds_center( ) ); const dec c_r = rdec( ) * total_area; uint index = 0; while( ( ( index + 1 ) < cumulative_area.size( ) ) && ( cumulative_area[ index ] < c_r ) ) { index++; } return( rPointTriangle( triangles[ index ] ) ); } // C! a triangle picked by its share of the area, then a point in it

    static Path rSegment( Line cref in_line, uint in_segments, Planc cref in_offset = P0 ) { return( rSegment( Path( in_line ), in_segments, in_offset ) ); }
    static Path rSegment( Path cref in_path, uint in_segments, Planc cref in_offset = P0 ) { varray<Point> pts = in_path.segmented( in_segments ).points( ); for_range( i, pts.size( ) ) { if( i && ( ( i + 1 ) < pts.size( ) ) ) { pts[ i ] = pts[ i ] + VectorA( rAngle( ), rPlanc( in_offset ) ); } } return( Path( pts ) ); }

    typeT static void negate( T1 ref in_x, declmp in_odds = 0.5 ) { ::negate( in_x, rbool( in_odds ) ); }
    typeT static T1 negated( T1 cref in_x, declmp in_odds = 0.5 ) { return( ::negated( in_x, rbool( in_odds ) ) ); }

    typeT static void shuffle( varray<T1> ref in_varray ) { default_random_engine engine = default_random_engine{ }; engine.seed( seed( ) ); ::shuffle( in_varray.begin( ), in_varray.end( ), engine ); }
    typeT static varray<T1> shuffled( varray<T1> cref in_varray ) { varray<T1> vs = in_varray; shuffle( vs ); return( vs ); }

    typeT static T1 cref at( varray<T1> cref in_varray, DISTRIBUTION_IN ) { Assert( in_varray.size( ) ); return( in_varray[ rint( in_varray.size( ) - 1, in_distribution ) ] ); } // C! passes in_distribution on now

};

inline void Countdown::reset( ) { m_countdown_remaining = m_countdown_duration = Random::rticks( m_countdown_duration_range ); }
inline void Countdown::randomize( ) { m_countdown_remaining = Random::rticks( m_countdown_duration ); }

} // namespace axn

#endif /* Random_hpp */
