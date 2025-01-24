#ifndef Line_hpp
#define Line_hpp

#include "Planc.hpp"
#include "Coordinate.hpp"
#include "Angle.hpp"
#include "Vector.hpp"
#include "Transform.hpp"

namespace axn
{
namespace geometry
{

class Line : public Transformable
{

private:

    Coordinate m_c1 = ORIGIN;
    Coordinate m_c2 = ORIGIN;

    Planc m_m = INFINITY;
    Planc m_b = P0;

    Line & solve_mb( ) { if( c1( ).x( ) == c2( ).x( ) ) { m_m = ( ( c1( ).y( ) <= c2( ).y( ) ) ? INFINITY_POS : INFINITY_NEG ); m_b = c1( ).x( ); } else { m_m = ( ( c2( ).y( ) - c1( ).y( ) ) / ( c2( ).x( ) - c1( ).x( ) ) ); m_b = ( c1( ).y( ) - ( m_m * c1( ).x( ) ) ); } rethis; }

public:

    Line( ) { }

    Line( Coordinate cref c1, Coordinate cref c2 ) : m_c1( c1 ), m_c2( c2 ) { solve_mb( ); }

    Line( Vector cref v ) : m_c1( v.origin( ) ), m_c2( v.destination( ) ) { solve_mb( ); }

    Coordinate cref c1( ) const { return m_c1; }
    Coordinate cref c2( ) const { return m_c2; }

    Line flipped( ) const { return Line( c2( ), c1( ) ); }

    Angle angle( ) const { return Angle( c1( ), c2( ) ); }
    Vector vector( ) const { return Vector( c1( ), c2( ) ); }

    bool vertical( ) const { return is_inf( m( ) ); }
    bool horizontal( ) const { return is_zero( m( ) ); }

    Planc cref m( ) const { return m_m; }
    Planc cref b( ) const { return m_b; }

    Planc length( ) const { return c1( ).distance_to( c2( ) ); }

    Planc cref lower_bound_x( ) const { return min( c1( ).x( ), c2( ).x( ) ); }
    Planc cref lower_bound_y( ) const { return min( c1( ).y( ), c2( ).y( ) ); }
    Planc cref upper_bound_x( ) const { return max( c1( ).x( ), c2( ).x( ) ); }
    Planc cref upper_bound_y( ) const { return max( c1( ).y( ), c2( ).y( ) ); }

    Planc x( Planc cref y ) const { return ( vertical( ) || horizontal( ) ) ? c1( ).x( ) : ( ( y - b( ) ) / m( ) ); }
    Planc y( Planc cref x ) const { return ( vertical( ) || horizontal( ) ) ? c1( ).y( ) : ( ( m( ) * x ) + b( ) ); }

    Planc cross( Coordinate cref c ) const { return ::cross( c1( ), c2( ), c ); }

    bool on( Coordinate cref c ) const { return equal( cross( c ), 0.0 ) && ( ( c1( ) != c2( ) ) || ( c == c1( ) ) ); }
    bool above( Coordinate cref c, cbool inclusive = false ) const { return greater( inclusive, cross( c ), 0.0 ) && ( c1( ) != c2( ) ); }
    bool below( Coordinate cref c, cbool inclusive = false ) const { return less( inclusive, cross( c ), 0.0 ) && ( c1( ) != c2( ) ); }

    bool in_bounds( Coordinate cref c, cbool inclusive = true ) const { return ( in_range( c.x( ), lower_bound_x( ), upper_bound_x( ), inclusive ) && in_range( c.y( ), lower_bound_y( ), upper_bound_y( ), inclusive ) ); }

    bool parallel( Line cref l ) const { return ( ( ( m( ) == l.m( ) ) || ( is_inf( m( ) ) && is_inf( l.m( ) ) ) ) && ( ( c1( ) != c2( ) ) && ( l.c1( ) != l.c2( ) ) ) ); }
    bool colinear( Line cref l ) const { return ( ( *this == l ) || ( parallel( l ) && ( b( ) == l.b( ) ) ) ); }

    bool intersects( Line cref l, bool inclusive = true ) const
    {
        if( parallel( l ) )
        {
            if( b( ) != l.b( ) )
            {
                return false;
            }
            else
            {
                if( vertical( ) )
                {
                    return ( in_range( l.c1( ).y( ), lower_bound_y( ), upper_bound_y( ), inclusive ) ||
                             in_range( l.c2( ).y( ), lower_bound_y( ), upper_bound_y( ), inclusive ) ||
                             in_range( c1( ).y( ), l.lower_bound_y( ), l.upper_bound_y( ), inclusive ) ||
                             in_range( c2( ).y( ), l.lower_bound_y( ), l.upper_bound_y( ), inclusive ) );
                }
                else
                {
                    return ( in_range( l.c1( ).x( ), lower_bound_x( ), upper_bound_x( ), inclusive ) ||
                             in_range( l.c2( ).x( ), lower_bound_x( ), upper_bound_x( ), l.c2( ).x( ), inclusive ) ||
                             in_range( c1( ).x( ), l.lower_bound_x( ), l.upper_bound_x( ), inclusive ) ||
                             in_range( c2( ).x( ), l.lower_bound_x( ), l.upper_bound_x( ), inclusive ) );
                }
            }
        }
        else
        {
            Coordinate intersect;

            if( vertical( ) )
            {
                intersect = Coordinate( lower_bound_x( ), ( l.horizontal( ) ? l.lower_bound_y( ) : ( lower_bound_x( ) * l.m( ) ) + l.b( ) ) );
            }
            else if( l.vertical( ) )
            {
                intersect = Coordinate( l.lower_bound_x( ), ( horizontal( ) ? lower_bound_y( ) : ( l.lower_bound_x( ) * m( ) ) + b( ) ) );
            }
            else if( horizontal( ) )
            {
                intersect = Coordinate( ( ( lower_bound_y( ) - l.b( ) ) / l.m( ) ), lower_bound_y( ) );
            }
            else if( l.horizontal( ) )
            {
                intersect = Coordinate( ( ( l.lower_bound_y( ) - b( ) ) / m( ) ), l.lower_bound_y( ) );
            }
            else
            {
                Planc x = ( l.b( ) - b( ) ) / ( m( ) - l.m( ) );
                intersect = Coordinate( x, ( x * m( ) ) + b( ) );
            }

            return ( in_bounds( intersect, inclusive ) && l.in_bounds( intersect, inclusive ) );
        }
    }

    Coordinate intersection( Line cref l ) const
    {
        if( parallel( l ) )
        {
            if( b( ) != l.b( ) )
            {
                return INVALID_COORDINATE;
            }
            else
            {
                return c1( );
            }
        }
        else
        {
            if( vertical( ) )
            {
                if( l.horizontal( ) )
                {
                    return Coordinate( c1( ).x( ), l.c1( ).y( ) );
                }
                else
                {
                    return Coordinate( c1( ).x( ), l.y( c1( ).x( ) ) );
                }
            }
            else if( horizontal( ) )
            {
                if( l.vertical( ) )
                {
                    return Coordinate( l.c1( ).x( ), c1( ).y( ) );
                }
                else
                {
                    return Coordinate( l.x( c1( ).y( ) ), c1( ).y( ) );
                }
            }
            else
            {
                if( l.vertical( ) )
                {
                    return Coordinate( l.c1( ).x( ), y( l.c1( ).x( ) ) );
                }
                else if( l.horizontal( ) )
                {
                    return Coordinate( x( l.c1( ).y( ) ), l.c1( ).y( ) );
                }
                else
                {
                    Planc x = ( l.b( ) - b( ) ) / ( m( ) - l.m( ) );
                    return Coordinate( x, l.y( x ) );
                }
            }
        }
    }

    virtual Line & transform( Transform cref t ) override { m_c1 = t.apply( m_c1 ); m_c2 = t.apply( m_c2 ); return solve_mb( ); }

    Line operator+( Vector cref v ) const { return Line( c1( ) + v, c2( ) + v ); }
    Line operator-( Vector cref v ) const { return Line( c1( ) - v, c2( ) - v ); }

    Line & operator+=( Vector cref v ) { rethis = *this + v; }
    Line & operator-=( Vector cref v ) { rethis = *this - v; }

    default_equal( Line );
};

} // namespace geometry
} // namespace axn

#endif /* Line_hpp */
