#ifndef Object_hpp
#define Object_hpp

#include "Physics.hpp"
#include "Graphics.hpp"
#include "Audio.hpp"

#include "World.hpp"
#include "Room.hpp"

#include "Terrain.hpp"

#include "Damage.hpp"

namespace axn
{
namespace reality
{

class Hitbox : public Polygon
{

public:

    Hitbox( ) { }

    Hitbox( Polygon cref hitbox ) : Polygon( hitbox ) { Assert( hitbox.convex( ) ); }

};

class Object : public Identifiable, public Visible, public Matter
{

private:

    #if defined ( AXN_DEBUG )
    static uint total_objects;
    #endif

public:

    virtual ~Object( );

    Object( Room * room );
    Object( Room * room, dec z );
    Object( Room * room, Coordinate cref position );
    Object( Room * room, Coordinate cref position, dec z );
    Object( Room * room, Coordinate cref position, Vector cref velocity );
    Object( Room * room, Coordinate cref position, Vector cref velocity, dec z );

private:

    bool m_initialized = false;
    void init( );

public:

    uint age( ) const { return m_age; }

    Room * room( ) const { return m_room; }
    World * world( ) const { return m_room->world( ); }

protected:

    virtual void render( ) override;

public:

    void render_object( );

    Drawing trajection_drawing( Planc cref distance, Color cref path_color, cdec alpha_start = 1.0, cdec alpha_end = 0.0 ) const;

    #if defined ( AXN_DEBUG )
    bool draw_debug = false;
    Color draw_debug_color = YELLOW;
    virtual Drawing debug_overlay( ) const;
    #endif

    bool foreground( ) const;
    bool background( ) const;

protected:

    virtual void update( );

public:

    void update_object( );

    bool marked_to_delete( ) const { return m_marked_to_delete; }
    bool deleted( ) const { return m_deleted; }

    void position( Coordinate cref ); // todo not protected?

    Coordinate position( ) const;
    Coordinate last_position( uint past = 0 ) const;

    void subscribe_to_movement( Object * );
    void unsubscribe_to_movement( Object * );

    virtual Planc width( ) const;
    virtual Planc height( ) const;

    bool interactive( ) const;
    bool stationary( ) const;

    Terrain::Node * ground( ) const;
    Terrain::Bumper ground_bumper( ) const;

    bool has_gravity( ) const { return gravity( ).has_magnitude( ); }

    virtual Vector gravity( ) const;

    virtual dec air_resistance( ) const;

    virtual dec friction_resistance( ) const;

    bool passing_terrain( ) const { return m_passing_terrain; }

    bool terrain_bound( ) const;

    virtual Hitbox hitbox( ) const; // todo remove in favor of hitboxes

    virtual varray<Hitbox> hitboxes( ) const;
    virtual FixedRectangle hitboxes_bounds( bool include_terrain = true ) const;

    virtual Hitbox terrain_hitbox( ) const;

protected:

    void interactive( bool );
    void stationary( bool );

    virtual void mark_to_delete( ) { m_marked_to_delete = true; }
    virtual void mark_deleted( ) { m_deleted = true; }

    virtual void out_of_bounds( );

    void track_position( uint count );

    virtual void update_velocity( );
    virtual void update_movement( );

    // returns 0 if should not move, 1 if no interruption, and (0,1) for interruption
    virtual dec check_movement( Vector cref velocity );

    virtual void move( Vector cref );

    virtual void react_to_movement( Object * object, Vector cref );

    virtual void add_movement_subscriber( Object * );
    virtual void remove_movement_subscriber( Object * );

    virtual bool collide( Object * object );

    virtual bool sticks( Terrain::Node * node ); // todo
    virtual void ground( Terrain::Node * node, Terrain::Bumper cref bumper );

    void gravity( Vector cref g ) { m_gravity_change = g; }

    void gravity_scale( Planc cref g ) { m_gravity_change.magnitude( g ); }
    void gravity_angle( Angle cref g ) { m_gravity_change.rotate_to( g ); }

    void normal_gravity( ) { gravity( VectorA( A0, 1.0 ) ); }
    void no_gravity( ) { gravity( V0 ); }

    void air_resistance( dec );
    void no_air_resistance( ) { air_resistance( 0.0 ); }

    void passing_terrain( bool b ) { m_passing_terrain = b; }

    void terrain_bound( bool );

    void foreground( bool );
    void background( bool );

private:

    Room * m_room = nullptr;

    uint m_age = 0;
    uint m_last_world_age_update = 0;

    bool m_deleted = false;
    bool m_marked_to_delete = false;

    bool m_foreground = false;
    bool m_background = false;

    Planc m_visible_width = P0;
    Planc m_visible_height = P0;

    Terrain::Node * m_ground = nullptr;
    Terrain::Bumper m_ground_bumper;
    bool m_passing_terrain = false;

    // rotation is change, magnitude is scale
    Vector m_gravity_change = VectorA( A0, 1.0 );

    dec m_air_resistance = 1.0;

    bool m_interactive = false;
    bool m_stationary = false;
    bool m_terrain_bound = true;

    uint m_last_position_count = 0;
    queue<varray<Coordinate>> m_last_positions;

    uset<Object *> m_movement_subscribers;
    uset<Object *> m_movement_subscriptions;

public:

    virtual bool operator==( Object cref other ) const { return id( ) == other.id( ); }

};

} // namespace reality
} // namespace axn

#define STATE( ... )                                                  \
    public: enum State { INVALID_STATE = 0, __VA_ARGS__ };            \
    public: State state( ) const { return m_state; }                  \
    protected: virtual void state( State state ) { m_state = state; } \
    private: State m_state = INVALID_STATE;                           \

#endif /* Object_hpp */
