#include "Angle.hpp"

#include "Vector.hpp"

AXN_NAMESPACES
using axn::geometry::Angle;

Angle::Angle( const float _radians, const bool _truncate ) {
    truncate( _truncate );
    radians( _radians );
}

Angle::Angle( const planc & _dx, const planc & _dy, const bool _truncate ) {
    truncate( _truncate );
    radians( atan2( (float)_dy, (float)_dx ) );
}

Angle::Angle( const Coordinate & _c, const bool _truncate ) {
    truncate( _truncate );
    radians( atan2( (float)_c.y( ), (float)_c.x( ) ) );
}

Angle::Angle( const Coordinate & _from, const Coordinate & _to, const bool _truncate ) {
    truncate( _truncate );
    radians( atan2( (float)( _to.y( ) - _from.y( ) ), (float)( _to.x( ) - _from.x( ) ) ) );
}

bool Angle::truncating( ) const { return m_truncate; }
Angle & Angle::truncate( const bool _truncate ) {
    m_truncate = _truncate;
    return *this;
}

float Angle::radians( ) const { return m_radians; }
float Angle::degrees( ) const { return ( radians( ) / PI ) * 180.f; }

Angle & Angle::radians( const planc & _dx, const planc & _dy ) {
    radians( atan2( (float)_dy, (float)_dx ) );
    return *this;
}
Angle & Angle::radians( const float _radians ) {
    float radians = _radians;
    if( is_infinity( _radians ) )
        radians = 0.f;
    if( truncating( ) ) {
        radians -= floor( radians / PI_2 ) * PI_2;
    }
    m_radians = radians;
#ifdef AXN_DEBUG
    m_degrees = radians_to_degrees( radians );
#endif
    return *this;
}

float Angle::sin( const float _multiplier ) const { return ::sin( radians( ) ) * _multiplier; }
float Angle::cos( const float _multiplier ) const { return ::cos( radians( ) ) * _multiplier; }

Quadrant Angle::quadrant( ) const { return VectorA( *this, 1.f ).destination( ).quadrant( ); }
bool Angle::in_quadrant( const Quadrant _quadrant ) const { return quadrant( ) == _quadrant; }

Axis Angle::axis( ) const { return VectorA( *this, 1.f ).destination( ).axis( ); }
bool Angle::on_axis( const Axis _axis ) const { return axis( ) == _axis; }

Angle & Angle::operator=( const float _radians ) {
    radians( _radians );
    return *this;
}

Angle Angle::operator-( ) const { return Angle( -radians( ), truncating( ) ); }

Angle Angle::operator+( const Angle & _angle ) const {
    return Angle( radians( ) + _angle.radians( ) );
}
Angle Angle::operator-( const Angle & _angle ) const {
    return Angle( radians( ) - _angle.radians( ) );
}

Angle & Angle::operator+=( const Angle & _angle ) {
    radians( radians( ) + _angle.radians( ) );
    return *this;
}
Angle & Angle::operator-=( const Angle & _angle ) {
    radians( radians( ) - _angle.radians( ) );
    return *this;
}

Angle Angle::operator+( const float _radians ) const { return Angle( radians( ) + _radians ); }
Angle Angle::operator-( const float _radians ) const { return Angle( radians( ) - _radians ); }

Angle & Angle::operator+=( const float _radians ) {
    radians( radians( ) + _radians );
    return *this;
}
Angle & Angle::operator-=( const float _radians ) {
    radians( radians( ) - _radians );
    return *this;
}

Angle Angle::operator*( const float _scale ) const { return Angle( radians( ) * _scale ); }
Angle Angle::operator/( const float _scale ) const { return Angle( radians( ) / _scale ); }

Angle & Angle::operator*=( const float _scale ) {
    radians( radians( ) * _scale );
    return *this;
}
Angle & Angle::operator/=( const float _scale ) {
    radians( radians( ) / _scale );
    return *this;
}

bool Angle::operator==( const Angle & _angle ) const {
    return float_eq( radians( ), _angle.radians( ) );
}
bool Angle::operator!=( const Angle & _angle ) const { return !( *this == _angle ); }
bool Angle::operator>( const Angle & _angle ) const {
    return float_gt( radians( ), _angle.radians( ) );
}
bool Angle::operator>=( const Angle & _angle ) const {
    return float_ge( radians( ), _angle.radians( ) );
}
bool Angle::operator<( const Angle & _angle ) const {
    return float_lt( radians( ), _angle.radians( ) );
}
bool Angle::operator<=( const Angle & _angle ) const {
    return float_le( radians( ), _angle.radians( ) );
}

float Angle::radians_to_degrees( const float _radians ) { return _radians * 180.f / PI; }
float Angle::degrees_to_radians( const float _degrees ) { return _degrees * PI / 180.f; }
