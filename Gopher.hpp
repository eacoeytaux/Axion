#ifndef Gopher_hpp
#define Gopher_hpp

#include "Enemy.hpp"

namespace mtmercy
{

class Gopher : public Enemy
{
public:
    virtual ~Gopher( ) { }
    Gopher( World *, const Coordinate & position );
    
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
    DirtBall( World *, const Planc & radius, const Coordinate & position, const Vector & velocity );
};

} // namespace mtmercy

#endif /* Gopher_hpp */
