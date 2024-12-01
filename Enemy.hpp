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
    virtual ~Enemy( ) { }
    Enemy( World * world, const Coordinate & position, dec health = 1.0 ) : Mob( world, position, health ) { }

protected:
    #ifdef AXN_DEBUG
    virtual Drawing debug_overlay( ) const override;
    #endif

    virtual void update( ) override;

    bool aggressive( ) const { return m_aggressive; }
    void aggressive( bool aggressive ) { m_aggressive = aggressive; }

    bool has_target( ) const { return target( ); }
    Player * target( ) const { return m_target; }

    virtual void set_target( Player * player ) { m_target = player; }
    virtual void clear_target( ) { set_target( nullptr ); }
    virtual bool overrides_target( const Player * player ) const;

    bool target_locked( ) const { return m_target_locked; }
    void target_locked( bool locked ) { m_target_locked = locked; }

    const Planc & sight_range( ) const { return m_sight_range; }
    void sight_range( const Planc & range );
    virtual bool in_sight_range( const Player * player ) const;

    const Planc & alert_range( ) const { return m_alert_range; }
    void alert_range( const Planc & range );
    virtual bool in_alert_range( const Player * player ) const;

private:
    bool m_aggressive = true;

    Player * m_target = nullptr;
    bool m_target_locked = false;

    // zero range means no limit
    Planc m_sight_range;
    Planc m_alert_range;
};

} // namespace reality
} // namespace axn

#endif /* Enemy_hpp */
