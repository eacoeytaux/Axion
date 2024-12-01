#include "Fox.hpp"

using mtmercy::Fox;

namespace
{
const Planc EYE_RADIUS = 1.0;
const uint BLINK_DURATION = 2;
const Span<uint> BLINK_WAIT = { 180, 240 };

const Color EYE_COLOR = BLACK;
const Color NOSE_COLOR = BLACK;
const Color FUR_COLOR1 = Color::rgb( 0xFFA500 );
const Color FUR_COLOR2 = WHITE;
}

Fox::Fox( World * world, const Coordinate & _position ) : Mob( world, _position )
{
    space( Rectangle( 32.0, 32.0 ) );

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

    // todo combine head1 and head2 and one convex polygon

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

    //draw_eyes( Coordinate( -7.0, 3.0 ).rotate( m_head_angle ) + neck );

    Polygon ear = Polygon( { Coordinate( -3.0, 5.0 ),
                             Coordinate( 0.0, 15.0 ),
                             Coordinate( 3.0, 10.0 ) } ) + neck;

    head_drawing.draw( RED, ear );

    //head_drawing.rotate( m_head_angle, neck );

    draw( head_drawing );
}
