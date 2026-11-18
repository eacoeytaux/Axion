#include "mt/environment/critters/Snail.hpp"

#include "axn/world/Terrain.hpp"

var_const( Planc SPEED = ( 0.025 ) );
var_const( Planc SHELL_RADIUS = ( 10.0 ) );
var_const( Planc SLUG_THICKNESS = ( 5.0 ) );
var_const( dec STRETCH_PERIOD = ( 10.0 ) );

var_const( Color SLUG_COLOR = ( BLACK ) );
var_const( Color SHELL_COLOR = ( RED ) );

Snail::Snail( ptr<Room> in_room, Point cref _position ) : Mob( in_room, _position )
{
    needs_render_always( true );

    space( Polygon::square( SHELL_RADIUS * 2 ) );

    terrain_bound( true );
}

void Snail::render( )
{
    Mob::render( );

    Angle angle;

    if( grounded( ) )
    {
        angle = ( ground( )->normal( ) - RIGHT );
    }

    Point tail = VectorA( angle, -SHELL_RADIUS );
    Point head = VectorA( angle, SHELL_RADIUS + ( half( SLUG_THICKNESS ) * sin( dec( age( ) ) / STRETCH_PERIOD ) ) );
    Point head_top = head + VectorA( angle, SLUG_THICKNESS ) + VectorA( angle + RIGHT, SLUG_THICKNESS );

    draw( SHELL_COLOR, Polygon::circle( SHELL_RADIUS, VectorA( angle + RIGHT, SHELL_RADIUS ) ) );
    draw( SLUG_COLOR, Polygon( { head, head_top, tail } ) );
}

void Snail::update( )
{
    if( grounded( ) )
    {
        add_velocity( VectorA( ground( )->normal( ) - RIGHT, SPEED ) );
    }

    Object::update( );
}
