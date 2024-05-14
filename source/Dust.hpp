#ifndef Dust_hpp
#define Dust_hpp

#include "Object.hpp"

namespace mtmercy
{

class Dust : public Object
{
public:
    virtual ~Dust( ) { }
    Dust( World *, const Coordinate & position, const Vector & velocity, const Color & color );

    virtual const Dust & render( ) const override;
    virtual Dust & update( ) override;

private:
    Planc m_radius;
    double m_alpha;
    uint m_sides;
    Angle m_rotation;
    Angle m_rotation_speed;
    Color m_color;
};

} // namespace mtmercy

#endif /* Dust_hpp */
