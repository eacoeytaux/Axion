#ifndef Porkupine_hpp
#define Porkupine_hpp

#include "MountMerciless.hpp"
#include "Enemy.hpp"

namespace mtmercy
{

class Porkupine : public Enemy
{
public:
    Porkupine( Room *, Coordinate cref position );

    virtual void render( ) override;

    virtual void update( ) override;

private:
    varray<Line> m_quills;
};

} // namespace mtmercy

#endif /* Porkupine_hpp */
