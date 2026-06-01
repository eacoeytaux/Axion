#include "mt/mobs/enemies/Fox.hpp"

var_const( Planc EYE_RADIUS = ( 1.0 ) );
var_const( ticks BLINK_DURATION = ( 8.0 ) );
var_const( Span<ticks> BLINK_WAIT = { 180.0, 240.0 } );

var_const( Color EYE_COLOR = ( BLACK ) );
var_const( Color NOSE_COLOR = ( BLACK ) );
var_const( Color FUR_COLOR_1 = ( Color::rgb( 0xFFA500 ) ) );
var_const( Color FUR_COLOR_2 = ( WHITE ) );

Fox::Fox( ptr<Room> in_room, Point cref in_position ) : Enemy( in_room, in_position )
{
    space( Polygon::rectangle( 32.0, 32.0 ) );

    eye_info( EYE_RADIUS, BLINK_DURATION, BLINK_WAIT, EYE_COLOR );
}

void Fox::render( )
{
    Enemy::render( );

    Point neck( 0.0, 4.0 * sin( dec( age( ) ) / 10.0 ) );

    Polygon body( { Point( -5.0, 5.0 ) + neck,
                    Point( 3.0, -16.0 ),
                    Point( 35.0, -20.0 ),
                    Point( 30.0, -10.0 ) } );

    draw( FUR_COLOR_1, body );

    Drawing head_drawing;

    Polygon head1 = Polygon( { Point( -12.0, 3.0 ),
                               Point( -20.0, 0.0 ),
                               Point( -16.0, -3.0 ),
                               Point( 0.0, -5.0 ),
                               Point( 5.0, 0.0 ) } ) + neck;

    Polygon head2 = Polygon( { Point( -12.0, 3.0 ),
                               Point( -20.0, 0.0 ),
                               Point( 5.0, 0.0 ) } ) + neck;

    Polygon head3 = Polygon( { Point( 0.0, 10.0 ),
                               Point( -8.0, 8.0 ),
                               Point( -12.0, 3.0 ),
                               Point( 5.0, 0.0 ) } ) + neck;

    Polygon nose = Polygon( { Point( -17.0, 1.25 ),
                              Point( -20.0, 0.0 ),
                              Point( -18.0, -1.5 ) } ) + neck;

    head_drawing.draw( FUR_COLOR_2, head1 );
    head_drawing.draw( FUR_COLOR_1, head2 );
    head_drawing.draw( FUR_COLOR_1, head3 );
    head_drawing.draw( NOSE_COLOR, nose );

    Polygon ear = Polygon( { Point( -3.0, 5.0 ),
                             Point( 0.0, 15.0 ),
                             Point( 3.0, 10.0 ) } ) + neck;

    head_drawing.draw( FUR_COLOR_1, ear );

    draw( head_drawing );
}
