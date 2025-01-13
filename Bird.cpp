#include "Bird.hpp"
#include "World.hpp"
#include "Player.hpp"

using mtmercy::Bird;
using mtmercy::Feather;

namespace
{
cuint BIRD_HEALTH = 1;

cPlanc FEATHER_LENGTH = 15.0;
cPlanc FEATHER_BASE_LENGTH = 5.0;
cPlanc FEATHER_STEM_LENGTH = 3.0;
cPlanc FEATHER_STEM_THICKNESS = 2.0;

cPlanc SPEED = 1.5;
cPlanc ROTATIONAL_SPEED = 1.0;
cPlanc ROTATIONAL_RADIUS = 160.0;
cPlanc LIFT_SPEED = 0.5;
cPlanc LIFT_DISTANCE = 16.0;
} // namespace

Feather::Feather( World * world, Coordinate cref _position ) : Object( world, _position )
{
    m_color = Random::rColor( );
    gravity_ratio( 0.025 );
}

void Feather::render( )
{
    Object::render( );

    // Coordinate center = position( );
    // Polygon feather = Polygon( { center + Vector::X( FEATHER_LENGTH ), center + Vector::Y( -FEATHER_BASE_LENGTH ),
    //                          center + Vector::X( -FEATHER_BASE_LENGTH ),
    //                          center + Vector::Y( FEATHER_BASE_LENGTH ) } );
    // Line stem = Line( position( ), position( ) - Vector( FEATHER_BASE_LENGTH + FEATHER_STEM_LENGTH, 0.0 ) );
    // draw( m_color, feather );
    // draw( m_color, stem, FEATHER_STEM_THICKNESS );
}

Bird::Bird( World * world, Coordinate cref _position ) : Mob( world, _position )
{
    no_gravity( );
    space( Polygon::rectangle( 32.0, 32.0 ) );
    health( BIRD_HEALTH );
}

void Bird::render( )
{
    Mob::render( );

    const Color COLOR = RED;

    Polygon wing = Polygon( { Coordinate( 0.0, 0.0 ), Coordinate( -24.0, sin( world( )->age( ) / 16.0 ) * 16.0 ), Coordinate( 16.0, 0.0 ) } );
    if( velocity( ).dx( ) < 0.0 )
        wing.mirror_y( );

    draw( COLOR, Line( Coordinate( -16.0, 0.0 ), Coordinate( 16.0, 0.0 ) ), 4.0 );
    draw( COLOR, wing );
}

void Bird::update( )
{
    if( alive( ) && world( )->player( ) )
    {
        Coordinate target = world( )->player( )->position( );
        target += Vector::Y( 270.0 );
        target += Vector( sin( (dec)( ( (Planc)( world( )->age( ) ) * ROTATIONAL_SPEED / ROTATIONAL_RADIUS ) * ROTATIONAL_RADIUS ) ), 0.0 );
        target += Vector::Y( -sin( (dec)( ( ( (Planc)( world( )->age( ) ) - ( LIFT_DISTANCE / 2.0 ) ) * LIFT_SPEED / LIFT_DISTANCE ) * LIFT_DISTANCE ) ) );
        Vector movement = Vector( position( ), target );
        if( movement.dx( ) > 0.0 )
            movement.dx( min( SPEED, movement.dx( ) ) );
        else
            movement.dx( max( -SPEED, movement.dx( ) ) );
        velocity( movement );
    }

    needs_render( true );

    Mob::update( );
}

void Bird::die( )
{
    normal_gravity( );
    Mob::die( );
}
