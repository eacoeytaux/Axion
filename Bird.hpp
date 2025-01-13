#ifndef Bird_hpp
#define Bird_hpp

#include "Mob.hpp"

namespace mtmercy
{

class Feather : public Object
{
public:
    Feather( World *, Coordinate cref position );

    virtual void render( ) override;

private:
    Color m_color;
};

class Bird : public Mob
{
public:
    Bird( World *, Coordinate cref position );

    virtual void render( ) override;

    virtual void update( ) override;

    virtual void die( ) override;

private:
};

} // namespace mtmercy

#endif /* Bird_hpp */
