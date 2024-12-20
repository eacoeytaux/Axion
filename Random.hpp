#ifndef Random_hpp
#define Random_hpp

#include "Utility.hpp"
#include "Planc.hpp"
#include "Angle.hpp"
#include "Color.hpp"

namespace axn
{

class Random
{
private:
    Random( ) { }

public:
    enum Distribution
    {
        UNIFORM,
    };

    static const Distribution DEFAULT_DISTRIBUTION;

    static error seed( );
    static error seed( uint seed );
    static error seed( string seed );

    static bool rbool( dec true_odds = 0.5 );

    template <typename T>
    static int rint( const Span<T> & span, Distribution distribution = DEFAULT_DISTRIBUTION ) { return rint( span.min( ), span.max( ), distribution ); }
    static int rint( int high_bound, Distribution distribution = DEFAULT_DISTRIBUTION ) { return rint( 0, high_bound, distribution ); }
    static int rint( int low_bound, int high_bound, Distribution distribution = DEFAULT_DISTRIBUTION );

    template <typename T>
    static dec rdec( const Span<T> & span, Distribution distribution = DEFAULT_DISTRIBUTION ) { return rdec( span.min( ), span.max( ), distribution ); }
    static dec rdec( dec high_bound, Distribution distribution = DEFAULT_DISTRIBUTION ) { return rdec( ZERO, high_bound, distribution ); }
    static dec rdec( dec low_bound, dec high_bound, Distribution distribution = DEFAULT_DISTRIBUTION );

    template <typename T>
    static Planc rPlanc( const Span<T> & span, Distribution distribution = DEFAULT_DISTRIBUTION ) { return rPlanc( span.min( ), span.max( ), distribution ); }
    static Planc rPlanc( const Planc & high_bound, Distribution distribution = DEFAULT_DISTRIBUTION ) { return rPlanc( ZERO, high_bound, distribution ); }
    static Planc rPlanc( const Planc & low_bound, const Planc & high_bound, Distribution distribution = DEFAULT_DISTRIBUTION );

    template <typename T>
    static Angle rAngle( const Span<T> & span, Distribution distribution = DEFAULT_DISTRIBUTION ) { return rAngle( span.min( ), span.max( ), distribution ); }
    static Angle rAngle( const Angle & high_bound, Distribution distribution = DEFAULT_DISTRIBUTION ) { return rAngle( Angle( ZERO ), high_bound, distribution ); }
    static Angle rAngle( const Angle & low_bound, const Angle & high_bound, Distribution distribution = DEFAULT_DISTRIBUTION );
    static Angle rAngle( Distribution distribution = DEFAULT_DISTRIBUTION ) { return rAngle( Angle( TAU ), distribution ); }

    static Color rColor( bool include_alpha = false );

    template <typename T>
    static T & flip( T & t, dec flip_odds = 0.5 )
    {
        if( rbool( flip_odds ) )
        {
            t *= -1.0;
        }
        return t;
    }

    template <typename T>
    static T negate( const T & t, dec flip_odds = 0.5 )
    {
        if( rbool( flip_odds ) )
        {
            return -t;
        }
        else
        {
            return t;
        }
    }
};

} // namespace axn

#endif /* Random_hpp */
