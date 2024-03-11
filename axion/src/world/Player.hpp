#ifndef Player_hpp
#define Player_hpp

#include "axn.hpp"
#include "Mob.hpp"
#include "Input.hpp"
#include "Hook.hpp"

namespace axn {
namespace reality {

class Player : public Mob {
public:
    virtual ~Player( ) {}
    Player( World* world, const Coordinate & position );

    virtual const Player & render( ) const override;
    virtual Player & update( ) override;

    virtual Player & input( Input* ) = 0;

    virtual Player & hurt( uint health ) override;

protected:
    virtual Player & die( ) override;

private:
    virtual Player & move( ) override;

    Camera m_camera;

    // give player the caMERA todo TODO

    // GOD MODE
public:
    bool god( ) const;
    Player & god( bool, uint64_t );
};

} // namespace reality
} // namespace axn

#endif /* Player_hpp */
