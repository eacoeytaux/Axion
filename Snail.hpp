#ifndef Snail_hpp
#define Snail_hpp

#include "Mob.hpp"

namespace mtmercy
{

class Snail : public Mob
{
public:
    virtual ~Snail( ) { }
    Snail( World *, const Coordinate & position );

    virtual void render( ) override;
    virtual void update( ) override;

private:
};

} // namespace mtmercy

#endif /* Snail_hpp */
