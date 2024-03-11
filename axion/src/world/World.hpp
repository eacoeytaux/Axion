#ifndef World_hpp
#define World_hpp

#include "axn.hpp"
#include "Constants.hpp"
#include "Geometry.hpp"
#include "Graphics.hpp"
#include "Physics.hpp"
#include "Engine.hpp"
#include "Event.hpp"
#include "Input.hpp"

#include "Camera.hpp"
#include "Lighting.hpp"
#include "Background.hpp"
#include "Foreground.hpp"
#include "Terrain.hpp"

#include "Player.hpp"

namespace axn {
namespace reality {

class World {
protected:
    World( );

public:
    virtual ~World( );

    World & init( );
    World & render( );
    World & update( );
    World & input( const varray<Input*> & inputs = { } );

    uint age( ) const;

    const Camera* active_camera( ) const;
    const Player* player( uint player_number = 0 );

    World & add_object( Object* object );
    World & add_objects( const varray<Object*> & objects ) {
        for_each( object, objects ) add_object( object );
        return *this;
    }

    // environment

    const Lighting & lighting( ) const;

    // bool darkness_active() const;
    // Lighting & darkness_active(bool);
    // ufloat darkness_intensity() const;
    // Lighting & darkness_intensity(ufloat);

    const Terrain* terrain( ) const;
    Vector wind( ) const;

    const list<Object*> & objects( ) const; // TODO consts don't line up

    list<Object*> objects_in_range( planc lower_x, planc upper_x );

protected:
    virtual World & create( );
    virtual World & destroy( );
    virtual World & reset( );

private:
    World & add_objects_from_queue( );
    World & clear_objects( );

    World & add_player( const Coordinate & position );
    virtual Player* create_player( const Coordinate & position ) = 0;

    uint m_age = 0;

    Camera* m_active_camera = nullptr;

    varray<Player*> m_players;
    varray<Camera> m_player_cameras;

    list<Object*> m_objects;
    queue<Object*> m_object_queue;

    Background* m_background;
    Foreground* m_foreground;

    Terrain* m_terrain;

    Lighting* m_lighting;

    Vector m_wind;
};

} // namespace reality
} // namespace axn

#endif /* World_hpp */
