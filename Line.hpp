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

    Line & solve_mb( ) { if( c1( ).x( ) == c2( ).x( ) ) { m_m = INFINITY; m_b = c1( ).x( ); } else { m_m = ( ( c2( ).y( ) - c1( ).y( ) ) / ( c2( ).x( ) - c1( ).x( ) ) ); m_b = ( c1( ).y( ) - ( m_m * c1( ).x( ) ) ); } rethis; }

public:
    Line( ) { }

    Line( const Coordinate & c1, const Coordinate & c2 ) : m_c1( c1 ), m_c2( c2 ) { solve_mb( ); }

    Line( const Vector & v ) : m_c1( v.origin( ) ), m_c2( v.destination( ) ) { solve_mb( ); }

    const Coordinate & c1( ) const { if( is_dirty( ) ) { } return m_c1; }
    const Coordinate & c2( ) const { return m_c2; }

    Angle angle( ) const { return Angle( c1( ), c2( ) ); }
    Vector vector( ) const { return Vector( c1( ), c2( ) ); }

    bool vertical( ) const { return is_inf( m( ) ); }
    bool horizontal( ) const { return is_zero( m( ) ); }

    const Planc & m( ) const { return m_m; }
    const Planc & b( ) const { return m_b; }

    Planc length( ) const { return c1( ).distance_to( c2( ) ); }

    const Planc & x_lower( ) const { return min( c1( ).x( ), c2( ).x( ) ); }
    const Planc & y_lower( ) const { return min( c1( ).y( ), c2( ).y( ) ); }
    const Planc & x_upper( ) const { return max( c1( ).x( ), c2( ).x( ) ); }
    const Planc & y_upper( ) const { return max( c1( ).y( ), c2( ).y( ) ); }

    Planc x( const Planc & y ) const { return ( vertical( ) || horizontal( ) ) ? c1( ).x( ) : ( ( y - b( ) ) / m( ) ); }
    Planc y( const Planc & x ) const { return ( vertical( ) || horizontal( ) ) ? c1( ).y( ) : ( ( m( ) * x ) + b( ) ); }

    Planc cross( const Coordinate & c ) const { return ::cross( c1( ), c2( ), c ); }

    bool on( const Coordinate & c ) const { return equal( cross( c ), ZERO ); }
    bool above( const Coordinate & c, const bool inclusive = false ) const { return greater( inclusive, cross( c ), ZERO ); }
    bool below( const Coordinate & c, const bool inclusive = false ) const { return less( inclusive, cross( c ), ZERO ); }

    bool in_bounds( const Coordinate & c, const bool inclusive = true ) const { return ( in_range( c.x( ), x_lower( ), x_upper( ), inclusive ) && in_range( c.y( ), y_lower( ), y_upper( ), inclusive ) ); }

    bool parallel( const Line & l ) const { return ( ( m( ) == l.m( ) ) && ( c1( ) != c2( ) ) && ( l.c1( ) != l.c2( ) ) ); }

    bool intersects( const Line & l, bool inclusive = true ) const
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
                    return ( in_range( l.c1( ).y( ), y_lower( ), y_upper( ), inclusive ) ||
                             in_range( l.c2( ).y( ), y_lower( ), y_upper( ), inclusive ) ||
                             in_range( c1( ).y( ), l.y_lower( ), l.y_upper( ), inclusive ) ||
                             in_range( c2( ).y( ), l.y_lower( ), l.y_upper( ), inclusive ) );
                }
                else
                {
                    return ( in_range( l.c1( ).x( ), x_lower( ), x_upper( ), inclusive ) ||
                             in_range( l.c2( ).x( ), x_lower( ), x_upper( ), l.c2( ).x( ), inclusive ) ||
                             in_range( c1( ).x( ), l.x_lower( ), l.x_upper( ), inclusive ) ||
                             in_range( c2( ).x( ), l.x_lower( ), l.x_upper( ), inclusive ) );
                }
            }
        }
        else
        {
            Coordinate intersect;

            if( vertical( ) )
            {
                intersect = Coordinate( x_lower( ), ( l.horizontal( ) ? l.y_lower( ) : ( x_lower( ) * l.m( ) ) + l.b( ) ) );
            }
            else if( l.vertical( ) )
            {
                intersect = Coordinate( l.x_lower( ), ( horizontal( ) ? y_lower( ) : ( l.x_lower( ) * m( ) ) + b( ) ) );
            }
            else if( horizontal( ) )
            {
                intersect = Coordinate( ( ( y_lower( ) - l.b( ) ) / l.m( ) ), y_lower( ) );
            }
            else if( l.horizontal( ) )
            {
                intersect = Coordinate( ( ( l.y_lower( ) - b( ) ) / m( ) ), l.y_lower( ) );
            }
            else
            {
                Planc x = ( l.b( ) - b( ) ) / ( m( ) - l.m( ) );
                intersect = Coordinate( x, ( x * m( ) ) + b( ) );
            }

            return ( in_bounds( intersect, inclusive ) && l.in_bounds( intersect, inclusive ) );
        }
    }

    Coordinate intersection( const Line & l ) const
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
                    return Coordinate( ( c1( ).y( ) - l.b( ) ) / l.m( ), c1( ).y( ) );
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
                    return Coordinate( ( c1( ).y( ) - l.b( ) ) / l.m( ), c1( ).y( ) );
                }
                else
                {
                    Planc x = ( l.b( ) - b( ) ) / ( m( ) - l.m( ) );
                    return Coordinate( x, l.y( x ) );
                }
            }
        }
    }

    virtual Line & transform( const Transform & t ) override { m_c1 = t.apply( m_c1 ); m_c2 = t.apply( m_c2 ); return solve_mb( ); }

    Line operator+( const Vector & v ) const { return Line( c1( ) + v, c2( ) + v ); }
    Line operator-( const Vector & v ) const { return Line( c1( ) - v, c2( ) - v ); }

    Line & operator+=( const Vector & v ) { rethis = *this + v; }
    Line & operator-=( const Vector & v ) { rethis = *this - v; }

    default_equal( Line );
};

} // namespace geometry
} // namespace axn

#endif /* Line_hpp */
