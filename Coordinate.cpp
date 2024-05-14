#include "Coordinate.hpp"

#include "Angle.hpp"
#include "Vector.hpp"
#include "Line.hpp"

Coordinate::Coordinate( const Vector & _v ) { *this = _v.destination( ); }
Coordinate::Coordinate( const Planc & _x, const Planc & _y )
{
    xy( _x, _y );
}

Planc Coordinate::x( ) const { return m_x; }
Coordinate & Coordinate::x( const Planc & _x )
{
    m_x = _x;
    return *this;
}

Planc Coordinate::y( ) const { return m_y; }
Coordinate & Coordinate::y( const Planc & _y )
{
    m_y = _y;
    return *this;
}

Coordinate & Coordinate::xy( const Planc & _x, const Planc & _y )
{
    x( _x );
    y( _y );
    return *this;
}

Planc Coordinate::distance( const Coordinate & c ) const
{
    Planc dx = x( ) - c.x( );
    Planc dy = y( ) - c.y( );
    if( dx == 0 )
        return abs( dy );
    else if( dy == 0 )
        return abs( dx );
    return sqrt( ( dx * dx ) + ( dy * dy ) );
}

Coordinate & Coordinate::rotate( const Angle & _angle, const Coordinate & _origin )
{
    return *this = Vector( _origin, *this ).rotate( _angle ).destination( );
}

Coordinate & Coordinate::mirror_x( ) { return mirror( X_HAT ); }
Coordinate & Coordinate::mirror_y( ) { return mirror( Y_HAT ); }
Coordinate & Coordinate::mirror( const Vector & _axis ) { return mirror( Line( _axis.origin( ), _axis.destination( ) ) ); }
Coordinate & Coordinate::mirror( const Line & _axis )
{
    xy( x( ) - _axis.c1( ).x( ), y( ) - _axis.c1( ).y( ) );
    rotate( -_axis.angle( ) );
    y( -y( ) );
    rotate( _axis.angle( ) );
    xy( x( ) + _axis.c1( ).x( ), y( ) + _axis.c1( ).y( ) );
    return *this;
}

Quadrant Coordinate::quadrant( ) const
{
    if( axis( ) != No_Axis )
        return No_Quadrant;

    bool x_positive = double_gt( this->x( ), 0 );
    bool y_positive = double_gt( this->y( ), 0 );

    if( x_positive && y_positive )
        return Q1;
    else if( x_positive && !y_positive )
        return Q4;
    else if( !x_positive && y_positive )
        return Q2;
    else
        return Q3;
}

Axis Coordinate::axis( ) const
{
    bool x_nonzero = x( );
    bool y_nonzero = y( );

    if( x_nonzero && y_nonzero )
        return No_Axis;
    else if( x_nonzero && !y_nonzero )
        return X_Axis;
    else if( !x_nonzero && y_nonzero )
        return Y_Axis;
    else
        return Origin;
}

bool Coordinate::in_quadrant( const Quadrant & _quadrant ) const { return quadrant( ) == _quadrant; }
bool Coordinate::on_axis( const Axis & _axis ) const { return axis( ) == _axis; }

Coordinate Coordinate::operator+( const Vector & _v ) const { return Coordinate( x( ) + _v.dx( ), y( ) + _v.dy( ) ); }
Coordinate Coordinate::operator-( const Vector & _v ) const { return Coordinate( x( ) - _v.dx( ), y( ) - _v.dy( ) ); }

Coordinate & Coordinate::operator+=( const Vector & _v )
{
    x( x( ) + _v.dx( ) );
    y( y( ) + _v.dy( ) );
    return *this;
}
Coordinate & Coordinate::operator-=( const Vector & _v )
{
    x( x( ) - _v.dx( ) );
    y( y( ) - _v.dy( ) );
    return *this;
}

bool Coordinate::operator==( const Coordinate & c ) const { return ( ( x( ) == c.x( ) ) && ( y( ) == c.y( ) ) ); }
bool Coordinate::operator!=( const Coordinate & c ) const { return !( *this == c ); }
