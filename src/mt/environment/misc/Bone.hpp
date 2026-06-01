#ifndef Bone_hpp
#define Bone_hpp

#include "mt/MountMerciless.hpp"
#include "axn/world/Object.hpp"

namespace mtmercy
{

class Bone : public Object
{

public:

    Bone( ptr<Room> in_room, Planc cref in_thickness, Point cref in_position );

    virtual void update( ) override;

    Planc thickness( ) const { return( m_thickness ); }
    dec rotation_distance( ) const { return( m_rotation_distance ); }
    dec movement_ratio( ) const { return( m_movement_ratio ); }

    Color bone_color( ) const { return( m_bone_color ); }

protected:

    virtual bool collide( ptr<Object> in_object ) override;

    virtual void thickness( Planc cref in_thickness );
    virtual void rotation_distance( Planc cref in_distance );
    virtual void movement_ratio( dec in_ratio );

private:

    Planc m_thickness = P0;
    dec m_rotation_distance = 0.0;
    dec m_movement_ratio = 1.0;

    Color m_bone_color;

};

class Skull : public Bone { public: Skull( ptr<Room> in_room, Point cref in_position ); };
class Femur : public Bone { public: Femur( ptr<Room> in_room, Point cref in_position ); };

} // namespace mtmercy

#endif /* Bone_hpp */
