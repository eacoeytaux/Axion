#ifndef World_hpp
#define World_hpp

#include "Engine.hpp"
#include "Room.hpp"

namespace axn
{

namespace graphics
{
class Camera;
} // namespace graphics

namespace reality
{
class Object;
class Player;
} // namespace reality

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

    virtual ~World( );

    virtual void init( );
    virtual void render( );
    virtual void update( );

    virtual void pause( bool );

    virtual void input( const list<Input *> & inputs = { } );

    uint age( ) const;

    Camera * camera( );

    Room * current_room( ) { return m_current_room; }

    virtual void assign_layer_position( Object * object );

protected:

    virtual void create( );
    virtual void destroy( );
    virtual void reset( );

protected: // todo make private?

    uint m_age = 0;

    Camera * m_camera = nullptr;
    Room * m_current_room = nullptr;
    
};

} // namespace reality
} // namespace axn

#endif /* World_hpp */
