#ifndef Door_hpp
#define Door_hpp

#include "axn/world/Object.hpp"

namespace axn
{
namespace reality
{

class Door : public Object
{

public:

    Door( ptr<Room> in_room, Point cref in_position, Planc cref in_width, Planc cref in_height, bool in_locked = false );

    virtual void render( ) override;

    #if defined( AXN_DEBUG )
    virtual Drawing debug_overlay( ) const override;
    #endif

    virtual Drawing cref door_drawing( ) const { return( m_door_drawing ); }

    ptr<Room> room( ) const { return( m_room ); }

    ptr<Door> out( ) const { return( m_out ); }
    void out( ptr<Door> in_out ) { m_out = in_out; }

    bool locked( ) const { return( m_locked ); }
    virtual void lock( bool in_locked ) { m_locked = in_locked; }

    virtual void open( varray<ptr<Object>> cref in_objects = { } );

    // C! which way is down for things coming out of this door: they're put as far that way as the door goes, centered across it (the room's
    // C! gravity where the door is, unless it's set). Not gravity( ), which every Object already has for its own fall
    Angle cref exit_gravity( ) const { return( m_gravity ); }
    void exit_gravity( Angle cref in_gravity ) { m_gravity = in_gravity; }

private:

    ptr<Door> m_out;

    ptr<Room> m_room;

    Point m_position = ORIGIN;
    bool m_locked = false;
    Angle m_gravity = VectorY( -1.0 ).angle( ); // C! down, until the constructor gives it the room's gravity

    Drawing m_door_drawing;

};

} // namespace reality
} // namespace axn

#endif /* Door_hpp */
