#include "Vector.hpp"

Vector::Vector( const Planc & _dx, const Planc & _dy, const Coordinate & _c )
{
    origin( _c );
    dx( _dx );
    dy( _dy );
}

Vector::Vector( const Coordinate & _c )
{
    origin( ORIGIN );
    dx( _c.x( ) );
    dy( _c.y( ) );
}

Vector::Vector( const Coordinate & _c1, const Coordinate & _c2 )
{
    origin( _c1 );
    dx( _c2.x( ) - _c1.x( ) );
    dy( _c2.y( ) - _c1.y( ) );
}

VectorX::VectorX( const Planc & _dx, const Coordinate & _origin ) : Vector( _dx, ZERO, _origin ) { }
VectorY::VectorY( const Planc & _dy, const Coordinate & _origin ) : Vector( ZERO, _dy, _origin ) { }
VectorA::VectorA( const Angle & _angle, const Planc & _magnitude, const Coordinate & _origin )
{
    origin( _origin );

    Angle angle = _angle;
    Planc magnitude = _magnitude;

    if( is_negative( magnitude ) )
    {
        magnitude *= -1;
        angle += PI;
    }

    dx( angle.cos( magnitude ) );
    dy( angle.sin( magnitude ) );
}

VectorA::VectorA( const Angle & _angle, const Coordinate & _origin )
{
    origin( _origin );
    dx( _angle.cos( ) );
    dy( _angle.sin( ) );
}

Planc Vector::dx( ) const { return m_dx; }
Vector & Vector::dx( const Planc & _dx )
{
    m_dx = _dx;
    return *this;
}

Planc Vector::dy( ) const { return m_dy; }
Vector & Vector::dy( const Planc & _dy )
{
    m_dy = _dy;
    return *this;
}

Vector & Vector::dxdy( const Planc & _dx, const Planc & _dy )
{
    dx( _dx );
    dy( _dy );
    return *this;
}

Coordinate Vector::origin( ) const { return m_origin; }
Vector & Vector::origin( const Coordinate & _origin )
{
    m_origin = _origin;
    return *this;
}

Coordinate Vector::destination( ) const
{
    Coordinate destination = origin( );
    destination.x( destination.x( ) + m_dx );
    destination.y( destination.y( ) + m_dy );
    return destination;
}

Vector & Vector::destination( Coordinate _destination )
{
    dx( _destination.x( ) - origin( ).x( ) );
    dy( _destination.y( ) - origin( ).y( ) );
    return *this;
}

bool Vector::has_magnitude( ) const
{
    return ( dx( ) || dy( ) );
}

Planc Vector::magnitude( ) const
{
    if( !dx( ) && !dy( ) )
    {
        return ZERO;
    }

    return origin( ).distance_to( destination( ) );
}

Vector & Vector::magnitude( const Planc & _magnitude )
{
    Angle a = angle( );
    dx( a.cos( _magnitude ) );
    dy( a.sin( _magnitude ) );
    return *this;
}

Vector & Vector::extend( const Planc & _length )
{
    return magnitude( magnitude( ) + _length );
}

Vector & Vector::normalize( )
{
    return magnitude( ONE );
}

Vector Vector::half( ) const
{
    return *this / TWO;
}

Angle Vector::angle( ) const { return Angle( m_dx, m_dy ); }
Vector & Vector::rotate( const Angle & _d_angle ) { return rotate_to_angle( angle( ) + _d_angle ); }
Vector & Vector::rotate_to_angle( const Angle & _angle )
{
    if( angle( ) != _angle )
    {
        Planc _magnitude = magnitude( );
        dx( _magnitude * _angle.cos( ) );
        dy( _magnitude * _angle.sin( ) );
    }
    return *this;
}

Vector & Vector::flatten( const Angle & a )
{
    rotate( -a );
    dy( ZERO );
    rotate( a );
    return *this;
}

Vector Vector::operator-( ) const { return Vector( origin( ), Coordinate( origin( ).x( ) - m_dx, origin( ).y( ) - m_dy ) ); }

Vector Vector::operator+( const Vector & _v ) const { return Vector( origin( ), Coordinate( origin( ).x( ) + ( m_dx + _v.dx( ) ), origin( ).y( ) + ( m_dy + _v.dy( ) ) ) ); }

Vector Vector::operator-( const Vector & _v ) const { return Vector( origin( ), Coordinate( origin( ).x( ) + ( m_dx - _v.dx( ) ), origin( ).y( ) + ( m_dy - _v.dy( ) ) ) ); }

Vector & Vector::operator+=( const Vector & _v )
{
    dx( m_dx + _v.m_dx );
    dy( m_dy + _v.m_dy );
    return *this;
}

Vector & Vector::operator-=( const Vector & _v )
{
    dx( m_dx - _v.m_dx );
    dy( m_dy - _v.m_dy );
    return *this;
}

Vector & Vector::operator*=( const dec _scale )
{
    dx( m_dx * _scale );
    dy( m_dy * _scale );
    return *this;
}

Vector & Vector::operator/=( const dec _scale )
{
    dx( m_dx / _scale );
    dy( m_dy / _scale );
    return *this;
}

Vector Vector::operator*( const dec _scale ) const { return Vector( origin( ), Coordinate( origin( ).x( ) + ( m_dx * _scale ), origin( ).y( ) + ( m_dy * _scale ) ) ); }

Vector Vector::operator/( const dec _scale ) const { return Vector( origin( ), Coordinate( origin( ).x( ) + ( m_dx / _scale ), origin( ).y( ) + ( m_dy / _scale ) ) ); }

Vector & Vector::operator=( const Coordinate & _coordinate )
{
    origin( ORIGIN );
    dx( _coordinate.x( ) );
    dy( _coordinate.y( ) );
    return *this;
}
