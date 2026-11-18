#ifndef Particle_hpp
#define Particle_hpp

#include "axn/world/Object.hpp"

namespace axn
{

class Particle : public Object
{

public:

    Particle( ptr<Room> in_room, Point cref in_position, Vector cref in_velocity = V0 );

    virtual void render( ) override;
    virtual void update( ) override;

    virtual void pop( );

protected:

    virtual bool collide( ptr<Object> in_object ) override;

    virtual void ground( ptr<Terrain::Node> in_node, Terrain::Bumper cref in_bumper ) override;

private:

    Planc m_radius;
    Angle m_rotation;
    uint m_sides;

    Planc m_radius_growth;
    Angle m_rotation_speed;

    Color m_color;
    dec m_alpha;

};

} // namespace mtmercy

#endif /* Particle_hpp */
