#include "mt/environment/critters/Butterfly.hpp"

var_const( Planc BODY_LENGTH = ( 7.0 ) );
var_const( Vector WING_OFFSET_HI = ( Vector( 0.275, 0.77 ) * BODY_LENGTH ) );
var_const( Vector WING_OFFSET_LO = ( Vector( -0.2, 0.5 ) * BODY_LENGTH ) - VectorX( BODY_LENGTH ) );
var_const( dec WING_CLOSE_MIN = ( 0.1 ) );

var_const( Planc FORWARD_SPEED = ( 0.08 ) );
var_const( Angle ROTATE_MAX = ( Angle::fraction( 5.0 ) ) );
var_const( dec ROTATE_DAMPER = ( 0.25 ) );

var_const( Span<Planc> ANCHOR_DISTANCE = { 25.0, 50.0 } );
var_const( Angle TARGET_ANGLE_EPSILON = ( Angle::fraction( 64.0 ) ) );
var_const( Planc TARGET_XY_EPSILON = ( BODY_LENGTH ) );
var_const( Span<ticks> TARGET_RESET_DURATION = { 15.0, 45.0 } );

var_const( Span<Planc> FLAP_RATE = { 8.0, 12.0 } );

Butterfly::Butterfly( ptr<Room> in_room, Point cref in_position ) : Object( in_room, in_position )
{
    background( true );

    needs_render_always( true );

    terrain_bound( false );
    no_gravity( );

    m_anchor = in_position;
    new_target( );

    m_wing_cycle = Cycle( Random::rPlanc( FLAP_RATE ), Random::rAngle( ) );

    m_color = Color::rgb( 0xFFB4DC ); // Color::hsv( Random::rdec( ), 1.0, 1.0 );
    m_color = Color::hsv( Random::rdec( 0.72, 0.88 ), Random::rdec( 0.3, 0.7 ), 1.0 );
}

void Butterfly::render( )
{
    Object::render( );

    Drawing wings;

    Planc p = m_wing_cycle.at( age( ) );

    if( m_wing_cycle.amplitude( ) )
    {
        p = ( ( ( abs( p / m_wing_cycle.amplitude( ) ) * one_minus( WING_CLOSE_MIN ) ) + WING_CLOSE_MIN ) * m_wing_cycle.amplitude( ) );
    }
    else
    {
        p = WING_CLOSE_MIN;
    }

    Polygon wing11 = Polygon::triangle( ORIGIN, PointX( -BODY_LENGTH ), WING_OFFSET_HI ).stretch( VectorY( p ) );
    Polygon wing12 = Polygon::triangle( ORIGIN, PointX( -BODY_LENGTH ), WING_OFFSET_LO ).stretch( VectorY( p ) );

    Polygon wing21 = Polygon( wing11 ).mirror_x( );
    Polygon wing22 = Polygon( wing12 ).mirror_x( );

    wings.draw( m_color, wing22 );
    wings.draw( m_color, wing21 );
    wings.draw( m_color, wing12 );
    wings.draw( m_color, wing11 );

    wings.rotate( velocity( ).angle( ) );

    draw( wings );
}

void Butterfly::update( )
{
    Angle angle = velocity( ).angle( );
    Angle target_angle = Vector( position( ), target( ) ).angle( );
    Angle d_angle = angle.min_distance( target_angle );

    d_angle *= one_minus( ROTATE_DAMPER );
    d_angle = min( max( d_angle, -ROTATE_MAX ), ROTATE_MAX );

    if( ( m_target_reset.tick( ) ) ||
        ( abs( d_angle ) <= TARGET_ANGLE_EPSILON ) ||
        ( position( ).distance_xy_to( target( ) ) <= TARGET_XY_EPSILON ) )
    {
        new_target( );
    }

    add_velocity( VectorA( angle + d_angle, FORWARD_SPEED ) );

    Object::update( );
}

void Butterfly::new_target( )
{
    m_target = m_anchor + VectorA( Random::rAngle( ), Random::rPlanc( ANCHOR_DISTANCE ) );
    m_target_reset.reset( Random::rdec( TARGET_RESET_DURATION ) );
}
