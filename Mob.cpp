#include "Mob.hpp"

// TODO make variable probably (even be in mob?)
static Span<uint> BLINK_WAIT = { 120, 240 };
static int BLINK_DURATION = 3;

Mob::Mob( World * world, const Coordinate & _position, const double _health ) : Object( world, _position )
{
#ifdef AXN_DEBUG
    draw_debug = true;
#endif

    solid( true );

    health( _health );

    m_blink_wait.reset( Random::rint( BLINK_WAIT ) );
    m_blink_duration.reset( BLINK_DURATION );
}

Mob & Mob::update( )
{
    Object::update( );

    if( m_blink_wait.tick( ) )
    {
        if( m_blink_duration.remaining( ) == BLINK_DURATION )
        {
            drawing_dirty( true );
        }

        if( m_blink_duration.tick( ) )
        {
            m_blink_duration.reset( BLINK_DURATION );
            m_blink_wait.reset( Random::rint( BLINK_WAIT ) );
            drawing_dirty( true );
        }
    }

    return *this;
}

Mob & Mob::update_velocity( )
{
    Object::update_velocity( );
    return *this;
}

Mob & Mob::move( )
{
    Object::move( );
    if( velocity( ).has_magnitude( ) )
    {
        facing_left( velocity( ).dx( ) < 0.0 );
    }
    return *this;
}

bool Mob::alive( ) const
{
    return m_alive;
}

Mob & Mob::die( )
{
    m_health.value( 0.0 );
    m_alive = false;
    return *this;
}

double Mob::health( ) const
{
    return m_health.value( );
}

Mob & Mob::health( double _health )
{
    m_health.value( _health, true );
    return *this;
}

Mob & Mob::health_percentage( const double _health_percentage )
{
    m_health.value_percentage( _health_percentage );
    return *this;
}

Mob & Mob::heal( double _health )
{
    m_health.value( min( m_health.value( ) + _health, m_health.max( ) ) );
    return *this;
}

Mob & Mob::heal_full( )
{
    m_health.value( m_health.max( ) );
    return *this;
}

Mob & Mob::hurt( double _damage )
{
    double health = Mob::health( ) - min( _damage, Mob::health( ) );
    if( health == 0.0 )
    {
        m_health.value( 0.0 );
        die( );
    }
    else
    {
        m_health.value( health );
    }
    return *this;
}

uint Mob::max_health( ) const
{
    return m_health.max( );
}

Mob & Mob::max_health( double _health )
{
    m_health = Slider<double>( _health );
    return *this;
}

bool Mob::facing_left( ) const
{
    return m_facing_left;
}

Mob & Mob::facing_left( const bool _facing_left )
{
    m_facing_left = _facing_left;
    return *this;
}

#ifdef AXN_DEBUG
Drawing Mob::debug_overlay( ) const
{
    Drawing debug_overlay;

    Planc health_width = hit_box( ).width( ) * 1.2;
    Planc health_height = 5.0;
    Coordinate offset = Coordinate( 0.0, -( ( space( ).bound_height( ).half( ) ) * 1.2 ) - health_height );

    debug_overlay.draw( WHITE, Polygon::rectangle( health_width + 2.0, health_height + 2.0, offset ) );

    double health_percentage = (double)health( ) / (double)max_health( );
    if( alive( ) )
    {
        if( health_percentage == 1.0 )
        {
            debug_overlay.draw( GREEN, Polygon::rectangle( health_width, health_height, offset ) );
        }
        else
        {
            debug_overlay.draw( RED, Polygon::rectangle( health_width, health_height, offset ) );
            Coordinate health_offset = offset + VectorX( floor( -health_width.half( ) + ( health_width.half( ) ) * health_percentage ) );
            debug_overlay.draw( GREEN, Polygon::rectangle( (int)( health_width * health_percentage ), health_height, health_offset ) );
        }
    }
    else
    {
        debug_overlay.draw( BLACK, Polygon::rectangle( health_width, health_height, offset ) );
    }

    debug_overlay.move( position( ) );
    debug_overlay.draw( Object::debug_overlay( ) );

    return debug_overlay;
}
#endif
