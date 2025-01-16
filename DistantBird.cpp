#include "DistantBird.hpp"
#include "World.hpp"

using mtmercy::DistantBird;

namespace
{
cPlanc SCALE = 3.0;

cPlanc X_SPEED = 2.0;

cPlanc WING_HEIGHT_SPAN = 6.0;
cuint WING_CYCLE = 4;

cPlanc FLIGHT_HEIGHT_SPAN = 16.0;
cuint FLIGHT_HEIGHT_CYCLE = 17;

const Color BIRD_COLOR = BLACK;
} // namespace

DistantBird::DistantBird( Room * room, Coordinate cref _position ) : Object( room, _position )
{
    background( true );

    needs_render_always( true );

    z( 0.5 );
    no_gravity( );
    air_resistance_ratio( 0.0 );
    terrain_boundaries( false );

    velocity( Vector::X( X_SPEED ) );

    m_wing_cycle_offset = Random::rdec( WING_CYCLE ) * TAU;
    m_flight_cycle_offset = Random::rdec( FLIGHT_HEIGHT_CYCLE ) * half( PI );
}

void DistantBird::render( )
{
    Object::render( );

    Planc wing_y = WING_HEIGHT_SPAN * sin( (dec)( age( ) + m_wing_cycle_offset ) / (dec)WING_CYCLE );
    Planc flight_y = FLIGHT_HEIGHT_SPAN * sin( (dec)( age( ) + m_flight_cycle_offset ) / (dec)FLIGHT_HEIGHT_CYCLE );

    Polygon bird_polygon = Polygon( { Coordinate( 0.0, 0.0 ), Coordinate( -1.0, 1.0 ), Coordinate( -4.0, 1.0 ), Coordinate( -12.0, -1.0 ), Coordinate( -4.0, -1.0 ), Coordinate( -2.0, 0.0 ) } );
    Polygon bird_wing = Polygon( { Coordinate( -2.0, 0.0 ), Coordinate( -9.0, -1.0 ), Coordinate( -5.0, wing_y ) } );

    bird_polygon.scale( SCALE );
    bird_wing.scale( SCALE );

    bird_polygon.move( Vector::Y( flight_y ) );
    bird_wing.move( Vector::Y( flight_y ) );

    if( velocity( ).dx( ) < 0.0 )
    {
        bird_polygon.mirror_y( );
        bird_wing.mirror_y( );
    }

    draw( BIRD_COLOR, bird_polygon );
    draw( BIRD_COLOR, bird_wing );
}
