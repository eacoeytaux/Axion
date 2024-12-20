#ifndef Transform_hpp
#define Transform_hpp

#include "Planc.hpp"
#include "Coordinate.hpp"
#include "Angle.hpp"
#include "Vector.hpp"

namespace axn
{
namespace geometry
{

class Transform
{
private:
    Planc m_matrix[ 3 ][ 3 ];

public:
    Transform( ) { clear( true ); }

    Transform( bool identity ) { clear( identity ); }

    Planc get( uint i, uint j ) const { Assert( ( ( i <= 2 ) && ( j <= 2 ) ), "index out of range" ); return m_matrix[ i ][ j ]; }

    Transform & set( uint i, uint j, const Planc p ) { Assert( ( ( i <= 2 ) && ( j <= 2 ) ), "index out of range" ); m_matrix[ i ][ j ] = p; rethis; }
    Transform & set( const Transform & t )
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
        rethis;
    }

    Transform & clear( bool identity ) { m_matrix[ 0 ][ 0 ] = m_matrix[ 1 ][ 1 ] = m_matrix[ 2 ][ 2 ] = identity ? 1.0 : 0.0; m_matrix[ 0 ][ 1 ] = m_matrix[ 0 ][ 2 ] = m_matrix[ 1 ][ 0 ] = m_matrix[ 1 ][ 2 ] = m_matrix[ 2 ][ 0 ] = m_matrix[ 2 ][ 1 ] = 0.0; rethis; }

    Coordinate apply( const Coordinate & c ) const { return ( is_identity( ) ? c : Coordinate( ( c.x( ) * m_matrix[ 0 ][ 0 ] ) + ( c.y( ) * m_matrix[ 0 ][ 1 ] ) + m_matrix[ 0 ][ 2 ], ( c.x( ) * m_matrix[ 1 ][ 0 ] ) + ( c.y( ) * m_matrix[ 1 ][ 1 ] ) + m_matrix[ 1 ][ 2 ] ) ); }

    Transform & chain( const Transform & t ) { return set( t * ( *this ) ); }

    Transform & invert( ) { rethis = inverse( ); }
    Transform inverse( ) const
    {
        if( dec d = determinant( ) )
        {
            auto determinant2x2 = [ ] ( const Planc & a, const Planc & b, const Planc & c, const Planc & d ) { return ( a * d ) - ( b * c ); };

            return Transform(
                determinant2x2( m_matrix[ 1 ][ 1 ], m_matrix[ 1 ][ 2 ], m_matrix[ 2 ][ 1 ], m_matrix[ 2 ][ 2 ] ) / d,
                determinant2x2( m_matrix[ 0 ][ 2 ], m_matrix[ 0 ][ 1 ], m_matrix[ 2 ][ 2 ], m_matrix[ 2 ][ 1 ] ) / d,
                determinant2x2( m_matrix[ 0 ][ 1 ], m_matrix[ 0 ][ 2 ], m_matrix[ 1 ][ 1 ], m_matrix[ 1 ][ 2 ] ) / d,
                determinant2x2( m_matrix[ 1 ][ 2 ], m_matrix[ 1 ][ 0 ], m_matrix[ 2 ][ 2 ], m_matrix[ 2 ][ 0 ] ) / d,
                determinant2x2( m_matrix[ 0 ][ 0 ], m_matrix[ 0 ][ 2 ], m_matrix[ 2 ][ 0 ], m_matrix[ 2 ][ 2 ] ) / d,
                determinant2x2( m_matrix[ 0 ][ 2 ], m_matrix[ 0 ][ 0 ], m_matrix[ 1 ][ 2 ], m_matrix[ 1 ][ 0 ] ) / d,
                determinant2x2( m_matrix[ 1 ][ 0 ], m_matrix[ 1 ][ 1 ], m_matrix[ 2 ][ 0 ], m_matrix[ 2 ][ 1 ] ) / d,
                determinant2x2( m_matrix[ 0 ][ 1 ], m_matrix[ 0 ][ 0 ], m_matrix[ 2 ][ 1 ], m_matrix[ 2 ][ 0 ] ) / d,
                determinant2x2( m_matrix[ 0 ][ 0 ], m_matrix[ 0 ][ 1 ], m_matrix[ 1 ][ 0 ], m_matrix[ 1 ][ 1 ] ) / d );
        }
        else
        {
            return Transform( false );
        }
    }

    bool is_identity( ) const
    {
        return ( ( m_matrix[ 0 ][ 0 ] == 1.0 ) &&
                 ( m_matrix[ 1 ][ 1 ] == 1.0 ) &&
                 ( m_matrix[ 2 ][ 2 ] == 1.0 ) &&
                 !( m_matrix[ 0 ][ 1 ] ||
                    m_matrix[ 0 ][ 2 ] ||
                    m_matrix[ 1 ][ 0 ] ||
                    m_matrix[ 1 ][ 2 ] ||
                    m_matrix[ 2 ][ 0 ] ||
                    m_matrix[ 2 ][ 1 ] ) );
    }

    bool is_null( ) const
    {
        return !( m_matrix[ 0 ][ 0 ] ||
                  m_matrix[ 0 ][ 1 ] ||
                  m_matrix[ 0 ][ 2 ] ||
                  m_matrix[ 1 ][ 0 ] ||
                  m_matrix[ 1 ][ 1 ] ||
                  m_matrix[ 1 ][ 2 ] ||
                  m_matrix[ 2 ][ 0 ] ||
                  m_matrix[ 2 ][ 1 ] ||
                  m_matrix[ 2 ][ 2 ] );
    }

    Planc determinant( ) const
    {
        return ( m_matrix[ 0 ][ 0 ] * ( ( m_matrix[ 1 ][ 1 ] * m_matrix[ 2 ][ 2 ] ) - ( m_matrix[ 1 ][ 2 ] * m_matrix[ 2 ][ 1 ] ) ) ) -
            ( m_matrix[ 0 ][ 1 ] * ( ( m_matrix[ 1 ][ 0 ] * m_matrix[ 2 ][ 2 ] ) - ( m_matrix[ 1 ][ 2 ] * m_matrix[ 2 ][ 0 ] ) ) ) +
            ( m_matrix[ 0 ][ 2 ] * ( ( m_matrix[ 1 ][ 0 ] * m_matrix[ 2 ][ 1 ] ) - ( m_matrix[ 1 ][ 1 ] * m_matrix[ 2 ][ 0 ] ) ) );
    }

    Transform translation_only( ) const { return Transform( 1.0, 0.0, translation_x( ), 0.0, 1.0, translation_y( ), 0.0, 0.0, 1.0 ); }

    Vector translation( ) const { return Vector( translation_x( ), translation_y( ) ); }

    Planc translation_x( ) const { return m_matrix[ 0 ][ 2 ]; }
    Planc translation_y( ) const { return m_matrix[ 1 ][ 2 ]; }

    Transform scale_only( ) const { return Transform( scale_x( ), 0.0, 0.0, 0.0, scale_y( ), 0.0, 0.0, 0.0, 1.0 ); }

    Vector scale( ) const { return Vector( scale_x( ), scale_y( ) ); }

    Planc scale_x( ) const { pythagorean( m_matrix[ 0 ][ 0 ], m_matrix[ 1 ][ 0 ] ); }
    Planc scale_y( ) const { pythagorean( m_matrix[ 0 ][ 1 ], m_matrix[ 1 ][ 1 ] ); }

    Transform rotation_only( ) const { Angle a = rotation( ); return Transform( a.cos( ), -a.sin( ), 0.0, a.sin( ), a.cos( ), 0.0, 0.0, 0.0, 1.0 ); }

    Angle rotation( ) const { return Angle( ( m_matrix[ 0 ][ 0 ] && m_matrix[ 1 ][ 1 ] ) ? atan( m_matrix[ 1 ][ 0 ] / m_matrix[ 1 ][ 1 ] ) : 0 ); }

    static Transform identity( )
    {
        return Transform( 1.0, 0.0, 0.0,
                          0.0, 1.0, 0.0,
                          0.0, 0.0, 1.0 );
    }

    static Transform move( const Vector & v )
    {
        return Transform( 1.0, 0.0, v.dx( ),
                          0.0, 1.0, v.dy( ),
                          0.0, 0.0, 1.0 );
    }

    static Transform scale( const Planc & s )
    {
        return Transform( s, 0.0, 0.0,
                          0.0, s, 0.0,
                          0.0, 0.0, 1.0 );
    }

    static Transform scale( const Planc & s, const Coordinate & origin )
    {
        return Transform( s, 0.0, origin.x( ) - ( s * square( origin.x( ) ) ),
                          0.0, s, origin.y( ) - ( s * square( origin.y( ) ) ),
                          0.0, 0.0, 1.0 );
    }

    static Transform rotate( const Angle & a )
    {
        Planc s = a.sin( );
        Planc c = a.cos( );

        return Transform( c, -s, 0.0,
                          s, c, 0.0,
                          0.0, 0.0, 1.0 );
    }

    static Transform rotate( const Angle & a, const Coordinate & origin )
    {
        Planc s = a.sin( );
        Planc c = a.cos( );

        return Transform( c, -s, ( ( 1.0 - c ) * origin.x( ) ) + ( s * origin.y( ) ),
                          s, c, ( ( 1.0 - c ) * origin.y( ) ) - ( s * origin.x( ) ),
                          0.0, 0.0, 1.0 );
    }

    static Transform reflect( const Angle & a )
    {
        Planc s = a.sin( );
        Planc c = a.cos( );

        Planc s2 = ( a * 2.0 ).sin( );
        Planc c2 = ( a * 2.0 ).cos( );

        return Transform( c2, s2, 0.0,
                          s2, -c2, 0.0,
                          0.0, 0.0, 1.0 );
    }

    static Transform reflect( const Vector & v )
    {
        if( v.origin( ).x( ) || v.origin( ).y( ) )
        {
            Angle a = v.angle( );

            Planc s = a.sin( );
            Planc c = a.cos( );

            Planc s2 = ( a * 2.0 ).sin( );
            Planc c2 = ( a * 2.0 ).cos( );

            return Transform( c2, s2, ( v.origin( ).x( ) * ( 1.0 - c2 ) ) - ( v.origin( ).y( ) * s2 ),
                              s2, -c2, ( v.origin( ).y( ) * ( 1.0 + c2 ) ) - ( v.origin( ).x( ) * s2 ),
                              0.0, 0.0, 1.0 );
        }
        else
        {
            return reflect( v.angle( ) );
        }
    }

    static Transform stretch( const Vector & v )
    {
        Planc m = v.magnitude( );

        Angle a = v.angle( );

        Planc s = a.sin( );
        Planc c = a.cos( );

        Planc sc = ( s * c );

        Planc d = sc * ( m - 1.0 );

        Planc mcs = ( square( c ) * m ) + square( s );
        Planc msc = ( square( s ) * m ) + square( c );

        return Transform( mcs, d, ( mcs * ( -v.origin( ).x( ) + 1.0 ) ) + ( ( sc * v.origin( ).y( ) ) * ( -m + 1.0 ) ),
                          d, msc, ( msc * ( -v.origin( ).y( ) + 1.0 ) ) + ( ( sc * v.origin( ).x( ) ) * ( -m + 1.0 ) ),
                          0.0, 0.0, 1.0 );
    }

    Transform & operator=( const Transform & t ) { return set( t ); }

    Transform operator+( const dec d ) const
    {
        return Transform(
            m_matrix[ 0 ][ 0 ] + d,
            m_matrix[ 0 ][ 1 ] + d,
            m_matrix[ 0 ][ 2 ] + d,
            m_matrix[ 1 ][ 0 ] + d,
            m_matrix[ 1 ][ 1 ] + d,
            m_matrix[ 1 ][ 2 ] + d,
            m_matrix[ 2 ][ 0 ] + d,
            m_matrix[ 2 ][ 1 ] + d,
            m_matrix[ 2 ][ 2 ] + d );
    }

    Transform operator-( const dec d ) const
    {
        return Transform(
            m_matrix[ 0 ][ 0 ] - d,
            m_matrix[ 0 ][ 1 ] - d,
            m_matrix[ 0 ][ 2 ] - d,
            m_matrix[ 1 ][ 0 ] - d,
            m_matrix[ 1 ][ 1 ] - d,
            m_matrix[ 1 ][ 2 ] - d,
            m_matrix[ 2 ][ 0 ] - d,
            m_matrix[ 2 ][ 1 ] - d,
            m_matrix[ 2 ][ 2 ] - d );
    }

    Transform operator*( const dec d ) const
    {
        return Transform(
            m_matrix[ 0 ][ 0 ] * d,
            m_matrix[ 0 ][ 1 ] * d,
            m_matrix[ 0 ][ 2 ] * d,
            m_matrix[ 1 ][ 0 ] * d,
            m_matrix[ 1 ][ 1 ] * d,
            m_matrix[ 1 ][ 2 ] * d,
            m_matrix[ 2 ][ 0 ] * d,
            m_matrix[ 2 ][ 1 ] * d,
            m_matrix[ 2 ][ 2 ] * d );
    }

    Transform operator/( const dec d ) const
    {
        Assert( !d, "cannot divide by zero" );

        return Transform(
            m_matrix[ 0 ][ 0 ] / d,
            m_matrix[ 0 ][ 1 ] / d,
            m_matrix[ 0 ][ 2 ] / d,
            m_matrix[ 1 ][ 0 ] / d,
            m_matrix[ 1 ][ 1 ] / d,
            m_matrix[ 1 ][ 2 ] / d,
            m_matrix[ 2 ][ 0 ] / d,
            m_matrix[ 2 ][ 1 ] / d,
            m_matrix[ 2 ][ 2 ] / d );
    }

    Transform & operator+=( const dec d )
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
        rethis;
    }

    Transform & operator-=( const dec d )
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
        rethis;
    }

    Transform & operator*=( const dec d )
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
        rethis;
    }

    Transform & operator/=( const dec d )
    {
        Assert( !d, "cannot divide by zero" );

        m_matrix[ 0 ][ 0 ] /= d;
        m_matrix[ 0 ][ 1 ] /= d;
        m_matrix[ 0 ][ 2 ] /= d;
        m_matrix[ 1 ][ 0 ] /= d;
        m_matrix[ 1 ][ 1 ] /= d;
        m_matrix[ 1 ][ 2 ] /= d;
        m_matrix[ 2 ][ 0 ] /= d;
        m_matrix[ 2 ][ 1 ] /= d;
        m_matrix[ 2 ][ 2 ] /= d;
        rethis;
    }

    Transform operator+( const Transform & t ) const
    {
        return Transform(
            m_matrix[ 0 ][ 0 ] + t.m_matrix[ 0 ][ 0 ],
            m_matrix[ 0 ][ 1 ] + t.m_matrix[ 0 ][ 1 ],
            m_matrix[ 0 ][ 2 ] + t.m_matrix[ 0 ][ 2 ],
            m_matrix[ 1 ][ 0 ] + t.m_matrix[ 1 ][ 0 ],
            m_matrix[ 1 ][ 1 ] + t.m_matrix[ 1 ][ 1 ],
            m_matrix[ 1 ][ 2 ] + t.m_matrix[ 1 ][ 2 ],
            m_matrix[ 2 ][ 0 ] + t.m_matrix[ 2 ][ 0 ],
            m_matrix[ 2 ][ 1 ] + t.m_matrix[ 2 ][ 1 ],
            m_matrix[ 2 ][ 2 ] + t.m_matrix[ 2 ][ 2 ] );
    }

    Transform & operator+=( const Transform & t )
    {
        rethis = ( *this ) + t;
    }

    Transform operator*( const Transform & t ) const
    {
        return Transform(
            ( m_matrix[ 0 ][ 0 ] * t.m_matrix[ 0 ][ 0 ] ) + ( m_matrix[ 0 ][ 1 ] * t.m_matrix[ 1 ][ 0 ] ) + ( m_matrix[ 0 ][ 2 ] * t.m_matrix[ 2 ][ 0 ] ),
            ( m_matrix[ 0 ][ 0 ] * t.m_matrix[ 0 ][ 1 ] ) + ( m_matrix[ 0 ][ 1 ] * t.m_matrix[ 1 ][ 1 ] ) + ( m_matrix[ 0 ][ 2 ] * t.m_matrix[ 2 ][ 1 ] ),
            ( m_matrix[ 0 ][ 0 ] * t.m_matrix[ 0 ][ 2 ] ) + ( m_matrix[ 0 ][ 1 ] * t.m_matrix[ 1 ][ 2 ] ) + ( m_matrix[ 0 ][ 2 ] * t.m_matrix[ 2 ][ 2 ] ),
            ( m_matrix[ 1 ][ 0 ] * t.m_matrix[ 0 ][ 0 ] ) + ( m_matrix[ 1 ][ 1 ] * t.m_matrix[ 1 ][ 0 ] ) + ( m_matrix[ 1 ][ 2 ] * t.m_matrix[ 2 ][ 0 ] ),
            ( m_matrix[ 1 ][ 0 ] * t.m_matrix[ 0 ][ 1 ] ) + ( m_matrix[ 1 ][ 1 ] * t.m_matrix[ 1 ][ 1 ] ) + ( m_matrix[ 1 ][ 2 ] * t.m_matrix[ 2 ][ 1 ] ),
            ( m_matrix[ 1 ][ 0 ] * t.m_matrix[ 0 ][ 2 ] ) + ( m_matrix[ 1 ][ 1 ] * t.m_matrix[ 1 ][ 2 ] ) + ( m_matrix[ 1 ][ 2 ] * t.m_matrix[ 2 ][ 2 ] ),
            ( m_matrix[ 2 ][ 0 ] * t.m_matrix[ 0 ][ 0 ] ) + ( m_matrix[ 2 ][ 1 ] * t.m_matrix[ 1 ][ 0 ] ) + ( m_matrix[ 2 ][ 2 ] * t.m_matrix[ 2 ][ 0 ] ),
            ( m_matrix[ 2 ][ 0 ] * t.m_matrix[ 0 ][ 1 ] ) + ( m_matrix[ 2 ][ 1 ] * t.m_matrix[ 1 ][ 1 ] ) + ( m_matrix[ 2 ][ 2 ] * t.m_matrix[ 2 ][ 1 ] ),
            ( m_matrix[ 2 ][ 0 ] * t.m_matrix[ 0 ][ 2 ] ) + ( m_matrix[ 2 ][ 1 ] * t.m_matrix[ 1 ][ 2 ] ) + ( m_matrix[ 2 ][ 2 ] * t.m_matrix[ 2 ][ 2 ] ) );
    }

    Transform & operator*=( const Transform & t )
    {
        rethis = ( *this ) * t;
    }

    default_equal( Transform );

private:
    Transform( const Planc & p1,
               const Planc & p2,
               const Planc & p3,
               const Planc & p4,
               const Planc & p5,
               const Planc & p6,
               const Planc & p7,
               const Planc & p8,
               const Planc & p9 )
    {
        m_matrix[ 0 ][ 0 ] = p1;
        m_matrix[ 0 ][ 1 ] = p2;
        m_matrix[ 0 ][ 2 ] = p3;
        m_matrix[ 1 ][ 0 ] = p4;
        m_matrix[ 1 ][ 1 ] = p5;
        m_matrix[ 1 ][ 2 ] = p6;
        m_matrix[ 2 ][ 0 ] = p7;
        m_matrix[ 2 ][ 1 ] = p8;
        m_matrix[ 2 ][ 2 ] = p9;
    }
};

const Transform ZERO_TRANSFORM = Transform( false );
const Transform IDENTITY_TRANSFORM = Transform( true );

class Transformable
{
private:
    mutable Transform m_transform = IDENTITY_TRANSFORM;
    mutable Transform m_cumulative_transform = IDENTITY_TRANSFORM;

    mutable bool m_dirty = false;

public:
    Transformable( ) { }

    bool is_dirty( ) const { return m_dirty; }
    bool is_clean( ) const { return !is_dirty( ); }

    const Transform & transform( ) const { return m_transform; }
    const Transform & cumulative_transform( ) const { return m_cumulative_transform; }

    virtual Transformable & transform( const Transform & t ) { dirty( ); m_transform.chain( t ); m_cumulative_transform.chain( t ); rethis; }

    virtual const Transformable & const_transform( const Transform & t ) const { dirty( ); m_transform.chain( t ); m_cumulative_transform.chain( t ); rethis; }

    virtual Transformable & clear_transform( ) { dirty( ); m_transform = IDENTITY_TRANSFORM; rethis; }
    virtual Transformable & clear_cumulative_transform( ) { clear_transform( ); m_cumulative_transform = IDENTITY_TRANSFORM; rethis; }

    virtual const Transformable & const_clear_transform( ) const { dirty( ); m_transform = IDENTITY_TRANSFORM; rethis; }
    virtual const Transformable & const_clear_cumulative_transform( ) const { const_clear_transform( ); m_cumulative_transform = IDENTITY_TRANSFORM; rethis; }

    Coordinate apply_transform( const Coordinate & coordinate ) const { return transform( ).apply( coordinate ); }
    Coordinate apply_cumulative_transform( const Coordinate & coordinate ) const { return cumulative_transform( ).apply( coordinate ); }

    Transformable & move( const Vector & v ) { return transform( Transform::move( v ) ); }
    Transformable & stretch( const Vector & v ) { return transform( Transform::stretch( v ) ); }
    Transformable & scale( dec scale ) { return transform( Transform::scale( scale ) ); }
    Transformable & scale( dec scale, const Coordinate & origin ) { return transform( Transform::scale( scale, origin ) ); }
    Transformable & rotate( const Angle & angle ) { return transform( Transform::rotate( angle ) ); }
    Transformable & rotate( const Angle & angle, const Coordinate & origin ) { return transform( Transform::rotate( angle, origin ) ); }
    Transformable & mirror( const Vector & axis ) { return transform( Transform::reflect( axis ) ); }
    Transformable & mirror_x( ) { return mirror( X_HAT ); }
    Transformable & mirror_y( ) { return mirror( Y_HAT ); }

    default_equal( Transformable );

protected:
    virtual const Transformable & dirty( ) const { m_dirty = true; rethis; }
    virtual const Transformable & clean( ) const { m_dirty = false; rethis; }
};

inline Coordinate & Coordinate::transform( const Transform & t ) { rethis = t.apply( *this ); }

} // namespace geometry
} // namespace axn

#define transform_functions( Class ) \
    Class & move( const Vector & v ) { Transformable::move( v ); rethis; } \
    Class & scale( dec scale ) { Transformable::scale( scale ); rethis; } \
    Class & scale( dec scale, const Coordinate & origin ) { Transformable::scale( scale, origin ); rethis; } \
    Class & stretch( const Vector & v ) { Transformable::stretch( v ); rethis; } \
    Class & rotate( const Angle & angle ) { Transformable::rotate( angle ); rethis; } \
    Class & rotate( const Angle & angle, const Coordinate & origin ) { Transformable::rotate( angle, origin ); rethis; } \
    Class & mirror( const Vector & axis ) { Transformable::mirror( axis ); rethis; } \
    Class & mirror_x( ) { Transformable::mirror_x( ); rethis; } \
    Class & mirror_y( ) { Transformable::mirror_y( ); rethis; }

#endif /* Transform_hpp */
