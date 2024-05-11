#ifndef Joint_hpp
#define Joint_hpp

#include "Geometry.hpp"

namespace axn
{

class Joint
{
public:
    virtual ~Joint( ) { }
    Joint( const Coordinate & c1, const Planc & arm_length_1, const Planc & arm_length_2, const Coordinate & c2, bool inward = true );

    Coordinate c1( ) const;
    Coordinate c2( ) const;
    Coordinate joint( ) const;
    Planc arm_length_1( ) const;
    Planc arm_length_2( ) const;

private:
    Coordinate m_c1;
    Coordinate m_c2;
    Planc m_arm_length_1;
    Planc m_arm_length_2;
    bool m_inward;
};

} // namespace axn

#endif /* Joint_hpp */
