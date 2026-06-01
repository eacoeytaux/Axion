#ifndef Transform_hpp
#define Transform_hpp

#include "axn/geometry/Planc.hpp"
#include "axn/geometry/Point.hpp"
#include "axn/geometry/Angle.hpp"
#include "axn/geometry/Vector.hpp"

namespace axn
{
namespace geometry
{

class Transform
{

private:

    Planc m_matrix[ 3 ][ 3 ];

public:

    Transform( bool in_identity = true ) { clear( in_identity ); }

    Point apply( Point cref in_pt ) const { return( Point( ( in_pt.x( ) * m_matrix[ 0 ][ 0 ] ) + ( in_pt.y( ) * m_matrix[ 0 ][ 1 ] ) + m_matrix[ 0 ][ 2 ], ( in_pt.y( ) * m_matrix[ 1 ][ 1 ] ) + ( in_pt.x( ) * m_matrix[ 1 ][ 0 ] ) + m_matrix[ 1 ][ 2 ] ) ); }

    #pragma warning( push )
    #pragma warning( disable : 6385 )

    Planc get( uint in_i, uint in_j ) const { Assert( ( ( in_i <= 2 ) && ( in_j <= 2 ) ), "index out of range" ); return( m_matrix[ in_i ][ in_j ] ); }

    Transform ref set( uint in_i, uint in_j, Planc cref in_p ) { Assert( ( ( in_i <= 2 ) && ( in_j <= 2 ) ), "index out of range" ); m_matrix[ in_i ][ in_j ] = in_p; rethis; }
    Transform ref set( Transform cref in_trans )
    {
        m_matrix[ 0 ][ 0 ] = in_trans.m_matrix[ 0 ][ 0 ];
        m_matrix[ 0 ][ 1 ] = in_trans.m_matrix[ 0 ][ 1 ];
        m_matrix[ 0 ][ 2 ] = in_trans.m_matrix[ 0 ][ 2 ];
        m_matrix[ 1 ][ 0 ] = in_trans.m_matrix[ 1 ][ 0 ];
        m_matrix[ 1 ][ 1 ] = in_trans.m_matrix[ 1 ][ 1 ];
        m_matrix[ 1 ][ 2 ] = in_trans.m_matrix[ 1 ][ 2 ];
        m_matrix[ 2 ][ 0 ] = in_trans.m_matrix[ 2 ][ 0 ];
        m_matrix[ 2 ][ 1 ] = in_trans.m_matrix[ 2 ][ 1 ];
        m_matrix[ 2 ][ 2 ] = in_trans.m_matrix[ 2 ][ 2 ];
        rethis;
    }

    #pragma warning( pop )

    Transform ref clear( bool in_identity ) { m_matrix[ 0 ][ 0 ] = m_matrix[ 1 ][ 1 ] = m_matrix[ 2 ][ 2 ] = in_identity ? 1.0 : 0.0; m_matrix[ 0 ][ 1 ] = m_matrix[ 0 ][ 2 ] = m_matrix[ 1 ][ 0 ] = m_matrix[ 1 ][ 2 ] = m_matrix[ 2 ][ 0 ] = m_matrix[ 2 ][ 1 ] = 0.0; rethis; }

    Transform ref chain( Transform cref in_trans ) { return( set( in_trans * ( *this ) ) ); }

    Transform ref invert( ) { rethis = inverse( ); }
    Transform inverse( ) const
    {
        if( dec det = determinant( ) )
        {
            lambdaf( determinant2x2 )( Planc cref in_a, Planc cref in_b, Planc cref in_c, Planc cref in_d ) { return( in_a * in_d ) - ( in_b * in_c ); };

            return( Transform(
                determinant2x2( m_matrix[ 1 ][ 1 ], m_matrix[ 1 ][ 2 ], m_matrix[ 2 ][ 1 ], m_matrix[ 2 ][ 2 ] ) / det,
                determinant2x2( m_matrix[ 0 ][ 2 ], m_matrix[ 0 ][ 1 ], m_matrix[ 2 ][ 2 ], m_matrix[ 2 ][ 1 ] ) / det,
                determinant2x2( m_matrix[ 0 ][ 1 ], m_matrix[ 0 ][ 2 ], m_matrix[ 1 ][ 1 ], m_matrix[ 1 ][ 2 ] ) / det,
                determinant2x2( m_matrix[ 1 ][ 2 ], m_matrix[ 1 ][ 0 ], m_matrix[ 2 ][ 2 ], m_matrix[ 2 ][ 0 ] ) / det,
                determinant2x2( m_matrix[ 0 ][ 0 ], m_matrix[ 0 ][ 2 ], m_matrix[ 2 ][ 0 ], m_matrix[ 2 ][ 2 ] ) / det,
                determinant2x2( m_matrix[ 0 ][ 2 ], m_matrix[ 0 ][ 0 ], m_matrix[ 1 ][ 2 ], m_matrix[ 1 ][ 0 ] ) / det,
                determinant2x2( m_matrix[ 1 ][ 0 ], m_matrix[ 1 ][ 1 ], m_matrix[ 2 ][ 0 ], m_matrix[ 2 ][ 1 ] ) / det,
                determinant2x2( m_matrix[ 0 ][ 1 ], m_matrix[ 0 ][ 0 ], m_matrix[ 2 ][ 1 ], m_matrix[ 2 ][ 0 ] ) / det,
                determinant2x2( m_matrix[ 0 ][ 0 ], m_matrix[ 0 ][ 1 ], m_matrix[ 1 ][ 0 ], m_matrix[ 1 ][ 1 ] ) / det ) );
        }
        else
        {
            return( Transform( false ) );
        }
    }

    bool is_null( ) const
    {
        return( is_zero( m_matrix[ 0 ][ 0 ] ) &&
                is_zero( m_matrix[ 0 ][ 1 ] ) &&
                is_zero( m_matrix[ 0 ][ 2 ] ) &&
                is_zero( m_matrix[ 1 ][ 0 ] ) &&
                is_zero( m_matrix[ 1 ][ 1 ] ) &&
                is_zero( m_matrix[ 1 ][ 2 ] ) &&
                is_zero( m_matrix[ 2 ][ 0 ] ) &&
                is_zero( m_matrix[ 2 ][ 1 ] ) &&
                is_zero( m_matrix[ 2 ][ 2 ] ) );
    }

    bool is_identity( ) const
    {
        return( is_one( m_matrix[ 0 ][ 0 ] ) &&
                is_one( m_matrix[ 1 ][ 1 ] ) &&
                is_one( m_matrix[ 2 ][ 2 ] ) &&
                is_zero( m_matrix[ 0 ][ 1 ] ) &&
                is_zero( m_matrix[ 0 ][ 2 ] ) &&
                is_zero( m_matrix[ 1 ][ 0 ] ) &&
                is_zero( m_matrix[ 1 ][ 2 ] ) &&
                is_zero( m_matrix[ 2 ][ 0 ] ) &&
                is_zero( m_matrix[ 2 ][ 1 ] ) );
    }

    Planc determinant( ) const
    {
        return( ( m_matrix[ 0 ][ 0 ] * ( ( m_matrix[ 1 ][ 1 ] * m_matrix[ 2 ][ 2 ] ) - ( m_matrix[ 1 ][ 2 ] * m_matrix[ 2 ][ 1 ] ) ) ) -
                ( m_matrix[ 0 ][ 1 ] * ( ( m_matrix[ 1 ][ 0 ] * m_matrix[ 2 ][ 2 ] ) - ( m_matrix[ 1 ][ 2 ] * m_matrix[ 2 ][ 0 ] ) ) ) +
                ( m_matrix[ 0 ][ 2 ] * ( ( m_matrix[ 1 ][ 0 ] * m_matrix[ 2 ][ 1 ] ) - ( m_matrix[ 1 ][ 1 ] * m_matrix[ 2 ][ 0 ] ) ) ) );
    }

    Transform translation_only( ) const { return( Transform( 1.0, 0.0, translation_x( ), 0.0, 1.0, translation_y( ), 0.0, 0.0, 1.0 ) ); }

    Vector translation( ) const { return( Vector( translation_x( ), translation_y( ) ) ); }

    Planc translation_x( ) const { return( m_matrix[ 0 ][ 2 ] ); }
    Planc translation_y( ) const { return( m_matrix[ 1 ][ 2 ] ); }

    Transform scale_only( ) const { return( Transform( scale_x( ), 0.0, 0.0, 0.0, scale_y( ), 0.0, 0.0, 0.0, 1.0 ) ); }

    Vector scale( ) const { return( Vector( scale_x( ), scale_y( ) ) ); }

    Planc scale_x( ) const { return( pythagorean( m_matrix[ 0 ][ 0 ], m_matrix[ 1 ][ 0 ] ) ); }
    Planc scale_y( ) const { return( pythagorean( m_matrix[ 0 ][ 1 ], m_matrix[ 1 ][ 1 ] ) ); }

    Transform rotation_only( ) const { Angle a = rotation( ); return( Transform( a.cos( ), -a.sin( ), 0.0, a.sin( ), a.cos( ), 0.0, 0.0, 0.0, 1.0 ) ); }

    Angle rotation( ) const { return( Angle( ( m_matrix[ 0 ][ 0 ] && m_matrix[ 1 ][ 1 ] ) ? atan( m_matrix[ 1 ][ 0 ] / m_matrix[ 1 ][ 1 ] ) : 0 ) ); }

    static Transform identity( )
    {
        return( Transform( ( 1.0 ), ( 0.0 ), ( 0.0 ),
                           ( 0.0 ), ( 1.0 ), ( 0.0 ),
                           ( 0.0 ), ( 0.0 ), ( 1.0 ) ) );
    }

    static Transform move( Vector cref in_vec )
    {
        return( Transform( ( 1.0 ), ( 0.0 ), ( in_vec.dx( ) ),
                           ( 0.0 ), ( 1.0 ), ( in_vec.dy( ) ),
                           ( 0.0 ), ( 0.0 ), ( 1.0 ) ) );
    }

    static Transform scale( Planc cref in_scale, Point cref in_origin = ORIGIN )
    {
        return( Transform( ( in_scale ), ( 0.0 ), ( in_origin.x( ) * one_minus( in_scale ) ),
                           ( 0.0 ), ( in_scale ), ( in_origin.y( ) * one_minus( in_scale ) ),
                           ( 0.0 ), ( 0.0 ), ( 1.0 ) ) );
    }

    static Transform scale( Vector cref in_vec )
    {
        return( Transform( ( in_vec.magnitude( ) ), ( 0.0 ), ( in_vec.origin( ).x( ) * one_minus( in_vec.magnitude( ) ) ),
                           ( 0.0 ), ( in_vec.magnitude( ) ), ( in_vec.origin( ).y( ) * one_minus( in_vec.magnitude( ) ) ),
                           ( 0.0 ), ( 0.0 ), ( 1.0 ) ) );
    }

    static Transform rotate( Angle cref in_a, Point cref in_origin = ORIGIN )
    {
        return( Transform( ( +in_a.cos( ) ), ( -in_a.sin( ) ), ( ( in_origin.x( ) * one_minus( in_a.cos( ) ) ) + ( in_origin.y( ) * in_a.sin( ) ) ),
                           ( +in_a.sin( ) ), ( +in_a.cos( ) ), ( ( in_origin.y( ) * one_minus( in_a.cos( ) ) ) - ( in_origin.x( ) * in_a.sin( ) ) ),
                           ( 0.0 ), ( 0.0 ), ( 1.0 ) ) );
    }

    static Transform reflect( Angle cref in_a )
    {
        return( Transform( ( +x2( in_a ).cos( ) ), ( +x2( in_a ).sin( ) ), ( 0.0 ),
                           ( +x2( in_a ).sin( ) ), ( -x2( in_a ).cos( ) ), ( 0.0 ),
                           ( 0.0 ), ( 0.0 ), ( 1.0 ) ) );
    }

    static Transform reflect( Vector cref in_vec )
    {
        return( Transform( ( +x2( in_vec.angle( ) ).cos( ) ), ( +x2( in_vec.angle( ) ).sin( ) ), ( ( in_vec.origin( ).x( ) * one_minus( x2( in_vec.angle( ) ).cos( ) ) ) - ( x2( in_vec.angle( ) ).sin( ) * in_vec.origin( ).y( ) ) ),
                           ( +x2( in_vec.angle( ) ).sin( ) ), ( -x2( in_vec.angle( ) ).cos( ) ), ( ( in_vec.origin( ).y( ) * one_plus( x2( in_vec.angle( ) ).cos( ) ) ) - ( x2( in_vec.angle( ) ).sin( ) * in_vec.origin( ).x( ) ) ),
                           ( 0.0 ), ( 0.0 ), ( 1.0 ) ) );
    }

    static Transform shear( Vector cref in_vec )
    {
        //https://matrixcalc.org/#{{1,0,x},{0,1,y},{0,0,1}}*{{1,m,0},{0,1,0},{0,0,1}}*{{m,0,0},{0,1,0},{0,0,1}}*{{c,-s,0},{s,c,0},{0,0,1}}^(-1)*{{1,0,x},{0,1,y},{0,0,1}}^(-1)

        Transform mt = move( in_vec.origin( ) );
        Transform rt = rotate( in_vec.angle( ) );
        Transform st = Transform( ( 1.0 ), ( in_vec.magnitude( ) ), ( 0.0 ),
                                  ( 0.0 ), ( 1.0 ), ( 0.0 ),
                                  ( 0.0 ), ( 0.0 ), ( 1.0 ) );

        return( mt * rt * st * rt.inverse( ) * mt.inverse( ) );

        // todo

        Planc m = ( in_vec.magnitude( ) );

        Planc x = ( in_vec.origin( ).x( ) );
        Planc y = ( in_vec.origin( ).y( ) );

        Angle a = ( in_vec.angle( ) );

        Planc s = ( a.sin( ) );
        Planc c = ( a.cos( ) );

        Planc ms = ( m * s );
        Planc mc = ( m * c );

        Planc t3 = ( ( x * s ) + ( y * c ) );

        return( Transform( ( -mc * s ), ( +mc * c ), ( +mc * t3 ),
                           ( -ms * s ), ( +ms * c ), ( +ms * t3 ),
                           ( 0.0 ), ( 0.0 ), ( 1.0 ) ) );
    }

    static Transform stretch( Vector cref in_vec )
    {
        //https://matrixcalc.org/#{{1,0,x},{0,1,y},{0,0,1}}*{{c,-s,0},{s,c,0},{0,0,1}}*{{m,0,0},{0,1,0},{0,0,1}}*{{c,-s,0},{s,c,0},{0,0,1}}^(-1)*{{1,0,x},{0,1,y},{0,0,1}}^(-1)

        Transform mt = move( in_vec.origin( ) );
        Transform rt = rotate( in_vec.angle( ) );
        Transform st = Transform( ( in_vec.magnitude( ) ), ( 0.0 ), ( 0.0 ),
                                  ( 0.0 ), ( 1.0 ), ( 0.0 ),
                                  ( 0.0 ), ( 0.0 ), ( 1.0 ) );

        return( mt * rt * st * rt.inverse( ) * mt.inverse( ) );

        // todo

        Planc m = ( in_vec.magnitude( ) );

        Planc x = ( in_vec.origin( ).x( ) );
        Planc y = ( in_vec.origin( ).y( ) );

        Angle a = ( in_vec.angle( ) );

        Planc s = ( a.sin( ) );
        Planc c = ( a.cos( ) );

        Planc t11 = ( ( m * square( c ) ) + square( s ) );
        Planc t22 = ( ( m * square( s ) ) + square( c ) );

        Planc t12 = ( ( s * c ) * ( m - 1.0 ) );
        Planc t21 = ( t12 );

        return( Transform( ( t11 ), ( t12 ), ( ( x * one_minus( t11 ) ) - ( y * t12 ) ),
                           ( t21 ), ( t22 ), ( ( y * one_minus( t22 ) ) - ( x * t21 ) ),
                           ( 0.0 ), ( 0.0 ), ( 1.0 ) ) );
    }

    static Transform squeeze( Vector cref in_vec )
    {
        //https://matrixcalc.org/#{{1,0,x},{0,1,y},{0,0,1}}*{{c,-s,0},{s,c,0},{0,0,1}}*{{m,0,0},{0,1/m,0},{0,0,1}}*{{c,-s,0},{s,c,0},{0,0,1}}^(-1)*{{1,0,x},{0,1,y},{0,0,1}}^(-1)

        return_if( !in_vec.has_magnitude( ), Transform( false ) );

        Transform mt = move( in_vec.origin( ) );
        Transform rt = rotate( in_vec.angle( ) );
        Transform st = Transform( ( in_vec.magnitude( ) ), ( 0.0 ), ( 0.0 ),
                                  ( 0.0 ), ( 1.0 / in_vec.magnitude( ) ), ( 0.0 ),
                                  ( 0.0 ), ( 0.0 ), ( 1.0 ) );

        return( mt * rt * st * rt.inverse( ) * mt.inverse( ) );

        // todo

        Planc m = ( in_vec.magnitude( ) );

        Planc x = ( in_vec.origin( ).x( ) );
        Planc y = ( in_vec.origin( ).y( ) );

        Angle a = ( in_vec.angle( ) );

        Planc s = ( a.sin( ) );
        Planc c = ( a.cos( ) );

        Planc t11 = ( m * c );
        Planc t21 = ( m * s );

        Planc t12 = ( -s / m );
        Planc t22 = ( +c / m );

        return( Transform( ( t11 ), ( t12 ), ( ( x * one_minus( t11 ) ) - ( y * t12 ) ),
                           ( t21 ), ( t22 ), ( ( y * one_minus( t22 ) ) - ( x * t21 ) ),
                           ( 0.0 ), ( 0.0 ), ( 1.0 ) ) );
    }

    Transform ref operator=( Transform cref in_trans ) { return( set( in_trans ) ); }

    Transform ref operator+=( Transform cref in_trans )
    {
        rethis = ( *this + in_trans );
    }

    Transform ref operator*=( Transform cref in_trans )
    {
        rethis = ( *this * in_trans );
    }

    Transform ref operator+=( dec in_d )
    {
        m_matrix[ 0 ][ 0 ] += in_d;
        m_matrix[ 0 ][ 1 ] += in_d;
        m_matrix[ 0 ][ 2 ] += in_d;
        m_matrix[ 1 ][ 0 ] += in_d;
        m_matrix[ 1 ][ 1 ] += in_d;
        m_matrix[ 1 ][ 2 ] += in_d;
        m_matrix[ 2 ][ 0 ] += in_d;
        m_matrix[ 2 ][ 1 ] += in_d;
        m_matrix[ 2 ][ 2 ] += in_d;

        rethis;
    }

    Transform ref operator-=( dec in_d )
    {
        m_matrix[ 0 ][ 0 ] -= in_d;
        m_matrix[ 0 ][ 1 ] -= in_d;
        m_matrix[ 0 ][ 2 ] -= in_d;
        m_matrix[ 1 ][ 0 ] -= in_d;
        m_matrix[ 1 ][ 1 ] -= in_d;
        m_matrix[ 1 ][ 2 ] -= in_d;
        m_matrix[ 2 ][ 0 ] -= in_d;
        m_matrix[ 2 ][ 1 ] -= in_d;
        m_matrix[ 2 ][ 2 ] -= in_d;

        rethis;
    }

    Transform ref operator*=( dec in_d )
    {
        m_matrix[ 0 ][ 0 ] *= in_d;
        m_matrix[ 0 ][ 1 ] *= in_d;
        m_matrix[ 0 ][ 2 ] *= in_d;
        m_matrix[ 1 ][ 0 ] *= in_d;
        m_matrix[ 1 ][ 1 ] *= in_d;
        m_matrix[ 1 ][ 2 ] *= in_d;
        m_matrix[ 2 ][ 0 ] *= in_d;
        m_matrix[ 2 ][ 1 ] *= in_d;
        m_matrix[ 2 ][ 2 ] *= in_d;

        rethis;
    }

    Transform ref operator/=( dec in_d )
    {
        m_matrix[ 0 ][ 0 ] /= in_d;
        m_matrix[ 0 ][ 1 ] /= in_d;
        m_matrix[ 0 ][ 2 ] /= in_d;
        m_matrix[ 1 ][ 0 ] /= in_d;
        m_matrix[ 1 ][ 1 ] /= in_d;
        m_matrix[ 1 ][ 2 ] /= in_d;
        m_matrix[ 2 ][ 0 ] /= in_d;
        m_matrix[ 2 ][ 1 ] /= in_d;
        m_matrix[ 2 ][ 2 ] /= in_d;

        rethis;
    }

    Transform operator+( Transform cref in_trans ) const
    {
        return( Transform(
            m_matrix[ 0 ][ 0 ] + in_trans.m_matrix[ 0 ][ 0 ],
            m_matrix[ 0 ][ 1 ] + in_trans.m_matrix[ 0 ][ 1 ],
            m_matrix[ 0 ][ 2 ] + in_trans.m_matrix[ 0 ][ 2 ],
            m_matrix[ 1 ][ 0 ] + in_trans.m_matrix[ 1 ][ 0 ],
            m_matrix[ 1 ][ 1 ] + in_trans.m_matrix[ 1 ][ 1 ],
            m_matrix[ 1 ][ 2 ] + in_trans.m_matrix[ 1 ][ 2 ],
            m_matrix[ 2 ][ 0 ] + in_trans.m_matrix[ 2 ][ 0 ],
            m_matrix[ 2 ][ 1 ] + in_trans.m_matrix[ 2 ][ 1 ],
            m_matrix[ 2 ][ 2 ] + in_trans.m_matrix[ 2 ][ 2 ] ) );
    }

    Transform operator*( Transform cref in_trans ) const
    {
        return( Transform(
            ( m_matrix[ 0 ][ 0 ] * in_trans.m_matrix[ 0 ][ 0 ] ) + ( m_matrix[ 0 ][ 1 ] * in_trans.m_matrix[ 1 ][ 0 ] ) + ( m_matrix[ 0 ][ 2 ] * in_trans.m_matrix[ 2 ][ 0 ] ),
            ( m_matrix[ 0 ][ 0 ] * in_trans.m_matrix[ 0 ][ 1 ] ) + ( m_matrix[ 0 ][ 1 ] * in_trans.m_matrix[ 1 ][ 1 ] ) + ( m_matrix[ 0 ][ 2 ] * in_trans.m_matrix[ 2 ][ 1 ] ),
            ( m_matrix[ 0 ][ 0 ] * in_trans.m_matrix[ 0 ][ 2 ] ) + ( m_matrix[ 0 ][ 1 ] * in_trans.m_matrix[ 1 ][ 2 ] ) + ( m_matrix[ 0 ][ 2 ] * in_trans.m_matrix[ 2 ][ 2 ] ),
            ( m_matrix[ 1 ][ 0 ] * in_trans.m_matrix[ 0 ][ 0 ] ) + ( m_matrix[ 1 ][ 1 ] * in_trans.m_matrix[ 1 ][ 0 ] ) + ( m_matrix[ 1 ][ 2 ] * in_trans.m_matrix[ 2 ][ 0 ] ),
            ( m_matrix[ 1 ][ 0 ] * in_trans.m_matrix[ 0 ][ 1 ] ) + ( m_matrix[ 1 ][ 1 ] * in_trans.m_matrix[ 1 ][ 1 ] ) + ( m_matrix[ 1 ][ 2 ] * in_trans.m_matrix[ 2 ][ 1 ] ),
            ( m_matrix[ 1 ][ 0 ] * in_trans.m_matrix[ 0 ][ 2 ] ) + ( m_matrix[ 1 ][ 1 ] * in_trans.m_matrix[ 1 ][ 2 ] ) + ( m_matrix[ 1 ][ 2 ] * in_trans.m_matrix[ 2 ][ 2 ] ),
            ( m_matrix[ 2 ][ 0 ] * in_trans.m_matrix[ 0 ][ 0 ] ) + ( m_matrix[ 2 ][ 1 ] * in_trans.m_matrix[ 1 ][ 0 ] ) + ( m_matrix[ 2 ][ 2 ] * in_trans.m_matrix[ 2 ][ 0 ] ),
            ( m_matrix[ 2 ][ 0 ] * in_trans.m_matrix[ 0 ][ 1 ] ) + ( m_matrix[ 2 ][ 1 ] * in_trans.m_matrix[ 1 ][ 1 ] ) + ( m_matrix[ 2 ][ 2 ] * in_trans.m_matrix[ 2 ][ 1 ] ),
            ( m_matrix[ 2 ][ 0 ] * in_trans.m_matrix[ 0 ][ 2 ] ) + ( m_matrix[ 2 ][ 1 ] * in_trans.m_matrix[ 1 ][ 2 ] ) + ( m_matrix[ 2 ][ 2 ] * in_trans.m_matrix[ 2 ][ 2 ] ) ) );
    }

    Transform operator+( dec in_d ) const
    {
        return( Transform(
            m_matrix[ 0 ][ 0 ] + in_d,
            m_matrix[ 0 ][ 1 ] + in_d,
            m_matrix[ 0 ][ 2 ] + in_d,
            m_matrix[ 1 ][ 0 ] + in_d,
            m_matrix[ 1 ][ 1 ] + in_d,
            m_matrix[ 1 ][ 2 ] + in_d,
            m_matrix[ 2 ][ 0 ] + in_d,
            m_matrix[ 2 ][ 1 ] + in_d,
            m_matrix[ 2 ][ 2 ] + in_d ) );
    }

    Transform operator-( dec in_d ) const
    {
        return( Transform(
            m_matrix[ 0 ][ 0 ] - in_d,
            m_matrix[ 0 ][ 1 ] - in_d,
            m_matrix[ 0 ][ 2 ] - in_d,
            m_matrix[ 1 ][ 0 ] - in_d,
            m_matrix[ 1 ][ 1 ] - in_d,
            m_matrix[ 1 ][ 2 ] - in_d,
            m_matrix[ 2 ][ 0 ] - in_d,
            m_matrix[ 2 ][ 1 ] - in_d,
            m_matrix[ 2 ][ 2 ] - in_d ) );
    }

    Transform operator*( dec in_d ) const
    {
        return( Transform(
            m_matrix[ 0 ][ 0 ] * in_d,
            m_matrix[ 0 ][ 1 ] * in_d,
            m_matrix[ 0 ][ 2 ] * in_d,
            m_matrix[ 1 ][ 0 ] * in_d,
            m_matrix[ 1 ][ 1 ] * in_d,
            m_matrix[ 1 ][ 2 ] * in_d,
            m_matrix[ 2 ][ 0 ] * in_d,
            m_matrix[ 2 ][ 1 ] * in_d,
            m_matrix[ 2 ][ 2 ] * in_d ) );
    }

    Transform operator/( dec in_d ) const
    {
        return( Transform(
            m_matrix[ 0 ][ 0 ] / in_d,
            m_matrix[ 0 ][ 1 ] / in_d,
            m_matrix[ 0 ][ 2 ] / in_d,
            m_matrix[ 1 ][ 0 ] / in_d,
            m_matrix[ 1 ][ 1 ] / in_d,
            m_matrix[ 1 ][ 2 ] / in_d,
            m_matrix[ 2 ][ 0 ] / in_d,
            m_matrix[ 2 ][ 1 ] / in_d,
            m_matrix[ 2 ][ 2 ] / in_d ) );
    }

    default_equal( Transform );

private:

    Transform( Planc cref in_p1,
               Planc cref in_p2,
               Planc cref in_p3,
               Planc cref in_p4,
               Planc cref in_p5,
               Planc cref in_p6,
               Planc cref in_p7,
               Planc cref in_p8,
               Planc cref in_p9 )
    {
        m_matrix[ 0 ][ 0 ] = in_p1;
        m_matrix[ 0 ][ 1 ] = in_p2;
        m_matrix[ 0 ][ 2 ] = in_p3;
        m_matrix[ 1 ][ 0 ] = in_p4;
        m_matrix[ 1 ][ 1 ] = in_p5;
        m_matrix[ 1 ][ 2 ] = in_p6;
        m_matrix[ 2 ][ 0 ] = in_p7;
        m_matrix[ 2 ][ 1 ] = in_p8;
        m_matrix[ 2 ][ 2 ] = in_p9;
    }

};

var_const( Transform ZERO_TRANSFORM( false ) );
var_const( Transform IDENTITY_TRANSFORM( true ) );

class Transformable
{

private:

    mutable Transform m_transform = IDENTITY_TRANSFORM;
    mutable Transform m_cumulative_transform = IDENTITY_TRANSFORM;

    mutable bool m_dirty = false;

public:

    Transformable( ) { }

    bool is_dirty( ) const { return( m_dirty ); }
    bool is_clean( ) const { return( !is_dirty( ) ); }

    Transform cref transform( ) const { return( m_transform ); }
    Transform cref cumulative_transform( ) const { return( m_cumulative_transform ); }

    Transform transform_inverse( ) const { return( m_transform.inverse( ) ); }
    Transform cumulative_transform_inverse( ) const { return( m_cumulative_transform.inverse( ) ); }

    Point apply_transform( Point cref in_pt ) const { return( transform( ).apply( in_pt ) ); }
    Point apply_cumulative_transform( Point cref in_pt ) const { return( cumulative_transform( ).apply( in_pt ) ); }

    Point apply_transform_inverse( Point cref in_pt ) const { return( transform_inverse( ).apply( in_pt ) ); }
    Point apply_cumulative_transform_inverse( Point cref in_pt ) const { return( cumulative_transform_inverse( ).apply( in_pt ) ); }

    virtual Transformable ref transform( Transform cref in_trans ) { dirty( ); m_transform.chain( in_trans ); m_cumulative_transform.chain( in_trans ); rethis; }
    virtual Transformable cref transform_const( Transform cref in_trans ) const { dirty( ); m_transform.chain( in_trans ); m_cumulative_transform.chain( in_trans ); rethis; }

    Transformable ref clear_transform( ) { dirty( ); m_transform = IDENTITY_TRANSFORM; rethis; }
    Transformable ref clear_cumulative_transform( ) { clear_transform( ); m_cumulative_transform = IDENTITY_TRANSFORM; rethis; }

    Transformable cref clear_transform_const( ) const { dirty( ); m_transform = IDENTITY_TRANSFORM; rethis; }
    Transformable cref clear_cumulative_transform_const( ) const { clear_transform_const( ); m_cumulative_transform = IDENTITY_TRANSFORM; rethis; }

    Transformable ref mirror_x( ) { return( mirror( XHAT ) ); }
    Transformable ref mirror_y( ) { return( mirror( YHAT ) ); }
    Transformable ref mirror( Vector cref in_vec ) { return( transform( Transform::reflect( in_vec ) ) ); }
    Transformable ref move( Vector cref in_vec ) { return( transform( Transform::move( in_vec ) ) ); }
    Transformable ref shear( Vector cref in_vec ) { return( transform( Transform::shear( in_vec ) ) ); }
    Transformable ref stretch( Vector cref in_vec ) { return( transform( Transform::stretch( in_vec ) ) ); }
    Transformable ref squeeze( Vector cref in_vec ) { return( transform( Transform::squeeze( in_vec ) ) ); }
    Transformable ref scale( Vector cref in_vec ) { return( transform( Transform::scale( in_vec ) ) ); }
    Transformable ref scale( Planc cref in_x, Point cref in_pt = ORIGIN ) { return( transform( Transform::scale( in_x, in_pt ) ) ); }
    Transformable ref rotate( Angle cref in_a, Point cref in_pt = ORIGIN ) { return( transform( Transform::rotate( in_a, in_pt ) ) ); }

    default_equal( Transformable );

protected:

    virtual Transformable cref dirty( ) const { m_dirty = true; rethis; }
    virtual Transformable cref clean( ) const { m_dirty = false; rethis; }

};

inline Point ref Point::transform( Transform cref in_trans ) { rethis = in_trans.apply( *this ); }

#define transform_functions( Class ) \
Class ref mirror_x( ) { Transformable::mirror_x( ); rethis; } \
Class ref mirror_y( ) { Transformable::mirror_y( ); rethis; } \
Class ref mirror( Vector cref in_vec ) { Transformable::mirror( in_vec ); rethis; } \
Class ref move( Vector cref in_vec ) { Transformable::move( in_vec ); rethis; } \
Class ref shear( Vector cref in_vec ) { Transformable::shear( in_vec ); rethis; } \
Class ref stretch( Vector cref in_vec ) { Transformable::stretch( in_vec ); rethis; } \
Class ref squeeze( Vector cref in_vec ) { Transformable::squeeze( in_vec ); rethis; } \
Class ref scale( Vector cref in_vec ) { Transformable::scale( in_vec ); rethis; } \
Class ref scale( Planc cref in_x, Point cref in_pt = ORIGIN ) { Transformable::scale( in_x, in_pt ); rethis; } \
Class ref rotate( Angle cref in_a, Point cref in_pt = ORIGIN ) { Transformable::rotate( in_a, in_pt ); rethis; } \

} // namespace geometry
} // namespace axn

#endif /* Transform_hpp */
