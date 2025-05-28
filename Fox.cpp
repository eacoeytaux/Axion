#include "Fox.hpp"

using mtmercy::Fox;

namespace
{

cPlanc EYE_RADIUS = 1.0;
cuint BLINK_DURATION = 2;
const Span<uint> BLINK_WAIT = { 180, 240 };

cColor EYE_COLOR = BLACK;
cColor NOSE_COLOR = BLACK;
cColor FUR_COLOR1 = Color::rgb( 0xFFA500 );
cColor FUR_COLOR2 = WHITE;
}

Fox::Fox( Room * room, Coordinate cref _position ) : Mob( room, _position )
{
    space( Polygon::rectangle( 32.0, 32.0 ) );

    eye_info( EYE_RADIUS, BLINK_DURATION, BLINK_WAIT, EYE_COLOR );
}

void Fox::render( )
{
    Mob::render( );

    Coordinate neck( 0.0, 4.0 * sin( (dec)age( ) / 10.0 ) );

    Polygon body( { Coordinate( -5.0, 5.0 ) + neck,
                    Coordinate( 3.0, -16.0 ),
                    Coordinate( 35.0, -20.0 ),
                    Coordinate( 30.0, -10.0 ) } );

    draw( FUR_COLOR1, body );

    Drawing head_drawing;

    Polygon head1 = Polygon( { Coordinate( -12.0, 3.0 ),
                               Coordinate( -20.0, 0.0 ),
                               Coordinate( -16.0, -3.0 ),
                               Coordinate( 0.0, -5.0 ),
                               Coordinate( 5.0, 0.0 ) } ) + neck;

    Polygon head2 = Polygon( { Coordinate( -12.0, 3.0 ),
                               Coordinate( -20.0, 0.0 ),
                               Coordinate( 5.0, 0.0 ) } ) + neck;

    Polygon head3 = Polygon( { Coordinate( 0.0, 10.0 ),
                               Coordinate( -8.0, 8.0 ),
                               Coordinate( -12.0, 3.0 ),
                               Coordinate( 5.0, 0.0 ) } ) + neck;

    Polygon nose = Polygon( { Coordinate( -17.0, 1.25 ),
                              Coordinate( -20.0, 0.0 ),
                              Coordinate( -18.0, -1.5 ) } ) + neck;

    head_drawing.draw( FUR_COLOR2, head1 );
    head_drawing.draw( FUR_COLOR1, head2 );
    head_drawing.draw( FUR_COLOR1, head3 );
    head_drawing.draw( NOSE_COLOR, nose );

    Polygon ear = Polygon( { Coordinate( -3.0, 5.0 ),
                             Coordinate( 0.0, 15.0 ),
                             Coordinate( 3.0, 10.0 ) } ) + neck;

    head_drawing.draw( FUR_COLOR1, ear );

    draw( head_drawing );
}
