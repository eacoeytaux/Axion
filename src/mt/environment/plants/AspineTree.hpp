#ifndef AspineTree_hpp
#define AspineTree_hpp

#include "mt/environment/plants/Tree.hpp"

namespace mtmercy
{

class AspineTree : public Tree
{

public:

    AspineTree( ptr<Room> in_room, Point cref in_root, dec in_z = 1.0 );

    virtual Drawing leaf_drawing( ) const override;

private:

    Point m_leaf_base;
    Point m_leaf_side1;
    Point m_leaf_side2;

    Countdown m_leaf_drop_countdown1;
    Countdown m_leaf_drop_countdown2;

};

} // namespace mtmercy

#endif /* AspineTree_hpp */
