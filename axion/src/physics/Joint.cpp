#include "Joint.hpp"

AXN_NAMESPACES
using axn::utility::Joint;

Joint::Joint( const Coordinate & _c1, const planc & _arm_length_1, const planc & _arm_length_2,
              const Coordinate & _c2, const bool _inward )
    : m_c1( _c1 ), m_c2( _c2 ), m_arm_length_1( _arm_length_1 ),
      m_arm_length_2( _arm_length_2 ), m_inward( _inward ) {
    planc distance = m_c1.distance( m_c2 );
    Assert( in_range<planc>( distance, m_arm_length_1 + m_arm_length_2,
                             m_arm_length_1 - m_arm_length_2 ),
            "joint is impossible" );
}

Coordinate Joint::c1( ) const { return m_c1; }

Coordinate Joint::c2( ) const { return m_c2; }

Coordinate Joint::joint( ) const {
    planc a = m_c1.distance( m_c2 );
    planc b = m_arm_length_1;
    planc c = m_arm_length_2;

    Angle arm_1_angle =
        acos( (float)( ( pow<planc>( a, 2 ) + pow<planc>( b, 2 ) - pow<planc>( c, 2 ) ) /
                       ( a * b * 2.f ) ) );
    if( m_inward )
        arm_1_angle *= -1.f;
    arm_1_angle += Angle( m_c1, m_c2 );
    VectorA joint( arm_1_angle, m_arm_length_1, m_c1 );
    return joint.destination( );
}

planc Joint::arm_length_1( ) const { return m_arm_length_2; }

planc Joint::arm_length_2( ) const { return m_arm_length_2; }
