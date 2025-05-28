#ifndef Room_hpp
#define Room_hpp

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

class World;
class Object;
class Player;
class Terrain;
class TerrainNode;

} // namespace reality

namespace reality
{

class Room
{
    
public:
    
    class Door
    {
        
    private:
        
        Room * m_room;
        
        Coordinate m_position;
        
        bool m_locked;
        
        Door * m_out;
        
    public:
        
        Door( Room * room, xCoordinate position, bool locked = false ) : m_room( room ), m_position( position ), m_locked( locked ) { }
        
        Room * room( ) const { return m_room; }
        
        Door * out( ) { return m_out; }
        
        virtual void lock( bool b ) { m_locked = b; }
        virtual bool locked( ) const { return m_locked; }
        
    };

public:

    virtual ~Room( );
    Room( World * );

    virtual void init( );
    virtual void render( );
    virtual void update( );

    virtual void input( const list<Input *> & inputs = { } );

    uint age( ) const;

    Camera * camera( );

    World * world( ) { return m_world; }

    FixedRectangle cref bounds( ) const;

private:

    virtual void render_bounds( Camera * camera );
    #if defined ( AXN_DEBUG )
    void render_object_grid( Camera * camera ) const;
    #endif

public:

    uint player_count( ) const { return m_players.size( ); }
    const varray<Player *> & players( ) { return m_players; }
    Player * player( uint player_number = 0 );
    Player * player_main( );

    // todo these should be in world
    virtual Player * add_player( Coordinate cref );
    virtual Player * create_player( Coordinate cref ) = 0;

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

public:

    void add_particle( Object * particle ) { return add_object( particle ); }
    void add_particles( const varray<Object *> & particles )
    {
        for_each( particle, particles )
        {
            add_particle( particle );
        }
    }
    
    const oset<Door *> & doors( ) const { return m_doors; }
    
    oset<Room *> connected_rooms( ) const;

    // environment

    const Lighting * lighting( ) const;

    bool lighting_active( ) const;
    void lighting_active( bool );

    const Terrain * terrain( ) const;
    Vector wind( ) const;

    const varray<Object *> & objects( ) const;

    varray<Object *> objects_in_range( FixedRectangle cref );

    varray<TerrainNode *> terrain_in_range( FixedRectangle cref );

public: // todo protected

    virtual void create( ) { return create( FixedRectangle( ) ); }
    virtual void create( FixedRectangle cref bounds );
    virtual void destroy( );
    virtual void reset( );

    virtual void bounds( FixedRectangle cref );

    virtual Terrain * generate_terrain( ) = 0;

    virtual void wind( Vector cref );

private:

    void add_objects_from_queue( );
    void clear_objects( );

protected: // todo make private?

    World * m_world = nullptr;
    
    oset<Door *> m_doors;

    uint m_age = 0;

    FixedRectangle m_bounds;

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
            
        public:
            
            void init( uint x, uint y )
            {
                #if defined ( AXN_DEBUG )
                Assert( !m_init );
                m_init = true;
                #endif
                
                m_x = x;
                m_y = y;
            }
            
            uint x( ) const { return m_x; }
            uint y( ) const { return m_y; }
            
            const uset<Object *> & objects( ) const { return m_objects; }
            
            void insert( Object * object ) { m_objects.insert( object ); }
            void remove( Object * object ) { m_objects.remove( object ); }
            
            const uset<TerrainNode *> & terrain_nodes( ) const { return m_terrain_nodes; }
            
            void insert( TerrainNode * terrain ) { m_terrain_nodes.insert( terrain ); }
            void remove( TerrainNode * terrain ) { m_terrain_nodes.remove( terrain ); }
            
            void clear( ) { m_objects.clear( ); m_terrain_nodes.clear( ); }
            
            default_equal( Block );
            
        private:
        
            #if defined ( AXN_DEBUG )
            bool m_init = false;
            #endif
            
            uint m_x = 0;
            uint m_y = 0;

            uset<Object *> m_objects;
            
            uset<TerrainNode *> m_terrain_nodes;
        };

    public:

        void init( FixedRectangle cref bounds );

        Block & block( uint x, uint y );
        Block cref block_const( uint x, uint y ) const;

        bool valid_x( uint x ) const { return ( x < m_grid_x_size ); }
        bool valid_y( uint y ) const { return ( y < m_grid_y_size ); }

        uint x( Planc cref ) const;
        uint y( Planc cref ) const;

        Span<uint> x_range( FixedRectangle cref ) const;
        Span<uint> y_range( FixedRectangle cref ) const;

        Span<uint> x_range( ) const { return Span<uint>{ 0, m_grid_x_size }; }
        Span<uint> y_range( ) const { return Span<uint>{ 0, m_grid_y_size }; }

        void traverse( FixedRectangle cref range, function<void( Block & )> f );
        void traverse( function<void( Block & )> f ) { return traverse( m_bounds, f ); }

        void traverse_const( FixedRectangle cref range, function<void( Block cref )> f ) const;
        void traverse_const( function<void( Block cref )> f ) const { return traverse_const( m_bounds, f ); }

        void insert( Object * object );
        void remove( Object * object );

        void insert( TerrainNode * terrain_node );
        void remove( TerrainNode * terrain_node );

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

};

} // namespace reality
} // namespace axn

#endif /* Room_hpp */
