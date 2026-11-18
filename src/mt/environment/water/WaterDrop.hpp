#ifndef WaterDrop_hpp
#define WaterDrop_hpp

#include "mt/MountMerciless.hpp"
#include "axn/world/Object.hpp"

namespace mtmercy
{

class WaterDrop : public Object
{

private:

    WaterDrop( ptr<Room> in_room, Point cref in_position, Vector cref in_upward, dec in_z = 1.0 );

public:

    WaterDrop( ptr<Room> in_room, Point cref in_position, dec in_z = 1.0 );

    virtual void render( ) override;

protected:

    virtual void ground( Terrain::Bumper cref in_ground ) override;

private:

    bool m_splashed;
    Planc m_radius;
    Color m_color;

};

class WaterDropper : public Object
{

public:

    WaterDropper( ptr<Room> in_room, Point cref in_position, dec in_z = 1.0 );

    #if defined( AXN_DEBUG )
    virtual Drawing debug_overlay( ) const override;
    #endif

    virtual void update( ) override;

private:

    Countdown m_drip;

};

} // namespace mtmercy

#endif /* WaterDrop_hpp */
