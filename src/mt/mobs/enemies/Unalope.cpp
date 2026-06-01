#include "mt/mobs/enemies/Unalope.hpp"

#include "axn/world/Player.hpp"

var_const( dec TEMP_SCALE_TODO = ( 1.0 ) );

var_const( Planc SIGHT_RANGE = ( METER * 10.0 ) );
var_const( Planc ALERT_RANGE = ( SIGHT_RANGE * 0.75 ) );

var_const( Polygon HEAD = ( Polygon( {
    Point( -3.0, -8.0 ),
    Point( -12.0, 0.0 ),
    Point( -9.0, 8.0 ),
    Point( -3.0, 12.0 ),
    Point( 3.0, 12.0 ),
    Point( 9.0, 8.0 ),
    Point( 12.0, 0.0 ),
    Point( 3.0, -8.0 ) } ).scale( TEMP_SCALE_TODO ) ) );

var_const( Polygon NOSE = ( Polygon( {
    Point( -3.0, -4.0 ),
    Point( 0.0, -6.4 ),
    Point( 3.0, -4.0 ) } ).scale( TEMP_SCALE_TODO ) ) );

var_const( Polygon EAR = ( Polygon( {
    Point( -6.0, 6.0 ),
    Point( -16.0, 24.0 ),
    Point( -9.0, 48.0 ),
    Point( -3.0, 12.0 ) } ).scale( TEMP_SCALE_TODO ) ) );

var_const( Polygon INNER_EAR = ( Polygon( {
    Point( -8.0, 14.0 ),
    Point( -13.0, 24.0 ),
    Point( -10.5, 32.0 ) } ).scale( TEMP_SCALE_TODO ) ) );

var_const( Polygon EAR_MIRRORED = ( Polygon( EAR ).mirror_y( ) ) );
var_const( Polygon INNER_EAR_MIRRORED = ( Polygon( INNER_EAR ).mirror_y( ) ) );

var_const( Polygon BODY = ( Polygon( {
    Point( 0.0, 0.0 ),
    Point( -9.0, -6.0 ),
    Point( -10.0, -9.0 ),
    Point( -5.0, -9.0 ) } ).scale( TEMP_SCALE_TODO ) ) );

var_const( Color FUR_COLOR = ( WHITE ) );
var_const( Color EAR_COLOR = ( FUR_COLOR ) );
var_const( Color INNER_EAR_COLOR = ( RED ) );
var_const( Color NOSE_COLOR = ( RED ) );

Unalope::Unalope( ptr<Room> in_room, Point cref in_position ) : Enemy( in_room, in_position, 10000.0 )
{
    space( Polygon::rectangle( 16.0, 16.0 ) );

    eye_info( 4, 1, 240 );

    sight_range( SIGHT_RANGE );
    alert_range( ALERT_RANGE );
}

void Unalope::update( )
{
    Enemy::update( );
}

void Unalope::render( )
{
    Enemy::render( );

    // draw( FUR_COLOR, BODY );

    draw( EAR_COLOR, EAR );
    draw( EAR_COLOR, EAR_MIRRORED );

    draw( INNER_EAR_COLOR, INNER_EAR );
    draw( INNER_EAR_COLOR, INNER_EAR_MIRRORED );

    draw( FUR_COLOR, HEAD );

    draw( NOSE_COLOR, NOSE );

    // draw_eyes( Point( -24.0, 0.0 ), Point( 24.0, 0.0 ) );
}
