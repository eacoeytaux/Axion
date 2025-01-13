#ifndef Dust_hpp
#define Dust_hpp

#include "Object.hpp"

namespace mtmercy
{

class Dust : public Object
{
public:
    virtual ~Dust( ) { }
    Dust( World *, Coordinate cref position, Vector cref velocity, Color cref color );

    virtual void render( ) override;

    virtual void update( ) override;

private:
    Planc m_radius;
    dec m_alpha;
    uint m_sides;
    Angle m_rotation;
    Angle m_rotation_speed;
    Color m_color;
};

} // namespace mtmercy

#endif /* Dust_hpp */
