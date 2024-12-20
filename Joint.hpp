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

    Planc m_arm_length_1;
    Planc m_arm_length_2;

    bool m_inward;

public:
    Joint( const Coordinate & c1, const Planc & arm_length_1, const Planc & arm_length_2, const Coordinate & c2, const bool inward = true ) : m_c1( c1 ), m_c2( c2 ), m_arm_length_1( arm_length_1 ), m_arm_length_2( arm_length_2 ), m_inward( inward ) { }

    Coordinate c1( ) const { return m_c1; }
    Coordinate c2( ) const { return m_c2; }

    Planc arm_length_1( ) const { return m_arm_length_2; }
    Planc arm_length_2( ) const { return m_arm_length_2; }

    bool inward( ) const { return m_inward; }

    Coordinate joint( ) const
    {
        Vector v = Vector( c1( ), c2( ) );
        Planc a = v.magnitude( );
        Planc b = arm_length_1( );
        Planc c = arm_length_2( );

        if( a < b + c )
        {
            return Vector::A( Angle( negate( acos( ( square( a ) + square( b ) - square( c ) ) / ( a * b * TWO ) ), m_inward ) ) + v.angle( ), arm_length_1( ), c1( ) );
        }
        else
        {
            return ( v * b / ( b + c ) );
        }
    }
};

} // namespace axn

#endif /* Joint_hpp */
