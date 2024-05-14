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
    Mob( World * world, const Coordinate & position, double health = 1 );

#ifdef AXN_DEBUG
    virtual Drawing debug_overlay( ) const override;
#endif

    virtual const Mob & render( ) const override
    {
        Object::render( );
        return *this;
    }
    virtual Mob & update( ) override;

    virtual bool alive( ) const;
    virtual Mob & die( );

    virtual double health( ) const;
    virtual Mob & health( double health ); // will increase max health if necessary
    virtual Mob & health_percentage( double health_percentage );

    virtual Mob & heal( double health );
    virtual Mob & heal_full( );
    virtual Mob & hurt( double damage );

    virtual uint max_health( ) const;
    virtual Mob & max_health( double health ); // will decrease health if necessary

    virtual bool facing_left( ) const;
    virtual Mob & facing_left( bool facing_left );

protected:
    virtual Mob & move( ) override;
    virtual Mob & update_velocity( ) override;

protected:
    Counter m_blink_wait;
    Counter m_blink_duration;

private:
    bool m_alive = true;
    Slider<double> m_health;

    bool m_facing_left = false;
};

} // namespace reality
} // namespace axn

#endif /* Mob_hpp */
