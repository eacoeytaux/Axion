#ifndef WaterDrop_hpp
#define WaterDrop_hpp

#include "mt/MountMerciless.hpp"
#include "axn/world/Object.hpp"

namespace mtmercy
{

class WaterDrop : public Object
{

public:

    WaterDrop( ptr<Room> in_room, Point cref in_position, dec in_z = 1.0 );
    WaterDrop( ptr<Room> in_room, Point cref in_position, Vector cref in_upward, dec in_z = 1.0 );

    virtual void render( ) override;

    virtual void ground( ptr<Terrain::Node> in_node, Terrain::Bumper cref in_bumper ) override;

private:

    bool m_splashed;
    Planc m_radius;
    Color m_color;

};

} // namespace mtmercy

#endif /* WaterDrop_hpp */
