#ifndef Fox_hpp
#define Fox_hpp

#include "Mob.hpp"

namespace mtmercy
{

class Fox : public Mob
{
public:
    virtual ~Fox( ) { }
    Fox( World *, const Coordinate & position );

    virtual const Fox & render( ) const override;
    virtual Fox & update( ) override;

private:
};

} // namespace mtmercy

#endif /* Fox_hpp */
