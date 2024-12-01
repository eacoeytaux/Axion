#ifndef Porkupine_hpp
#define Porkupine_hpp

#include "Enemy.hpp"

namespace mtmercy
{

class Porkupine : public Enemy
{
public:
    virtual ~Porkupine( ) { }
    Porkupine( World *, const Coordinate & position );

    virtual void render( ) override;

    virtual void update( ) override;

private:
    varray<Line> m_quills;
};

} // namespace mtmercy

#endif /* Porkupine_hpp */
