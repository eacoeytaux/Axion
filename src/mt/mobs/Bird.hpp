#ifndef Bird_hpp
#define Bird_hpp

#include "mt/MountMerciless.hpp"
#include "axn/world/Mob.hpp"

namespace mtmercy
{

class Feather : public Object
{

public:

    Feather( ptr<Room> in_room, Point cref in_position );

    virtual void render( ) override;

private:

    Color m_color;

};

class Bird : public Mob
{

public:

    Bird( ptr<Room> in_room, Point cref in_position );

    virtual void render( ) override;

    virtual void update( ) override;

    virtual void die( ) override;

};

} // namespace mtmercy

#endif /* Bird_hpp */
