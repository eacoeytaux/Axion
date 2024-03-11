#ifndef Mob_hpp
#define Mob_hpp

#include "axn.hpp"
#include "Object.hpp"

namespace axn {
namespace reality {

class Mob : public Object {
public:
    virtual ~Mob( ) {}
    Mob( World* world, const Coordinate & position, uint health = 1 );

#ifdef AXN_DEBUG
    virtual Drawing debug_overlay( ) const override;
#endif

    virtual const Mob & render( ) const override {
        Object::render( );
        return *this;
    }
    virtual Mob & update( ) override;

    virtual bool alive( ) const;
    virtual Mob & die( );

    virtual uint health( ) const;
    virtual Mob & health( uint health ); // will increase max health if necessary
    virtual Mob & health( ufloat health_percentage );

    virtual Mob & heal( uint health );
    virtual Mob & heal_full( );
    virtual Mob & hurt( uint damage );

    virtual uint max_health( ) const;
    virtual Mob & max_health( uint health ); // will decrease health if necessary

    virtual bool facing_left( ) const;
    virtual Mob & facing_left( bool facing_left );

protected:
    virtual Mob & move( ) override;
    virtual Mob & adjust_velocity( ) override;

protected:
    Counter m_blink_wait_counter;
    Counter m_blink_duration_counter;

private:
    bool m_alive = true;
    Slider<uint> m_health;

    bool m_facing_left = false;
};

} // namespace reality
} // namespace axn

#endif /* Mob_hpp */
