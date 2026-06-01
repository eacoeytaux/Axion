#include "Mob.hpp"

Mob::Mob( Room * room, Coordinate cref _position, cdec _health ) : Object( room, _position )
{
    #if defined ( AXN_DEBUG )
    draw_debug = true;
    #endif

    solid( true );
    interactive( true );

    max_health( _health );
    heal_full( );

    m_hurt_display_timer.duration( 1 );
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

void Mob::draw_eye( Coordinate cref _position, Angle cref _angle )
{
    if( m_eye_info_set )
    {
        if( alive( ) )
        {
            Planc eye_width = m_eye_radius * 2.0;
            Planc eye_height = eyes_closed( ) ? Planc( 1.0 ) : ( eye_width * ( squinting( ) ? 0.5 : 1.0 ) );

            Polygon eye = Polygon::rectangle( eye_width, eye_height ).rotate( _angle ).move( _position );

            draw( eye_color( ), eye );
        }
        else
        {
            draw( eye_color( ), Line( _position + Vector( m_eye_radius, m_eye_radius ), _position + Vector( -m_eye_radius, -m_eye_radius ) ), 0.75 );
            draw( eye_color( ), Line( _position + Vector( m_eye_radius, -m_eye_radius ), _position + Vector( -m_eye_radius, m_eye_radius ) ), 0.75 );
        }
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

    m_blink_timer.duration( m_blink_duration );

    m_blink_pause_timer.reset( Random::rint( m_blink_pause_duration ) );

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
        needs_render( true );

        m_blink_timer.reset( m_blink_duration );
        m_blink_pause_timer.reset( Random::rint( m_blink_pause_duration ) );
    }

    if( m_blink_timer.tick( ) )
    {
        needs_render( true );
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
    // todo Assert( alive( ) );

    m_alive = false;
}

void Mob::revive( )
{
    // todo Assert( dead( ) );

    m_alive = true;

    heal_full( );
}

dec Mob::health_percentage( ) const
{
    return m_health.value_percentage( );
}

dec Mob::health( ) const
{
    return m_health.value( );
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

void Mob::heal( dec _health )
{
    m_health.value( min( m_health.value( ) + _health, m_health.max( ) ) );
}

void Mob::hurt( dec _damage )
{
    if( _damage )
    {
        dec health = Mob::health( ) - min( _damage, Mob::health( ) );

        if( health <= 0.0 )
        {
            die( );
        }
        else
        {
            invincible_pause( invincible_duration( ) );
        }

        m_health.value( max( 0.0, health ) );
        m_hurt_display_timer.reset( );
    }
}

void Mob::damage( Damage cref _damage )
{
    if( damages( _damage ) )
    {
        hurt( _damage.value( ) );
    }
}

bool Mob::facing_right( ) const
{
    return m_facing_right;
}

void Mob::facing_right( cbool _facing_right )
{
    m_facing_right = _facing_right;
}

#if defined ( AXN_DEBUG )
Drawing Mob::debug_overlay( ) const
{
    cPlanc HEALTH_BAR_BORDER_WIDTH = 1.5;
    cPlanc HEALTH_BAR_WIDTH_MIN = 20.0;
    cPlanc HEALTH_BAR_HEIGHT = 5.0;

    cdec HEALTH_BAR_TO_OBJECT_RATIO = 1.2;

    cdec RED_START = 0.1;
    cdec YELLOW_START = 0.5;

    Assert( RED_START < YELLOW_START );

    Drawing debug_overlay;

    if( Settings::get( Settings::DEBUG_HEALTH ) )
    {
        if( alive( ) )
        {
            Planc health_width = max( HEALTH_BAR_WIDTH_MIN, width( ) * HEALTH_BAR_TO_OBJECT_RATIO );
            Planc health_height = HEALTH_BAR_HEIGHT;

            Coordinate offset( 0.0, -( half( height( ) ) + ( health_height * 1.5 ) + HEALTH_BAR_BORDER_WIDTH ) );

            dec health_percentage = health( ) / max_health( );
            Assert( in_range( health_percentage, 0.0, 1.0, true ) );

            Color health_color;
            if( invincible_always( ) )
            {
                health_color = CYAN;
            }
            else if( health_percentage == 1.0 )
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

            debug_overlay.draw( BLACK, Polygon::rectangle( health_width, health_height, offset ) );

            debug_overlay.draw( health_color, Polygon::rectangle( health_width * health_percentage, health_height,
                                                                  offset + VectorX( ( half( health_width ) * health_percentage ) - half( health_width ) ) ) );

            debug_overlay.draw( WHITE, Polygon::rectangle( health_width, health_height, offset ), HEALTH_BAR_BORDER_WIDTH, true );
        }
    }

    return ( debug_overlay.draw( Object::debug_overlay( ) ) );
}
#endif

void Mob::RagDollLimb::update( )
{
    cdec SWING_DAMPER = 0.95; // todo
    cAngle SWING_MIN = 0.009; // todo
    cAngle GRAVITY_SWING_MAX = 0.01; // todo
    Assert( SWING_MIN < GRAVITY_SWING_MAX ); // todo

    Angle d_angle_total;

    for_range( i, m_limbs.size( ) )
    {
        Limb & limb = m_limbs[ i ];

        if( parent( )->has_gravity( ) )
        {
            Angle gravity_offset = limb.angle.min_distance( parent( )->gravity( ).angle( ) );
            limb.d_angle += minmax( -GRAVITY_SWING_MAX, gravity_offset, GRAVITY_SWING_MAX );
        }

        limb.d_angle *= SWING_DAMPER;

        if( in_range( limb.d_angle, SWING_MIN, -SWING_MIN ) )
        {
            limb.d_angle = A0;
        }
        else
        {
            d_angle_total += limb.d_angle;
            limb.angle += d_angle_total;

            Angle prev_angle;
            for_range( j, i )
            {
                prev_angle += angle( j );
            }

            if( limb.has_ccw_limit )
            {
                if( is_pos( d_angle_total ) )
                {
                    limb.angle = min( limb.angle, prev_angle + limb.ccw_limit );
                }
            }

            if( limb.has_cw_limit )
            {
                if( is_neg( d_angle_total ) )
                {
                    limb.angle = max( limb.angle, prev_angle - limb.cw_limit );
                }
            }
        }
    }
}
