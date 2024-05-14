#ifndef Object_hpp
#define Object_hpp

#include "AXN.hpp"

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
    Object( World * world, const Coordinate & position = ORIGIN );

    uint age( ) const { return m_age; }
    World * world( ) const { return m_world; }

protected:
    virtual Object & update( );
    virtual const Object & render( ) const override;

public:
    Object & update_object( );
    const Object & render_object( ) const;

#ifdef AXN_DEBUG
    bool draw_debug = false;
    virtual Drawing debug_overlay( ) const;
#endif

    bool deleted( ) const { return m_deleted; }
    Object & mark_deleted( )
    {
        m_deleted = true;
        return *this;
    }

    virtual Planc width( ) const;
    virtual Planc height( ) const;

    bool stationary( ) const;
    Object & stationary( bool );
    double gravity_ratio( ) const;
    Object & gravity_ratio( double );
    bool terrain_boundaries( ) const;
    Object & terrain_boundaries( bool );
    TerrainEdge * ground( ) const;

    virtual FixedRectangle hit_box( ) const;
    virtual FixedRectangle visible_box( ) const;

    Object & track_position( uint count );
    Coordinate last_position( uint past = 0 );

    Object & subscribe_to_movement( Object * );

    virtual bool operator==( const Object & other ) const { return id( ) == other.id( ); }
    virtual bool operator!=( const Object & other ) const { return !( *this == other ); }

protected:
    virtual Object & move( );
    virtual Object & update_velocity( );
    virtual Object & ground( TerrainEdge * ground );
    virtual Object & react_to_movement( Object * object, const Vector & );

    virtual bool collide( Object * object );

    virtual double friction_resistance( ) const;

private:
    Object & add_movement_subscriber( Object * );

private:
    World * m_world = nullptr;
    uint m_age = 0;
    uint m_last_world_age_update = 0;
    bool m_deleted = false;

    Planc m_visible_width = 0.0;
    Planc m_visible_height = 0.0;

    Planc visible_width( ) const;
    Object & visible_width( const Planc & );
    Planc visible_height( ) const;
    Object & visible_height( const Planc & );

    TerrainEdge * m_ground = nullptr;

    double m_gravity_ratio = 1.0;

    bool m_stationary = false;
    bool m_terrain_boundaries = true;

    uint m_last_position_count = 0;
    uint m_last_position_index = 0;
    varray<Coordinate> m_last_positions;

    set<Object *> m_movement_subscribers;
    set<Object *> m_movement_subscriptions;
};

class StationaryObject : public Object
{
public:
    virtual ~StationaryObject( ) { }
    StationaryObject( World * world, const Coordinate & position = ORIGIN ) : Object( world, position ) { }

private:
    virtual Object & move( ) { return *this; }
    virtual Object & update_velocity( )
    {
        velocity( ZERO_VECTOR );
        return *this;
    }
};

} // namespace reality
} // namespace axn

#endif /* Object_hpp */
