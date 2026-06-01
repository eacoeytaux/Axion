#ifndef Arrow_hpp
#define Arrow_hpp

#include "mt/MountMerciless.hpp"
#include "axn/world/Projectile.hpp"

namespace mtmercy
{

class Arrow : public Projectile
{

private:

    Arrow( ptr<Room> in_room, Point cref in_tip, Vector cref in_launch_speed, Color cref in_feather_color );

public:

    static Arrow tip( ptr<Room> in_room, Point cref in_tip, Vector cref in_launch_speed, Color cref in_feather_color );
    static Arrow base( ptr<Room> in_room, Point cref in_base, Vector cref in_launch_speed, Color cref in_feather_color );

    virtual void render( ) override;

    virtual void update( ) override;

    Angle angle( ) const;
    void angle( Angle cref in_a );

    dec length( ) const;

protected:

    virtual void ground( ptr<Terrain::Node> in_node, Terrain::Bumper cref in_bumper ) override;
    virtual void react_to_movement( ptr<Object> in_object, Vector cref in_movement ) override;

    virtual void draw_head( );
    virtual void draw_shaft( );
    virtual void draw_feather( );

private:

    Damage m_damage;
    Angle m_angle;
    Color m_feather_color;

};

} // namespace mtmercy

#endif /* Arrow_hpp */
