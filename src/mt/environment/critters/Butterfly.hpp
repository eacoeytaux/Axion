#ifndef Butterfly_hpp
#define Butterfly_hpp

#include "mt/MountMerciless.hpp"
#include "axn/world/Mob.hpp"

namespace mtmercy
{

class Butterfly : public Object
{

public:

    Butterfly( ptr<Room> in_room, Point cref in_position );

    Point anchor( ) const { return( m_anchor ); }
    Point target( ) const { return( m_target ); }

    virtual void render( ) override;
    virtual void update( ) override;

private:

    void new_target( );

    Point m_anchor;
    Point m_target;
    Countdown m_target_reset;

    Cycle m_wing_cycle;

    Color m_color;

};

} // namespace mtmercy

#endif /* Butterfly_hpp */
