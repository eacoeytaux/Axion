#include "Transform.hpp"

using axn::geometry::Transform;

MoveTransform::MoveTransform( const Vector & _translation ) : Transform( true )
{
    set( 0, 2, _translation.dx( ) );
    set( 1, 2, _translation.dy( ) );
}

RotateTransform::RotateTransform( const Angle & _angle, const Coordinate & _origin ) : Transform( true )
{
    set( 0, 0, _angle.cos( ) );
    set( 0, 1, -_angle.sin( ) );
    set( 1, 0, _angle.sin( ) );
    set( 1, 1, _angle.cos( ) );

    if( _origin != ORIGIN )
    {
        Transform mt1 = MoveTransform( Vector( _origin ) );
        Transform mt2 = mt1.inverse( );

        set( mt1 * ( *this ) * mt2 );
    }
}

ReflectionTransform::ReflectionTransform( const Vector & _axis ) : Transform( true )
{
    set( 1, 1, -1.0 );

    Transform rt1 = RotateTransform( _axis.angle( ), Vector( _axis.origin( ) ) );
    Transform rt2 = rt1.inverse( );

    set( rt1 * ( *this ) * rt2 );
}

ScaleTransform::ScaleTransform( const Planc & _scale, const Coordinate & _origin ) : Transform( true )
{
    set( 0, 0, _scale );
    set( 1, 1, _scale );

    if( _origin != ORIGIN )
    {
        Transform mt1 = MoveTransform( Vector( _origin ) );
        Transform mt2 = mt1.inverse( );

        set( mt1 * ( *this ) * mt2 );
    }
}

StretchTransform::StretchTransform( const Vector & stretch ) : Transform( true )
{
    set( 0, 0, stretch.magnitude( ) );

    Transform rt1 = RotateTransform( stretch.angle( ), stretch.origin( ) );
    Transform rt2 = rt1.inverse( );

    set( rt1 * ( *this ) * rt2 );
}

Transform::Transform( bool identity )
{
    m_matrix[ 0 ][ 0 ] = m_matrix[ 1 ][ 1 ] = m_matrix[ 2 ][ 2 ] = identity ? 1.0 : 0.0;
    m_matrix[ 0 ][ 1 ] = m_matrix[ 0 ][ 2 ] = m_matrix[ 1 ][ 0 ] = m_matrix[ 1 ][ 2 ] = m_matrix[ 2 ][ 0 ] = m_matrix[ 2 ][ 1 ] = 0.0;
}

Planc Transform::get( const uint _i, const uint _j ) const
{
    Assert( in_range<uint>( _i, 2, true ) && in_range<uint>( _j, 2, true ), "index out of range" );
    return m_matrix[ _i ][ _j ];
}

Transform & Transform::set( const uint _i, const uint _j, const Planc _v )
{
    Assert( in_range<uint>( _i, 2, true ) && in_range<uint>( _j, 2, true ), "index out of range" );
    m_matrix[ _i ][ _j ] = _v;
    return *this;
}

Transform & Transform::set( const Transform & t )
{
    m_matrix[ 0 ][ 0 ] = t.m_matrix[ 0 ][ 0 ];
    m_matrix[ 0 ][ 1 ] = t.m_matrix[ 0 ][ 1 ];
    m_matrix[ 0 ][ 2 ] = t.m_matrix[ 0 ][ 2 ];
    m_matrix[ 1 ][ 0 ] = t.m_matrix[ 1 ][ 0 ];
    m_matrix[ 1 ][ 1 ] = t.m_matrix[ 1 ][ 1 ];
    m_matrix[ 1 ][ 2 ] = t.m_matrix[ 1 ][ 2 ];
    m_matrix[ 2 ][ 0 ] = t.m_matrix[ 2 ][ 0 ];
    m_matrix[ 2 ][ 1 ] = t.m_matrix[ 2 ][ 1 ];
    m_matrix[ 2 ][ 2 ] = t.m_matrix[ 2 ][ 2 ];
    return *this;
}

Coordinate Transform::transform( const Coordinate & coordinate ) const
{
    Planc x = coordinate.x( );
    Planc y = coordinate.y( );

    Planc xt = ( x * m_matrix[ 0 ][ 0 ] ) + ( y * m_matrix[ 0 ][ 1 ] ) + m_matrix[ 0 ][ 2 ];
    Planc yt = ( x * m_matrix[ 1 ][ 0 ] ) + ( y * m_matrix[ 1 ][ 1 ] ) + m_matrix[ 1 ][ 2 ];

    return Coordinate( xt, yt );
}

Transform & Transform::chain( const Transform & t )
{
    return set( t * ( *this ) );
}

bool Transform::identity( ) const
{
    if( m_matrix[ 0 ][ 0 ] != 1.0 )
        return false;
    if( m_matrix[ 0 ][ 1 ] )
        return false;
    if( m_matrix[ 0 ][ 2 ] )
        return false;
    if( m_matrix[ 1 ][ 0 ] )
        return false;
    if( m_matrix[ 1 ][ 1 ] != 1.0 )
        return false;
    if( m_matrix[ 1 ][ 2 ] )
        return false;
    if( m_matrix[ 2 ][ 0 ] )
        return false;
    if( m_matrix[ 2 ][ 1 ] )
        return false;
    if( m_matrix[ 2 ][ 2 ] != 1.0 )
        return false;
    return true;
}

bool Transform::null( ) const
{
    if( m_matrix[ 0 ][ 0 ] )
        return false;
    if( m_matrix[ 0 ][ 1 ] )
        return false;
    if( m_matrix[ 0 ][ 2 ] )
        return false;
    if( m_matrix[ 1 ][ 0 ] )
        return false;
    if( m_matrix[ 1 ][ 1 ] )
        return false;
    if( m_matrix[ 1 ][ 2 ] )
        return false;
    if( m_matrix[ 2 ][ 0 ] )
        return false;
    if( m_matrix[ 2 ][ 1 ] )
        return false;
    if( m_matrix[ 2 ][ 2 ] )
        return false;
    return true;
}

double Transform::determinant( ) const
{
    return ( m_matrix[ 0 ][ 0 ] * ( ( m_matrix[ 1 ][ 1 ] * m_matrix[ 2 ][ 2 ] ) - ( m_matrix[ 1 ][ 2 ] * m_matrix[ 2 ][ 1 ] ) ) ) -
           ( m_matrix[ 0 ][ 1 ] * ( ( m_matrix[ 1 ][ 0 ] * m_matrix[ 2 ][ 2 ] ) - ( m_matrix[ 1 ][ 2 ] * m_matrix[ 2 ][ 0 ] ) ) ) +
           ( m_matrix[ 0 ][ 2 ] * ( ( m_matrix[ 1 ][ 0 ] * m_matrix[ 2 ][ 1 ] ) - ( m_matrix[ 1 ][ 1 ] * m_matrix[ 2 ][ 0 ] ) ) );
}

inline double determinant2x2( Planc a, Planc b, Planc c, Planc d )
{
    return ( a * d ) - ( b * c );
}

Transform & Transform::invert( )
{
    return *this = inverse( );
}

Transform Transform::inverse( )
{
    double d = determinant( );
    if( !d )
        return Transform( false );
    d = 1.0 / d;

    Transform t;
    t.m_matrix[ 0 ][ 0 ] = determinant2x2( m_matrix[ 1 ][ 1 ], m_matrix[ 1 ][ 2 ], m_matrix[ 2 ][ 1 ], m_matrix[ 2 ][ 2 ] ) * d;
    t.m_matrix[ 0 ][ 1 ] = determinant2x2( m_matrix[ 0 ][ 2 ], m_matrix[ 0 ][ 1 ], m_matrix[ 2 ][ 2 ], m_matrix[ 2 ][ 1 ] ) * d;
    t.m_matrix[ 0 ][ 2 ] = determinant2x2( m_matrix[ 0 ][ 1 ], m_matrix[ 0 ][ 2 ], m_matrix[ 1 ][ 1 ], m_matrix[ 1 ][ 2 ] ) * d;
    t.m_matrix[ 1 ][ 0 ] = determinant2x2( m_matrix[ 1 ][ 2 ], m_matrix[ 1 ][ 0 ], m_matrix[ 2 ][ 2 ], m_matrix[ 2 ][ 0 ] ) * d;
    t.m_matrix[ 1 ][ 1 ] = determinant2x2( m_matrix[ 0 ][ 0 ], m_matrix[ 0 ][ 2 ], m_matrix[ 2 ][ 0 ], m_matrix[ 2 ][ 2 ] ) * d;
    t.m_matrix[ 1 ][ 2 ] = determinant2x2( m_matrix[ 0 ][ 2 ], m_matrix[ 0 ][ 0 ], m_matrix[ 1 ][ 2 ], m_matrix[ 1 ][ 0 ] ) * d;
    t.m_matrix[ 2 ][ 0 ] = determinant2x2( m_matrix[ 1 ][ 0 ], m_matrix[ 1 ][ 1 ], m_matrix[ 2 ][ 0 ], m_matrix[ 2 ][ 1 ] ) * d;
    t.m_matrix[ 2 ][ 1 ] = determinant2x2( m_matrix[ 0 ][ 1 ], m_matrix[ 0 ][ 0 ], m_matrix[ 2 ][ 1 ], m_matrix[ 2 ][ 0 ] ) * d;
    t.m_matrix[ 2 ][ 2 ] = determinant2x2( m_matrix[ 0 ][ 0 ], m_matrix[ 0 ][ 1 ], m_matrix[ 1 ][ 0 ], m_matrix[ 1 ][ 1 ] ) * d;
    return t;
}

Transform & Transform::operator=( const Transform & t )
{
    return set( t );
}

Transform Transform::operator+( double d ) const
{
    Transform t;
    t.m_matrix[ 0 ][ 0 ] = m_matrix[ 0 ][ 0 ] + d;
    t.m_matrix[ 0 ][ 1 ] = m_matrix[ 0 ][ 1 ] + d;
    t.m_matrix[ 0 ][ 2 ] = m_matrix[ 0 ][ 2 ] + d;
    t.m_matrix[ 1 ][ 0 ] = m_matrix[ 1 ][ 0 ] + d;
    t.m_matrix[ 1 ][ 1 ] = m_matrix[ 1 ][ 1 ] + d;
    t.m_matrix[ 1 ][ 2 ] = m_matrix[ 1 ][ 2 ] + d;
    t.m_matrix[ 2 ][ 0 ] = m_matrix[ 2 ][ 0 ] + d;
    t.m_matrix[ 2 ][ 1 ] = m_matrix[ 2 ][ 1 ] + d;
    t.m_matrix[ 0 ][ 2 ] = m_matrix[ 2 ][ 2 ] + d;
    return t;
}

Transform Transform::operator-( double d ) const
{
    Transform t;
    t.m_matrix[ 0 ][ 0 ] = m_matrix[ 0 ][ 0 ] - d;
    t.m_matrix[ 0 ][ 1 ] = m_matrix[ 0 ][ 1 ] - d;
    t.m_matrix[ 0 ][ 2 ] = m_matrix[ 0 ][ 2 ] - d;
    t.m_matrix[ 1 ][ 0 ] = m_matrix[ 1 ][ 0 ] - d;
    t.m_matrix[ 1 ][ 1 ] = m_matrix[ 1 ][ 1 ] - d;
    t.m_matrix[ 1 ][ 2 ] = m_matrix[ 1 ][ 2 ] - d;
    t.m_matrix[ 2 ][ 0 ] = m_matrix[ 2 ][ 0 ] - d;
    t.m_matrix[ 2 ][ 1 ] = m_matrix[ 2 ][ 1 ] - d;
    t.m_matrix[ 0 ][ 2 ] = m_matrix[ 2 ][ 2 ] - d;
    return t;
}

Transform Transform::operator*( double d ) const
{
    Transform t;
    t.m_matrix[ 0 ][ 0 ] = m_matrix[ 0 ][ 0 ] * d;
    t.m_matrix[ 0 ][ 1 ] = m_matrix[ 0 ][ 1 ] * d;
    t.m_matrix[ 0 ][ 2 ] = m_matrix[ 0 ][ 2 ] * d;
    t.m_matrix[ 1 ][ 0 ] = m_matrix[ 1 ][ 0 ] * d;
    t.m_matrix[ 1 ][ 1 ] = m_matrix[ 1 ][ 1 ] * d;
    t.m_matrix[ 1 ][ 2 ] = m_matrix[ 1 ][ 2 ] * d;
    t.m_matrix[ 2 ][ 0 ] = m_matrix[ 2 ][ 0 ] * d;
    t.m_matrix[ 2 ][ 1 ] = m_matrix[ 2 ][ 1 ] * d;
    t.m_matrix[ 0 ][ 2 ] = m_matrix[ 2 ][ 2 ] * d;
    return t;
}

Transform Transform::operator/( double d ) const
{
    Transform t;
    t.m_matrix[ 0 ][ 0 ] = m_matrix[ 0 ][ 0 ] / d;
    t.m_matrix[ 0 ][ 1 ] = m_matrix[ 0 ][ 1 ] / d;
    t.m_matrix[ 0 ][ 2 ] = m_matrix[ 0 ][ 2 ] / d;
    t.m_matrix[ 1 ][ 0 ] = m_matrix[ 1 ][ 0 ] / d;
    t.m_matrix[ 1 ][ 1 ] = m_matrix[ 1 ][ 1 ] / d;
    t.m_matrix[ 1 ][ 2 ] = m_matrix[ 1 ][ 2 ] / d;
    t.m_matrix[ 2 ][ 0 ] = m_matrix[ 2 ][ 0 ] / d;
    t.m_matrix[ 2 ][ 1 ] = m_matrix[ 2 ][ 1 ] / d;
    t.m_matrix[ 0 ][ 2 ] = m_matrix[ 2 ][ 2 ] / d;
    return t;
}

Transform & Transform::operator+=( double d )
{
    m_matrix[ 0 ][ 0 ] += d;
    m_matrix[ 0 ][ 1 ] += d;
    m_matrix[ 0 ][ 2 ] += d;
    m_matrix[ 1 ][ 0 ] += d;
    m_matrix[ 1 ][ 1 ] += d;
    m_matrix[ 1 ][ 2 ] += d;
    m_matrix[ 2 ][ 0 ] += d;
    m_matrix[ 2 ][ 1 ] += d;
    m_matrix[ 2 ][ 2 ] += d;
    return *this;
}

Transform & Transform::operator-=( double d )
{
    m_matrix[ 0 ][ 0 ] -= d;
    m_matrix[ 0 ][ 1 ] -= d;
    m_matrix[ 0 ][ 2 ] -= d;
    m_matrix[ 1 ][ 0 ] -= d;
    m_matrix[ 1 ][ 1 ] -= d;
    m_matrix[ 1 ][ 2 ] -= d;
    m_matrix[ 2 ][ 0 ] -= d;
    m_matrix[ 2 ][ 1 ] -= d;
    m_matrix[ 2 ][ 2 ] -= d;
    return *this;
}

Transform & Transform::operator*=( double d )
{
    m_matrix[ 0 ][ 0 ] *= d;
    m_matrix[ 0 ][ 1 ] *= d;
    m_matrix[ 0 ][ 2 ] *= d;
    m_matrix[ 1 ][ 0 ] *= d;
    m_matrix[ 1 ][ 1 ] *= d;
    m_matrix[ 1 ][ 2 ] *= d;
    m_matrix[ 2 ][ 0 ] *= d;
    m_matrix[ 2 ][ 1 ] *= d;
    m_matrix[ 2 ][ 2 ] *= d;
    return *this;
}

Transform & Transform::operator/=( double d )
{
    m_matrix[ 0 ][ 0 ] /= d;
    m_matrix[ 0 ][ 1 ] /= d;
    m_matrix[ 0 ][ 2 ] /= d;
    m_matrix[ 1 ][ 0 ] /= d;
    m_matrix[ 1 ][ 1 ] /= d;
    m_matrix[ 1 ][ 2 ] /= d;
    m_matrix[ 2 ][ 0 ] /= d;
    m_matrix[ 2 ][ 1 ] /= d;
    m_matrix[ 2 ][ 2 ] /= d;
    return *this;
}

Transform & Transform::operator+=( const Transform & t )
{
    return *this = ( *this ) + t;
}

Transform Transform::operator+( const Transform & t ) const
{
    Transform t2;
    t2.m_matrix[ 0 ][ 0 ] = m_matrix[ 0 ][ 0 ] + t.m_matrix[ 0 ][ 0 ];
    t2.m_matrix[ 0 ][ 1 ] = m_matrix[ 0 ][ 1 ] + t.m_matrix[ 0 ][ 1 ];
    t2.m_matrix[ 0 ][ 2 ] = m_matrix[ 0 ][ 2 ] + t.m_matrix[ 0 ][ 2 ];
    t2.m_matrix[ 1 ][ 0 ] = m_matrix[ 1 ][ 0 ] + t.m_matrix[ 1 ][ 0 ];
    t2.m_matrix[ 1 ][ 1 ] = m_matrix[ 1 ][ 1 ] + t.m_matrix[ 1 ][ 1 ];
    t2.m_matrix[ 1 ][ 2 ] = m_matrix[ 1 ][ 2 ] + t.m_matrix[ 1 ][ 2 ];
    t2.m_matrix[ 2 ][ 0 ] = m_matrix[ 2 ][ 0 ] + t.m_matrix[ 2 ][ 0 ];
    t2.m_matrix[ 2 ][ 1 ] = m_matrix[ 2 ][ 1 ] + t.m_matrix[ 2 ][ 1 ];
    t2.m_matrix[ 0 ][ 2 ] = m_matrix[ 2 ][ 2 ] + t.m_matrix[ 2 ][ 2 ];
    return t2;
}

Transform & Transform::operator*=( const Transform & t )
{
    return *this = ( *this ) * t;
}

Transform Transform::operator*( const Transform & t ) const
{
    Transform t2;
    t2.m_matrix[ 0 ][ 0 ] = ( m_matrix[ 0 ][ 0 ] * t.m_matrix[ 0 ][ 0 ] ) + ( m_matrix[ 0 ][ 1 ] * t.m_matrix[ 1 ][ 0 ] ) + ( m_matrix[ 0 ][ 2 ] * t.m_matrix[ 2 ][ 0 ] );
    t2.m_matrix[ 0 ][ 1 ] = ( m_matrix[ 0 ][ 0 ] * t.m_matrix[ 0 ][ 1 ] ) + ( m_matrix[ 0 ][ 1 ] * t.m_matrix[ 1 ][ 1 ] ) + ( m_matrix[ 0 ][ 2 ] * t.m_matrix[ 2 ][ 1 ] );
    t2.m_matrix[ 0 ][ 2 ] = ( m_matrix[ 0 ][ 0 ] * t.m_matrix[ 0 ][ 2 ] ) + ( m_matrix[ 0 ][ 1 ] * t.m_matrix[ 1 ][ 2 ] ) + ( m_matrix[ 0 ][ 2 ] * t.m_matrix[ 2 ][ 2 ] );
    t2.m_matrix[ 1 ][ 0 ] = ( m_matrix[ 1 ][ 0 ] * t.m_matrix[ 0 ][ 0 ] ) + ( m_matrix[ 1 ][ 1 ] * t.m_matrix[ 1 ][ 0 ] ) + ( m_matrix[ 1 ][ 2 ] * t.m_matrix[ 2 ][ 0 ] );
    t2.m_matrix[ 1 ][ 1 ] = ( m_matrix[ 1 ][ 0 ] * t.m_matrix[ 0 ][ 1 ] ) + ( m_matrix[ 1 ][ 1 ] * t.m_matrix[ 1 ][ 1 ] ) + ( m_matrix[ 1 ][ 2 ] * t.m_matrix[ 2 ][ 1 ] );
    t2.m_matrix[ 1 ][ 2 ] = ( m_matrix[ 1 ][ 0 ] * t.m_matrix[ 0 ][ 2 ] ) + ( m_matrix[ 1 ][ 1 ] * t.m_matrix[ 1 ][ 2 ] ) + ( m_matrix[ 1 ][ 2 ] * t.m_matrix[ 2 ][ 2 ] );
    t2.m_matrix[ 2 ][ 0 ] = ( m_matrix[ 2 ][ 0 ] * t.m_matrix[ 0 ][ 0 ] ) + ( m_matrix[ 2 ][ 1 ] * t.m_matrix[ 1 ][ 0 ] ) + ( m_matrix[ 2 ][ 2 ] * t.m_matrix[ 2 ][ 0 ] );
    t2.m_matrix[ 2 ][ 1 ] = ( m_matrix[ 2 ][ 0 ] * t.m_matrix[ 0 ][ 1 ] ) + ( m_matrix[ 2 ][ 1 ] * t.m_matrix[ 1 ][ 1 ] ) + ( m_matrix[ 2 ][ 2 ] * t.m_matrix[ 2 ][ 1 ] );
    t2.m_matrix[ 2 ][ 2 ] = ( m_matrix[ 2 ][ 0 ] * t.m_matrix[ 0 ][ 2 ] ) + ( m_matrix[ 2 ][ 1 ] * t.m_matrix[ 1 ][ 2 ] ) + ( m_matrix[ 2 ][ 2 ] * t.m_matrix[ 2 ][ 2 ] );
    return t2;
}

bool Transform::operator==( const Transform & t ) const
{
    return ( ( m_matrix[ 0 ][ 0 ] == t.m_matrix[ 0 ][ 0 ] ) &&
             ( m_matrix[ 0 ][ 1 ] == t.m_matrix[ 0 ][ 1 ] ) &&
             ( m_matrix[ 0 ][ 2 ] == t.m_matrix[ 0 ][ 2 ] ) &&
             ( m_matrix[ 1 ][ 0 ] == t.m_matrix[ 1 ][ 0 ] ) &&
             ( m_matrix[ 1 ][ 1 ] == t.m_matrix[ 1 ][ 1 ] ) &&
             ( m_matrix[ 1 ][ 2 ] == t.m_matrix[ 1 ][ 2 ] ) &&
             ( m_matrix[ 2 ][ 0 ] == t.m_matrix[ 2 ][ 0 ] ) &&
             ( m_matrix[ 2 ][ 1 ] == t.m_matrix[ 2 ][ 1 ] ) &&
             ( m_matrix[ 2 ][ 2 ] == t.m_matrix[ 2 ][ 2 ] ) );
}
