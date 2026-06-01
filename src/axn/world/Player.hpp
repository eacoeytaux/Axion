#ifndef Player_hpp
#define Player_hpp

#include "axn/world/Mob.hpp"

namespace axn
{
namespace reality
{

class Player : public Mob
{

private:

    uint m_player_number;
    Color m_player_color;

public:

    Player( uint in_player_number, ptr<Room> in_room, Point cref in_position );

    virtual void render( ) override;

    virtual void update( ) override;

    virtual void input( ptr<Input> in_input ) override = 0;
    virtual void clear_input( ) override = 0;

    virtual void add_force( Vector cref in_force ) override { if( !god( ) ) { Mob::add_force( in_force ); } }

    virtual Planc light_sight( ) const = 0;

    virtual void hurt( dec in_health ) override { if( !god( ) ) { Mob::hurt( in_health ); } }

    virtual bool damages( Damage cref in_damage ) const override;

    virtual bool collide( ptr<Object> in_object );

    virtual bool opening_door( ) const { return( false ); }

    virtual void out_of_bounds( ) override;

    virtual bool layer_above( ptr<const Visible> in_visible ) const override { if( cast_as( player, const Player, in_visible ) ) { return( player_number( ) > player->player_number( ) ); } else { return( Mob::layer_above( in_visible ) ); } }

    uint player_number( ) const { return( m_player_number ); }

    Color cref player_color( ) const { return( m_player_color ); }
    void player_color( Color cref in_color ) { m_player_color = in_color; }

protected:

    virtual void die( ) override;

    virtual void update_movement( ) override;

    // -------------- GOD MODE --------------

public:

    bool god( ) const { return( m_god ); }
    void god( bool in_god );

private:

    bool m_god = false;

};

} // namespace reality
} // namespace axn

#endif /* Player_hpp */
