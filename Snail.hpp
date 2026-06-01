#ifndef Snail_hpp
#define Snail_hpp

#include "MountMerciless.hpp"
#include "Mob.hpp"

namespace mtmercy
{

class Snail : public Mob
{

public:

    Snail( Room *, Coordinate cref position );

    virtual void render( ) override;
    virtual void update( ) override;

private:

};

} // namespace mtmercy

#endif /* Snail_hpp */
