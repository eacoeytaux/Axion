#include "mt/background/DistantBird.hpp"

var_const( dec GRAIVTY_RATIO = ( 0.5 ) );
var_const( Planc X_SPEED = ( 2.0 ) );

var_const( Planc WING_OFFSET_CYCLE = ( 4.0 ) );
var_const( Planc WING_OFFSET_HEIGHT = ( 18.0 ) );

var_const( Planc FLIGHT_OFFSET_CYCLE = ( 17.0 ) );
var_const( Planc FLIGHT_OFFSET_SPAN = ( 16.0 ) );

var_const( Polygon BIRD_BODY = ( Polygon( {
    Point( 0.0, 0.0 ),
    Point( -3.0, 3.0 ),
    Point( -12.0, 3.0 ),
    Point( -36.0, -3.0 ),
    Point( -12.0, -3.0 ),
    Point( -6.0, 0.0 ) } ) ) );

var_const( Point WING_COORDINATE_BASE_1 = ( Point( -6.0, 0.0 ) ) );
var_const( Point WING_COORDINATE_BASE_2 = ( Point( -27.0, -3.0 ) ) );
var_const( Planc WING_TIP_X = ( -15.0 ) );

var_const( Color BIRD_COLOR = ( BLACK ) );

DistantBird::DistantBird( ptr<Room> in_room, Point cref in_position ) : Object( in_room, in_position )
{
    background( true );

    needs_render_always( true );

    z( GRAIVTY_RATIO );

    no_gravity( );
    no_air_resistance( );

    terrain_bound( false );

    velocity( VectorX( X_SPEED ) );

    m_wing_offset = Cycle( WING_OFFSET_CYCLE, WING_OFFSET_HEIGHT, Random::rAngle( ) );
    m_flight_offset = Cycle( FLIGHT_OFFSET_CYCLE, FLIGHT_OFFSET_SPAN, Random::rAngle( RIGHT ) );
}

void DistantBird::render( )
{
    Object::render( );

    Planc wing_y = m_wing_offset.at( age( ) );
    Planc flight_y = m_flight_offset.at( age( ) );

    Polygon bird_body = BIRD_BODY;
    Polygon bird_wing = Polygon( { WING_COORDINATE_BASE_1, WING_COORDINATE_BASE_2, Point( WING_TIP_X, wing_y ) } );

    bird_body.move( VectorY( flight_y ) );
    bird_wing.move( VectorY( flight_y ) );

    if( is_neg( velocity( ).dx( ) ) )
    {
        bird_body.mirror_y( );
        bird_wing.mirror_y( );
    }

    draw( BIRD_COLOR, bird_body );
    draw( BIRD_COLOR, bird_wing );
}
