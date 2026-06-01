#include "axn/world/Mob.hpp"

#if defined( AXN_DEBUG )
section( DEBUG_OVERLAY )
{
    var_const( Planc HEALTH_BAR_BORDER_WIDTH = ( 1.5 ) );
    var_const( Planc HEALTH_BAR_WIDTH_MIN = ( 20.0 ) );
    var_const( Planc HEALTH_BAR_HEIGHT = ( 5.0 ) );

    var_const( dec HEALTH_BAR_TO_OBJECT_RATIO = ( 1.2 ) );

    var_const( dec YELLOW_START = ( 0.5 ) );
    var_const( dec RED_START = ( 0.1 ) );
}
#endif

section( RAGDOLL )
{
    var_const( dec SWING_DAMPER = ( 0.95 ) );
    var_const( Angle SWING_MIN = ( 0.009 ) );
    var_const( Angle SWING_MAX = ( 0.01 ) );
}

Mob::Mob( ptr<Room> in_room, Point cref in_position, dec in_health ) : Object( in_room, in_position )
{
    #if defined( AXN_DEBUG )
    draw_debug = true;
    #endif

    solid( true );
    interactive( true );

    max_health( in_health, true );

    m_hurt_display_timer.duration( 1 );
}

void Mob::render( )
{
    Object::render( );

    if( !m_hurt_display_timer.completed( ) )
    {
        hurt_display_settings( );
    }
}

void Mob::hurt_display_settings( )
{
    color_filter( [ ] ( Color ref color )
    {
        color.r( ( one_minus( color.r( ) ) * 0.25 ) + color.r( ) );
        color.g( 0.0 );
        color.b( 0.0 );
    } );
}

void Mob::draw_eye( Point cref in_position, Angle cref in_angle )
{
    if( m_eye_info_set )
    {
        if( alive( ) )
        {
            Planc eye_width = x2( m_eye_radius );
            Planc eye_height = eyes_closed( ) ? Planc( 1.0 ) : ( eye_width * ( squinting( ) ? 0.5 : 1.0 ) );

            Polygon eye = Polygon::rectangle( eye_width, eye_height ).rotate( in_angle ).move( in_position );

            draw( eye_color( ), eye );
        }
        else
        {
            draw( eye_color( ), Line( in_position + Vector( m_eye_radius, m_eye_radius ), in_position + Vector( -m_eye_radius, -m_eye_radius ) ), 0.75 );
            draw( eye_color( ), Line( in_position + Vector( m_eye_radius, -m_eye_radius ), in_position + Vector( -m_eye_radius, m_eye_radius ) ), 0.75 );
        }
    }
}

void Mob::eye_info( Planc cref in_eye_radius, ticks in_blink_duration, Span<ticks> cref in_blink_pause_duration, Color cref in_eye_color )
{
    m_eye_radius = in_eye_radius;
    m_eyes_closed = false;
    m_eyes_squinting = false;
    m_blink_duration = in_blink_duration;
    m_blink_pause_duration = in_blink_pause_duration;
    m_eye_color = in_eye_color;

    m_blink_timer.duration( m_blink_duration );

    m_blink_pause_timer.reset( Random::rdec( m_blink_pause_duration ) );

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

void Mob::trim_velocity( )
{
    Object::trim_velocity( );
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
    return( m_alive );
}

void Mob::die( )
{
    if( alive( ) )
    {
        m_alive = false;
    }
}

void Mob::revive( )
{
    if( dead( ) )
    {
        m_alive = true;

        heal_full( );
    }
}

dec Mob::health_percentage( ) const
{
    return( m_health.value_percentage( ) );
}

dec Mob::health( ) const
{
    return( m_health.value( ) );
}

dec Mob::max_health( ) const
{
    return( m_health.max( ) );
}

void Mob::max_health( dec in_health, bool in_add )
{
    dec health = Mob::health( );

    if( in_add )
    {
        dec max_health = Mob::max_health( );
        dec d_health = in_health - max_health;

        if( is_pos( d_health ) )
        {
            health += d_health;
        }
    }

    m_health = Slider<dec>( in_health );
    m_health.value( min( health, in_health ) );
}

void Mob::heal( dec in_health )
{
    m_health.value( min( m_health.value( ) + in_health, m_health.max( ) ) );
}

void Mob::hurt( dec in_damage )
{
    if( in_damage )
    {
        dec health = Mob::health( ) - min( in_damage, Mob::health( ) );

        if( health <= 0.0 )
        {
            if( alive( ) )
            {
                m_killing_blow = in_damage;

                die( );
            }
        }
        else
        {
            invincible_pause( invincible_duration( ) );
        }

        m_health.value( max( 0.0, health ) );
        m_hurt_display_timer.reset( );
    }
}

void Mob::damage( Damage cref in_damage )
{
    if( damages( in_damage ) )
    {
        hurt( in_damage.value( ) );
    }
}

bool Mob::facing_right( ) const
{
    return( m_facing_right );
}

void Mob::facing_right( bool in_facing_right )
{
    m_facing_right = in_facing_right;
}

#if defined( AXN_DEBUG )
Drawing Mob::debug_overlay( ) const
{
    Assert( RED_START < YELLOW_START );

    Drawing debug_overlay;

    if( Settings::get( Settings::DEBUG_HEALTH ) )
    {
        if( alive( ) )
        {
            Planc health_width = max( HEALTH_BAR_WIDTH_MIN, width( ) * HEALTH_BAR_TO_OBJECT_RATIO );
            Planc health_height = HEALTH_BAR_HEIGHT;

            Point offset( 0.0, ( health_height * -1.5 ) - HEALTH_BAR_BORDER_WIDTH - half( height( ) ) );

            dec health_percentage = health( ) / max_health( );
            Assert( in_range( health_percentage, 0.0, 1.0, true ) );

            Color health_color;
            if( invincible_always( ) )
            {
                health_color = CYAN;
            }
            else if( is_one( health_percentage ) )
            {
                health_color = GREEN;
            }
            else if( health_percentage >= YELLOW_START )
            {
                health_color = ColorSlider( YELLOW, GREEN ).color_at( ( health_percentage - YELLOW_START ) * inverse( one_minus( YELLOW_START ) ) );
            }
            else if( health_percentage >= RED_START )
            {
                health_color = ColorSlider( RED, YELLOW ).color_at( ( health_percentage - RED_START ) * inverse( one_minus( RED_START ) ) );
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

    return( debug_overlay.draw( Object::debug_overlay( ) ) );
}
#endif

void Mob::RagDollLimb::update( )
{
    Assert( SWING_MIN < SWING_MAX ); // todo

    Angle d_angle_total;

    for_range( i, m_limbs.size( ) )
    {
        Limb ref limb = m_limbs[ i ];

        Vector force;

        force += ( parent( )->force( ).flip( ) * limb.force_ratio );
        force += ( parent( )->gravity( ) );

        Angle force_offset = A0;

        if( force.has_magnitude( ) )
        {
            force_offset = limb.angle.min_distance( force.angle( ) );
        }

        limb.d_angle += minmax( force_offset, -SWING_MAX, SWING_MAX );

        // TODO add turbulence

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
