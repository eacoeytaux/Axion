#include "Mob.hpp"

// TODO make variable probably (even be in mob?)
static Span<uint> BLINK_WAIT = { 120, 240 };
static int BLINK_DURATION = 3;

Mob::Mob( World * world, const Coordinate & _position, const dec _health ) : Object( world, _position )
{
#ifdef AXN_DEBUG
    draw_debug = true;
#endif

    solid( true );

    health( _health );

    m_blink_wait.reset( Random::rint( BLINK_WAIT ) );
    m_blink_duration.reset( BLINK_DURATION );
}

const Mob & Mob::render( ) const
{
    Object::render( );
    return *this;
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

Mob & Mob::update_movement( )
{
    Object::update_movement( );
    if( velocity( ).dx( ) )
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

dec Mob::health( ) const
{
    return m_health.value( );
}

Mob & Mob::health( dec _health )
{
    m_health.value( _health, true );
    return *this;
}

Mob & Mob::health_percentage( const dec _health_percentage )
{
    m_health.value_percentage( _health_percentage );
    return *this;
}

Mob & Mob::heal( dec _health )
{
    m_health.value( min( m_health.value( ) + _health, m_health.max( ) ) );
    return *this;
}

Mob & Mob::heal_full( )
{
    m_health.value( m_health.max( ) );
    return *this;
}

Mob & Mob::hurt( dec _damage )
{
    dec health = Mob::health( ) - min( _damage, Mob::health( ) );
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

dec Mob::max_health( ) const
{
    return m_health.max( );
}

Mob & Mob::max_health( dec _health )
{
    m_health = Slider<dec>( _health );
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
    const Planc HEALTH_BAR_BORDER_WIDTH = 2.0;
    const Planc HEALTH_BAR_WIDTH_MIN = 20.0;
    const dec HEALTH_BAR_TO_OBJECT_RATIO = 1.2;
    const Planc HEALTH_BAR_HEIGHT = 4.0;
    const Planc HEALTH_BAR_OFFSET = HEALTH_BAR_HEIGHT;

    const dec RED_START = 0.1;
    const dec YELLOW_START = 0.5;
    static_once( ) { Assert( RED_START < YELLOW_START ); }

    Drawing debug_overlay;

    Planc health_width = max( HEALTH_BAR_WIDTH_MIN, hit_box( ).width( ) * HEALTH_BAR_TO_OBJECT_RATIO );
    Planc health_height = HEALTH_BAR_HEIGHT;
    Coordinate offset( 0.0, -( hit_box( ).height( ).half( ) + ( health_height * 1.5 ) + HEALTH_BAR_BORDER_WIDTH ) );

    debug_overlay.draw( WHITE, Rectangle( health_width + HEALTH_BAR_BORDER_WIDTH, health_height + HEALTH_BAR_BORDER_WIDTH, offset ) );
    debug_overlay.draw( BLACK, Rectangle( health_width, health_height, offset ) );

    if( alive( ) )
    {
        dec health_percentage = health( ) / max_health( );

        Color health_color;
        if( health_percentage == 1.0 )
        {
            health_color = GREEN;
        }
        else if( health_percentage >= YELLOW_START )
        {
            health_color = ColorSlider( YELLOW, GREEN ).color_at( ( health_percentage - YELLOW_START ) * ( 1.0 / ( 1.0 - YELLOW_START ) ) );
        }
        else if( health_percentage >= RED_START )
        {
            health_color = ColorSlider( RED, YELLOW ).color_at( ( health_percentage - RED_START ) * ( 1.0 / ( 1.0 - RED_START ) ) );
        }
        else // if( health_percentage < RED_START )
        {
            health_color = RED;
        }

        debug_overlay.draw( health_color, Rectangle( health_width * health_percentage, health_height,
                                                     offset + VectorX( ( health_width.half( ) * health_percentage ) - health_width.half( ) ) ) );
    }
    else
    {
        debug_overlay.draw( BLACK, Rectangle( health_width, health_height, offset ) );
    }

    debug_overlay.draw( Object::debug_overlay( ) );

    return debug_overlay;
}
#endif
