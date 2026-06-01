#include "mt/mobs/Bird.hpp"

#include "axn/world/Player.hpp"

var_const( uint BIRD_HEALTH = ( 1 ) );

var_const( Planc FEATHER_LENGTH = ( 15.0 ) );
var_const( Planc FEATHER_BASE_LENGTH = ( 5.0 ) );
var_const( Planc FEATHER_STEM_LENGTH = ( 3.0 ) );
var_const( Planc FEATHER_STEM_THICKNESS = ( 2.0 ) );

var_const( Planc SPEED = ( 1.5 ) );
var_const( Planc ROTATIONAL_SPEED = ( 1.0 ) );
var_const( Planc ROTATIONAL_RADIUS = ( 160.0 ) );
var_const( Planc LIFT_SPEED = ( 0.5 ) );
var_const( Planc LIFT_DISTANCE = ( 16.0 ) );

var_const( Color COLOR = ( RED ) );

Feather::Feather( ptr<Room> in_room, Point cref in_position ) : Object( in_room, in_position )
{
    m_color = Random::rColor( );
    gravity_scale( 0.025 );
}

void Feather::render( )
{
    Object::render( );

    // Point center = position( );
    // Polygon feather = Polygon( { center + VectorX( FEATHER_LENGTH ), center + VectorY( -FEATHER_BASE_LENGTH ),
    //                          center + VectorX( -FEATHER_BASE_LENGTH ),
    //                          center + VectorY( FEATHER_BASE_LENGTH ) } );
    // Line stem = Line( position( ), position( ) - Vector( FEATHER_BASE_LENGTH + FEATHER_STEM_LENGTH, 0.0 ) );
    // draw( m_color, feather );
    // draw( m_color, stem, FEATHER_STEM_THICKNESS );
}

Bird::Bird( ptr<Room> in_room, Point cref in_position ) : Mob( in_room, in_position, BIRD_HEALTH )
{
    no_gravity( );

    space( Polygon::rectangle( 32.0, 32.0 ) );
}

void Bird::render( )
{
    Mob::render( );

    // todo make cycle
    Polygon wing = Polygon( { Point( 0.0, 0.0 ), Point( -24.0, sin( age( ) / 16.0 ) * 16.0 ), Point( 16.0, 0.0 ) } );
    if( velocity( ).dx( ) < 0.0 )
        wing.mirror_y( );

    draw( COLOR, Line( Point( -16.0, 0.0 ), Point( 16.0, 0.0 ) ), 4.0 );
    draw( COLOR, wing );
}

void Bird::update( )
{
    if( alive( ) && room( )->player( ) )
    {
        Point target = room( )->player( )->position( );
        target += VectorY( 270.0 );
        target += Vector( sin( dec( ( Planc( room( )->age( ) ) * ROTATIONAL_SPEED / ROTATIONAL_RADIUS ) * ROTATIONAL_RADIUS ) ), 0.0 );
        target += VectorY( -sin( dec( ( ( Planc( room( )->age( ) ) - ( LIFT_DISTANCE / 2.0 ) ) * LIFT_SPEED / LIFT_DISTANCE ) * LIFT_DISTANCE ) ) );
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
