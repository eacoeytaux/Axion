#ifndef Object_hpp
#define Object_hpp

#include "axn.hpp"
#include "Identifiable.hpp"
#include "Constants.hpp"
#include "Graphics.hpp"
#include "Audio.hpp"
#include "Geometry.hpp"
#include "Physics.hpp"

namespace axn {
namespace reality {

class World;
class Object;
class TerrainEdge;

class Object : public Identifiable, public Visible, public Matter {

#ifdef AXN_DEBUG
private:
    static uint total_objects;
#endif
public:
    virtual ~Object( );
    Object( World* world, const Coordinate & position = ORIGIN );

    uint age( ) const { return m_age; }
    World* world( ) const { return m_world; }

    virtual const Object & render( ) const override {
        Visible::position_drawing( position( ) );
        Visible::render( );
        return *this;
    }

#ifdef AXN_DEBUG
    bool draw_debug = false;
    virtual Drawing debug_overlay( ) const;
#endif

    virtual Object & update( );
    Object & pre_update( );
    Object & post_update( );

    bool deleted( ) const { return m_deleted; }
    Object & mark_deleted( ) {
        m_deleted = true;
        return *this;
    }

    virtual planc width( ) const;
    virtual planc height( ) const;

    ufloat gravity_ratio( ) const;
    Object & gravity_ratio( ufloat );
    bool terrarin_boundaries( ) const;
    Object & terrarin_boundaries( bool );

    virtual Shape hit_box( ) const;
    virtual Shape visible_box( ) const;

    virtual bool operator==( const Object & other ) const { return id( ) == other.id( ); }
    virtual bool operator!=( const Object & other ) const { return !( *this == other ); }

protected:
    virtual Object & move( );
    virtual Object & adjust_velocity( );
    virtual ufloat friction_resistance( ) const;

    TerrainEdge* m_ground = nullptr;

private:
    World* m_world = nullptr;
    uint m_age = 0;
    uint m_last_world_age = 0;
    bool m_deleted = false;

    planc m_visible_width = 0.f;
    planc m_visible_height = 0.f;

    planc visible_width( ) const;
    Object & visible_width( const planc & );
    planc visible_height( ) const;
    Object & visible_height( const planc & );

    ufloat m_gravity_ratio = 1.f;

    bool m_terrarin_boundaries = true;

    Coordinate m_last_position;
};

class StationaryObject : public Object {
public:
    virtual ~StationaryObject( ) {}
    StationaryObject( World* world, const Coordinate & position = ORIGIN )
        : Object( world, position ) {}

private:
    virtual Object & move( ) { return *this; }
    virtual Object & adjust_velocity( ) {
        velocity( Vector( 0.f, 0.f ) );
        return *this;
    }
};

} // namespace reality
} // namespace axn

#endif /* Object_hpp */
