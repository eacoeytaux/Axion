#include "Snail.hpp"
#include "Terrain.hpp"

using mtmercy::Snail;

namespace
{

cPlanc SPEED = 0.025;
cPlanc SHELL_RADIUS = 10.0;
cPlanc SLUG_THICKNESS = 5.0;
cuint STRETCH_PERIOD = 10;

cColor SLUG_COLOR = BLACK;
cColor SHELL_COLOR = RED;

} // namespace

Snail::Snail( Room * room, Coordinate cref _position ) : Mob( room, _position )
{
    needs_render_always( true );

    space( Polygon::square( SHELL_RADIUS * 2 ) );
}

void Snail::render( )
{
    Mob::render( );

    Angle angle;

    if( ground( ) )
    {
        angle = ground( )->normal( ) + RIGHT;
    }

    Coordinate tail = VectorA( angle, -SHELL_RADIUS );
    Coordinate head = VectorA( angle, SHELL_RADIUS + ( half( SLUG_THICKNESS ) * sin( (dec)age( ) / STRETCH_PERIOD ) ) );
    Coordinate head_top = head + VectorA( angle, SLUG_THICKNESS ) + VectorA( angle + RIGHT, SLUG_THICKNESS );

    draw( SHELL_COLOR, Polygon::circle( SHELL_RADIUS, VectorA( angle + RIGHT, SHELL_RADIUS ) ) );
    draw( SLUG_COLOR, Polygon( { head, head_top, tail } ) );
}

void Snail::update( )
{
    if( ground( ) )
    {
        add_velocity( VectorA( ground( )->normal( ) + RIGHT, SPEED ) );
    }

    Object::update( );
}
