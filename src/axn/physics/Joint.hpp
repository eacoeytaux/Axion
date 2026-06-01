#ifndef Joint_hpp
#define Joint_hpp

#include "axn/geometry/Geometry.hpp"

namespace axn
{

class Joint
{

private:

    Point m_pt1;
    Point m_pt2;

    Planc m_arm_length1;
    Planc m_arm_length2;

    bool m_inward;

public:

    Joint( Point cref in_pt1, Point cref in_pt2, Planc cref in_arm_length1, Planc cref in_arm_length2, bool in_inward = true ) : m_pt1( in_pt1 ), m_pt2( in_pt2 ), m_arm_length1( in_arm_length1 ), m_arm_length2( in_arm_length2 ), m_inward( in_inward ) { }

    Point pt1( ) const { return( m_pt1 ); }
    Point pt2( ) const { return( m_pt2 ); }

    Planc arm_length1( ) const { return( m_arm_length1 ); }
    Planc arm_length2( ) const { return( m_arm_length2 ); }

    Line arm1( ) const { return( Line( pt1( ), joint( ) ) ); }
    Line arm2( ) const { return( Line( joint( ), pt2( ) ) ); }

    bool inward( ) const { return( m_inward ); }

    Point joint( ) const
    {
        Vector v = Vector( pt1( ), pt2( ) );
        Planc m = v.magnitude( );

        Planc a1 = arm_length1( );
        Planc a2 = arm_length2( );
        Planc aT = ( a1 + a2 );

        if( m < aT )
        {
            return( VectorA( Angle( negated( acos( ( square( m ) + square( a1 ) - square( a2 ) ) / x2( m * a1 ) ), m_inward ) ) + v.angle( ), arm_length1( ), pt1( ) ) );
        }
        else
        {
            return( v * ( a1 / aT ) );
        }
    }

};

} // namespace axn

#endif /* Joint_hpp */
