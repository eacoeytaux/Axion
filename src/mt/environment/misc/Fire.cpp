#include "mt/environment/misc/Fire.hpp"

var_const( uint EDGE_COUNT = ( 4 ) );
var_const( Planc FLAME_RADIUS_MIN_ABSOLUTE = ( 0.0001 ) );

Fire::Fire( ptr<Room> in_room, Point cref in_position ) : Object( in_room, in_position )
{
    needs_render_always( true );

    no_gravity( );
}

void Fire::render( )
{
    Object::render( );

    lambdaf( draw_flame )( Flame cref in_flame, bool in_inner = false, bool in_border = false )
    {
        draw( Color( in_inner ? m_flame_inner_color : m_flame_color ).a( Settings::get( Settings::GRAPHICS_DETAIL_ALPHA ) ? ( in_flame.alpha ) : ( 1.0 ) ), Polygon::equilateral( EDGE_COUNT, in_flame.radius / ( in_border ? m_flame_inner_outline_ratio : 1.0 ), in_flame.offset ) );
    };

    if( m_flame_base_enabled )
    {
        draw_flame( m_flame_base );
    }

    for_each( flame, m_flames )
    {
        draw_flame( flame );
    }

    if( m_flames_inner_enabled )
    {
        for_each( flame, m_flames_inner )
        {
            draw_flame( flame, false, true );
        }

        for_each( flame, m_flames_inner )
        {
            draw_flame( flame, true, false );
        }

        if( m_flame_base_enabled )
        {
            draw_flame( m_flame_base_inner, false, true );
            draw_flame( m_flame_base_inner, true, false );
        }
    }
}

void Fire::update( )
{
    Object::update( );

    if( !lit( ) )
    {
        m_flame_radius -= m_flame_extinguished_shrink;
        m_flame_radius = max( P0, m_flame_radius );
    }

    if( m_flame_timer.tick( ) )
    {
        m_flame_timer.reset( Random::rdec( m_flame_pause ) );

        if( m_flame_base_enabled && is_pos( m_flame_radius ) )
        {
            m_flames.insert_back( ).radius = m_flame_radius;

            if( m_flames_inner_enabled )
            {
                m_flames_inner.insert_back( ).radius = m_flame_inner_radius_ratio * m_flame_radius;
            }
        }
    }

    if( m_flame_base_enabled )
    {
        m_flame_base.radius = Random::rPlanc( m_flame_base_radius_ratio ) * m_flame_radius;
        m_flame_base.offset = VectorA( Random::rAngle( ), Random::rPlanc( m_flame_base_max_offset_ratio ) * m_flame_base.radius );

        if( m_flames_inner_enabled )
        {
            m_flame_base_inner.radius = m_flame_base.radius * m_flame_inner_radius_ratio;
            m_flame_base_inner.offset = m_flame_base.offset;
        }
    }

    lambdaf( update_flames )( list<Flame> ref in_flames, dec in_radius_ratio = 1.0 )
    {
        for_each( flame, in_flames )
        {
            flame.radius -= min( m_flame_shrink * in_radius_ratio, flame.radius );
            flame.offset += VectorA( RIGHT + Random::negated( Random::rAngle( m_flame_deviation ) ), m_flame_speed ) + ( room( )->wind( ) * one_minus( m_wind_resistance_ratio ) );

            if( m_flame_alpha_shrink )
            {
                flame.alpha -= min( m_flame_alpha_shrink, flame.alpha );
            }
        }

        in_flames.remove_if( [ & ] ( Flame cref in_flame )
        {
            return( ( in_flame.radius < max( m_flame_radius_min, FLAME_RADIUS_MIN_ABSOLUTE ) ) || is_zero( in_flame.alpha ) );
        } );
    };

    update_flames( m_flames );

    if( m_flames_inner_enabled )
    {
        update_flames( m_flames_inner, m_flame_inner_shrink_ratio );
    }

    if( !lit( ) && !m_flames.size( ) && ( !m_flames_inner_enabled || !m_flames_inner.size( ) ) )
    {
        mark_to_delete( );
    }
}

void Fire::move( Vector cref in_velocity )
{
    for_each( flame, m_flames )
    {
        flame.offset -= ( in_velocity * one_minus( m_movement_resistance_ratio ) );
    }

    if( m_flames_inner_enabled )
    {
        for_each( flame, m_flames_inner )
        {
            flame.offset -= ( in_velocity * one_minus( m_movement_resistance_ratio ) );
        }
    }
}

bool Fire::lit( ) const { return( m_lit ); }
void Fire::extinguish( )
{
    if( m_lit )
    {
        m_lit = false;

        if( m_flame_base_enabled )
        {
            // todo make base shrink in size instead of just stop

            m_flame_base_enabled = false;
            m_flames.insert_back( m_flame_base );

            if( m_flames_inner_enabled )
            {
                m_flames_inner.insert_back( m_flame_base_inner );
            }
        }
    }
}

void Fire::enable_base_flame( bool in_enable ) { m_flame_base_enabled = in_enable; }
void Fire::enable_inner_flames( bool in_enable ) { m_flames_inner_enabled = in_enable; }

void Fire::flame_radius( Planc cref in_flame_radius ) { m_flame_radius = in_flame_radius; space( Polygon::square( m_flame_radius ) ); }
void Fire::flame_speed( Planc cref in_flame_speed ) { m_flame_speed = in_flame_speed; }
void Fire::flame_shrink( Planc cref in_flame_shrink ) { m_flame_shrink = in_flame_shrink; }
void Fire::flame_extinguished_shrink( Planc cref in_flame_extinguished_shrink ) { m_flame_extinguished_shrink = in_flame_extinguished_shrink; }

void Fire::flame_deviation( Angle cref in_flame_deviation ) { m_flame_deviation = in_flame_deviation; }

void Fire::flame_pause( Span<ticks> cref in_flame_pause ) { m_flame_pause = in_flame_pause; m_flame_timer.reset( Random::rint( m_flame_pause ) ); }

void Fire::flame_color( Color cref in_flame_color ) { m_flame_color = in_flame_color; }

void Fire::flame_radius_min( Planc cref in_flame_radius_min ) { m_flame_radius_min = in_flame_radius_min; }

void Fire::flame_base_radius_ratio( Span<dec> cref in_flame_base_radius_ratio ) { m_flame_base_radius_ratio = in_flame_base_radius_ratio; }
void Fire::flame_base_max_offset_ratio( dec in_flame_base_max_offset_ratio ) { m_flame_base_max_offset_ratio = in_flame_base_max_offset_ratio; }

void Fire::flame_inner_radius_ratio( dec in_flame_inner_radius_ratio ) { m_flame_inner_radius_ratio = in_flame_inner_radius_ratio; }
void Fire::flame_inner_speed_ratio( dec in_flame_inner_speed_ratio ) { m_flame_inner_speed_ratio = in_flame_inner_speed_ratio; }
void Fire::flame_inner_shrink_ratio( dec in_flame_inner_shrink_ratio ) { m_flame_inner_shrink_ratio = in_flame_inner_shrink_ratio; }
void Fire::flame_inner_outline_ratio( dec in_flame_inner_outline_ratio ) { m_flame_inner_outline_ratio = in_flame_inner_outline_ratio; }

void Fire::flame_inner_color( Color cref in_flame_inner_color ) { m_flames_inner_enabled = true; m_flame_inner_color = in_flame_inner_color; }

void Fire::flame_alpha_shrink( dec in_flame_alpha_shrink ) { m_flame_alpha_shrink = in_flame_alpha_shrink; }

void Fire::wind_resistance_ratio( dec in_wind_resistance_ratio ) { m_wind_resistance_ratio = in_wind_resistance_ratio; }
void Fire::movement_resistance_ratio( dec in_movement_resistance_ratio ) { m_movement_resistance_ratio = in_movement_resistance_ratio; }

void Fire::light_distance( Planc cref in_light_distance ) { m_light_distance = in_light_distance; set_light_source( ); }

// todo make ratio to light distance instead? tint shouldn't be greater than light
void Fire::tint_distance( Planc cref in_tint_distance ) { m_tint_distance = in_tint_distance; set_light_source( ); }
void Fire::tint_flicker( Planc cref in_tint_flicker ) { m_tint_flicker = in_tint_flicker; set_light_source( ); }
void Fire::tint_intensity( dec in_tint_intensity ) { m_tint_intensity = in_tint_intensity; set_light_source( ); }

void Fire::set_light_source( )
{
    clear_light_sources( );

    if( m_light_distance )
    {
        add_light_source( position( ), m_light_distance );

        if( m_tint_distance && m_tint_intensity )
        {
            add_light_source( position( ), m_tint_distance, m_tint_flicker, Color( m_flame_color, m_tint_intensity ) );
            // todo add_light_source( position( ), m_tint_distance * 0.8, m_tint_flicker, Color( m_flame_color, m_tint_intensity ) );
        }
    }
}
