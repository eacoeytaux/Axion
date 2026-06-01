#ifndef AspineTree_hpp
#define AspineTree_hpp

#include "mt/MountMerciless.hpp"
#include "axn/world/Object.hpp"

namespace mtmercy
{

class AspineTree : public Object
{

public:

    AspineTree( ptr<Room> in_room, Point cref in_root, dec in_z = 1.0 );

    void update( ) override;

    class Leaf : public Object
    {

    public:

        Leaf( ptr<Room> in_room, Color cref in_color, Point cref in_center, dec in_z = 1.0 );

        void render( ) override;

    protected:

        void ground( ptr<Terrain::Node> in_node, Terrain::Bumper cref in_bumper ) override;

    private:

        dec m_scale;
        Angle m_offset;

        Color m_color;
    };

private:

    Point m_leaf_base;
    Point m_leaf_side1;
    Point m_leaf_side2;

    Countdown m_leaf_drop_countdown1;
    Countdown m_leaf_drop_countdown2;

    Color m_leaf_color;

};

} // namespace mtmercy

#endif /* AspineTree_hpp */
