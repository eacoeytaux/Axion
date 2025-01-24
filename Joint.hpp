#ifndef Joint_hpp
#define Joint_hpp

#include "Geometry.hpp"

namespace axn
{

class Joint
{

private:

    Coordinate m_c1;
    Coordinate m_c2;

    Planc m_arm_length1;
    Planc m_arm_length2;

    bool m_inward;

public:

    Joint( Coordinate cref c1, Coordinate cref c2, Planc cref arm_length1, Planc cref arm_length2, cbool inward = true ) : m_c1( c1 ), m_c2( c2 ), m_arm_length1( arm_length1 ), m_arm_length2( arm_length2 ), m_inward( inward ) { }

    Coordinate c1( ) const { return m_c1; }
    Coordinate c2( ) const { return m_c2; }

    Planc arm_length1( ) const { return m_arm_length1; }
    Planc arm_length2( ) const { return m_arm_length2; }

    Line arm1( ) const { return Line( c1( ), joint( ) ); }
    Line arm2( ) const { return Line( joint( ), c2( ) ); }

    bool inward( ) const { return m_inward; }

    Coordinate joint( ) const
    {
        Vector v = Vector( c1( ), c2( ) );
        Planc m = v.magnitude( );

        Planc a1 = arm_length1( );
        Planc a2 = arm_length2( );
        Planc aT = ( a1 + a2 );

        if( m < aT )
        {
            return Vector::A( Angle( negate( acos( ( square( m ) + square( a1 ) - square( a2 ) ) / ( m * a1 * 2.0 ) ), m_inward ) ) + v.angle( ), a1, c1( ) );
        }
        else
        {
            return ( v * ( a1 / aT ) );
        }
    }
};

} // namespace axn

#endif /* Joint_hpp */
