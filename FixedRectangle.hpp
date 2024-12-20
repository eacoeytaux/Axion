#ifndef FixedRectangle_hpp
#define FixedRectangle_hpp

#include "Planc.hpp"
#include "Coordinate.hpp"
#include "Angle.hpp"
#include "Vector.hpp"
#include "Transform.hpp"
#include "Line.hpp"
#include "Path.hpp"
#include "Polygon.hpp"

namespace axn
{
namespace geometry
{

class FixedRectangle
{
private:
    Planc m_width = P0;
    Planc m_height = P0;

    Coordinate m_center = ORIGIN;

public:
    FixedRectangle( ) { }

    FixedRectangle( const Planc & width, const Planc & height ) { FixedRectangle::width( width ); FixedRectangle::height( height ); }
    FixedRectangle( const Planc & width, const Planc & height, const Coordinate & center ) { FixedRectangle::width( width ); FixedRectangle::height( height ); FixedRectangle::center( center ); }

    FixedRectangle( const Coordinate & bottom, const Coordinate & top ) { FixedRectangle::width( abs( top.x( ) - bottom.x( ) ) ); FixedRectangle::height( abs( top.y( ) - bottom.y( ) ) ); FixedRectangle::center( midpoint( bottom, top ) ); }

    static FixedRectangle bounds( const Polygon & p, bool tight = false )
    {
        Coordinate top( p.upper_bound_x( tight ), p.upper_bound_y( tight ) );
        Coordinate bottom( p.lower_bound_x( tight ), p.lower_bound_y( tight ) );

        return FixedRectangle( abs( top.x( ) - bottom.x( ) ), abs( top.y( ) - bottom.y( ) ), midpoint( bottom, top ) );
    }

    const Coordinate & center( ) const { return m_center; }
    FixedRectangle & center( const Coordinate & center )
    {
        m_center = center;
        rethis;
    }

    const Planc & width( ) const { return m_width; }
    FixedRectangle & width( const Planc & width )
    {
        Assert( !is_neg( width ), "cannot have negative width" );
        m_width = width;
        rethis;
    }

    const Planc & height( ) const { return m_height; }
    FixedRectangle & height( const Planc & height )
    {
        Assert( !is_neg( height ), "cannot have negative height" );
        m_height = height;
        rethis;
    }

    Coordinate top_right( ) const { return center( ) + half( Vector( width( ), height( ) ) ); }
    Coordinate top_left( ) const { return center( ) + half( Vector( -width( ), height( ) ) ); }
    Coordinate bottom_left( ) const { return center( ) + half( Vector( -width( ), -height( ) ) ); }
    Coordinate bottom_right( ) const { return center( ) + half( Vector( width( ), -height( ) ) ); }

    Coordinate top( ) const { return top_right( ); }
    Coordinate bottom( ) const { return bottom_left( ); }

    Planc upper_bound_x( ) const { return center( ).x( ) + half( width( ) ); }
    Planc upper_bound_y( ) const { return center( ).y( ) + half( height( ) ); }

    Planc lower_bound_x( ) const { return center( ).x( ) - half( width( ) ); }
    Planc lower_bound_y( ) const { return center( ).y( ) - half( height( ) ); }

    bool contains( const Coordinate & c, const bool inclusive = true ) const { return ( in_range( c.x( ), lower_bound_x( ), upper_bound_x( ), inclusive ) && in_range( c.y( ), lower_bound_y( ), upper_bound_y( ), inclusive ) ); }

    bool intersects( const Line & line ) const { return ( intersection( line ).size( ) > 0 ); }
    varray<Line> intersection( const Line & line ) const
    {
        if( width( ) && height( ) )
        {
            return Polygon( *this ).intersection( line );
        }
        else
        {
            if( width( ) )
            {
                Line line( center( ) - Vector::X( half( width( ) ) ), center( ) + Vector::X( half( width( ) ) ) );
                if( line.intersects( line ) )
                {
                    Coordinate c = line.intersection( line );
                    return { Line( c, c ) };
                }
            }
            else if( height( ) )
            {
                Line line( center( ) - Vector::Y( half( height( ) ) ), center( ) + Vector::Y( half( height( ) ) ) );
                if( line.intersects( line ) )
                {
                    Coordinate c = line.intersection( line );
                    return { Line( c, c ) };
                }
            }
            else
            {
                if( line.on( center( ) ) )
                {
                    return { Line( center( ), center( ) ) };
                }
            }
        }

        return { };
    }

    FixedRectangle & union_with( const FixedRectangle & rect )
    {
        rethis = FixedRectangle( Coordinate( min( lower_bound_x( ), rect.lower_bound_x( ) ),
                                             min( lower_bound_y( ), rect.lower_bound_y( ) ) ),
                                 Coordinate( max( upper_bound_x( ), rect.upper_bound_x( ) ),
                                             max( upper_bound_y( ), rect.upper_bound_y( ) ) ) );
    }

    FixedRectangle & intersection_with( const FixedRectangle & rect )
    {
        if( !has_intersection_with( rect ) )
        {
            Assert( "does not intersect" );
            rethis = FixedRectangle( );
        }

        rethis = FixedRectangle( Coordinate( max( lower_bound_x( ), rect.lower_bound_x( ) ),
                                             max( lower_bound_y( ), rect.lower_bound_y( ) ) ),
                                 Coordinate( min( upper_bound_x( ), rect.upper_bound_x( ) ),
                                             min( upper_bound_y( ), rect.upper_bound_y( ) ) ) );
    }

    bool has_intersection_with( const FixedRectangle & rect ) const
    {
        return ( ( lower_bound_x( ) < rect.upper_bound_x( ) ) &&
                 ( upper_bound_x( ) > rect.lower_bound_x( ) ) &&
                 ( upper_bound_y( ) > rect.lower_bound_y( ) ) &&
                 ( lower_bound_y( ) < rect.upper_bound_y( ) ) );
    }

    Planc area( ) const { return width( ) * height( ); }

    FixedRectangle & expand( const Planc & d_width_and_height ) { return expand( d_width_and_height, d_width_and_height ); }
    FixedRectangle & shrink( const Planc & d_width_and_height ) { return shrink( d_width_and_height, d_width_and_height ); }

    FixedRectangle & expand( const Planc & px, const Planc & py ) { expand_width( px ); expand_height( py ); rethis; }
    FixedRectangle & shrink( const Planc & px, const Planc & py ) { shrink_width( px ); shrink_height( py ); rethis; }

    FixedRectangle & expand_width( const Planc & p )
    {
        Assert( !is_neg( p ), "cannot expand by negative amount, use shrink instead" );
        return width( width( ) + p );
    }

    FixedRectangle & shrink_width( const Planc & p )
    {
        Assert( !is_neg( p ), "cannot shrink by negative amount, use expand instead" );
        Assert( p >= width( ), "cannot shrink by more than current width" );
        return width( width( ) - p );
    }

    FixedRectangle & expand_height( const Planc & p )
    {
        Assert( !is_neg( p ), "cannot expand by negative amount, use shrink instead" );
        return height( height( ) + p );
    }

    FixedRectangle & shrink_height( const Planc & p )
    {
        Assert( !is_neg( p ), "cannot shrink by negative amount, use expand instead" );
        Assert( p >= height( ), "cannot shrink by more than current height" );
        return height( height( ) - p );
    }

    FixedRectangle operator+( const Vector & v ) const { return FixedRectangle( width( ), height( ), center( ) + v ); }
    FixedRectangle operator-( const Vector & v ) const { return FixedRectangle( width( ), height( ), center( ) - v ); }

    FixedRectangle & operator+=( const Vector & v ) { return center( center( ) + v ); }
    FixedRectangle & operator-=( const Vector & v ) { return center( center( ) - v ); }

    operator Polygon( ) const { return Polygon::rectangle( width( ), height( ), center( ) ); }

    default_equal( FixedRectangle );
};

} // namespace geometry
} // namespace axn

#endif /* FixedRectangle_hpp */
