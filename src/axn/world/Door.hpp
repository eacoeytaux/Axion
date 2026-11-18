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

    virtual Drawing door_drawing( ) const { return( m_door_drawing ); }

    ptr<Room> room( ) const { return( m_room ); }

    ptr<Door> out( ) const { return( m_out ); }
    void out( ptr<Door> in_out ) { m_out = in_out; }

    bool locked( ) const { return( m_locked ); }
    virtual void lock( bool in_locked ) { m_locked = in_locked; }

    virtual void open( varray<ptr<Object>> cref in_objects = { } );

protected:

    #if defined( AXN_DEBUG )
    virtual Drawing debug_overlay( ) const override;
    #endif

private:

    ptr<Door> m_out;

    ptr<Room> m_room;

    Point m_position = ORIGIN;
    bool m_locked = false;

    Drawing m_door_drawing;

};

} // namespace reality
} // namespace axn

#endif /* Door_hpp */
