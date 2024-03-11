#include "Vector.hpp"

AXN_NAMESPACES
using axn::geometry::Vector;

Vector::Vector( const planc & _dx, const planc & _dy, const Coordinate & _c ) {
    origin( _c );
    dx( _dx );
    dy( _dy );
}

Vector::Vector( const Coordinate & _c ) {
    origin( ORIGIN );
    dx( _c.x( ) );
    dy( _c.y( ) );
}

Vector::Vector( const Coordinate & _c1, const Coordinate & _c2 ) {
    origin( _c1 );
    dx( _c2.x( ) - _c1.x( ) );
    dy( _c2.y( ) - _c1.y( ) );
}

VectorA::VectorA( const Angle & _angle, const planc & _magnitude, const Coordinate & _origin ) {
    origin( _origin );

    Angle angle = _angle;
    planc magnitude = _magnitude;

    if( magnitude < planc( 0.f ) ) {
        magnitude *= -1;
        angle += PI;
    }

    dx( angle.cos( magnitude ) );
    dy( angle.sin( magnitude ) );
}

planc Vector::dx( ) const { return m_dx; }
Vector & Vector::dx( const planc & _dx ) {
    m_dx = _dx;
    return *this;
}
planc Vector::dy( ) const { return m_dy; }
Vector & Vector::dy( const planc & _dy ) {
    m_dy = _dy;
    return *this;
}
Vector & Vector::dxdy( const planc & _dx, const planc & _dy ) {
    dx( _dx );
    dy( _dy );
    return *this;
}

Coordinate Vector::origin( ) const { return m_origin; }
Vector & Vector::origin( const Coordinate & _origin ) {
    m_origin = _origin;
    return *this;
}

Coordinate Vector::destination( ) const {
    Coordinate destination = origin( );
    destination.x( destination.x( ) + m_dx );
    destination.y( destination.y( ) + m_dy );
    return destination;
}

Vector & Vector::destination( Coordinate _destination ) {
    dx( _destination.x( ) - origin( ).x( ) );
    dy( _destination.y( ) - origin( ).y( ) );
    return *this;
}

planc Vector::magnitude( ) const { return origin( ).distance( destination( ) ); }
Vector & Vector::magnitude( const planc & _magnitude ) {
    Angle a = angle( );
    dx( a.cos( _magnitude ) );
    dy( a.sin( _magnitude ) );
    return *this;
}

Vector & Vector::extend( const planc & _length ) { return magnitude( magnitude( ) + _length ); }

Vector & Vector::normalize( ) { return magnitude( 1.f ); }

Vector Vector::half( ) const { return *this / 2.f; }
Vector & Vector::halve( ) {
    m_dx.halve( );
    m_dy.halve( );
    return *this;
}

Angle Vector::angle( ) const { return Angle( m_dx, m_dy ); }
Vector & Vector::rotate( const Angle & _d_angle ) {
    return rotate_to_angle( angle( ) + _d_angle );
}
Vector & Vector::rotate_to_angle( const Angle & _angle ) {
    if( angle( ) != _angle ) {
        planc _magnitude = magnitude( );
        dx( _magnitude * _angle.cos( ) );
        dy( _magnitude * _angle.sin( ) );
    }
    return *this;
}
Vector Vector::operator-( ) const {
    return Vector( origin( ), Coordinate( origin( ).x( ) - m_dx, origin( ).y( ) - m_dy ) );
}

Vector Vector::operator+( const Vector & _v ) const {
    return Vector( origin( ), Coordinate( origin( ).x( ) + ( m_dx + _v.dx( ) ),
                                          origin( ).y( ) + ( m_dy + _v.dy( ) ) ) );
}
Vector Vector::operator-( const Vector & _v ) const {
    return Vector( origin( ), Coordinate( origin( ).x( ) + ( m_dx - _v.dx( ) ),
                                          origin( ).y( ) + ( m_dy - _v.dy( ) ) ) );
}

Vector & Vector::operator+=( const Vector & _v ) {
    dx( m_dx + _v.m_dx );
    dy( m_dy + _v.m_dy );
    return *this;
}
Vector & Vector::operator-=( const Vector & _v ) {
    dx( m_dx - _v.m_dx );
    dy( m_dy - _v.m_dy );
    return *this;
}
Vector & Vector::operator*=( const float _scale ) {
    dx( m_dx * _scale );
    dy( m_dy * _scale );
    return *this;
}
Vector & Vector::operator/=( const float _scale ) {
    dx( m_dx / _scale );
    dy( m_dy / _scale );
    return *this;
}

Vector Vector::operator*( const float _scale ) const {
    return Vector( origin( ), Coordinate( origin( ).x( ) + ( m_dx * _scale ),
                                          origin( ).y( ) + ( m_dy * _scale ) ) );
}
Vector Vector::operator/( const float _scale ) const {
    return Vector( origin( ), Coordinate( origin( ).x( ) + ( m_dx / _scale ),
                                          origin( ).y( ) + ( m_dy / _scale ) ) );
}

bool Vector::operator==( const Vector & _v ) const {
    return ( ( origin( ) == _v.origin( ) ) && ( m_dx == _v.m_dx ) && ( m_dy == _v.m_dy ) );
}
bool Vector::operator!=( const Vector & _v ) const { return !( *this == _v ); }
