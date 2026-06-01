#include "mt/environment/misc/CampFire.hpp"

var_const( Span<ticks> FLAME_PAUSE = { 8 } );

var_const( Planc FLAME_ORANGE_RADIUS = ( 30.0 ) );
var_const( Planc FLAME_ORANGE_SPEED = ( 1.0 ) );
var_const( Planc FLAME_ORANGE_SHRINK = ( 0.375 ) );

var_const( dec FLAME_YELLOW_RADIUS_RATIO = ( 0.625 ) );
var_const( dec FLAME_YELLOW_SPEED_RATIO = ( 1.25 ) );
var_const( dec FLAME_YELLOW_SHRINK_RATIO = ( 1.0 ) );
var_const( dec FLAME_YELLOW_OUTLINE_RATIO = ( 1.5 ) );

var_const( Span<dec> FLAME_BASE_RADIUS_RATIO = { 1.0, 1.1 } );
var_const( dec FLAME_BASE_OFFSET_MAX_RATIO = ( 0.01 ) );

var_const( Planc FLAME_RADIUS_MIN = ( 0.1 ) );

var_const( Planc FLAME_EXTINGUISHED_SHRINK = ( 2.5 ) );

var_const( Angle FLAME_DEVIATION = ( Angle::fraction( 8.0 ) ) );

var_const( dec WIND_RESISTANCE_RATIO = ( 0.5 ) );

var_const( Planc LIGHT_DISTANCE = ( FLAME_ORANGE_RADIUS * 4.0 ) );
var_const( Planc TINT_DISTANCE = ( LIGHT_DISTANCE * 0.8 ) );
var_const( Planc TINT_FLICKER = ( 2.0 ) );
var_const( dec TINT_INTENSITY = ( 0.125 ) );

var_const( dec FLAME_ALPHA_SHRINK = ( 0.01 ) );

var_const( Color FLAME_COLOR_ORANGE = ( Color::rgb( 0xFF8000 ) ) );
var_const( Color FLAME_COLOR_YELLOW = ( Color::rgb( 0xFFC000 ) ) );

CampFire::CampFire( ptr<Room> in_room, Point cref in_position ) : Fire( in_room, in_position )
{
    enable_base_flame( true );

    enable_inner_flames( true );

    flame_pause( FLAME_PAUSE );

    flame_radius( FLAME_ORANGE_RADIUS );
    flame_speed( FLAME_ORANGE_SPEED );
    flame_shrink( FLAME_ORANGE_SHRINK );
    flame_extinguished_shrink( FLAME_EXTINGUISHED_SHRINK );
    flame_deviation( FLAME_DEVIATION );
    flame_color( FLAME_COLOR_ORANGE );

    flame_inner_radius_ratio( FLAME_YELLOW_RADIUS_RATIO );
    flame_inner_speed_ratio( FLAME_YELLOW_SPEED_RATIO );
    flame_inner_shrink_ratio( FLAME_YELLOW_SHRINK_RATIO );
    flame_inner_outline_ratio( FLAME_YELLOW_OUTLINE_RATIO );
    flame_inner_color( FLAME_COLOR_YELLOW );

    flame_base_radius_ratio( FLAME_BASE_RADIUS_RATIO );
    flame_base_max_offset_ratio( FLAME_BASE_OFFSET_MAX_RATIO );

    flame_radius_min( FLAME_RADIUS_MIN );

    flame_alpha_shrink( FLAME_ALPHA_SHRINK );

    wind_resistance_ratio( WIND_RESISTANCE_RATIO );

    light_distance( LIGHT_DISTANCE );

    tint_distance( TINT_DISTANCE );
    tint_flicker( TINT_FLICKER );
    tint_intensity( TINT_INTENSITY );
}
