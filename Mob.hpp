#ifndef Mob_hpp
#define Mob_hpp

#include "Object.hpp"

namespace axn
{
namespace reality
{

class Mob : public Object
{
public:
    virtual ~Mob( ) { }
    Mob( World * world, const Coordinate & position, dec health = 1 );

#ifdef AXN_DEBUG
    virtual Drawing debug_overlay( ) const override;
#endif

    virtual bool alive( ) const;
    virtual Mob & die( );

    virtual dec health( ) const;
    virtual Mob & health( dec health ); // will increase max health if necessary
    virtual Mob & health_percentage( dec health_percentage );

    virtual Mob & heal( dec health ); // won't excede max health
    virtual Mob & heal_full( );
    virtual Mob & hurt( dec damage );

    virtual dec max_health( ) const;
    virtual Mob & max_health( dec health ); // will decrease health if necessary

    virtual bool facing_left( ) const;
    virtual Mob & facing_left( bool facing_left );

protected:
    virtual const Mob & render( ) const override;
    virtual Mob & update( ) override;
    virtual Mob & update_movement( ) override;
    virtual Mob & update_velocity( ) override;

protected:
    Counter m_blink_wait;
    Counter m_blink_duration;

private:
    bool m_alive = true;
    Slider<dec> m_health;

    bool m_facing_left = false;
};

} // namespace reality
} // namespace axn

#endif /* Mob_hpp */
