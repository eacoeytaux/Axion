#ifndef Joint_hpp
#define Joint_hpp

#include "axn.hpp"
#include "Geometry.hpp"

namespace axn {
namespace utility {

class Joint {
public:
    virtual ~Joint( ) {}
    Joint( const Coordinate & c1, const planc & arm_length_1, const planc & arm_length_2,
           const Coordinate & c2, bool inward = true );

    Coordinate c1( ) const;
    Coordinate c2( ) const;
    Coordinate joint( ) const;
    planc arm_length_1( ) const;
    planc arm_length_2( ) const;

private:
    Coordinate m_c1;
    Coordinate m_c2;
    planc m_arm_length_1;
    planc m_arm_length_2;
    bool m_inward;
};

} // namespace utility
} // namespace axn

#endif /* Joint_hpp */
