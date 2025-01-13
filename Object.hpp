#ifndef Object_hpp
#define Object_hpp

#include "Physics.hpp"
#include "Graphics.hpp"
#include "Audio.hpp"

#include "World.hpp"

namespace axn
{
namespace reality
{

class TerrainVertex;
class TerrainEdge;

class Object : public Identifiable, public Visible, public Matter
{

    #ifdef AXN_DEBUG
private:
    static uint total_objects;
    #endif
public:
    virtual ~Object( );

    Object( World * world );
    Object( World * world, Coordinate cref position, Vector cref velocity = V0 );
    Object( World * world, Vector cref position_velocity );

private:
    bool m_initialized = false;
    void init( );

public:
    uint age( ) const { return m_age; }

    World * world( ) const { return m_world; }

protected:
    virtual void render( ) override;

public:
    void render_object( );

    Drawing trajection_drawing( Planc cref distance, Color cref path_color, cdec alpha_start = 1.0, cdec alpha_end = 0.0 ) const;

    #ifdef AXN_DEBUG
    bool m_draw_debug = false;
    static bool draw_physics;
    virtual Drawing debug_overlay( ) const;
    #endif

protected:
    virtual void update( );

public:
    void update_object( );

    Coordinate position( ) const;
    void position( Coordinate cref );

    virtual Planc width( ) const;
    virtual Planc height( ) const;

    bool deleted( ) const { return m_deleted; }
    virtual void mark_deleted( ) { m_deleted = true; }

    bool marked_to_delete( ) const { return m_marked_to_delete; }
    virtual void mark_to_delete( ) { m_marked_to_delete = true; }

    // todo make setters protected?

    bool foreground( ) const;
    void foreground( bool );

    bool background( ) const;
    void background( bool );

    bool interactive( ) const;
    void interactive( bool );

    bool stationary( ) const;
    void stationary( bool );

    dec gravity_ratio( ) const;
    void gravity_ratio( dec );
    void normal_gravity( ) { return gravity_ratio( 1.0 ); }
    void no_gravity( ) { return gravity_ratio( 0.0 ); }
    bool has_gravity( ) const { return gravity_ratio( ); }

    dec air_resistance_ratio( ) const;
    void air_resistance_ratio( dec );

    bool terrain_boundaries( ) const;
    void terrain_boundaries( bool );

    TerrainEdge * ground( ) const;

    bool passing_terrain( ) const { return m_passing_terrain; }
    void passing_terrain( bool b ) { m_passing_terrain = b; }

    virtual FixedRectangle hit_box( ) const;

    void track_position( uint count );
    Coordinate last_position( uint past = 0 );

    void subscribe_to_movement( Object * );
    void unsubscribe_to_movement( Object * );

    virtual bool operator==( Object cref other ) const { return id( ) == other.id( ); }
    default_non_equal( Object );

protected:
    virtual void update_movement( );
    virtual void update_velocity( );

    virtual void move( Vector cref );
    virtual void ground( TerrainEdge * ground );
    virtual void out_of_bounds( );

    virtual void react_to_movement( Object * object, Vector cref );

    virtual void add_movement_subscriber( Object * );
    virtual void remove_movement_subscriber( Object * );

    virtual bool collide( Object * object );

    virtual dec friction_resistance( ) const;

private:
    World * m_world = nullptr;

    uint m_age = 0;
    uint m_last_world_age_update = 0;

    bool m_deleted = false;
    bool m_marked_to_delete = false;

    bool m_foreground = false;
    bool m_background = false;

    Planc m_visible_width = P0;
    Planc m_visible_height = P0;

    TerrainEdge * m_ground = nullptr;
    bool m_passing_terrain = false;

    dec m_gravity_ratio = 1.0;
    dec m_air_resistance_ratio = AIR_RESISTANCE;

    bool m_interactive = false;
    bool m_stationary = false;
    bool m_terrain_boundaries = true;

    uint m_last_position_count = 0;
    uint m_last_position_index = 0;
    varray<Coordinate> m_last_positions;

    uset<Object *> m_movement_subscribers;
    uset<Object *> m_movement_subscriptions;
};

} // namespace reality
} // namespace axn

#define STATE( ... )                                                  \
    public: enum State { INVALID_STATE = 0, __VA_ARGS__ };            \
    public: State state( ) const { return m_state; }                  \
    protected: virtual void state( State state ) { m_state = state; } \
    private: State m_state = INVALID_STATE;                           \

#endif /* Object_hpp */
