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
    Object( World * world, const Coordinate & position, const Vector & velocity = ZERO_VECTOR );
    Object( World * world, const Vector & position_velocity );
    
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
    
    Drawing path( const Planc &, const Color &, const dec alpha_start = ONE, const dec alpha_end = ZERO ) const;

#ifdef AXN_DEBUG
    bool draw_debug = false;
    virtual Drawing debug_overlay( ) const;
#endif

protected:
    virtual void update( );

public:
    void update_object( );

    Coordinate position( ) const;
    void position( const Coordinate & );

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
    bool has_gravity( ) const
    {
        return gravity_ratio( );
    }
    void gravity_ratio( dec );
    void normal_gravity( )
    {
        return gravity_ratio( ONE );
    }
    void no_gravity( )
    {
        return gravity_ratio( ZERO );
    }
    
    bool terrain_boundaries( ) const;
    void terrain_boundaries( bool );
    
    TerrainEdge * ground( ) const;

    virtual FixedRectangle hit_box( ) const;
    virtual FixedRectangle visible_box( ) const;

    void track_position( uint count );
    Coordinate last_position( uint past = 0 );

    void subscribe_to_movement( Object * );
    void unsubscribe_to_movement( Object * );

    virtual bool operator==( const Object & other ) const { return id( ) == other.id( ); }
    default_non_equal( Object );

protected:
    virtual void update_movement( );
    virtual void update_velocity( );

    virtual void move( const Vector & );
    virtual void ground( TerrainEdge * ground );
    virtual void out_of_bounds( );

    virtual void react_to_movement( Object * object, const Vector & );

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

    Planc m_visible_width = 0.0;
    Planc m_visible_height = 0.0;

    Planc visible_width( ) const;
    void visible_width( const Planc & );
    Planc visible_height( ) const;
    void visible_height( const Planc & );

    TerrainEdge * m_ground = nullptr;

    dec m_gravity_ratio = 1.0;
    
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
