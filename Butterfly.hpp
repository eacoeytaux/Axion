#ifndef Butterfly_hpp
#define Butterfly_hpp

#include "Mob.hpp"

namespace mtmercy
{

class Butterfly : public Mob
{
public:
    Butterfly( Room *, Coordinate cref position );

    virtual void render( ) override;

private:
};

} // namespace mtmercy

#endif /* Butterfly_hpp */
