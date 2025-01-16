#ifndef Gopher_hpp
#define Gopher_hpp

#include "Enemy.hpp"

namespace mtmercy
{

class Gopher : public Enemy
{
public:
    virtual ~Gopher( ) { }
    Gopher( Room *, Coordinate cref position );

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
    virtual ~DirtBall( ) { }
    DirtBall( Room *, Planc cref radius, Coordinate cref position, Vector cref velocity );
};

} // namespace mtmercy

#endif /* Gopher_hpp */
