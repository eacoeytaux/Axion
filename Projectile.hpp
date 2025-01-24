#ifndef Projectile_hpp
#define Projectile_hpp

#include "MountMerciless.hpp"
#include "Object.hpp"

namespace mtmercy
{

class Projectile : public Object
{

private:

    Projectile( Room *, Coordinate cref position );

public:

    virtual void render( ) override;
    virtual void update( ) override;

    Damage damage( ) const { return m_damage; }

    bool active( ) const { return m_active; }
    void deactivate( );

protected:

    virtual bool collide( Object * ) override;
    virtual void react_to_movement( Object *, Vector cref ) override;
    virtual void ground( TerrainEdge * ) override;

private:

    bool m_active = true;
    Damage m_damage;
    Object * m_stuck_object;
};

} // namespace mtmercy

#endif /* Projectile_hpp */
