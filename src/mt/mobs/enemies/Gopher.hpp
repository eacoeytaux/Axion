#ifndef Gopher_hpp
#define Gopher_hpp

#include "mt/MountMerciless.hpp"
#include "axn/world/Enemy.hpp"

namespace mtmercy
{

class Gopher : public Enemy
{

public:

    Gopher( ptr<Room> in_room, Point cref in_position );

    virtual void render( ) override;

    virtual void update( ) override;

private:

    Countdown m_reload_timer;

    STATE( EXPOSED,
           BURIED,
           ASCENDING,
           DESCENDING );

};

class DirtBall : public Object
{

public:

    DirtBall( ptr<Room> in_room, Planc cref in_radius, Point cref in_position, Vector cref in_velocity );

};

} // namespace mtmercy

#endif /* Gopher_hpp */
