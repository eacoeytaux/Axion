#include "Snail.hpp"
#include "Terrain.hpp"

using mtmercy::Snail;

namespace
{
const Planc SPEED = 0.025;
const Planc SHELL_RADIUS = 10.0;
const Planc SLUG_THICKNESS = 5.0;
const uint STRETCH_PERIOD = 10;

const Color SLUG_COLOR = BLACK;
const Color SHELL_COLOR = RED;
} // namespace

Snail::Snail( World * world, const Coordinate & _position ) : Mob( world, _position )
{
    needs_render_always( true );

    // space( Square( SHELL_RADIUS * 2 ) );
}

void Snail::render( )
{
    Mob::render( );

    Angle angle;

    if( ground( ) )
    {
        angle = ground( )->vector( ).angle( );
    }

    Coordinate tail = VectorA( angle, -SHELL_RADIUS );
    Coordinate head = VectorA( angle, SHELL_RADIUS + ( half( SLUG_THICKNESS ) * sin( (dec)age( ) / STRETCH_PERIOD ) ) );
    Coordinate head_top = head + VectorA( angle, SLUG_THICKNESS ) + VectorA( angle + RIGHT_ANGLE, SLUG_THICKNESS );

    draw( SHELL_COLOR, Circle( SHELL_RADIUS, VectorA( angle + RIGHT_ANGLE, SHELL_RADIUS ) ) );
    draw( SLUG_COLOR, Polygon( { head, head_top, tail } ) );
}

void Snail::update( )
{
    if( ground( ) )
    {
        add_velocity( ground( )->vector( ).magnitude( SPEED ) );
    }

    Object::update( );
}
