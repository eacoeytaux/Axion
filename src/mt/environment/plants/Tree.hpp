#ifndef Tree_hpp
#define Tree_hpp

#include "mt/MountMerciless.hpp"
#include "axn/world/Object.hpp"

namespace mtmercy
{

class Tree : public Object
{

public:

    Tree( ptr<Room> in_room, Point cref in_root, dec in_z = 1.0 );

    virtual void update( ) override;

protected:

    dec leaf_gravity( ) const { return( m_leaf_gravity ); }
    void leaf_gravity( dec in_gravity ) { m_leaf_gravity = in_gravity; }

    Color cref leaf_color( ) const { return( m_leaf_color ); }
    void leaf_color( Color cref in_color ) { m_leaf_color = in_color; }

public:

    class Leaf : public Object
    {

    public:

        Leaf( ptr<Room> in_room, Drawing cref in_drawing, Point cref in_center, dec in_gravity_scale, dec in_z = 1.0 );

        void render( ) override;

    protected:

        void ground( Terrain::Bumper cref in_ground ) override;

    private:

        const Drawing m_drawing;

        Angle m_rotation_speed;

        Cycle m_spin;
        Cycle m_sway;

    };

    virtual Drawing leaf_drawing( ) const { return( Drawing( ) ); }

protected:

    struct LeafDrop
    {

        Path path;
        Countdown countdown;

    };

    varray<LeafDrop> m_leaf_drops;

    dec m_leaf_gravity;
    Color m_leaf_color;

};

} // namespace mtmercy

#endif /* Tree_hpp */
