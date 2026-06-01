#ifndef Enemy_hpp
#define Enemy_hpp

#include "axn/world/Mob.hpp"

namespace axn
{
namespace reality
{

class Enemy : public Mob
{

public:

    Enemy( ptr<Room> in_room, Point cref in_position, dec in_health = 1.0 ) : Mob( in_room, in_position, in_health ) { }

protected: // todo should this be the case for all objects?

    #if defined( AXN_DEBUG )
    virtual Drawing debug_overlay( ) const override;
    #endif

    virtual void update( ) override;

    bool aggressive( ) const { return( m_aggressive ); }
    void aggressive( bool in_aggressive ) { m_aggressive = in_aggressive; }

    bool has_target( ) const { return( target( ) ); }
    ptr<Object> target( ) const { return( m_target ); }

    virtual void set_target( ptr<Object> in_target ) { m_target = in_target; }
    virtual void clear_target( ) { set_target( nullptr ); }
    virtual bool overrides_target( ptr<const Object> in_target ) const;

    bool target_locked( ) const { return( m_target_locked ); }
    void target_locked( bool in_locked ) { m_target_locked = in_locked; }

    Planc cref sight_range( ) const { return( m_sight_range ); }
    void sight_range( Planc cref in_range );
    virtual bool in_sight_range( ptr<const Player> in_player ) const;

    Planc cref alert_range( ) const { return( m_alert_range ); }
    void alert_range( Planc cref in_range );
    virtual bool in_alert_range( ptr<const Player> in_player ) const;

private:

    bool m_aggressive = true;

    ptr<Object> m_target = nullptr;
    bool m_target_locked = false;

    // zero range means no limit
    Planc m_sight_range;
    Planc m_alert_range;

};

} // namespace reality
} // namespace axn

#endif /* Enemy_hpp */
