#ifndef Porkupine_hpp
#define Porkupine_hpp

#include "mt/MountMerciless.hpp"
#include "axn/world/Enemy.hpp"

namespace mtmercy
{

class Porkupine : public Enemy
{

public:

    Porkupine( ptr<Room> in_room, Point cref in_position );

    virtual void render( ) override;

    virtual void update( ) override;

private:

    varray<Line> m_quills;

};

} // namespace mtmercy

#endif /* Porkupine_hpp */
