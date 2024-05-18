#ifndef PineTree_hpp
#define PineTree_hpp

#include "Object.hpp"

namespace mtmercy
{

class PineTree : public Object
{
public:
    virtual ~PineTree( ) { }
    PineTree( World *, const Coordinate & root, dec z = 1.0 );

    virtual const PineTree & render( ) const override;

private:
    varray<Drawing> m_leave_bunches_and_trunk;
    mutable varray<Coordinate> m_leave_bunches_rotation_points;
};

} // namespace mtmercy

#endif /* PineTree_hpp */
