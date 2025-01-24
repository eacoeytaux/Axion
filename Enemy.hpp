#ifndef Enemy_hpp
#define Enemy_hpp

#include "Mob.hpp"

namespace axn
{
namespace reality
{

class Enemy : public Mob
{

public:

    Enemy( Room * room, Coordinate cref position, dec health = 1.0 ) : Mob( room, position, health ) { }

protected:

    #ifdef AXN_DEBUG
    virtual Drawing debug_overlay( ) const override;
    #endif

    virtual void update( ) override;

    bool aggressive( ) const { return m_aggressive; }
    void aggressive( bool aggressive ) { m_aggressive = aggressive; }

    bool has_target( ) const { return target( ); }
    Object * target( ) const { return m_target; }

    virtual void set_target( Object * target ) { m_target = target; }
    virtual void clear_target( ) { set_target( nullptr ); }
    virtual bool overrides_target( const Object * target ) const;

    bool target_locked( ) const { return m_target_locked; }
    void target_locked( bool locked ) { m_target_locked = locked; }

    Planc cref sight_range( ) const { return m_sight_range; }
    void sight_range( Planc cref range );
    virtual bool in_sight_range( const Player * player ) const;

    Planc cref alert_range( ) const { return m_alert_range; }
    void alert_range( Planc cref range );
    virtual bool in_alert_range( const Player * player ) const;

private:

    bool m_aggressive = true;

    Object * m_target = nullptr;
    bool m_target_locked = false;

    // zero range means no limit
    Planc m_sight_range;
    Planc m_alert_range;
};

} // namespace reality
} // namespace axn

#endif /* Enemy_hpp */
