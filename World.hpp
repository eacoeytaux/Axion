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
    const FixedRectangle & bounds( ) const;

    virtual World & pause( bool );

    const Camera * active_camera( ) const;
    const Player * player( uint player_number = 0 );

    virtual World & add_object( Object * object );
    World & add_objects( const list<Object *> & objects )
    {
        for_each( object, objects ) add_object( object );
        return *this;
    }

protected:
    virtual World & update_object( Object * object );
    World & update_objects( const list<Object *> & objects )
    {
        for_each( object, objects ) update_object( object );
        return *this;
    }

    virtual World & remove_object( Object * object );
    World & remove_objects( const list<Object *> & objects )
    {
        for_each( object, objects ) remove_object( object );
        return *this;
    }

public:
    World & add_particle( Object * particle ) { return add_object( particle ); }
    World & add_particles( const list<Object *> & particles )
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

    const list<Object *> & objects( ) const; // TODO list is const but objects are not

    list<Object *> objects_in_range( const Planc & lower_x, const Planc & upper_x );
    list<Object *> solid_objects_in_range( const Planc & lower_x, const Planc & upper_x );

protected:
    virtual World & create( );
    virtual World & destroy( );
    virtual World & reset( );

    virtual World & bounds( const FixedRectangle & );

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

    varray<Player *> m_players;

    queue<Object *> m_object_queue;
    list<Object *> m_objects;
    list<Object *> m_solid_objects;

    Background * m_background;
    Foreground * m_foreground;

    Lighting * m_lighting;
    bool m_lighting_active;
    Terrain * m_terrain;
    Vector m_wind;

    Camera * m_active_camera = nullptr;
    varray<Camera> m_player_cameras;

    FixedRectangle m_bounds;

    class Grid
    {
    public:
        struct Block
        {
            void init( uint x, uint y )
            {
                m_x = x;
                m_y = y;
            }

            uint m_x = 0;
            uint m_y = 0;

            set<Object*> m_objects;
        };

    public:
        virtual ~Grid( ) { }

        Grid & init( const FixedRectangle & bounds );
        
        Block & block( uint x, uint y );

        const set<Object *> & objects( uint x, uint y ) const;

        uint x( const Planc & ) const;
        uint y( const Planc & ) const;

        bool valid_x( uint x ) const { return ( x < m_grid_x_size ); }
        bool valid_y( uint y ) const { return ( y < m_grid_y_size ); }

        Span<uint> x_range( ) const { return Span<uint>{ 0, m_grid_x_size }; }
        Span<uint> y_range( ) const { return Span<uint>{ 0, m_grid_y_size }; }

        Span<uint> x_range( const FixedRectangle & ) const;
        Span<uint> y_range( const FixedRectangle & ) const;

        Grid & mark( bool present, Object * object );
        Grid & mark_present( Object * object ) { return mark( true, object ); }
        Grid & mark_absent( Object * object ) { return mark( false, object ); }

    private:
        uint m_grid_x_size;
        uint m_grid_y_size;
        Block m_out_of_bounds_block;
        varray<varray<Block>> m_grid;
        Coordinate m_offset;
    } m_object_grid;

    Grid & object_grid( ) { return m_object_grid; }

    void render_bounds( Camera * camera );
#ifdef AXN_DEBUG
    void render_object_grid( Camera * camera ) { return render_object_grid( camera, false ); }
    void render_object_grid( Camera * camera, bool fill_blocks, std::function<bool( const Grid::Block & block )> = []( const Grid::Block & block ) { return false; } );
    void render_camera_fps( Camera * camera, bool show_crosshairs = true );
#endif
};

} // namespace reality
} // namespace axn

#endif /* World_hpp */
