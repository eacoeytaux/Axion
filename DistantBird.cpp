#include "DistantBird.hpp"
#include "World.hpp"

using mtmercy::DistantBird;

namespace
{

cdec GRAIVTY_RATIO = 0.5;
cPlanc X_SPEED = 2.0;

cuint WING_OFFSET_CYCLE = 4;
cPlanc WING_OFFSET_HEIGHT = 18.0;

cuint FLIGHT_OFFSET_CYCLE = 17;
cPlanc FLIGHT_OFFSET_SPAN = 16.0;

const Polygon BIRD_BODY = Polygon( { Coordinate( 0.0, 0.0 ),
                                     Coordinate( -3.0, 3.0 ),
                                     Coordinate( -12.0, 3.0 ),
                                     Coordinate( -36.0, -3.0 ),
                                     Coordinate( -12.0, -3.0 ),
                                     Coordinate( -6.0, 0.0 ) } );

const Coordinate WING_COORDINATE_BASE_1 = Coordinate( -6.0, 0.0 );
const Coordinate WING_COORDINATE_BASE_2 = Coordinate( -27.0, -3.0 );
cPlanc WING_TIP_X = -15.0;

cColor BIRD_COLOR = BLACK;

} // namespace

DistantBird::DistantBird( Room * room, Coordinate cref _position ) : Object( room, _position )
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
    Polygon bird_wing = Polygon( { WING_COORDINATE_BASE_1, WING_COORDINATE_BASE_2, Coordinate( WING_TIP_X, wing_y ) } );

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
