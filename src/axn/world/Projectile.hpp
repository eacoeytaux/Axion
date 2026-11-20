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

    virtual bool collide( ptr<Object> in_object ) override;

    virtual void ground( Terrain::Bumper cref in_ground ) override;

private:

    bool m_active = false; // must be activated (the constructor does)

    bool m_player_only = false;

    Damage m_damage;

    varray<ptr<Object>> m_ignored_objects;

};

} // namespace mtmercy

#endif /* Projectile_hpp */
