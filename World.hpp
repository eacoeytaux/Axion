#ifndef World_hpp
#define World_hpp

#include "Engine.hpp"
#include "Physics.hpp"

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

    bool processed( ) const { return ( processed_count( ) > 0 ); }
    uint processed_count( ) const { return m_processed_count; }
    virtual void process( Object * object = nullptr ) { ++m_processed_count; }

};

namespace reality
{

class World
{

protected:

    World( );

public:

    virtual void assign_layer_position( Object * object );

    virtual ~World( );

    virtual void init( );
    virtual void render( );
    virtual void update( );

    virtual void pause( bool );

    virtual void input( const list<Input *> & inputs = { } );

    uint age( ) const { return m_age; }

    Camera * camera( ) { return m_camera; }

    Room * current_room( ) { return m_current_room; }

    virtual dec air_resistance( ) const { return 0.0; }
    virtual Vector gravity( ) const { return V0; }

protected:

    virtual void create( );
    virtual void destroy( );
    virtual void reset( );

protected: // todo make private

    uint m_age = 0;

    Camera * m_camera = nullptr;

    Room * m_current_room = nullptr;

};

} // namespace reality
} // namespace axn

#endif /* World_hpp */
