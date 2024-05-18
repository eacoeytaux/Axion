#include "FixedRectangle.hpp"

FixedRectangle::FixedRectangle( )
{
    width( P0 );
    height( P0 );
    center( ORIGIN );
}

FixedRectangle::FixedRectangle( const Planc & _width, const Planc & _height, const Coordinate & _center )
{
    width( _width );
    height( _height );
    center( _center );
}

FixedRectangle::FixedRectangle( const Coordinate & _bottom, const Coordinate & _top )
{
    width( _top.x( ) - _bottom.x( ) );
    height( _top.y( ) - _bottom.y( ) );
    center( Vector( _bottom, _top ).half( ) );
}

const Coordinate & FixedRectangle::center( ) const { return m_center; }
FixedRectangle & FixedRectangle::center( const Coordinate & _center )
{
    m_center = _center;
    return *this;
}

const Planc & FixedRectangle::width( ) const { return m_width; }
FixedRectangle & FixedRectangle::width( const Planc & _width )
{
    Assert( _width >= 0.0, "cannot have negative width" );
    m_width = _width;
    return *this;
}

const Planc & FixedRectangle::height( ) const { return m_height; }
FixedRectangle & FixedRectangle::height( const Planc & _height )
{
    Assert( _height >= 0.0, "cannot have negative height" );
    m_height = _height;
    return *this;
}

Coordinate FixedRectangle::top_right( ) const
{
    return center( ) + half( Vector( width( ), height( ) ) );
}

Coordinate FixedRectangle::top_left( ) const
{
    return center( ) + half( Vector( -width( ), height( ) ) );
}

Coordinate FixedRectangle::bottom_left( ) const
{
    return center( ) + half( Vector( -width( ), -height( ) ) );
}

Coordinate FixedRectangle::bottom_right( ) const
{
    return center( ) + half( Vector( width( ), -height( ) ) );
}

Planc FixedRectangle::upper_bound_x( ) const
{
    return center( ).x( ) + half( width( ) );
}

Planc FixedRectangle::upper_bound_y( ) const
{
    return center( ).y( ) + half( height( ) );
}

Planc FixedRectangle::lower_bound_x( ) const
{
    return center( ).x( ) - half( width( ) );
}

Planc FixedRectangle::lower_bound_y( ) const
{
    return center( ).y( ) - half( height( ) );
}

bool FixedRectangle::contains( const Coordinate & _coordinate, const bool _inclusive ) const
{
    return ( in_range( _coordinate.x( ), lower_bound_x( ), upper_bound_x( ), _inclusive ) &&
             in_range( _coordinate.y( ), lower_bound_y( ), upper_bound_y( ), _inclusive ) );
}

bool FixedRectangle::intersects( const Line & _line ) const
{
    return ( intersection( _line ).size( ) > 0 );
}

varray<Line> FixedRectangle::intersection( const Line & _line ) const
{
    if( width( ) && height( ) )
    {
        return Polygon( *this ).intersection( _line );
    }
    else
    {
        if( width( ) )
        {
            Line line( center( ) - VectorX( half( width( ) ) ), center( ) + VectorX( half( width( ) ) ) );
            if( line.intersects( _line ) )
            {
                Coordinate c = line.intersection( _line );
                return { Line( c, c ) };
            }
        }
        else if( height( ) )
        {
            Line line( center( ) - VectorY( half( height( ) ) ), center( ) + VectorY( half( height( ) ) ) );
            if( line.intersects( _line ) )
            {
                Coordinate c = line.intersection( _line );
                return { Line( c, c ) };
            }
        }
        else
        {
            if( _line.on( center( ) ) )
            {
                return { Line( center( ), center( ) ) };
            }
        }
    }

    return { };
}

Planc FixedRectangle::area( ) const
{
    return width( ) * height( );
}

FixedRectangle & FixedRectangle::expand( const Planc & _px, const Planc & _py )
{
    expand_width( _px );
    expand_height( _py );
    return *this;
}

FixedRectangle & FixedRectangle::shrink( const Planc & _px, const Planc & _py )
{
    shrink_width( _px );
    shrink_height( _py );
    return *this;
}

FixedRectangle & FixedRectangle::expand_width( const Planc & _p )
{
    Assert( _p >= 0.0, "cannot expand by negative amount, use shrink instead" );
    width( width( ) + _p );
    return *this;
}

FixedRectangle & FixedRectangle::shrink_width( const Planc & _p )
{
    Assert( _p <= 0.0, "cannot shrink by negative amount, use expand instead" );
    Assert( _p >= width( ), "cannot shrink by more than current width" );
    width( width( ) - _p );
    return *this;
}

FixedRectangle & FixedRectangle::expand_height( const Planc & _p )
{
    Assert( _p >= 0.0, "cannot expand by negative amount, use shrink instead" );
    height( height( ) + _p );
    return *this;
}

FixedRectangle & FixedRectangle::shrink_height( const Planc & _p )
{
    Assert( _p <= 0.0, "cannot shrink by negative amount, use expand instead" );
    Assert( _p >= height( ), "cannot shrink by more than current height" );
    height( height( ) - _p );
    return *this;
}

FixedRectangle FixedRectangle::operator+( const Vector & _v ) const
{
    return FixedRectangle( width( ), height( ), center( ) + _v );
}

FixedRectangle & FixedRectangle::operator+=( const Vector & _v )
{
    return center( center( ) + _v );
}

FixedRectangle FixedRectangle::operator-( const Vector & _v ) const
{
    return FixedRectangle( width( ), height( ), center( ) - _v );
}

FixedRectangle & FixedRectangle::operator-=( const Vector & _v )
{
    return center( center( ) - _v );
}

bool FixedRectangle::operator==( const FixedRectangle & _rect ) const
{
    return ( ( width( ) == _rect.width( ) ) &&
             ( height( ) == _rect.height( ) ) &&
             ( center( ) == _rect.center( ) ) );
}

FixedRectangle::operator Polygon( ) const
{
    return Rectangle( width( ), height( ), center( ) );
}
