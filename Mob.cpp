#include "Mob.hpp"

#ifdef AXN_DEBUG
bool Mob::draw_health = true;
#endif

Mob::Mob( Room * room, Coordinate cref _position, cdec _health ) : Object( room, _position )
{
    #ifdef AXN_DEBUG
    m_draw_debug = true;
    #endif

    solid( true );
    interactive( true );

    health( _health );

    m_hurt_display_timer.duration( 0 );
}

void Mob::render( )
{
    Object::render( );

    if( m_hurt_display_timer.remaining( ) )
    {
        hurt_display_settings( );
    }
}

void Mob::hurt_display_settings( )
{
    filter_function( [ ] ( Color & color )
    {
        color.r( ( ( 1.0 - color.r( ) ) * 0.25 ) + color.r( ) );
        color.g( 0.0 );
        color.b( 0.0 );
    } );
}

void Mob::draw_eyes( Coordinate cref _position, Angle cref _angle )
{
    if( m_eye_info_set )
    {
        Planc eye_width = m_eye_radius * 2.0;
        Planc eye_height = eyes_closed( ) ? Planc( 1.0 ) : ( eye_width * ( squinting( ) ? 0.5 : 1.0 ) );

        Polygon eye = Polygon::rectangle( eye_width, eye_height ).rotate( _angle ).move( _position );

        draw( eye_color( ), eye );
    }
}

void Mob::eye_info( cPlanc _eye_radius, cuint _blink_duration, const Span<uint> & _blink_pause_duration, Color cref _eye_color )
{
    m_eye_radius = _eye_radius;
    m_eyes_closed = false;
    m_eyes_squinting = false;
    m_blink_duration = _blink_duration;
    m_blink_pause_duration = _blink_pause_duration;
    m_eye_color = _eye_color;

    m_blink_pause_timer.reset( Random::rint( m_blink_pause_duration ) );
    m_blink_timer.reset( m_blink_duration );

    m_eye_info_set = true;
}

void Mob::update( )
{
    Object::update( );

    m_hurt_display_timer.tick( );
    m_invincible_timer.tick( );

    // eyes
    if( m_blink_pause_timer.tick( ) )
    {
        if( m_blink_timer.remaining( ) == m_blink_duration )
        {
            needs_render( true );
        }

        if( m_blink_timer.tick( ) )
        {
            m_blink_timer.reset( m_blink_duration );
            m_blink_pause_timer.reset( Random::rint( m_blink_pause_duration ) );
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
        facing_left( velocity( ).dx( ) < 0.0 );
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
        m_health.value( 0.0 );
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

void Mob::health_percentage( cdec _health_percentage )
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
        if( health <= 0.0 )
        {
            m_health.value( 0.0 );
            die( );
        }
        else
        {
            m_health.value( health );
            m_hurt_display_timer.reset( );
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

void Mob::facing_right( cbool _facing_right )
{
    m_facing_right = _facing_right;
}

#ifdef AXN_DEBUG
Drawing Mob::debug_overlay( ) const
{
    cPlanc HEALTH_BAR_BORDER_WIDTH = 1.5;
    cPlanc HEALTH_BAR_WIDTH_MIN = 20.0;
    cdec HEALTH_BAR_TO_OBJECT_RATIO = 1.2;
    cPlanc HEALTH_BAR_HEIGHT = 5.0;
    cPlanc HEALTH_BAR_OFFSET = HEALTH_BAR_HEIGHT;

    cdec RED_START = 0.1;
    cdec YELLOW_START = 0.5;
    Assert( RED_START < YELLOW_START );

    Drawing debug_overlay;

    if( draw_health )
    {
        // if( !invincible_always( ) )
        {
            Planc health_width = max( HEALTH_BAR_WIDTH_MIN, hit_box( ).width( ) * HEALTH_BAR_TO_OBJECT_RATIO );
            Planc health_height = HEALTH_BAR_HEIGHT;
            Coordinate offset( 0.0, -( half( hit_box( ).height( ) ) + ( health_height * 1.5 ) + HEALTH_BAR_BORDER_WIDTH ) );

            debug_overlay.draw( BLACK, Polygon::rectangle( health_width, health_height, offset ) );

            if( alive( ) )
            {
                dec health_percentage = health( ) / max_health( );
                Assert( in_range( health_percentage, 0.0, 1.0, true ) );

                Color health_color;
                if( health_percentage == 1.0 )
                {
                    health_color = GREEN;
                }
                else if( health_percentage >= YELLOW_START )
                {
                    health_color = ColorSlider( YELLOW, GREEN ).color_at( ( health_percentage - YELLOW_START ) * inverse( 1.0 - YELLOW_START ) );
                }
                else if( health_percentage >= RED_START )
                {
                    health_color = ColorSlider( RED, YELLOW ).color_at( ( health_percentage - RED_START ) * inverse( 1.0 - RED_START ) );
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
