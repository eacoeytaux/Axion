#ifndef Random_hpp
#define Random_hpp

#include "Utility.hpp"
#include "Clock.hpp"
#include "Color.hpp"
#include "Planc.hpp"
#include "Angle.hpp"
#include "Coordinate.hpp"
#include "Vector.hpp"

namespace axn
{

class Random
{

public:

    // todo add more
    enum Distribution
    {
        UNIFORM,
    };

    #define DISTRIBUTION Distribution distribution = UNIFORM

private:

    static uint seed_value;
    static rng_engine engine;

    static uint random_int( );

    Random( ) { }

public:

    static uint seed( ) { return seed_value; }

    static error reset( ) { return set( seed( ) ); }

    static error set( uint seed = milliseconds( ) );
    static error set( string seed ) { return set( stoi( seed ) ); }

    static bool rbool( dec true_odds = 0.5 ) { return ( rdec( 1.0 ) <= true_odds ); }

    static int rint( int bound, DISTRIBUTION ) { return rint( 0, bound, distribution ); }
    static int rint( int bound1, int bound2, DISTRIBUTION ) { return ( random_int( ) % ( (uint)( abs( bound1 - bound2 ) ) + 1 ) ) + min( bound1, bound2 ); }

    static dec rdec( DISTRIBUTION ) { return rdec( 0.0, 1.0, distribution ); }
    static dec rdec( dec bound, DISTRIBUTION ) { return rdec( 0.0, bound, distribution ); }
    static dec rdec( dec bound1, dec bound2, DISTRIBUTION ) { return ( ( (dec)( random_int( ) ) / (dec)( RAND_MAX ) ) * ( bound1 - bound2 ) ) + bound2; }

    static Planc rPlanc( Planc cref bound, DISTRIBUTION ) { return rPlanc( P0, bound, distribution ); }
    static Planc rPlanc( Planc cref bound1, Planc cref bound2, DISTRIBUTION ) { return Planc( rdec( bound1, bound2, distribution ) ); }

    static Angle rAngle( DISTRIBUTION ) { return rAngle( A0, TAU, distribution ); }
    static Angle rAngle( Angle cref bound, DISTRIBUTION ) { return rAngle( A0, bound, distribution ); }
    static Angle rAngle( Angle cref bound1, Angle cref bound2, DISTRIBUTION ) { return Angle( rdec( bound1, bound2, distribution ) ); }

    static Color rColor( bool include_alpha = false ) { return Color::rgba( rdec( 1.0 ), rdec( 1.0 ), rdec( 1.0 ), include_alpha ? rdec( 1.0 ) : 1.0 ); }

    typeT static void negate( T & t, dec odds = 0.5 ) { if( rbool( odds ) ) { t = -t; } }
    typeT static T negated( T cref t, dec odds = 0.5 ) { return ( rbool( odds ) ? -t : t ); }

    typeT static void shuffle( varray<T> & v ) { std::shuffle( v.begin( ), v.end( ), engine ); }
    typeT static varray<T> shuffled( varray<T> cref v ) { varray<T> vs = v; shuffle( vs ); return vs; }

    typeT static int rint( const Span<T> & span, DISTRIBUTION ) { return rint( span.min( ), span.max( ), distribution ); }
    typeT static dec rdec( const Span<T> & span, DISTRIBUTION ) { return rdec( span.min( ), span.max( ), distribution ); }
    typeT static Planc rPlanc( const Span<T> & span, DISTRIBUTION ) { return rPlanc( span.min( ), span.max( ), distribution ); }
    typeT static Angle rAngle( const Span<T> & span, DISTRIBUTION ) { return rAngle( span.min( ), span.max( ), distribution ); }

};

} // namespace axn

#endif /* Random_hpp */
