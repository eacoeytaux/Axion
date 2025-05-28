#ifndef Triangle_hpp
#define Triangle_hpp

#include "Planc.hpp"
#include "Coordinate.hpp"
#include "Angle.hpp"
#include "Vector.hpp"
#include "Transform.hpp"
#include "Line.hpp"
#include "Path.hpp"

namespace axn
{
namespace geometry
{

axnclass( Triangle ) : public Transformable
{

private:

    Coordinate m_c1 = ORIGIN;
    Coordinate m_c2 = ORIGIN;
    Coordinate m_c3 = ORIGIN;

public:

    Triangle( ) { }

    Triangle( xCoordinate c1, xCoordinate c2, xCoordinate c3 ) : m_c1( c1 ), m_c2( c2 ), m_c3( c3 ) { Assert( c1.valid( ) && c2.valid( ) && c3.valid( ) );}

    Coordinate cref c1( ) const { return m_c1; }
    Coordinate cref c2( ) const { return m_c2; }
    Coordinate cref c3( ) const { return m_c3; }

    Planc cref lower_bound_x( ) const { return min( min( c1( ).x( ), c2( ).x( ) ), c3( ).x( ) ); }
    Planc cref lower_bound_y( ) const { return min( min( c1( ).y( ), c2( ).y( ) ), c3( ).y( ) ); }
    Planc cref upper_bound_x( ) const { return max( max( c1( ).x( ), c2( ).x( ) ), c3( ).x( ) ); }
    Planc cref upper_bound_y( ) const { return max( max( c1( ).y( ), c2( ).y( ) ), c3( ).y( ) ); }

    Planc bound_width( ) const { return upper_bound_x( ) - lower_bound_x( ); }
    Planc bound_height( ) const { return upper_bound_y( ) - lower_bound_y( ); }

    Planc area( ) const { return abs( half( ( c1( ).x( ) * ( c2( ).y( ) - c3( ).y( ) ) ) + ( c2( ).x( ) * ( c3( ).y( ) - c1( ).y( ) ) ) + ( c3( ).x( ) * ( c1( ).y( ) - c2( ).y( ) ) ) ) ); }

    Coordinate centroid( ) const { return Coordinate( ( ( c1( ).x( ) + c2( ).x( ) + c3( ).x( ) ) / 3.0 ), ( ( c1( ).y( ) + c2( ).y( ) + c3( ).y( ) ) / 3.0 ) ); }

    Coordinate circumcenter( ) const
    {
        dec d12 = distance( c1( ), c2( ) );
        dec d13 = distance( c1( ), c3( ) );
        dec d23 = distance( c2( ), c3( ) );

        dec s1 = sin( acos( ( square( d12 ) + square( d13 ) - square( d23 ) ) / ( d12 * d13 * 2.0 ) ) * 2.0 );
        dec s2 = sin( acos( ( square( d12 ) + square( d23 ) - square( d13 ) ) / ( d12 * d23 * 2.0 ) ) * 2.0 );
        dec s3 = sin( acos( ( square( d13 ) + square( d23 ) - square( d12 ) ) / ( d13 * d23 * 2.0 ) ) * 2.0 );

        return Coordinate( ( ( s1 * c1( ).x( ) ) + ( s2 * c2( ).x( ) ) + ( s3 * c3( ).x( ) ) ),
                           ( ( s1 * c1( ).y( ) ) + ( s2 * c2( ).y( ) ) + ( s3 * c3( ).y( ) ) ) ) / ( s1 + s2 + s3 );
    }

    bool contains( xCoordinate c, bool inclusive = true ) const
    {
        dec a = 0.5 / half( ( c1( ).x( ) * ( c2( ).y( ) - c3( ).y( ) ) ) + ( c1( ).y( ) * ( c3( ).x( ) - c2( ).x( ) ) ) + ( c2( ).x( ) * c3( ).y( ) ) + ( c3( ).x( ) * -c2( ).y( ) ) );

        dec s = a * ( ( c1( ).y( ) * c3( ).x( ) ) - ( c1( ).x( ) * c3( ).y( ) ) + ( ( c3( ).y( ) - c1( ).y( ) ) * c.x( ) ) + ( ( c1( ).x( ) - c3( ).x( ) ) * c.y( ) ) );
        return_false_if( less( !inclusive, s, 0.0 ) );

        dec t = a * ( ( c1( ).x( ) * c2( ).y( ) ) - ( c1( ).y( ) * c2( ).x( ) ) + ( ( c1( ).y( ) - c2( ).y( ) ) * c.x( ) ) + ( ( c2( ).x( ) - c1( ).x( ) ) * c.y( ) ) );
        return_false_if( less( !inclusive, t, 0.0 ) );

        return !less( !inclusive, ( 1.0 - s - t ), 0.0 );
    }

    bool intersects( Line cref line ) const { return ( ( contains( line.c1( ) ) || contains( line.c2( ) ) ) || ( line.intersects( Line( c1( ), c2( ) ) ) || line.intersects( Line( c2( ), c3( ) ) ) || line.intersects( Line( c3( ), c1( ) ) ) ) ); }
    Line intersection( Line cref line ) const
    {
        bool c1c = contains( line.c1( ) );
        bool c2c = contains( line.c2( ) );

        return_if( ( c1c && c2c ), line );

        Line l1( c1( ), c2( ) );
        Line l2( c2( ), c3( ) );
        Line l3( c3( ), c1( ) );

        bool l1i = l1.intersects( line );
        bool l2i = l2.intersects( line );

        return_if( c1c, Line( line.c1( ), l1i ? l1.intersection( line ) : ( l2i ? l2.intersection( line ) : l3.intersection( line ) ) ) );
        return_if( c2c, Line( l1i ? l1.intersection( line ) : ( l2i ? l2.intersection( line ) : l3.intersection( line ) ), line.c2( ) ) );

        Coordinate c1 = l1i ? l1.intersection( line ) : l2.intersection( line );
        Coordinate c2 = ( l1i && l2i ) ? l2.intersection( line ) : l3.intersection( line );

        return ( ( line.c1( ).closer_than( c1, c2 ) ) ? Line( c1, c2 ) : Line( c2, c1 ) );
    }

    virtual Triangle & transform( Transform cref t ) override { m_c1 = t.apply( m_c1 ); m_c2 = t.apply( m_c2 ); m_c3 = t.apply( m_c3 ); rethis; }

    Triangle operator+( Vector cref v ) const { return Triangle( c1( ) + v, c2( ) + v, c3( ) + v ); }
    Triangle operator-( Vector cref v ) const { return Triangle( c1( ) - v, c2( ) - v, c3( ) - v ); }

    Triangle & operator+=( Vector cref v ) { rethis = *this + v; }
    Triangle & operator-=( Vector cref v ) { rethis = *this - v; }

    default_equal( Triangle );
    
};

} // namespace geometry
} // namespace axn

#endif /* Triangle_hpp */
