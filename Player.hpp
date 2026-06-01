#ifndef Player_hpp
#define Player_hpp

#include "Mob.hpp"

namespace axn
{
namespace reality
{

class Player : public Mob
{

private:

    uint m_player_number;

public:

    Player( uint player_number, Room * room, Coordinate cref position );

    virtual void render( ) override;

    virtual void update( ) override;

    virtual void input( Input * ) = 0;
    virtual void clear_input( ) = 0;

    virtual Planc light_sight( ) const = 0;

    virtual bool damages( Damage cref ) const override;

    virtual void out_of_bounds( ) override;

    uint player_number( ) const { return m_player_number; }
    virtual bool layer_above( const Visible * v ) const override
    {
        if( const Player * player = dynamic_cast<const Player *>( v ) )
        {
            return ( player_number( ) > player->player_number( ) );
        }
        else
        {
            return false;
        }
    }

protected:

    virtual void die( ) override;

private:

    virtual void update_movement( ) override;


    // -------------- GOD MODE --------------

public:

    bool god( ) const { return m_god; }
    void god( bool god );

private:

    bool m_god = false;

};

} // namespace reality
} // namespace axn

#endif /* Player_hpp */
