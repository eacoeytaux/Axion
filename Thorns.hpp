#ifndef Dust_hpp
#define Dust_hpp

#include "MountMerciless.hpp"
#include "Hazard.hpp"

namespace mtmercy
{

class Thorns : public Hazard
{

public:

    Thorns( Room *, const Coordinate &, const Coordinate & );

    bool collide( Object * ) override;

};

} // namespace mtmercy

#endif /* Dust_hpp */
