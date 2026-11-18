#ifndef Projectile_hpp
#define Projectile_hpp

#include "axn/world/Object.hpp"

namespace axn
{

class Projectile : public Object
{

protected:

    Projectile( ptr<Room> in_room, Damage cref in_damage, Point cref in_position, Vector cref in_velocity = V0 );

public:

    virtual void render( ) override;
    virtual void update( ) override;

    Damage damage( ) const { return( m_damage ); }

    bool active( ) const { return( m_active ); }

    virtual void activate( bool in_activate );

protected:

    virtual void react_to_movement( ptr<Object> in_object, Vector cref in_movement ) override;

    virtual bool collide( ptr<Object> in_object ) override;

    virtual void ground( ptr<Terrain::Node> in_node, Terrain::Bumper cref in_bumper ) override;

private:

    bool m_active = false; // must be activated

    bool m_player_only;

    Damage m_damage;

    varray<ptr<Object>> m_ignored_objects;

};

} // namespace mtmercy

#endif /* Projectile_hpp */
