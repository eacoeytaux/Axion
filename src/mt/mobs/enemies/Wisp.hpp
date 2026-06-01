#ifndef Wisp_hpp
#define Wisp_hpp

#include "mt/MountMerciless.hpp"
#include "axn/world/Enemy.hpp"
#include "axn/world/Projectile.hpp"
#include "mt/environment/misc/Fire.hpp"

namespace mtmercy
{

class Wisp : public Enemy
{

public:

    Wisp( ptr<Room> in_room, Point cref in_position );

    virtual void render( ) override;
    virtual void update( ) override;

    void die( ) override;

    void move( Vector cref in_movement ) override;

private:

    Vector face_offset( ) const;

    Angle m_rotation = A0;
    Angle m_rotation_speed = A0;

    Countdown m_reload_timer;
    Fire m_fire;

public:

    class Bullet : public Projectile
    {
    public:

        Bullet( ptr<Room> in_room, Point cref in_position, Vector cref in_velocity );

        virtual void render( ) override;
        virtual void update( ) override;

        virtual void activate( bool in_activate ) override;

    private:

        Planc m_radius;
        Planc m_radius_max;

        Fire m_fire;
    };


};

} // namespace mtmercy

#endif /* Wisp_hpp */
