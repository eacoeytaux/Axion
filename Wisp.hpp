#ifndef Wisp_hpp
#define Wisp_hpp

#include "MountMerciless.hpp"
#include "Enemy.hpp"
#include "Fire.hpp"

namespace mtmercy
{

class Wisp : public Enemy
{
    
public:

    Wisp( Room *, Coordinate cref position );

    virtual void render( ) override;
    virtual void update( ) override;
    
    void die( ) override;

    void move( Vector cref ) override;

private:

    Countdown m_reload_timer;
    Fire m_fire;
    
public:

    class Bullet : public Object
    {
    public:

        Bullet( Room *, Coordinate cref position, Vector cref velocity );

        virtual void render( ) override;
        virtual void update( ) override;

        void move( Vector cref ) override;

        bool collide( Object * object ) override;

    private:

        Fire m_fire;
    };

    
};

} // namespace mtmercy

#endif /* Wisp_hpp */
