#include "Mob.hpp"

AXN_NAMESPACES
using axn::reality::Mob;

// TODO make variable probably (even be in mob?)
static int BLINK_WAIT_MIN = 240;
static int BLINK_WAIT_MAX = 300;
static int BLINK_DURATION = 10;

Mob::Mob( World* world, const Coordinate & _position, const uint _health )
    : Object( world, _position ) {
#ifdef AXN_DEBUG
    draw_debug = true;
#endif

    health( _health );

    m_blink_wait_counter.reset( Random::rng_int( BLINK_WAIT_MIN, BLINK_WAIT_MAX ) );
    m_blink_duration_counter.reset( BLINK_DURATION );
}

Mob & Mob::update( ) {
    Object::update( );
    if( m_blink_wait_counter.tick( ) ) {
        if( m_blink_duration_counter.remaining( ) == BLINK_DURATION )
            drawing_dirty( true );
        if( m_blink_duration_counter.tick( ) ) {
            m_blink_duration_counter.reset( BLINK_DURATION );
            m_blink_wait_counter.reset( Random::rng_int( BLINK_WAIT_MIN, BLINK_WAIT_MAX ) );
            drawing_dirty( true );
        }
    }
    return *this;
}

Mob & Mob::adjust_velocity( ) {
    Object::adjust_velocity( );
    return *this;
}

Mob & Mob::move( ) {
    Object::move( );
    if( velocity( ).magnitude( ) > 0.f ) {
        facing_left( velocity( ).dx( ) < 0.f );
    }
    return *this;
}

bool Mob::alive( ) const { return m_alive; }

Mob & Mob::die( ) {
    m_health.value( 0 );
    m_alive = false;
    return *this;
}

uint Mob::health( ) const { return m_health.value( ); }

Mob & Mob::health( uint _health ) {
    if( _health > m_health.max_value( ) )
        m_health = Slider<uint>( _health );
    m_health.value( _health );
    return *this;
}

Mob & Mob::health( const ufloat _health_percentage ) {
    m_health.value_percentage( _health_percentage );
    return *this;
}

Mob & Mob::heal( uint _health ) {
    m_health.value( max<int>( m_health.value( ) + _health, m_health.max_value( ) ) );
    return *this;
}

Mob & Mob::heal_full( ) {
    m_health.value( m_health.max_value( ) );
    return *this;
}

Mob & Mob::hurt( uint _damage ) {
    uint health = Mob::health( ) - min<ufloat>( _damage, Mob::health( ) );
    if( health == 0 ) {
        m_health.value( 0 );
        die( );
    } else {
        m_health.value( health );
    }
    return *this;
}

uint Mob::max_health( ) const { return m_health.max_value( ); }

Mob & Mob::max_health( uint _health ) {
    m_health = Slider<uint>( _health );
    return *this;
}

bool Mob::facing_left( ) const { return m_facing_left; }

Mob & Mob::facing_left( const bool _facing_left ) {
    m_facing_left = _facing_left;
    return *this;
}

#ifdef AXN_DEBUG
Drawing Mob::debug_overlay( ) const {
    Drawing debug_overlay;

    planc health_width = hit_box( ).bound_width( ) * 1.2f;
    planc health_height = 5.f;
    Coordinate offset =
        Coordinate( 0.f, -( ( space( ).bound_height( ).half( ) ) * 1.2f ) - health_height );
    ufloat health_percentage = (ufloat)health( ) / (ufloat)max_health( );
    if( alive( ) ) {
        if( health_percentage == 1.f ) {
            debug_overlay.add( GREEN, Shape::rectangle( health_width, health_height, offset ) );
        } else {
            debug_overlay.add( RED, Shape::rectangle( health_width, health_height, offset ) );
            Coordinate health_offset =
                offset +
                Vector(
                    (planc)floor( (float)( ( -health_width.half( ) ) +
                                           ( ( health_width.half( ) ) * health_percentage ) ) ),
                    0.f );
            debug_overlay.add( GREEN, Shape::rectangle( (int)( health_width * health_percentage ),
                                                 health_height, health_offset ) );
        }
    } else
        debug_overlay.add( BLACK, Shape::rectangle( health_width, health_height, offset ) );
    debug_overlay.add( WHITE, Shape::rectangle( health_width, health_height, offset ), 1.f, true );

    debug_overlay.move( position( ) );
    debug_overlay.add( Object::debug_overlay( ) );

    return debug_overlay;
}
#endif
