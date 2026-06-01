#ifndef Room_hpp
#define Room_hpp

#include "axn/world/World.hpp"
#include "axn/world/Terrain.hpp"

namespace axn
{

namespace graphics
{

class Camera;
class Lighting;

} // namespace graphics

namespace reality
{

class World;
class Object;
class Player;
class Door;

} // namespace reality

namespace reality
{

class Room
{

public:

    virtual ~Room( );

    Room( ptr<World> in_world );

    virtual void init( );
    virtual void render( );
    virtual void update( );

    Color cref fill_color( ) const { return( m_fill_color ); }

    virtual void input( list<ptr<Input>> cref in_inputs = { } );

    uint age( ) const { return( m_age ); }
    void age( ticks in_time ) { for_range( i, in_time ) { update( ); } } // todo just update with time?

    ptr<Camera> camera( ) { return( world( )->camera( ) ); }

    ptr<World> world( ) { return( m_world ); }
    ptr<const World> world( ) const { return( m_world ); }

    FixedRectangle bounds( dec in_z = 1.0 ) const;

    virtual Vector gravity( ) const;
    virtual dec air_resistance( ) const;

    virtual Vector gravity( Point cref in_position ) const;
    virtual dec air_resistance( Point cref in_position ) const;

public:

    varray<ptr<Player>> cref players( ) { return( m_players ); }
    uint player_count( ) const { return( m_players.size( ) ); }
    ptr<Player> player( uint in_player_number = 0 );
    ptr<Player> player_main( );

    // todo these should be in world
    virtual ptr<Player> add_player( Point cref in_player );
    virtual ptr<Player> create_player( Point cref in_player ) { return( nullptr ); }

    #if defined( AXN_DEBUG )
    void render_object_grid( ptr<Camera> in_camera ) const;
    #endif

    virtual ptr<Object> add_object( ptr<Object> in_object );
    varray<ptr<Object>> cref add_objects( varray<ptr<Object>> cref in_objects )
    {
        for_each( object, in_objects )
        {
            add_object( object );
        }

        return( in_objects );
    }

protected:

    void fill_color( Color cref in_color ) { m_fill_color = in_color; }

    virtual void update_object( ptr<Object> in_object );
    void update_objects( varray<ptr<Object>> cref in_objects )
    {
        for_each( object, in_objects )
        {
            update_object( object );
            update_object( object );
        }
    }

    virtual void remove_object( ptr<Object> in_object );
    void remove_objects( varray<ptr<Object>> cref in_objects )
    {
        for_each( object, in_objects )
        {
            remove_object( object );
        }
    }

public:

    ptr<Object> add_particle( ptr<Object> in_particle ) { return( add_object( in_particle ) ); }
    varray<ptr<Object>> add_particles( varray<ptr<Object>> cref in_particles )
    {
        for_each( particle, in_particles )
        {
            add_particle( particle );
        }

        return( in_particles );
    }

    ptr<Door> add_door( ptr<Door> in_door );
    uset<ptr<Door>> cref doors( ) const { return( m_doors ); }

    uset<ptr<Room>> connected_rooms( ) const;

    void subscribe_object_input( ptr<Object> in_object );

    // environment

    ptr<const Lighting> lighting( ) const;

    bool lighting_active( ) const;
    void lighting_active( bool in_activate );

    ptr<const Terrain> terrain( ) const;
    Vector wind( ) const;

    varray<ptr<Object>> cref objects( ) const;

    varray<ptr<Object>> objects_in_range( FixedRectangle cref in_bounds );

    varray<ptr<Terrain::Node>> terrain_in_range( FixedRectangle cref in_bounds );

public: // todo protected

    virtual void create( ) { return( create( FixedRectangle( ) ) ); }
    virtual void create( FixedRectangle cref in_bounds );
    virtual void destroy( );
    virtual void reset( );

    virtual void bounds( FixedRectangle cref in_bounds );

    virtual ptr<Terrain> generate_terrain( ) { return( nullptr ); }

    virtual void wind( Vector cref in_wind );

private:

    void add_objects_from_queue( );
    void clear_objects( );

protected: // todo make private?

    ptr<World> m_world = nullptr;

    uset<ptr<Door>> m_doors;

    uint m_age = 0;

    Color m_fill_color;

    FixedRectangle m_bounds;

    varray<ptr<Player>> m_players;

    varray<ptr<Object>> m_objects;
    queue<ptr<Object>> m_object_queue;

    varray<ptr<Object>> m_foreground_objects;
    varray<ptr<Object>> m_background_objects;

    varray<ptr<Object>> m_input_subscribers;

    bool m_lighting_active = false;
    ptr<Lighting> m_lighting = nullptr;

    ptr<Terrain> m_terrain = nullptr;

    Vector m_wind;

public:

    class Grid
    {

    public:

        struct Block
        {

        private:

            #if defined( AXN_DEBUG )
            bool m_init = false;
            #endif

            uint m_x = 0;
            uint m_y = 0;

            uset<ptr<Object>> m_objects;

            uset<ptr<Terrain::Node>> m_terrain_nodes;

        public:

            void init( uint in_x, uint in_y )
            {
                #if defined( AXN_DEBUG )
                Assert( !m_init );
                m_init = true;
                #endif

                m_x = in_x;
                m_y = in_y;
            }

            uint x( ) const { return( m_x ); }
            uint y( ) const { return( m_y ); }

            uset<ptr<Object>> cref objects( ) const { return( m_objects ); }

            void insert( ptr<Object> in_object ) { m_objects.insert( in_object ); }
            void remove( ptr<Object> in_object ) { m_objects.remove( in_object ); }

            uset<ptr<Terrain::Node>> cref terrain_nodes( ) const { return( m_terrain_nodes ); }

            void insert_terrain( ptr<Terrain::Node> in_terrain ) { m_terrain_nodes.insert( in_terrain ); }
            void remove_terrain( ptr<Terrain::Node> in_terrain ) { m_terrain_nodes.remove( in_terrain ); }

            void clear( ) { m_objects.clear( ); m_terrain_nodes.clear( ); }

            default_equal( Block );

        };

    public:

        void init( FixedRectangle cref in_bounds );

        Block ref block( uint in_x, uint in_y );
        Block cref block_const( uint in_x, uint in_y ) const;

        bool valid_x( uint in_x ) const { return( in_x < m_grid_x_size ); }
        bool valid_y( uint in_y ) const { return( in_y < m_grid_y_size ); }

        uint x( Planc cref in_x ) const;
        uint y( Planc cref in_y ) const;

        Span<uint> x_range( FixedRectangle cref in_bounds ) const;
        Span<uint> y_range( FixedRectangle cref in_bounds ) const;

        Span<uint> x_range( ) const { return( Span<uint>{ 0, m_grid_x_size } ); }
        Span<uint> y_range( ) const { return( Span<uint>{ 0, m_grid_y_size } ); }

        void traverse( FixedRectangle cref in_range, function<void( Block ref in_block )> cref in_func );
        void traverse( function<void( Block ref in_block )> cref in_func ) { return( traverse( m_bounds, in_func ) ); }

        void traverse_const( FixedRectangle cref in_range, function<void( Block cref in_block )> cref in_func ) const;
        void traverse_const( function<void( Block cref in_block )> cref in_func ) const { return( traverse_const( m_bounds, in_func ) ); }

        void insert( ptr<Object> in_object );
        void remove( ptr<Object> in_object );

        void insert_terrain( ptr<Terrain::Node> in_terrain_node );
        void remove_terrain( ptr<Terrain::Node> in_terrain_node );

        void clear( );

    private:

        uint m_grid_x_size;
        uint m_grid_y_size;
        Block m_out_of_bounds_block;
        vvarray<Block> m_grid;
        FixedRectangle m_bounds;
    };

private:

    Grid m_object_grid;

public:

    Grid ref object_grid( ) { return( m_object_grid ); }

};

} // namespace reality
} // namespace axn

#endif /* Room_hpp */
