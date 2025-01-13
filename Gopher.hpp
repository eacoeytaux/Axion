#ifndef Gopher_hpp
#define Gopher_hpp

#include "Enemy.hpp"

namespace mtmercy
{

class Gopher : public Enemy
{
public:
    virtual ~Gopher( ) { }
    Gopher( World *, Coordinate cref position );

    virtual void render( ) override;

    virtual void update( ) override;

private:
    Counter m_reload;

    STATE( EXPOSED,
           BURIED,
           ASCENDING,
           DESCENDING );
};

class DirtBall : public Object
{
public:
    virtual ~DirtBall( ) { }
    DirtBall( World *, Planc cref radius, Coordinate cref position, Vector cref velocity );
};

} // namespace mtmercy

#endif /* Gopher_hpp */
