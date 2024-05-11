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

    virtual const Snail & render( ) const override;
    virtual Snail & update( ) override;

private:
};

} // namespace mtmercy

#endif /* Snail_hpp */
