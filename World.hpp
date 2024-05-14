#ifndef World_hpp
#define World_hpp

#include "Engine.hpp"

namespace axn
{

namespace graphics
{
class Camera;
class Lighting;
} // namespace graphics

namespace reality
{
class Object;
class Player;
class Terrain;
class Background;
class Foreground;
} // namespace reality

class Event
{
private:
    uint m_processed_count = 0;

public:
    bool processed( ) const { return ( processed_count( ) > 0 ); }
    uint processed_count( ) const { return m_processed_count; }
    virtual Event & process( Object * object = nullptr )
    {
        ++m_processed_count;
        return *this;
    }
};

namespace reality
{

class World
{
protected:
    World( );

public:
    virtual ~World( );

    World & init( );
    World & render( );
    World & update( );
    virtual World & input( const varray<Input *> & inputs = { } );

    uint age( ) const;

    virtual World & pause( bool );

    const Camera * active_camera( ) const;
    const Player * player( uint player_number = 0 );

    World & add_object( Object * object );
    World & add_objects( const varray<Object *> & objects )
    {
        for_each( object, objects ) add_object( object );
        return *this;
    }

    World & add_particle( Object * particle ) { return add_object( particle ); }
    World & add_particles( const varray<Object *> & particles )
    {
        for_each( particle, particles ) add_particle( particle );
        return *this;
    }

    // environment

    const Lighting * lighting( ) const;

    bool lighting_active( ) const;
    World & lighting_active( bool );

    const Terrain * terrain( ) const;
    Vector wind( ) const;

    const list<Object *> & objects( ) const; // TODO consts don't line up

    list<Object *> objects_in_range( const Planc & lower_x, const Planc & upper_x );
    list<Object *> solid_objects_in_range( const Planc & lower_x, const Planc & upper_x );

protected:
    virtual World & create( );
    virtual World & destroy( );
    virtual World & reset( );

    virtual World & add_player( const Coordinate & position );
    virtual Player * create_player( const Coordinate & position ) = 0;

    virtual World & background( Background * );

    virtual Terrain * generate_terrain( ) = 0;

    virtual World & wind( const Vector & );

private:
    World & add_objects_from_queue( );
    World & clear_objects( );

private:
    uint m_age = 0;

    Camera * m_active_camera = nullptr;

    varray<Player *> m_players;
    varray<Camera> m_player_cameras;

    queue<Object *> m_object_queue;
    list<Object *> m_objects;
    list<Object *> m_solid_objects;

    Background * m_background;
    Foreground * m_foreground;

    Terrain * m_terrain;

    Lighting * m_lighting;
    bool m_lighting_active;

    Vector m_wind;
};

} // namespace reality
} // namespace axn

#endif /* World_hpp */
