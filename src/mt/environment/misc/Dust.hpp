#ifndef Dust_hpp
#define Dust_hpp

#include "mt/MountMerciless.hpp"
#include "axn/world/Object.hpp"

namespace mtmercy
{

class Dust : public Object
{

public:

    Dust( ptr<Room> in_room, Point cref in_position, Vector cref in_velocity, Color cref in_color );

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
