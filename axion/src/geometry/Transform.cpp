#include "Transform.hpp"

AXN_NAMESPACES
using axn::geometry::Transform;

Transform::Transform( bool identity ) {
    m_matrix[ 0 ][ 0 ] =
        m_matrix[ 1 ][ 1 ] =
        m_matrix[ 2 ][ 2 ] =
        identity ? 1.f : 0.f;
    m_matrix[ 0 ][ 1 ] =
        m_matrix[ 0 ][ 2 ] =
        m_matrix[ 1 ][ 0 ] =
        m_matrix[ 1 ][ 2 ] =
        m_matrix[ 2 ][ 0 ] =
        m_matrix[ 2 ][ 1 ] = 0.f;
}

Transform & Transform::set( const uint _i, const uint _j, const planc _v ) {
    Assert( in_range<uint>( _i, 2 ) && in_range<uint>( _j, 2 ), "index out of range" );
    m_matrix[ _i ][ _j ] = _v;
    return *this;
}

Transform & Transform::set( const Transform & t ) {
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

Coordinate Transform::transform( const Coordinate & coordinate ) {
    planc x = coordinate.x( );
    planc y = coordinate.y( );

    planc xt = ( x * m_matrix[ 0 ][ 0 ] ) + ( y * m_matrix[ 0 ][ 1 ] ) + m_matrix[ 0 ][ 2 ];
    planc yt = ( x * m_matrix[ 1 ][ 0 ] ) + ( y * m_matrix[ 1 ][ 1 ] ) + m_matrix[ 1 ][ 2 ];

    return Coordinate( xt, yt );
}

Transform & Transform::chain( const Transform & t ) {
    set( t * *this );
    return *this;
}

bool Transform::null( ) const {
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

float Transform::determinant( ) const {
    return ( m_matrix[ 0 ][ 0 ] *
             ( ( m_matrix[ 1 ][ 1 ] * m_matrix[ 2 ][ 2 ] ) -
              ( m_matrix[ 1 ][ 2 ] * m_matrix[ 2 ][ 1 ] ) ) ) +
           ( m_matrix[ 0 ][ 1 ] *
             ( ( m_matrix[ 1 ][ 0 ] * m_matrix[ 2 ][ 2 ] ) -
              ( m_matrix[ 1 ][ 2 ] * m_matrix[ 2 ][ 0 ] ) ) ) +
           ( m_matrix[ 0 ][ 2 ] *
             ( ( m_matrix[ 1 ][ 0 ] * m_matrix[ 2 ][ 1 ] ) -
              ( m_matrix[ 1 ][ 1 ] * m_matrix[ 2 ][ 0 ] ) ) );
}

inline float determinant2x2( planc a, planc b, planc c, planc d ) {
    return ( a * d ) - ( b * c );
}

Transform & Transform::invert( ) {
    *this = inverse( );
    return *this;
}
Transform Transform::inverse( ) {
    float d = determinant( );
    if( !d )
        return Transform( false );
    d = 1.f / d;

    Transform ret;
    ret.m_matrix[ 0 ][ 0 ] =
        determinant2x2( m_matrix[ 1 ][ 1 ], m_matrix[ 1 ][ 2 ], m_matrix[ 2 ][ 1 ], m_matrix[ 2 ][ 2 ] ) * d;
    ret.m_matrix[ 0 ][ 1 ] =
        determinant2x2( m_matrix[ 0 ][ 2 ], m_matrix[ 0 ][ 1 ], m_matrix[ 2 ][ 2 ], m_matrix[ 2 ][ 1 ] ) * d;
    ret.m_matrix[ 0 ][ 2 ] =
        determinant2x2( m_matrix[ 0 ][ 1 ], m_matrix[ 0 ][ 2 ], m_matrix[ 1 ][ 1 ], m_matrix[ 1 ][ 2 ] ) * d;
    ret.m_matrix[ 1 ][ 0 ] =
        determinant2x2( m_matrix[ 1 ][ 2 ], m_matrix[ 1 ][ 0 ], m_matrix[ 2 ][ 2 ], m_matrix[ 2 ][ 0 ] ) * d;
    ret.m_matrix[ 1 ][ 1 ] =
        determinant2x2( m_matrix[ 0 ][ 0 ], m_matrix[ 0 ][ 2 ], m_matrix[ 2 ][ 0 ], m_matrix[ 2 ][ 2 ] ) * d;
    ret.m_matrix[ 1 ][ 2 ] =
        determinant2x2( m_matrix[ 0 ][ 2 ], m_matrix[ 0 ][ 0 ], m_matrix[ 1 ][ 2 ], m_matrix[ 1 ][ 0 ] ) * d;
    ret.m_matrix[ 2 ][ 0 ] =
        determinant2x2( m_matrix[ 1 ][ 0 ], m_matrix[ 1 ][ 1 ], m_matrix[ 2 ][ 0 ], m_matrix[ 2 ][ 1 ] ) * d;
    ret.m_matrix[ 2 ][ 1 ] =
        determinant2x2( m_matrix[ 0 ][ 1 ], m_matrix[ 0 ][ 0 ], m_matrix[ 2 ][ 1 ], m_matrix[ 2 ][ 0 ] ) * d;
    ret.m_matrix[ 2 ][ 2 ] =
        determinant2x2( m_matrix[ 0 ][ 0 ], m_matrix[ 0 ][ 1 ], m_matrix[ 1 ][ 0 ], m_matrix[ 1 ][ 1 ] ) * d;
    return ret;
}

Transform & Transform::operator=( const Transform & t ) {
    set( t );
    return *this;
}

Transform Transform::operator+( float f ) const {
    Transform ret;
    ret.m_matrix[ 0 ][ 0 ] = m_matrix[ 0 ][ 0 ] + f;
    ret.m_matrix[ 0 ][ 1 ] = m_matrix[ 0 ][ 1 ] + f;
    ret.m_matrix[ 0 ][ 2 ] = m_matrix[ 0 ][ 2 ] + f;
    ret.m_matrix[ 1 ][ 0 ] = m_matrix[ 1 ][ 0 ] + f;
    ret.m_matrix[ 1 ][ 1 ] = m_matrix[ 1 ][ 1 ] + f;
    ret.m_matrix[ 1 ][ 2 ] = m_matrix[ 1 ][ 2 ] + f;
    ret.m_matrix[ 2 ][ 0 ] = m_matrix[ 2 ][ 0 ] + f;
    ret.m_matrix[ 2 ][ 1 ] = m_matrix[ 2 ][ 1 ] + f;
    ret.m_matrix[ 0 ][ 2 ] = m_matrix[ 2 ][ 2 ] + f;
    return ret;
}

Transform Transform::operator-( float f ) const {
    Transform ret;
    ret.m_matrix[ 0 ][ 0 ] = m_matrix[ 0 ][ 0 ] - f;
    ret.m_matrix[ 0 ][ 1 ] = m_matrix[ 0 ][ 1 ] - f;
    ret.m_matrix[ 0 ][ 2 ] = m_matrix[ 0 ][ 2 ] - f;
    ret.m_matrix[ 1 ][ 0 ] = m_matrix[ 1 ][ 0 ] - f;
    ret.m_matrix[ 1 ][ 1 ] = m_matrix[ 1 ][ 1 ] - f;
    ret.m_matrix[ 1 ][ 2 ] = m_matrix[ 1 ][ 2 ] - f;
    ret.m_matrix[ 2 ][ 0 ] = m_matrix[ 2 ][ 0 ] - f;
    ret.m_matrix[ 2 ][ 1 ] = m_matrix[ 2 ][ 1 ] - f;
    ret.m_matrix[ 0 ][ 2 ] = m_matrix[ 2 ][ 2 ] - f;
    return ret;
}

Transform Transform::operator*( float f ) const {
    Transform ret;
    ret.m_matrix[ 0 ][ 0 ] = m_matrix[ 0 ][ 0 ] * f;
    ret.m_matrix[ 0 ][ 1 ] = m_matrix[ 0 ][ 1 ] * f;
    ret.m_matrix[ 0 ][ 2 ] = m_matrix[ 0 ][ 2 ] * f;
    ret.m_matrix[ 1 ][ 0 ] = m_matrix[ 1 ][ 0 ] * f;
    ret.m_matrix[ 1 ][ 1 ] = m_matrix[ 1 ][ 1 ] * f;
    ret.m_matrix[ 1 ][ 2 ] = m_matrix[ 1 ][ 2 ] * f;
    ret.m_matrix[ 2 ][ 0 ] = m_matrix[ 2 ][ 0 ] * f;
    ret.m_matrix[ 2 ][ 1 ] = m_matrix[ 2 ][ 1 ] * f;
    ret.m_matrix[ 0 ][ 2 ] = m_matrix[ 2 ][ 2 ] * f;
    return ret;
}

Transform Transform::operator/( float f ) const {
    Transform ret;
    ret.m_matrix[ 0 ][ 0 ] = m_matrix[ 0 ][ 0 ] / f;
    ret.m_matrix[ 0 ][ 1 ] = m_matrix[ 0 ][ 1 ] / f;
    ret.m_matrix[ 0 ][ 2 ] = m_matrix[ 0 ][ 2 ] / f;
    ret.m_matrix[ 1 ][ 0 ] = m_matrix[ 1 ][ 0 ] / f;
    ret.m_matrix[ 1 ][ 1 ] = m_matrix[ 1 ][ 1 ] / f;
    ret.m_matrix[ 1 ][ 2 ] = m_matrix[ 1 ][ 2 ] / f;
    ret.m_matrix[ 2 ][ 0 ] = m_matrix[ 2 ][ 0 ] / f;
    ret.m_matrix[ 2 ][ 1 ] = m_matrix[ 2 ][ 1 ] / f;
    ret.m_matrix[ 0 ][ 2 ] = m_matrix[ 2 ][ 2 ] / f;
    return ret;
}

Transform & Transform::operator+=( float f ) {
    m_matrix[ 0 ][ 0 ] += f;
    m_matrix[ 0 ][ 1 ] += f;
    m_matrix[ 0 ][ 2 ] += f;
    m_matrix[ 1 ][ 0 ] += f;
    m_matrix[ 1 ][ 1 ] += f;
    m_matrix[ 1 ][ 2 ] += f;
    m_matrix[ 2 ][ 0 ] += f;
    m_matrix[ 2 ][ 1 ] += f;
    m_matrix[ 2 ][ 2 ] += f;
    return *this;
}

Transform & Transform::operator-=( float f ) {
    m_matrix[ 0 ][ 0 ] -= f;
    m_matrix[ 0 ][ 1 ] -= f;
    m_matrix[ 0 ][ 2 ] -= f;
    m_matrix[ 1 ][ 0 ] -= f;
    m_matrix[ 1 ][ 1 ] -= f;
    m_matrix[ 1 ][ 2 ] -= f;
    m_matrix[ 2 ][ 0 ] -= f;
    m_matrix[ 2 ][ 1 ] -= f;
    m_matrix[ 2 ][ 2 ] -= f;
    return *this;
}

Transform & Transform::operator*=( float f ) {
    m_matrix[ 0 ][ 0 ] *= f;
    m_matrix[ 0 ][ 1 ] *= f;
    m_matrix[ 0 ][ 2 ] *= f;
    m_matrix[ 1 ][ 0 ] *= f;
    m_matrix[ 1 ][ 1 ] *= f;
    m_matrix[ 1 ][ 2 ] *= f;
    m_matrix[ 2 ][ 0 ] *= f;
    m_matrix[ 2 ][ 1 ] *= f;
    m_matrix[ 2 ][ 2 ] *= f;
    return *this;
}

Transform & Transform::operator/=( float f ) {
    m_matrix[ 0 ][ 0 ] /= f;
    m_matrix[ 0 ][ 1 ] /= f;
    m_matrix[ 0 ][ 2 ] /= f;
    m_matrix[ 1 ][ 0 ] /= f;
    m_matrix[ 1 ][ 1 ] /= f;
    m_matrix[ 1 ][ 2 ] /= f;
    m_matrix[ 2 ][ 0 ] /= f;
    m_matrix[ 2 ][ 1 ] /= f;
    m_matrix[ 2 ][ 2 ] /= f;
    return *this;
}

Transform & Transform::operator+=( const Transform & t ) {
    *this = *this + t;
    return *this;
}
Transform Transform::operator+( const Transform & t ) const {
    Transform ret;
    ret.m_matrix[ 0 ][ 0 ] = m_matrix[ 0 ][ 0 ] + t.m_matrix[ 0 ][ 0 ];
    ret.m_matrix[ 0 ][ 1 ] = m_matrix[ 0 ][ 1 ] + t.m_matrix[ 0 ][ 1 ];
    ret.m_matrix[ 0 ][ 2 ] = m_matrix[ 0 ][ 2 ] + t.m_matrix[ 0 ][ 2 ];
    ret.m_matrix[ 1 ][ 0 ] = m_matrix[ 1 ][ 0 ] + t.m_matrix[ 1 ][ 0 ];
    ret.m_matrix[ 1 ][ 1 ] = m_matrix[ 1 ][ 1 ] + t.m_matrix[ 1 ][ 1 ];
    ret.m_matrix[ 1 ][ 2 ] = m_matrix[ 1 ][ 2 ] + t.m_matrix[ 1 ][ 2 ];
    ret.m_matrix[ 2 ][ 0 ] = m_matrix[ 2 ][ 0 ] + t.m_matrix[ 2 ][ 0 ];
    ret.m_matrix[ 2 ][ 1 ] = m_matrix[ 2 ][ 1 ] + t.m_matrix[ 2 ][ 1 ];
    ret.m_matrix[ 0 ][ 2 ] = m_matrix[ 2 ][ 2 ] + t.m_matrix[ 2 ][ 2 ];
    return ret;
}

Transform & Transform::operator*=( const Transform & t ) {
    *this = *this * t;
    return *this;
}
Transform Transform::operator*( const Transform & t ) const {
    Transform ret;
    ret.m_matrix[ 0 ][ 0 ] = ( m_matrix[ 0 ][ 0 ] * t.m_matrix[ 0 ][ 0 ] ) +
                        ( m_matrix[ 0 ][ 1 ] * t.m_matrix[ 1 ][ 0 ] ) +
                        ( m_matrix[ 0 ][ 2 ] * t.m_matrix[ 2 ][ 0 ] );
    ret.m_matrix[ 0 ][ 1 ] = ( m_matrix[ 0 ][ 0 ] * t.m_matrix[ 0 ][ 1 ] ) +
                        ( m_matrix[ 0 ][ 1 ] * t.m_matrix[ 1 ][ 1 ] ) +
                        ( m_matrix[ 0 ][ 2 ] * t.m_matrix[ 2 ][ 1 ] );
    ret.m_matrix[ 0 ][ 2 ] = ( m_matrix[ 0 ][ 0 ] * t.m_matrix[ 0 ][ 2 ] ) +
                        ( m_matrix[ 0 ][ 1 ] * t.m_matrix[ 1 ][ 2 ] ) +
                        ( m_matrix[ 0 ][ 2 ] * t.m_matrix[ 2 ][ 2 ] );
    ret.m_matrix[ 1 ][ 0 ] = ( m_matrix[ 1 ][ 0 ] * t.m_matrix[ 0 ][ 0 ] ) +
                        ( m_matrix[ 1 ][ 1 ] * t.m_matrix[ 1 ][ 0 ] ) +
                        ( m_matrix[ 1 ][ 2 ] * t.m_matrix[ 2 ][ 0 ] );
    ret.m_matrix[ 1 ][ 1 ] = ( m_matrix[ 1 ][ 0 ] * t.m_matrix[ 0 ][ 1 ] ) +
                        ( m_matrix[ 1 ][ 1 ] * t.m_matrix[ 1 ][ 1 ] ) +
                        ( m_matrix[ 1 ][ 2 ] * t.m_matrix[ 2 ][ 1 ] );
    ret.m_matrix[ 1 ][ 2 ] = ( m_matrix[ 1 ][ 0 ] * t.m_matrix[ 0 ][ 2 ] ) +
                        ( m_matrix[ 1 ][ 1 ] * t.m_matrix[ 1 ][ 2 ] ) +
                        ( m_matrix[ 1 ][ 2 ] * t.m_matrix[ 2 ][ 2 ] );
    ret.m_matrix[ 2 ][ 0 ] = ( m_matrix[ 2 ][ 0 ] * t.m_matrix[ 0 ][ 0 ] ) +
                        ( m_matrix[ 2 ][ 1 ] * t.m_matrix[ 1 ][ 0 ] ) +
                        ( m_matrix[ 2 ][ 2 ] * t.m_matrix[ 2 ][ 0 ] );
    ret.m_matrix[ 2 ][ 1 ] = ( m_matrix[ 2 ][ 0 ] * t.m_matrix[ 0 ][ 1 ] ) +
                        ( m_matrix[ 2 ][ 1 ] * t.m_matrix[ 1 ][ 1 ] ) +
                        ( m_matrix[ 2 ][ 2 ] * t.m_matrix[ 2 ][ 1 ] );
    ret.m_matrix[ 2 ][ 2 ] = ( m_matrix[ 2 ][ 0 ] * t.m_matrix[ 0 ][ 2 ] ) +
                        ( m_matrix[ 2 ][ 1 ] * t.m_matrix[ 1 ][ 2 ] ) +
                        ( m_matrix[ 2 ][ 2 ] * t.m_matrix[ 2 ][ 2 ] );
    return ret;
}
