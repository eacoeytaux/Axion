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

    void init( );
    void render( );
    void update( );
    virtual void input( const varray<Input *> & inputs = { } );

    uint age( ) const;
    const FixedRectangle & bounds( ) const;

    virtual void pause( bool );

    Camera * camera( );

    uint player_count( ) const { return m_players.size( ); }
    const varray<Player *> & players( ) { return m_players; }
    Player * player( uint player_number = 0 );
    Player * player_main( );

    varray<Coordinate> update_points( ) const;
    virtual Planc default_update_distance( ) const;

    virtual void add_object( Object * object );
    void add_objects( const varray<Object *> & objects )
    {
        for_each( object, objects )
        {
            add_object( object );
        }
    }

protected:
    virtual void update_object( Object * object );
    void update_objects( const varray<Object *> & objects )
    {
        for_each( object, objects )
        {
            update_object( object );
        }
    }

    virtual void remove_object( Object * object );
    void remove_objects( const varray<Object *> & objects )
    {
        for_each( object, objects )
        {
            remove_object( object );
        }
    }

    virtual void assign_layer_position( Object * object );

public:
    void add_particle( Object * particle ) { return add_object( particle ); }
    void add_particles( const varray<Object *> & particles )
    {
        for_each( particle, particles )
        {
            add_particle( particle );
        }
    }

    // environment

    const Lighting * lighting( ) const;

    bool lighting_active( ) const;
    void lighting_active( bool );

    const Terrain * terrain( ) const;
    Vector wind( ) const;

    const varray<Object *> & objects( ) const;

    varray<Object *> objects_in_range( const FixedRectangle & );

protected:
    virtual void create( ) { return create( FixedRectangle( ) ); }
    virtual void create( const FixedRectangle & bounds );
    virtual void destroy( );
    virtual void reset( );

    virtual void bounds( const FixedRectangle & );

    virtual Player * add_player( const Coordinate & position );
    virtual Player * create_player( const Coordinate & position ) = 0;

    virtual Terrain * generate_terrain( ) = 0;

    virtual void wind( const Vector & );

private:
    void add_objects_from_queue( );
    void clear_objects( );

protected: // todo make private?
    uint m_age = 0;

    FixedRectangle m_bounds;
    Camera * m_camera = nullptr;

    varray<Player *> m_players;

    varray<Object *> m_objects;
    queue<Object *> m_object_queue;

    varray<Object *> m_foreground_objects;
    varray<Object *> m_background_objects;

    bool m_lighting_active = false;
    Lighting * m_lighting = nullptr;

    Terrain * m_terrain = nullptr;

    Vector m_wind;

public:
    class Grid
    {
    public:
        struct Block
        {
            void init( uint xx, uint yy )
            {
                x = xx;
                y = yy;
            }

            uint x = 0;
            uint y = 0;

            uset<Object *> objects;

            default_equal( Block );
        };

    public:
        virtual ~Grid( ) { }

        void init( const FixedRectangle & bounds );

        Block & block( uint x, uint y );

        bool valid_x( uint x ) const { return ( x < m_grid_x_size ); }
        bool valid_y( uint y ) const { return ( y < m_grid_y_size ); }

        uint x( const Planc & ) const;
        uint y( const Planc & ) const;

        Span<uint> x_range( const FixedRectangle & ) const;
        Span<uint> y_range( const FixedRectangle & ) const;

        Span<uint> x_range( ) const { return Span<uint>{ 0, m_grid_x_size }; }
        Span<uint> y_range( ) const { return Span<uint>{ 0, m_grid_y_size }; }

        void traverse( const FixedRectangle & range, function<void( Block & )> f );
        void traverse( function<void( Block & )> f ) { return traverse( m_bounds, f ); }

        void add( Object * object );
        void erase( Object * object );

        void clear( );

    private:
        uint m_grid_x_size;
        uint m_grid_y_size;
        Block m_out_of_bounds_block;
        varray<varray<Block>> m_grid;
        FixedRectangle m_bounds;
    };

private:
    Grid m_object_grid;

public:
    Grid & object_grid( ) { return m_object_grid; }

private:
    virtual void render_bounds( Camera * camera );
    #ifdef AXN_DEBUG
    void render_object_grid( Camera * camera ) { return render_object_grid( camera, false ); }
    void render_object_grid( Camera * camera, bool fill_blocks, function<bool( const Grid::Block & block )> = [ ] ( const Grid::Block & block )
    {
        return false;
    } );
    #endif

    #ifdef AXN_DEBUG
public:
    bool m_display_forebackground = true;
    bool m_draw_grid = false;
    #endif
};

} // namespace reality
} // namespace axn

#endif /* World_hpp */
