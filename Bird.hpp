#ifndef Bird_hpp
#define Bird_hpp

#include "MountMerciless.hpp"
#include "Mob.hpp"

namespace mtmercy
{

class Feather : public Object
{

public:

    Feather( Room *, Coordinate cref position );

    virtual void render( ) override;

private:

    Color m_color;
};

class Bird : public Mob
{

public:

    Bird( Room *, Coordinate cref position );

    virtual void render( ) override;

    virtual void update( ) override;

    virtual void die( ) override;
};

} // namespace mtmercy

#endif /* Bird_hpp */
