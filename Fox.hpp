#ifndef Fox_hpp
#define Fox_hpp

#include "Mob.hpp"

namespace mtmercy
{

class Fox : public Mob
{
public:
    virtual ~Fox( ) { }
    Fox( Room *, Coordinate cref position );

    virtual void render( ) override;
};

} // namespace mtmercy

#endif /* Fox_hpp */
