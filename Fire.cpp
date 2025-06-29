#include "Fire.hpp"
#include "World.hpp"

using mtmercy::Fire;

namespace
{

const uint EDGE_COUNT = 4;
const Planc FLAME_RADIUS_MIN_ABSOLUTE = 0.0001;

} // namespace

Fire::Fire( Room * room, Coordinate cref _position ) : Object( room, _position )
{
    needs_render_always( true );

    no_gravity( );
}

void Fire::render( )
{
    Object::render( );

    auto draw_flame = [ & ] ( Flame cref flame, bool inner = false, bool border = false )
    {
        draw( Color( inner ? m_flame_inner_color : m_flame_color ).a( Settings::get( Settings::GRAPHICS_DETAIL_ALPHA ) ? flame.alpha : 1.0 ), Polygon::equilateral( EDGE_COUNT, flame.radius / ( border ? m_flame_inner_outline_ratio : 1.0 ), flame.offset ) );
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

    if( m_flame_timer.tick( ) )
    {
        if( lit( ) )
        {
            m_flame_timer.reset( Random::rint( m_flame_pause ) );
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

    auto update_flames = [ & ] ( list<Flame> & flames, dec radius_ratio = 1.0 )
    {
        for_each( flame, flames )
        {
            flame.radius -= min( m_flame_shrink * radius_ratio, flame.radius );
            flame.offset += VectorA( RIGHT + Random::negated( Random::rAngle( m_flame_deviation ) ), m_flame_speed ) + ( room( )->wind( ) * ( 1.0 - m_wind_resistance_ratio ) );

            if( m_flame_alpha_shrink )
            {
                flame.alpha -= min( m_flame_alpha_shrink, flame.alpha );
            }
        }
        flames.remove_if( [ & ] ( Flame cref flame )
        {
            return ( ( flame.radius < max( m_flame_radius_min, FLAME_RADIUS_MIN_ABSOLUTE ) ) || is_zero( flame.alpha ) );
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

void Fire::move( Vector cref _velocity )
{
    for_each( flame, m_flames )
    {
        flame.offset -= ( _velocity * ( 1.0 - m_movement_resistance_ratio ) );
    }

    if( m_flames_inner_enabled )
    {
        for_each( flame, m_flames_inner )
        {
            flame.offset -= ( _velocity * ( 1.0 - m_movement_resistance_ratio ) );
        }
    }
}

bool Fire::lit( ) const { return m_lit; }
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

void Fire::enable_base_flame( const bool _b ) { m_flame_base_enabled = _b; }
void Fire::enable_inner_flames( const bool _b ) { m_flames_inner_enabled = _b; }

void Fire::flame_radius( Planc cref _flame_radius ) { m_flame_radius = _flame_radius; space( Polygon::square( m_flame_radius ) ); }
void Fire::flame_speed( Planc cref _flame_speed ) { m_flame_speed = _flame_speed; }
void Fire::flame_shrink( Planc cref _flame_shrink ) { m_flame_shrink = _flame_shrink; }
void Fire::flame_deviation( Angle cref _flame_deviation ) { m_flame_deviation = _flame_deviation; }

void Fire::flame_pause( Span<uint> cref _flame_pause ) { m_flame_pause = _flame_pause; m_flame_timer.reset( Random::rint( m_flame_pause ) ); }

void Fire::flame_color( Color cref _flame_color ) { m_flame_color = _flame_color; }

void Fire::flame_radius_min( Planc cref _flame_radius_min ) { m_flame_radius_min = _flame_radius_min; }

void Fire::flame_base_radius_ratio( Span<dec> cref _flame_base_radius_ratio ) { m_flame_base_radius_ratio = _flame_base_radius_ratio; }
void Fire::flame_base_max_offset_ratio( const dec _flame_base_max_offset_ratio ) { m_flame_base_max_offset_ratio = _flame_base_max_offset_ratio; }

void Fire::flame_inner_radius_ratio( const dec _flame_inner_radius_ratio ) { m_flame_inner_radius_ratio = _flame_inner_radius_ratio; }
void Fire::flame_inner_speed_ratio( const dec _flame_inner_speed_ratio ) { m_flame_inner_speed_ratio = _flame_inner_speed_ratio; }
void Fire::flame_inner_shrink_ratio( const dec _flame_inner_shrink_ratio ) { m_flame_inner_shrink_ratio = _flame_inner_shrink_ratio; }
void Fire::flame_inner_outline_ratio( const dec _flame_inner_outline_ratio ) { m_flame_inner_outline_ratio = _flame_inner_outline_ratio; }

void Fire::flame_inner_color( Color cref _flame_inner_color ) { m_flames_inner_enabled = true; m_flame_inner_color = _flame_inner_color; }

void Fire::flame_alpha_shrink( const dec _flame_alpha_shrink ) { m_flame_alpha_shrink = _flame_alpha_shrink; }

void Fire::wind_resistance_ratio( const dec _wind_resistance_ratio ) { m_wind_resistance_ratio = _wind_resistance_ratio; }
void Fire::movement_resistance_ratio( const dec _movement_resistance_ratio ) { m_movement_resistance_ratio = _movement_resistance_ratio; }

void Fire::light_distance( Planc cref _light_distance ) { m_light_distance = _light_distance; set_light_source( ); }

// todo make ratio to light distance instead? tint shouldn't be greater than light
void Fire::tint_distance( Planc cref _tint_distance ) { m_tint_distance = _tint_distance; set_light_source( ); }
void Fire::tint_flicker( Planc cref _tint_flicker ) { m_tint_flicker = _tint_flicker; set_light_source( ); }
void Fire::tint_intensity( const dec _tint_intensity ) { m_tint_intensity = _tint_intensity; set_light_source( ); }

void Fire::set_light_source( )
{
    clear_light_sources( );

    if( m_light_distance )
    {
        add_light_source( position( ), m_light_distance );

        if( m_tint_distance && m_tint_intensity )
        {
            add_light_source( position( ), m_tint_distance, Color( m_flame_color, m_tint_intensity ), m_tint_flicker );
            // todo add_light_source( position( ), m_tint_distance * 0.8, Color( m_flame_color, m_tint_intensity ), m_tint_flicker );
        }
    }
}
