#include "CampFire.hpp"
#include "World.hpp"

using mtmercy::CampFire;

namespace
{

const Span<uint> FLAME_PAUSE = { 8 };

cPlanc FLAME_ORANGE_RADIUS = 30.0;
cPlanc FLAME_ORANGE_SPEED = 1.0;
cPlanc FLAME_ORANGE_SHRINK = 0.375;

cdec FLAME_YELLOW_RADIUS_RATIO = 0.625;
cdec FLAME_YELLOW_SPEED_RATIO = 1.25;
cdec FLAME_YELLOW_SHRINK_RATIO = 1.0;
cdec FLAME_YELLOW_OUTLINE_RATIO = 1.5;

const Span<dec> FLAME_BASE_RADIUS_RATIO = { 1.0, 1.1 };
const dec FLAME_BASE_OFFSET_MAX_RATIO = 0.01;

cPlanc FLAME_RADIUS_MIN = 0.1;

cAngle FLAME_DEVIATION = RIGHT / 2.0;

cdec WIND_RESISTANCE_RATIO = 0.5;

cPlanc LIGHT_DISTANCE = FLAME_ORANGE_RADIUS * 2.0;
cPlanc TINT_DISTANCE = LIGHT_DISTANCE * 0.8;
cdec TINT_FLICKER = 2.0;
cdec TINT_INTENSITY = 0.125;

cdec FLAME_ALPHA_SHRINK = 0.01;

cColor FLAME_COLOR_ORANGE = Color::rgb( 0xFF8000 );
cColor FLAME_COLOR_YELLOW = Color::rgb( 0xFFC000 );

} // namespace

CampFire::CampFire( Room * room, Coordinate cref _position ) : Fire( room, _position )
{
    enable_base_flame( true );
    enable_inner_flames( true );
    
    flame_pause( FLAME_PAUSE );
    
    flame_radius( FLAME_ORANGE_RADIUS );
    flame_speed( FLAME_ORANGE_SPEED );
    flame_shrink( FLAME_ORANGE_SHRINK );
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
