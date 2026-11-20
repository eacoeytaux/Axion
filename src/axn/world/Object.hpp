#ifndef Object_hpp
#define Object_hpp

#include "axn/physics/Physics.hpp"
#include "axn/graphics/Graphics.hpp"
#include "axn/audio/Audio.hpp"

#include "axn/world/World.hpp"
#include "axn/world/Room.hpp"
#include "axn/world/Terrain.hpp"

#include "axn/world/Hitbox.hpp"
#include "axn/world/Damage.hpp"

namespace axn
{
namespace reality
{

class Object : public Identifiable, public Visible, public Mass
{

private:

    #if defined( AXN_DEBUG )
    static uint total_objects;

    // C! set by Object::update( ) and Object::render( ), so update_object( ) and render_object( ) can check that an override didn't skip them
    bool m_base_update_reached = false;
    bool m_base_render_reached = false;
    #endif

public:

    virtual ~Object( );

    Object( ptr<Room> in_room );
    Object( ptr<Room> in_room, dec in_z );
    Object( ptr<Room> in_room, Point cref in_position );
    Object( ptr<Room> in_room, Point cref in_position, dec in_z );
    Object( ptr<Room> in_room, Point cref in_position, Vector cref in_velocity );
    Object( ptr<Room> in_room, Point cref in_position, Vector cref in_velocity, dec in_z );

protected:

    virtual void init( );

private:

    bool m_initialized = false;

public:

    uint age( ) const { return( m_age ); }

    FixedRectangle bounds( bool in_positioned = false ) const { return( space( in_positioned ).bounds( ) ); }

    FixedRectangle room_bounds( dec in_z = 1.0 ) const { return( room( )->bounds( in_z ) ); }
    FixedRectangle room_bounds_z( ) const { return( room_bounds( z( ) ) ); }

    ptr<Room> room( ) const { return( m_room ); }

    // C! moves the object's own state to another room: Room::pass( ) calls it, and moves the object between the rooms' lists itself
    virtual void room( ptr<Room> in_room );

    // C! called once it's come out of in_door into its new room (Room::pass( )), placed where it'll be: for anything it has to put
    // C! right straight away, since the room doesn't update while the camera transitions in. Nothing by default
    virtual void pass_door( ptr<Door> in_door ) { }

    ptr<World> world( ) const { return_null_if( !room( ) ); return( room( )->world( ) ); } // C! null when not in a room (no ?: with nullptr: IntelliSense then reports ptr<World> as incomplete, E0070)
    // C! queues in_sound with the world, played from this object's position (see World::play_sound( ))
    void play_sound( SoundFX cref in_sound ) const;

    ptr<Camera> camera( ) const { return_null_if( !room( ) ); return( room( )->camera( ) ); } // C! null when not in a room, or the room's world has no camera

    Drawing cref drawing( ) const { return( *cast_const( this, Drawing ) ); }

protected:

    virtual void render( ) override;

public:

    void render_object( );

    Drawing trajection_drawing( Planc cref in_distance, Color cref in_path_color, dec in_alpha_start = 1.0, dec in_alpha_end = 0.0 ) const;

    #if defined( AXN_DEBUG )
    bool draw_debug = false;
    Color draw_debug_color = YELLOW;
    virtual Drawing debug_overlay( ) const;
    #endif

    bool foreground( ) const;
    bool background( ) const;

protected:

    virtual void update( );

public:

    virtual void input( ptr<Input> in_input ) { };
    virtual void clear_input( ) { };

    void update_object( );

    bool deleted( ) const { return( m_deleted ); }

    bool marked_to_delete( ) const { return( m_marked_to_delete ); }
    Countdown cref deletion_countdown( ) const { return( m_deletion_countdown ); }
    Countdown cref to_deletion_countdown( ) const { return( m_to_deletion_countdown ); }

    virtual void position( Point cref in_pos ) override; // todo not protected?

    Point position( ) const;

    Point last_position( uint in_past = 0 ) const;
    queue<Point> last_positions( uint in_past ) const; // no default, last_positions( ) is the whole history
    queue<queue<Point>> cref last_positions( ) const;

    void subscribe_to_movement( ptr<Object> in_object );
    void unsubscribe_to_movement( ptr<Object> in_object );

    uset<ptr<Object>> cref movement_subscribers( ) const;
    uset<ptr<Object>> cref movement_subscriptions( ) const;

    virtual Planc width( ) const;
    virtual Planc height( ) const;

    virtual void rotate( Angle cref in_a, Point cref in_origin );
    void rotate( Angle cref in_a ) { rotate( in_a, position( ) ); }

    Angle cref rotation( ) const { return( m_rotation ); }

    bool interactive( ) const;
    bool stationary( ) const;

    bool grounded( ) const { return( !is_null( ground( ).node( ) ) ); }
    void find_ground( ); // C! grounds it on whatever floor it's resting on (gravity pulls it into), when it's been put somewhere without moving there

    Terrain::Bumper cref ground( ) const { return( m_ground ); } // C! the bumper the object is on, ground( ).node( ) is the terrain node
    Line ground_bumper_line( ) const; // C! the bumper line the object is standing on, only meaningful when grounded

    bool has_gravity( ) const { return( gravity( ).has_magnitude( ) ); }

    virtual Vector gravity( ) const;

    virtual dec air_resistance( ) const;

    virtual dec friction_resistance( ) const;

    bool passing_terrain( ) const { return( m_passing_terrain ); }

    bool terrain_bound( ) const;

    virtual varray<Hitbox> hitboxes( ) const;
    virtual FixedRectangle hitboxes_bounds( bool in_include_terrain = true ) const;

    virtual Hitbox terrain_hitbox( ) const;

protected:

    void interactive( bool in_interactive );
    void stationary( bool in_stationary );

    virtual void mark_deleted( ticks in_deletion_countdown = 0 );
    virtual void mark_to_delete( ticks in_countdown = 0, ticks in_deletion_countdown = 0 );

    virtual void out_of_bounds( );

    void track_position( uint in_count );

    virtual void trim_velocity( );
    virtual void update_velocity( );
    virtual void update_movement( );

    // returns 0 if should not move, 1 if no interruption, and (0,1) for interruption
    virtual dec check_movement( Vector cref in_velocity );

    virtual void move( Vector cref in_movement ) override;

    virtual void react_to_movement( ptr<Object> in_object, Vector cref in_movement );

    virtual void add_movement_subscriber( ptr<Object> in_object );
    virtual void remove_movement_subscriber( ptr<Object> in_object );

    virtual bool collide( ptr<Object> in_object );

    virtual bool sticks( ptr<Terrain::Node> in_node ); // todo
    // C! TODO sticks( ) is never called: decide what sticking to terrain means (stop on contact like an arrow, or cling like the climber) or remove it

    virtual void ground( Terrain::Bumper cref in_ground );
    virtual void clear_ground( ) { ground( Terrain::Bumper( ) ); }

    void gravity( Vector cref in_vec ) { m_gravity_change = in_vec; }

    void gravity_scale( Planc cref in_vec ) { m_gravity_change.magnitude( in_vec ); }
    void gravity_angle( Angle cref in_vec ) { m_gravity_change.rotate_to( in_vec ); }

    void normal_gravity( ) { gravity( VectorA( A0, 1.0 ) ); }
    void no_gravity( ) { gravity( V0 ); }

    void air_resistance( dec in_resistance );
    void no_air_resistance( ) { air_resistance( 0.0 ); }

    void passing_terrain( bool in_passing ) { m_passing_terrain = in_passing; }

    void terrain_bound( bool in_bound );

    void foreground( bool in_foreground );
    void background( bool in_background );

private:

    ptr<Room> m_room;

    uint m_age = 0;
    uint m_last_world_age_update = 0;
    bool m_ever_updated = false; // C! until then m_last_world_age_update means nothing (0 would match a room at age 0 and skip the first update)

    bool m_deleted = false;
    bool m_marked_to_delete = false;
    Countdown m_to_deletion_countdown;
    Countdown m_deletion_countdown;

    Angle m_rotation = A0;

    bool m_foreground = false;
    bool m_background = false;

    Planc m_visible_width = P0;
    Planc m_visible_height = P0;

    Terrain::Bumper m_ground; // C! knows its own node, no node means not grounded

    // C! bumpers depend only on the node and the hitbox polygon but are asked for many times per update,
    // C! so update_movement( ) caches them while it runs (cleared at the start and end, so nothing goes stale)
    mutable umap<ptr<const Terrain::Node>, Terrain::Bumper> m_bumper_cache;
    mutable bool m_bumper_cache_active = false;
    varray<Point> m_bumper_poly; // C! Terrain::Node::bumper_polygon( terrain_hitbox( ) ) for this update

    Terrain::Bumper bumper( ptr<const Terrain::Node> in_node ) const;
    bool m_passing_terrain = false;

    // rotation is change, magnitude is scale
    Vector m_gravity_change = VectorA( A0, 1.0 );

    dec m_air_resistance = 1.0;

    bool m_interactive = false;
    bool m_stationary = false;
    bool m_terrain_bound = true;

    uint m_last_position_count = 0;
    queue<queue<Point>> m_last_positions;

    uset<ptr<Object>> m_movement_subscribers;
    uset<ptr<Object>> m_movement_subscriptions;

public:

    virtual bool operator==( Object cref in_other ) const { return( id( ) == in_other.id( ) ); }
    virtual bool operator!=( Object cref in_other ) const { return( id( ) != in_other.id( ) ); }

};

#define STATE( ... ) \
    public: enum state_enum { INVALID_STATE = 0, __VA_ARGS__ }; \
    private: state_enum m_state = INVALID_STATE; \
    public: state_enum state( ) const { return( m_state ); } \
    protected: virtual void state( state_enum in_state ) { if( m_state != in_state ) { m_state = in_state; } } \
    macro_end

} // namespace reality
} // namespace axn

#endif /* Object_hpp */
