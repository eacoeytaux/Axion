#include "Angle.hpp"
#include "Vector.hpp"

Angle::Angle( const dec _radians, const bool _truncate )
{
    truncate( _truncate );
    radians( _radians );
}

Angle::Angle( const Planc & _dx, const Planc & _dy, const bool _truncate )
{
    truncate( _truncate );
    radians( atan2( _dy, _dx ) );
}

Angle::Angle( const Coordinate & _c, const bool _truncate )
{
    truncate( _truncate );
    radians( atan2( (dec)_c.y( ), (dec)_c.x( ) ) );
}

Angle::Angle( const Coordinate & _from, const Coordinate & _to, const bool _truncate )
{
    truncate( _truncate );
    radians( atan2( (dec)( _to.y( ) - _from.y( ) ), (dec)( _to.x( ) - _from.x( ) ) ) );
}

bool Angle::truncating( ) const
{
    return m_truncate;
}
Angle & Angle::truncate( const bool _truncate )
{
    m_truncate = _truncate;
    return *this;
}

Angle Angle::truncated( )
{
    return Angle( radians( ), true );
}

dec Angle::radians( ) const
{
    return m_radians;
}
dec Angle::degrees( ) const
{
    #ifdef AXN_DEBUG
    return m_degrees;
    #else
    return radians_to_degrees( m_radians );
    #endif
}

Angle & Angle::radians( const Planc & _dx, const Planc & _dy )
{
    radians( atan2( (dec)_dy, (dec)_dx ) );
    return *this;
}
Angle & Angle::radians( const dec _radians )
{
    dec radians = _radians;

    if( is_infinity( _radians ) )
    {
        radians = ZERO;
    }
    if( truncating( ) )
    {
        radians -= floor( radians / TAU ) * TAU;
    }

    m_radians = radians;
    #ifdef AXN_DEBUG
    m_degrees = radians_to_degrees( radians );
    #endif
    return *this;
}

dec Angle::sin( const dec _multiplier ) const
{
    return ::sin( radians( ) ) * _multiplier;
}
dec Angle::cos( const dec _multiplier ) const
{
    return ::cos( radians( ) ) * _multiplier;
}

Angle & Angle::flip( )
{
    return radians( radians( ) + PI );
}
Angle Angle::flipped( ) const
{
    return Angle( *this + PI );
}

Angle Angle::half( ) const
{
    return Angle( ::half( radians( ) ) );
}

Quadrant Angle::quadrant( ) const
{
    return VectorA( *this, 1.0 ).destination( ).quadrant( );
}
bool Angle::in_quadrant( const Quadrant _quadrant ) const
{
    return quadrant( ) == _quadrant;
}

Axis Angle::axis( ) const
{
    return VectorA( *this, 1.0 ).destination( ).axis( );
}
bool Angle::on_axis( const Axis _axis ) const
{
    return axis( ) == _axis;
}

bool Angle::right( ) const
{
    return ( dec_eq( radians( ), RIGHT_ANGLE ) );
}

bool Angle::straight( ) const
{
    return ( dec_eq( radians( ), PI ) );
}

bool Angle::acute( ) const
{
    return ( is_positive( radians( ) ) && dec_lt( radians( ), RIGHT_ANGLE ) );
}

bool Angle::obtuse( ) const
{
    return ( is_positive( radians( ) ) && dec_gt( radians( ), RIGHT_ANGLE ) && dec_lt( radians( ), PI ) );
}

bool Angle::reflex( ) const
{
    return ( is_positive( radians( ) ) && dec_gt( radians( ), PI ) && dec_lt( radians( ), TAU ) );
}

Angle & Angle::operator=( const dec _radians )
{
    return radians( _radians );
}

Angle Angle::operator-( ) const
{
    return Angle( -radians( ), truncating( ) );
}

Angle Angle::operator+( const Angle & _angle ) const
{
    return Angle( radians( ) + _angle.radians( ), truncating( ) );
}
Angle Angle::operator-( const Angle & _angle ) const
{
    return Angle( radians( ) - _angle.radians( ), truncating( ) );
}

Angle & Angle::operator+=( const Angle & _angle )
{
    return radians( radians( ) + _angle.radians( ) );
}
Angle & Angle::operator-=( const Angle & _angle )
{
    return radians( radians( ) - _angle.radians( ) );
}

bool Angle::operator==( const Angle & _angle ) const
{
    return dec_eq( radians( ), _angle.radians( ) );
}
bool Angle::operator!=( const Angle & _angle ) const
{
    return dec_neq( radians( ), _angle.radians( ) );
}
bool Angle::operator<=( const Angle & _angle ) const
{
    return dec_le( radians( ), _angle.radians( ) );
}
bool Angle::operator>=( const Angle & _angle ) const
{
    return dec_ge( radians( ), _angle.radians( ) );
}
bool Angle::operator<( const Angle & _angle ) const
{
    return dec_lt( radians( ), _angle.radians( ) );
}
bool Angle::operator>( const Angle & _angle ) const
{
    return dec_gt( radians( ), _angle.radians( ) );
}

Angle Angle::operator+( const dec _radians ) const
{
    return Angle( radians( ) + _radians, truncating( ) );
}
Angle Angle::operator-( const dec _radians ) const
{
    return Angle( radians( ) - _radians, truncating( ) );
}

Angle & Angle::operator+=( const dec _radians )
{
    return radians( radians( ) + _radians );
}
Angle & Angle::operator-=( const dec _radians )
{
    return radians( radians( ) - _radians );
}

Angle Angle::operator*( const dec _scale ) const
{
    return Angle( radians( ) * _scale, truncating( ) );
}
Angle Angle::operator/( const dec _scale ) const
{
    return Angle( radians( ) / _scale, truncating( ) );
}

Angle & Angle::operator*=( const dec _scale )
{
    return radians( radians( ) * _scale );
}
Angle & Angle::operator/=( const dec _scale )
{
    return radians( radians( ) / _scale );
}

bool Angle::operator==( const dec _radians ) const
{
    return dec_eq( radians( ), _radians );
}
bool Angle::operator!=( const dec _radians ) const
{
    return dec_neq( radians( ), _radians );
}
bool Angle::operator<=( const dec _radians ) const
{
    return dec_le( radians( ), _radians );
}
bool Angle::operator>=( const dec _radians ) const
{
    return dec_ge( radians( ), _radians );
}
bool Angle::operator<( const dec _radians ) const
{
    return dec_lt( radians( ), _radians );
}
bool Angle::operator>( const dec _radians ) const
{
    return dec_gt( radians( ), _radians );
}

Angle Angle::operator+( const int _radians ) const
{
    return *this + (dec)_radians;
}
Angle Angle::operator-( const int _radians ) const
{
    return *this - (dec)_radians;
}
Angle Angle::operator*( const int _scale ) const
{
    return *this * (dec)_scale;
}
Angle Angle::operator/( const int _scale ) const
{
    return *this / (dec)_scale;
}

Angle & Angle::operator+=( const int _radians )
{
    return *this += (dec)_radians;
}
Angle & Angle::operator-=( const int _radians )
{
    return *this -= (dec)_radians;
}
Angle & Angle::operator*=( const int _scale )
{
    return *this *= (dec)_scale;
}
Angle & Angle::operator/=( const int _scale )
{
    return *this /= (dec)_scale;
}

bool Angle::operator==( const int _radians ) const
{
    return dec_eq( radians( ), (dec)_radians );
}
bool Angle::operator!=( const int _radians ) const
{
    return dec_neq( radians( ), (dec)_radians );
}
bool Angle::operator<=( const int _radians ) const
{
    return dec_le( radians( ), (dec)_radians );
}
bool Angle::operator>=( const int _radians ) const
{
    return dec_ge( radians( ), (dec)_radians );
}
bool Angle::operator<( const int _radians ) const
{
    return dec_lt( radians( ), (dec)_radians );
}
bool Angle::operator>( const int _radians ) const
{
    return dec_gt( radians( ), (dec)_radians );
}

Angle Angle::operator+( const uint _radians ) const
{
    return *this + (dec)_radians;
}
Angle Angle::operator-( const uint _radians ) const
{
    return *this - (dec)_radians;
}
Angle Angle::operator*( const uint _scale ) const
{
    return *this * (dec)_scale;
}
Angle Angle::operator/( const uint _scale ) const
{
    return *this / (dec)_scale;
}

Angle & Angle::operator+=( const uint _radians )
{
    return *this += (dec)_radians;
}
Angle & Angle::operator-=( const uint _radians )
{
    return *this -= (dec)_radians;
}
Angle & Angle::operator*=( const uint _scale )
{
    return *this *= (dec)_scale;
}
Angle & Angle::operator/=( const uint _scale )
{
    return *this /= (dec)_scale;
}

bool Angle::operator==( const uint _radians ) const
{
    return dec_eq( radians( ), (dec)_radians );
}
bool Angle::operator!=( const uint _radians ) const
{
    return dec_neq( radians( ), (dec)_radians );
}
bool Angle::operator<=( const uint _radians ) const
{
    return dec_le( radians( ), (dec)_radians );
}
bool Angle::operator>=( const uint _radians ) const
{
    return dec_ge( radians( ), (dec)_radians );
}
bool Angle::operator<( const uint _radians ) const
{
    return dec_lt( radians( ), (dec)_radians );
}
bool Angle::operator>( const uint _radians ) const
{
    return dec_gt( radians( ), (dec)_radians );
}

dec Angle::radians_to_degrees( const dec _radians )
{
    return _radians * 360.0 / TAU;
}
dec Angle::degrees_to_radians( const dec _degrees )
{
    return _degrees * TAU / 360.0;
}
