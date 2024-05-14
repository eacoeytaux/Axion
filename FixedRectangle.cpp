#include "FixedRectangle.hpp"

FixedRectangle::FixedRectangle( ) : m_width( 0.0 ), m_height( 0.0 ), m_center( ORIGIN ) { }

FixedRectangle::FixedRectangle( const Planc & _width, const Planc & _height, const Coordinate & _center )
    : m_center( _center ), m_width( _width ), m_height( _height )
{
}

FixedRectangle::FixedRectangle( const Coordinate & _bottom, const Coordinate & _top )
    : m_center( Vector( _bottom, _top ).half( ).destination( ) ), m_width( _top.x( ) - _bottom.x( ) ), m_height( _top.y( ) - _bottom.y( ) )
{
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
    m_width = _width;
    return *this;
}

const Planc & FixedRectangle::height( ) const { return m_height; }
FixedRectangle & FixedRectangle::height( const Planc & _height )
{
    m_height = _height;
    return *this;
}

Coordinate FixedRectangle::top_left( ) const
{
    return center( ) + half( Vector( -width( ), height( ) ) );
}

Coordinate FixedRectangle::top_right( ) const
{
    return center( ) + half( Vector( width( ), height( ) ) );
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
    return center( ).x( ) + width( ).half( );
}

Planc FixedRectangle::upper_bound_y( ) const
{
    return center( ).y( ) + height( ).half( );
}

Planc FixedRectangle::lower_bound_x( ) const
{
    return center( ).x( ) - width( ).half( );
}

Planc FixedRectangle::lower_bound_y( ) const
{
    return center( ).y( ) + height( ).half( );
}

bool FixedRectangle::contains( const Coordinate & _c ) const
{
    return ( in_range( _c.x( ), lower_bound_x( ), upper_bound_x( ), true ) && in_range( _c.y( ), lower_bound_y( ), upper_bound_y( ), true ) );
}

varray<Line> FixedRectangle::intersection( const Line & _line ) const
{
    return ( ( Polygon ) * this ).intersection( _line );
}

Planc FixedRectangle::area( ) const
{
    return width( ) * height( );
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
    return ( ( center( ) == _rect.center( ) ) &&
             ( width( ) == _rect.width( ) ) &&
             ( height( ) == _rect.height( ) ) );
}

FixedRectangle::operator Polygon( ) const
{
    return Polygon( { top_right( ), top_left( ), bottom_left( ), bottom_right( ) } );
}
