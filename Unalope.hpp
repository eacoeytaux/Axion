#ifndef Unalope_hpp
#define Unalope_hpp

#include "MountMerciless.hpp"
#include "Enemy.hpp"

namespace mtmercy
{

class Unalope : public Enemy
{

public:

    Unalope( Room *, Coordinate cref position );

    virtual void render( ) override;

    virtual void update( ) override;
};

} // namespace mtmercy

#endif /* Unalope_hpp */
