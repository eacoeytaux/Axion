#include "Mob.hpp"

#ifdef AXN_DEBUG
bool Mob::draw_health = true;
#endif

Mob::Mob( World * world, const Coordinate & _position, const dec _health ) : Object( world, _position )
{
    #ifdef AXN_DEBUG
    m_draw_debug = true;
    #endif

    solid( true );
    interactive( true );

    health( _health );

    m_hurt_display.set( ONE );
}

void Mob::render( )
{
    Object::render( );

    if( m_hurt_display.remaining( ) )
    {
        hurt_display_settings( );
    }
}

void Mob::hurt_display_settings( )
{
    filter_function( [ ] ( Color & color )
    {
        color.r( ( ( ONE - color.r( ) ) * 0.25 ) + color.r( ) );
        color.g( ZERO );
        color.b( ZERO );
    } );
}

void Mob::draw_eyes( const Coordinate & _position, const Angle & _angle )
{
    if( m_eye_info_set )
    {
        Planc eye_width = m_eye_radius * TWO;
        Planc eye_height = eyes_closed( ) ? (Planc)ONE : ( eye_width * ( squinting( ) ? HALF : ONE ) );

        Polygon eye = Polygon::rectangle( eye_width, eye_height ).rotate( _angle ).move( _position );

        draw( eye_color( ), eye );
    }
}

void Mob::eye_info( const Planc _eye_radius, const uint _blink_duration, const Span<uint> & _blink_wait_span, const Color & _eye_color )
{
    m_eye_radius = _eye_radius;
    m_eyes_closed = false;
    m_eyes_squinting = false;
    m_blink_duration = _blink_duration;
    m_blink_wait_span = _blink_wait_span;
    m_eye_color = _eye_color;

    m_blink_wait_counter.reset( Random::rint( m_blink_wait_span ) );
    m_blink_duration_counter.reset( m_blink_duration );

    m_eye_info_set = true;
}

void Mob::update( )
{
    Object::update( );

    m_hurt_display.tick( );
    m_invincible_counter.tick( );

    // eyes
    if( m_blink_wait_counter.tick( ) )
    {
        if( m_blink_duration_counter.remaining( ) == m_blink_duration )
        {
            needs_render( true );
        }

        if( m_blink_duration_counter.tick( ) )
        {
            m_blink_duration_counter.reset( m_blink_duration );
            m_blink_wait_counter.reset( Random::rint( m_blink_wait_span ) );
            needs_render( true );
        }
    }
}

void Mob::update_velocity( )
{
    Object::update_velocity( );
}

void Mob::update_movement( )
{
    Object::update_movement( );
    if( velocity( ).dx( ) )
    {
        facing_left( velocity( ).dx( ) < ZERO );
    }
}

bool Mob::alive( ) const
{
    return m_alive;
}

void Mob::die( )
{
    if( !invincible( ) )
    {
        m_health.value( ZERO );
        m_alive = false;
    }
}

dec Mob::health( ) const
{
    return m_health.value( );
}

void Mob::health( dec _health )
{
    m_health.value( _health, true );
}

dec Mob::health_percentage( ) const
{
    return m_health.value_percentage( );
}

void Mob::health_percentage( const dec _health_percentage )
{
    m_health.value_percentage( _health_percentage );
}

void Mob::heal( dec _health )
{
    m_health.value( min( m_health.value( ) + _health, m_health.max( ) ) );
}

void Mob::heal_full( )
{
    m_health.value( m_health.max( ) );
}

void Mob::hurt( dec _damage )
{
    if( !invincible( ) )
    {
        dec health = Mob::health( ) - min( _damage, Mob::health( ) );
        if( health <= ZERO )
        {
            m_health.value( ZERO );
            die( );
        }
        else
        {
            m_health.value( health );
            m_hurt_display.reset( );
            invincible_pause( invincible_duration( ) );
        }
    }
}

dec Mob::max_health( ) const
{
    return m_health.max( );
}

void Mob::max_health( dec _health )
{
    dec health = Mob::health( );
    m_health = Slider<dec>( _health );
    m_health.value( min( health, _health ) );
}

bool Mob::facing_right( ) const
{
    return m_facing_right;
}

void Mob::facing_right( const bool _facing_right )
{
    m_facing_right = _facing_right;
}

#ifdef AXN_DEBUG
Drawing Mob::debug_overlay( ) const
{
    const Planc HEALTH_BAR_BORDER_WIDTH = 1.5;
    const Planc HEALTH_BAR_WIDTH_MIN = 20.0;
    const dec HEALTH_BAR_TO_OBJECT_RATIO = 1.2;
    const Planc HEALTH_BAR_HEIGHT = 5.0;
    const Planc HEALTH_BAR_OFFSET = HEALTH_BAR_HEIGHT;

    const dec RED_START = 0.1;
    const dec YELLOW_START = 0.5;
    Assert( RED_START < YELLOW_START );

    Drawing debug_overlay;

    if( draw_health )
    {
        // if( !invincible_always( ) )
        {
            Planc health_width = max( HEALTH_BAR_WIDTH_MIN, hit_box( ).width( ) * HEALTH_BAR_TO_OBJECT_RATIO );
            Planc health_height = HEALTH_BAR_HEIGHT;
            Coordinate offset( ZERO, -( half( hit_box( ).height( ) ) + ( health_height * 1.5 ) + HEALTH_BAR_BORDER_WIDTH ) );

            debug_overlay.draw( BLACK, Polygon::rectangle( health_width, health_height, offset ) );

            if( alive( ) )
            {
                dec health_percentage = health( ) / max_health( );
                Assert( in_range( health_percentage, ZERO, ONE, true ) );

                Color health_color;
                if( health_percentage == ONE )
                {
                    health_color = GREEN;
                }
                else if( health_percentage >= YELLOW_START )
                {
                    health_color = ColorSlider( YELLOW, GREEN ).color_at( ( health_percentage - YELLOW_START ) * inverse( ONE - YELLOW_START ) );
                }
                else if( health_percentage >= RED_START )
                {
                    health_color = ColorSlider( RED, YELLOW ).color_at( ( health_percentage - RED_START ) * inverse( ONE - RED_START ) );
                }
                else
                {
                    health_color = RED;
                }

                debug_overlay.draw( health_color, Polygon::rectangle( health_width * health_percentage, health_height,
                                                                      offset + Vector::X( ( half( health_width ) * health_percentage ) - half( health_width ) ) ) );
            }

            debug_overlay.draw( WHITE, Polygon::rectangle( health_width, health_height, offset ), HEALTH_BAR_BORDER_WIDTH, true );
        }
    }

    debug_overlay.draw( Object::debug_overlay( ) );
    return debug_overlay;
}
#endif
