#include "Joint.hpp"

Joint::Joint( const Coordinate & _c1, const Planc & _arm_length_1, const Planc & _arm_length_2, const Coordinate & _c2, const bool _inward ) : m_c1( _c1 ), m_c2( _c2 ), m_arm_length_1( _arm_length_1 ), m_arm_length_2( _arm_length_2 ), m_inward( _inward )
{
    // Assert( in_range( m_c1.distance( m_c2 ), m_arm_length_1 + m_arm_length_2, m_arm_length_1 - m_arm_length_2 ), "joint is impossible" );
}

Coordinate Joint::c1( ) const
{
    return m_c1;
}

Coordinate Joint::c2( ) const
{
    return m_c2;
}

Coordinate Joint::joint( ) const
{
    Vector v = Vector( c1( ), c2( ) );
    Planc a = v.magnitude( );
    Planc b = arm_length_1( );
    Planc c = arm_length_2( );

    if( a < b + c )
    {
        return VectorA( Angle( flipped<dec>( acos( ( pow( a, 2 ) + pow( b, 2 ) - pow( c, 2 ) ) / ( a * b * 2.0 ) ), m_inward ) ) + v.angle( ), arm_length_1( ), c1( ) );
    }
    else
    {
        return ( v * b / ( b + c ) );
    }
}

Planc Joint::arm_length_1( ) const
{
    return m_arm_length_2;
}

Planc Joint::arm_length_2( ) const
{
    return m_arm_length_2;
}
