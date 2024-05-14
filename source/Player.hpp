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
    virtual ~Player( ) { }
    Player( World * world, const Coordinate & position );

    virtual const Player & render( ) const override;
    virtual Player & update( ) override;

    virtual Player & input( Input * ) = 0;
    virtual Player & clear_input( ) = 0;

    virtual Planc light_sight( ) const = 0;

    virtual Player & hurt( double health ) override;

protected:
    virtual Player & die( ) override;

private:
    virtual Player & move( ) override;

    // -------------- GOD MODE --------------
public:
    bool god( ) const;
    Player & god( bool, uint64_t = 0 );
    // --------------------------------------
};

} // namespace reality
} // namespace axn

#endif /* Player_hpp */
