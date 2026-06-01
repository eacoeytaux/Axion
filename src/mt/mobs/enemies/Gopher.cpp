#include "mt/mobs/enemies/Gopher.hpp"

var_const( dec BALL_GRAVITY = ( 0.125 ) );
var_const( Planc BALL_WIDTH = ( 5.0 ) );

var_const( Planc SIGHT_RANGE = ( METER * 10.0 ) );
var_const( Planc ALERT_RANGE = ( SIGHT_RANGE * 0.75 ) );

var_const( ticks RELOAD_TIME = ( 100.0 ) );

var_const( Planc HEAD_RADIUS = ( 12.0 ) );
var_const( Planc HEAD_X_STRETCH = ( 1.1 ) );

var_const( Planc BODY_WIDTH = ( HEAD_RADIUS * 2.0 * 0.8 ) );
var_const( Planc BODY_HEIGHT = ( HEAD_RADIUS * 2.0 ) );

var_const( Planc INNER_FUR_WIDTH = ( BODY_WIDTH * 0.8 ) );
var_const( Planc INNER_FUR_HEIGHT = ( BODY_HEIGHT * 0.8 ) );

var_const( Planc CHEEK_RADIUS = ( HEAD_RADIUS * 0.3 ) );

var_const( Planc NOSE_RADIUS = ( HEAD_RADIUS * 0.25 ) );
var_const( Vector NOSE_OFFSET = ( VectorX( CHEEK_RADIUS ) ) );

var_const( Planc TEETH_WIDTH = ( NOSE_RADIUS * 0.9 ) );
var_const( Planc TEETH_LENGTH = ( HEAD_RADIUS * 0.64 ) );

var_const( Planc EAR_RADIUS = ( HEAD_RADIUS * 0.3 ) );
var_const( Planc EAR_INNER_RADIUS = ( EAR_RADIUS * 0.5 ) );
var_const( Planc EAR_PLACEMENT_DISTANCE = ( HEAD_RADIUS ) );
var_const( Angle EAR_ANGLE_FRONT = ( RIGHT * 1.5 ) );
var_const( Angle EAR_ANGLE_BACK = ( RIGHT * 0.5 ) );

var_const( Planc EYE_SPACING_UP = ( NOSE_RADIUS * 1.25 ) );
var_const( Planc EYE_SPACING_SIDE = ( NOSE_RADIUS * 1.75 ) );
var_const( Planc EYE_RADIUS = ( 1.0 ) );
var_const( ticks BLINK_DURATION = ( 8.0 ) );
var_const( Span<ticks> BLINK_WAIT = { 360.0, 480.0 } );

var_const( Color EYE_COLOR = ( BLACK ) );
var_const( Color TEETH_COLOR = ( WHITE ) );
var_const( Color NOSE_COLOR = ( Color::rgb( 0x654321 ) ) );
var_const( Color FUR_COLOR_1 = ( Color::rgb( 0xA0522D ) ) );
var_const( Color FUR_COLOR_2 = ( Color::rgb( 0xF4A460 ) ) );

Gopher::Gopher( ptr<Room> in_room, Point cref in_position ) : Enemy( in_room, in_position, 10000.0 )
{
    space( Polygon::square( 16.0 ) );

    eye_info( EYE_RADIUS, BLINK_DURATION, BLINK_WAIT, EYE_COLOR );

    sight_range( SIGHT_RANGE );
    alert_range( ALERT_RANGE );

    state( EXPOSED );
}

void Gopher::update( )
{
    Enemy::update( );

    m_reload_timer.tick( );

    if( state( ) == EXPOSED )
    {
        if( has_target( ) )
        {
            facing_left( target( )->position( ).x( ) < position( ).x( ) );

            if( m_reload_timer.completed( ) )
            {
                room( )->add_object( new DirtBall( room( ), BALL_WIDTH, position( ), VectorA( Angle( position( ), target( )->position( ) ), 5.0 ) ) );
                m_reload_timer.reset( RELOAD_TIME );
            }
        }
    }
}

void Gopher::render( )
{
    Enemy::render( );

    if( state( ) == EXPOSED )
    {
        draw( FUR_COLOR_1, Polygon::rectangle( BODY_WIDTH, BODY_HEIGHT ) );

        Polygon inner_fur = Polygon::circle( half( INNER_FUR_WIDTH ) ).stretch( Vector( 0.0, INNER_FUR_HEIGHT / INNER_FUR_WIDTH ) );
        draw( FUR_COLOR_2, inner_fur );

        Vector head_offset( 0.0, half( BODY_HEIGHT ) );
        Drawing head_drawing;

        head_drawing.draw( FUR_COLOR_1, Polygon::circle( EAR_RADIUS, VectorA( EAR_ANGLE_BACK, EAR_PLACEMENT_DISTANCE ) ) );
        head_drawing.draw( FUR_COLOR_2, Polygon::circle( EAR_INNER_RADIUS, VectorA( EAR_ANGLE_BACK, EAR_PLACEMENT_DISTANCE ) ) );

        head_drawing.draw( FUR_COLOR_1, Polygon::circle( HEAD_RADIUS ).stretch( Vector( HEAD_X_STRETCH, 0.0 ) ) );

        head_drawing.draw( FUR_COLOR_1, Polygon::circle( EAR_RADIUS, VectorA( EAR_ANGLE_FRONT, EAR_PLACEMENT_DISTANCE ) ) );
        head_drawing.draw( FUR_COLOR_2, Polygon::circle( EAR_INNER_RADIUS, VectorA( EAR_ANGLE_FRONT, EAR_PLACEMENT_DISTANCE ) ) );

        head_drawing.draw( TEETH_COLOR, Polygon::rectangle( TEETH_WIDTH, TEETH_LENGTH, NOSE_OFFSET + Vector( 0.0, -half( TEETH_LENGTH ) ) ) );

        head_drawing.draw( FUR_COLOR_2, Polygon::circle( CHEEK_RADIUS, NOSE_OFFSET + VectorA( RIGHT * 2.5, CHEEK_RADIUS ) ) );
        head_drawing.draw( FUR_COLOR_2, Polygon::circle( CHEEK_RADIUS, NOSE_OFFSET + VectorA( RIGHT * 3.5, CHEEK_RADIUS ) ) );

        head_drawing.draw( NOSE_COLOR, Polygon::circle( NOSE_RADIUS, NOSE_OFFSET ) );

        draw( head_drawing.move( head_offset ) );

        draw_eyes( NOSE_OFFSET + Vector( EYE_SPACING_SIDE, EYE_SPACING_UP ) + head_offset, NOSE_OFFSET + Vector( -EYE_SPACING_SIDE, EYE_SPACING_UP ) + head_offset );

        if( facing_left( ) )
        {
            mirror_y( );
        }
    }
}

DirtBall::DirtBall( ptr<Room> room, Planc cref in_radius, Point cref in_position, Vector cref in_velocity ) : Object( room, in_position, in_velocity )
{
    #if defined( AXN_DEBUG )
    draw_debug = true;
    #endif

    space( Polygon::rectangle( in_radius, in_radius ) );
    position( in_position );

    solid( true );
    interactive( true );

    gravity_scale( BALL_GRAVITY );
    terrain_bound( false );

    velocity( in_velocity );

    persist_render( true );

    draw( WHITE, Polygon::circle( in_radius ) );
}
