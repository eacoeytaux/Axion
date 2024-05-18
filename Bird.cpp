#include "Bird.hpp"

#include "World.hpp"
#include "Player.hpp"

using mtmercy::Bird;
using mtmercy::Feather;

namespace
{
const uint BIRD_HEALTH = 1;

const Planc FEATHER_LENGTH = 15.0;
const Planc FEATHER_BASE_LENGTH = 5.0;
const Planc FEATHER_STEM_LENGTH = 3.0;
const Planc FEATHER_STEM_THICKNESS = 2.0;

const Planc SPEED = 3.0;
const Planc ROTATIONAL_SPEED = 2.0;
const Planc ROTATIONAL_RADIUS = 160.0;
const Planc LIFT_SPEED = 1.0;
const Planc LIFT_DISTANCE = 16.0;
} // namespace

Feather::Feather( World * world, const Coordinate & _position ) : Object( world, _position )
{
    m_color = Random::rColor( );
    gravity_ratio( 0.05 );
}

const Feather & Feather::render( ) const
{
    Object::render( );

    // Coordinate center = position( );
    // Polygon feather = Polygon( { center + VectorX( FEATHER_LENGTH ), center + VectorY( -FEATHER_BASE_LENGTH ),
    //                          center + VectorX( -FEATHER_BASE_LENGTH ),
    //                          center + VectorY( FEATHER_BASE_LENGTH ) } );
    // Line stem = Line( position( ), position( ) - Vector( FEATHER_BASE_LENGTH + FEATHER_STEM_LENGTH, 0.0 ) );
    // draw( m_color, feather );
    // draw( m_color, stem, FEATHER_STEM_THICKNESS );

    return *this;
}

Bird::Bird( World * world, const Coordinate & _position ) : Mob( world, _position )
{
    gravity_ratio( 0.0 );
    space( Rectangle( 32.0, 32.0 ) );
    health( BIRD_HEALTH );
}

const Bird & Bird::render( ) const
{
    Mob::render( );

    const Color color = RED; // todo

    Polygon wing = Polygon( { Coordinate( 0.0, 0.0 ), Coordinate( -24.0, sin( world( )->age( ) / 16.0 ) * 16.0 ), Coordinate( 16.0, 0.0 ) } );
    if( velocity( ).dx( ) < 0.0 )
        wing.mirror_y( );

    draw( color, Line( Coordinate( -16.0, 0.0 ), Coordinate( 16.0, 0.0 ) ), 4.0 );
    draw( color, wing );

    return *this;
}

Bird & Bird::update( )
{
    if( alive( ) && world( )->player( ) )
    {
        Coordinate target = world( )->player( )->position( );
        target += VectorY( 270.0 );
        target += Vector( sin( (dec)( ( (Planc)( world( )->age( ) ) * ROTATIONAL_SPEED / ROTATIONAL_RADIUS ) * ROTATIONAL_RADIUS ) ), 0.0 );
        target += VectorY( -sin( (dec)( ( ( (Planc)( world( )->age( ) ) - ( LIFT_DISTANCE / 2.0 ) ) * LIFT_SPEED / LIFT_DISTANCE ) * LIFT_DISTANCE ) ) );
        Vector movement = Vector( position( ), target );
        if( movement.dx( ) > 0.0 )
            movement.dx( min( SPEED, movement.dx( ) ) );
        else
            movement.dx( max( -SPEED, movement.dx( ) ) );
        velocity( movement );
    }

    drawing_dirty( true );

    Mob::update( );
    return *this;
}

Bird & Bird::die( )
{
    Mob::die( );
    gravity_ratio( 1.0 );
    return *this;
}
