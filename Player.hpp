#ifndef Player_hpp
#define Player_hpp

#include "Mob.hpp"

namespace axn
{
namespace reality
{

class Player : public Mob
{
public:
    Player( Room * room, Coordinate cref position );

    virtual void render( ) override;

    virtual void update( ) override;

    virtual void input( Input * ) = 0;
    virtual void clear_input( ) = 0;

    virtual Planc light_sight( ) const = 0;

    virtual void hurt( dec health ) override;

    virtual void out_of_bounds( ) override;

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
    // --------------------------------------
};

} // namespace reality
} // namespace axn

#endif /* Player_hpp */
