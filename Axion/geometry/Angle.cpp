#include "Angle.hpp"

#include "Vector.hpp"

using axn::geometry::Angle;

Angle::Angle( const double _radians, const bool _truncate )
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
    radians( atan2( (double)_c.y( ), (double)_c.x( ) ) );
}

Angle::Angle( const Coordinate & _from, const Coordinate & _to, const bool _truncate )
{
    truncate( _truncate );
    radians( atan2( (double)( _to.y( ) - _from.y( ) ), (double)( _to.x( ) - _from.x( ) ) ) );
}

bool Angle::truncating( ) const { return m_truncate; }
Angle & Angle::truncate( const bool _truncate )
{
    m_truncate = _truncate;
    return *this;
}

Angle Angle::truncated( )
{
    return Angle( radians( ), true );
}

double Angle::radians( ) const { return m_radians; }
double Angle::degrees( ) const
{
#ifdef AXN_DEBUG
    return m_degrees;
#else
    return radians_to_degrees( m_radians );
#endif
}

Angle & Angle::radians( const Planc & _dx, const Planc & _dy )
{
    radians( atan2( (double)_dy, (double)_dx ) );
    return *this;
}
Angle & Angle::radians( const double _radians )
{
    double radians = _radians;

    if( is_infinity( _radians ) )
    {
        radians = 0.0;
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

double Angle::sin( const double _multiplier ) const { return ::sin( radians( ) ) * _multiplier; }
double Angle::cos( const double _multiplier ) const { return ::cos( radians( ) ) * _multiplier; }

Quadrant Angle::quadrant( ) const { return VectorA( *this, 1.0 ).destination( ).quadrant( ); }
bool Angle::in_quadrant( const Quadrant _quadrant ) const { return quadrant( ) == _quadrant; }

Axis Angle::axis( ) const { return VectorA( *this, 1.0 ).destination( ).axis( ); }
bool Angle::on_axis( const Axis _axis ) const { return axis( ) == _axis; }

Angle & Angle::flip( ) { return radians( radians( ) + PI ); }
Angle Angle::flipped( ) const { return Angle( *this + PI ); }

Angle Angle::half( ) const { return Angle( *this / 2.0 ); }
Angle & Angle::halve( ) { return radians( radians( ) / 2.0 ); }

Angle & Angle::operator=( const double _radians ) { return radians( _radians ); }

Angle Angle::operator-( ) const { return Angle( -radians( ), truncating( ) ); }

Angle Angle::operator+( const Angle & _angle ) const { return Angle( radians( ) + _angle.radians( ), truncating( ) ); }
Angle Angle::operator-( const Angle & _angle ) const { return Angle( radians( ) - _angle.radians( ), truncating( ) ); }

Angle & Angle::operator+=( const Angle & _angle ) { return radians( radians( ) + _angle.radians( ) ); }
Angle & Angle::operator-=( const Angle & _angle ) { return radians( radians( ) - _angle.radians( ) ); }

bool Angle::operator==( const Angle & _angle ) const { return double_eq( radians( ), _angle.radians( ) ); }
bool Angle::operator!=( const Angle & _angle ) const { return double_neq( radians( ), _angle.radians( ) ); }
bool Angle::operator>( const Angle & _angle ) const { return double_gt( radians( ), _angle.radians( ) ); }
bool Angle::operator>=( const Angle & _angle ) const { return double_ge( radians( ), _angle.radians( ) ); }
bool Angle::operator<( const Angle & _angle ) const { return double_lt( radians( ), _angle.radians( ) ); }
bool Angle::operator<=( const Angle & _angle ) const { return double_le( radians( ), _angle.radians( ) ); }

Angle Angle::operator+( const double _radians ) const { return Angle( radians( ) + _radians, truncating( ) ); }
Angle Angle::operator-( const double _radians ) const { return Angle( radians( ) - _radians, truncating( ) ); }

Angle & Angle::operator+=( const double _radians ) { return radians( radians( ) + _radians ); }
Angle & Angle::operator-=( const double _radians ) { return radians( radians( ) - _radians ); }

Angle Angle::operator*( const double _scale ) const { return Angle( radians( ) * _scale, truncating( ) ); }
Angle Angle::operator/( const double _scale ) const { return Angle( radians( ) / _scale, truncating( ) ); }

Angle & Angle::operator*=( const double _scale ) { return radians( radians( ) * _scale ); }
Angle & Angle::operator/=( const double _scale ) { return radians( radians( ) / _scale ); }

bool Angle::operator==( const double _radians ) const { return double_eq( radians( ), _radians ); }
bool Angle::operator!=( const double _radians ) const { return double_neq( radians( ), _radians ); }
bool Angle::operator<( const double _radians ) const { return double_lt( radians( ), _radians ); }
bool Angle::operator<=( const double _radians ) const { return double_le( radians( ), _radians ); }
bool Angle::operator>( const double _radians ) const { return double_gt( radians( ), _radians ); }
bool Angle::operator>=( const double _radians ) const { return double_ge( radians( ), _radians ); }

Angle Angle::operator+( const int _radians ) const { return *this + (double)_radians; }
Angle Angle::operator-( const int _radians ) const { return *this - (double)_radians; }
Angle Angle::operator*( const int _scale ) const { return *this * (double)_scale; }
Angle Angle::operator/( const int _scale ) const { return *this / (double)_scale; }

Angle & Angle::operator+=( const int _radians ) { return *this += (double)_radians; }
Angle & Angle::operator-=( const int _radians ) { return *this -= (double)_radians; }
Angle & Angle::operator*=( const int _scale ) { return *this *= (double)_scale; }
Angle & Angle::operator/=( const int _scale ) { return *this /= (double)_scale; }

bool Angle::operator==( const int _radians ) const { return double_eq( radians( ), (double)_radians ); }
bool Angle::operator!=( const int _radians ) const { return double_neq( radians( ), (double)_radians ); }
bool Angle::operator<( const int _radians ) const { return double_lt( radians( ), (double)_radians ); }
bool Angle::operator<=( const int _radians ) const { return double_le( radians( ), (double)_radians ); }
bool Angle::operator>( const int _radians ) const { return double_gt( radians( ), (double)_radians ); }
bool Angle::operator>=( const int _radians ) const { return double_ge( radians( ), (double)_radians ); }

Angle Angle::operator+( const uint _radians ) const { return *this + (double)_radians; }
Angle Angle::operator-( const uint _radians ) const { return *this - (double)_radians; }
Angle Angle::operator*( const uint _scale ) const { return *this * (double)_scale; }
Angle Angle::operator/( const uint _scale ) const { return *this / (double)_scale; }

Angle & Angle::operator+=( const uint _radians ) { return *this += (double)_radians; }
Angle & Angle::operator-=( const uint _radians ) { return *this -= (double)_radians; }
Angle & Angle::operator*=( const uint _scale ) { return *this *= (double)_scale; }
Angle & Angle::operator/=( const uint _scale ) { return *this /= (double)_scale; }

bool Angle::operator==( const uint _radians ) const { return double_eq( radians( ), (double)_radians ); }
bool Angle::operator!=( const uint _radians ) const { return double_neq( radians( ), (double)_radians ); }
bool Angle::operator<( const uint _radians ) const { return double_lt( radians( ), (double)_radians ); }
bool Angle::operator<=( const uint _radians ) const { return double_le( radians( ), (double)_radians ); }
bool Angle::operator>( const uint _radians ) const { return double_gt( radians( ), (double)_radians ); }
bool Angle::operator>=( const uint _radians ) const { return double_ge( radians( ), (double)_radians ); }

double Angle::radians_to_degrees( const double _radians ) { return _radians * 360.0 / TAU; }
double Angle::degrees_to_radians( const double _degrees ) { return _degrees * TAU / 360.0; }
