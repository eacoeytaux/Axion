#include "Butterfly.hpp"

using mtmercy::Butterfly;

namespace
{
const Planc WING_RADIUS = 5.0;
const dec UNDER_WING_RATIO = 0.9;
const dec UNDER_WING_OFFSET = 0.75;
const Color WING_COLOR = RED;
} // namespace

Butterfly::Butterfly( World * world, const Coordinate & _position ) : Mob( world, _position )
{
    needs_render_always( true );

    // space( Square( SHELL_RADIUS * 2 ) );
}

void Butterfly::render( )
{
    Mob::render( );
    
    Drawing wings;

    Path path1 = Arc::ccw( WING_RADIUS, ZERO, PI );
    Path path2 = Arc::ccw( WING_RADIUS * UNDER_WING_RATIO, ZERO, PI );
    
    Polygon wing1 = Polygon( path1.points( ) );
    //wing1.stretch( Vector( ZERO, 2.0 ) );
    
    Polygon wing2 = Polygon( path2.points( ) );
    //wing2.stretch( Vector( ZERO, 2.0 ) );
    wing2.move( VectorX( WING_RADIUS * ( UNDER_WING_RATIO + ONE ) * UNDER_WING_OFFSET ) );

    wings.draw( WING_COLOR, wing1 );
    wings.draw( WING_COLOR, wing2 );
    
    //wings.stretch( VectorY( 0.5 ) );
    wings.rotate( RIGHT_ANGLE );
    
    draw( wings );
}
