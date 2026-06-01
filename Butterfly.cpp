#include "Butterfly.hpp"

using mtmercy::Butterfly;

namespace
{

cPlanc WING_RADIUS = 5.0;
cdec UNDER_WING_RATIO = 0.9;
cdec UNDER_WING_OFFSET = 0.75;
cColor WING_COLOR = RED;

} // namespace

Butterfly::Butterfly( Room * room, Coordinate cref _position ) : Mob( room, _position )
{
    needs_render_always( true );

    // space( Polygon::square( SHELL_RADIUS * 2 ) );
}

void Butterfly::render( )
{
    Mob::render( );

    Drawing wings;

    Path path1 = Arc::semi_ccw( WING_RADIUS ).path( );
    Path path2 = Arc::semi_ccw( WING_RADIUS * UNDER_WING_RATIO ).path( );

    Polygon wing1 = Polygon( path1.points( ) );
    wing1.stretch( Vector( 0.0, 2.0 ) );

    Polygon wing2 = Polygon( path2.points( ) );
    wing2.stretch( Vector( 0.0, 2.0 ) );
    wing2.move( VectorX( WING_RADIUS * ( UNDER_WING_RATIO + 1.0 ) * UNDER_WING_OFFSET ) );

    wings.draw( WING_COLOR, wing1 );
    wings.draw( WING_COLOR, wing2 );

    wings.stretch( VectorY( 0.5 ) );
    wings.rotate( RIGHT );

    draw( wings );
}
