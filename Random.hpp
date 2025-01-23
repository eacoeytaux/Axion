#ifndef Random_hpp
#define Random_hpp

#include "Utility.hpp"
#include "Color.hpp"
#include "Planc.hpp"
#include "Angle.hpp"
#include "Coordinate.hpp"
#include "Vector.hpp"

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

    // todo
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
    static dec rdec( dec high_bound, Distribution distribution = DEFAULT_DISTRIBUTION ) { return rdec( 0.0, high_bound, distribution ); }
    static dec rdec( dec low_bound, dec high_bound, Distribution distribution = DEFAULT_DISTRIBUTION );

    template <typename T>
    static Planc rPlanc( const Span<T> & span, Distribution distribution = DEFAULT_DISTRIBUTION ) { return rPlanc( span.min( ), span.max( ), distribution ); }
    static Planc rPlanc( Planc cref high_bound, Distribution distribution = DEFAULT_DISTRIBUTION ) { return rPlanc( 0.0, high_bound, distribution ); }
    static Planc rPlanc( Planc cref low_bound, Planc cref high_bound, Distribution distribution = DEFAULT_DISTRIBUTION );

    template <typename T>
    static Angle rAngle( const Span<T> & span, Distribution distribution = DEFAULT_DISTRIBUTION ) { return rAngle( span.min( ), span.max( ), distribution ); }
    static Angle rAngle( Angle cref high_bound, Distribution distribution = DEFAULT_DISTRIBUTION ) { return rAngle( Angle( 0.0 ), high_bound, distribution ); }
    static Angle rAngle( Angle cref low_bound, Angle cref high_bound, Distribution distribution = DEFAULT_DISTRIBUTION );
    static Angle rAngle( Distribution distribution = DEFAULT_DISTRIBUTION ) { return rAngle( Angle( TAU ), distribution ); }

    static Color rColor( bool include_alpha = false );

    template <typename T>
    static T & negate( T & t, dec flip_odds = 0.5 )
    {
        if( rbool( flip_odds ) )
        {
            t *= -1.0;
        }
        return t;
    }

    template <typename T>
    static T negated( T cref t, dec flip_odds = 0.5 )
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
