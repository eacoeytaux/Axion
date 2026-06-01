#ifndef World_hpp
#define World_hpp

#include "axn/system/Engine.hpp"
#include "axn/physics/Physics.hpp"

namespace axn
{

namespace graphics
{

class Camera;

} // namespace graphics

namespace reality
{

class Room;
class Object;
class Player;

} // namespace reality

class Entity : public Identifiable
{

};

class Event
{

private:

    uint m_processed_count = 0;

public:

    bool processed( ) const { return( processed_count( ) > 0 ); }
    uint processed_count( ) const { return( m_processed_count ); }
    virtual void process( ptr<Object> in_object = nullptr ) { m_processed_count++; }

};

namespace reality
{

class World
{

protected:

    World( );

public:

    virtual void assign_layer_position( ptr<Object> in_object );

    virtual ~World( );

    virtual string name( ) const { return( "AXION" ); }

    virtual bool quit( ) const { return( false ); }

    virtual void init( );
    virtual void render( );
    virtual void update( );

    virtual void pause( bool in_pause );
    virtual bool paused( ) const;

    virtual void input( list<ptr<Input>> cref in_inputs = { } );

    uint age( ) const { return( m_age ); }
    void age( ticks in_time ) { for_range( i, in_time ) { update( ); } }

    ptr<Room> next_room( );
    ptr<Room> current_room( );
    virtual ptr<Room> current_room( ptr<Room> in_room );

    virtual ptr<Room> add_room( ptr<Room> in_room, bool in_current = false );

    ptr<Camera> camera( ) const { return( m_camera ); }

    virtual dec air_resistance( ) const { return( 0.0 ); }
    virtual Vector gravity( ) const { return( V0 ); }

    bool has_gravity( ) const { return( gravity( ).has_magnitude( ) ); }

protected:

    virtual void create( );
    virtual void destroy( );
    virtual void reset( );

    ptr<Room> current_room_swap( ptr<Room> in_room );

private:

    uint m_age = 0;

    bool m_paused = false;

    ptr<Camera> m_camera = nullptr;

    uset<ptr<Room>> m_rooms;

    ptr<Room> m_next_room = nullptr;
    ptr<Room> m_current_room = nullptr;

};

} // namespace reality
} // namespace axn

#endif /* World_hpp */
