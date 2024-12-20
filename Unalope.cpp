#include "Unalope.hpp"

#include "Player.hpp"

using mtmercy::Unalope;

namespace
{
const dec TEMP_SCALE_TODO = 1.0;

const Planc SIGHT_RANGE = METER * 10.0;
const Planc ALERT_RANGE = SIGHT_RANGE * 0.75;

const Planc FACE_WIDTH;

const Polygon HEAD = Polygon( {
    Coordinate( -3.0, -8.0 ),
    Coordinate( -12.0, 0.0 ),
    Coordinate( -9.0, 8.0 ),
    Coordinate( -3.0, 12.0 ),
    Coordinate( 3.0, 12.0 ),
    Coordinate( 9.0, 8.0 ),
    Coordinate( 12.0, 0.0 ),
    Coordinate( 3.0, -8.0 )
                              } ).scale( TEMP_SCALE_TODO );

const Polygon NOSE = Polygon( {
    Coordinate( -3.0, -4.0 ),
    Coordinate( 0.0, -6.4 ),
    Coordinate( 3.0, -4.0 )
                              } ).scale( TEMP_SCALE_TODO );

const Polygon EAR = Polygon( {
    Coordinate( -6.0, 6.0 ),
    Coordinate( -16.0, 24.0 ),
    Coordinate( -9.0, 48.0 ),
    Coordinate( -3.0, 12.0 )
                             } ).scale( TEMP_SCALE_TODO );

const Polygon INNER_EAR = Polygon( {
    Coordinate( -7.0, 12.0 ),
    Coordinate( -13.0, 24.0 ),
    Coordinate( -10.0, 36.0 )
                                   } ).scale( TEMP_SCALE_TODO );

const Polygon EAR_MIRRORED = Polygon( EAR ).mirror_y( );
const Polygon INNER_EAR_MIRRORED = Polygon( INNER_EAR ).mirror_y( );

const Polygon BODY = Polygon( {
    Coordinate( 0.0, 0.0 ),
    Coordinate( -9.0, -6.0 ),
    Coordinate( -10.0, -9.0 ),
    Coordinate( -5.0, -9.0 )
                              } ).scale( TEMP_SCALE_TODO );

const Color FUR_COLOR = WHITE;
const Color EAR_COLOR = FUR_COLOR;
const Color INNER_EAR_COLOR = RED;
const Color NOSE_COLOR = RED;
}

Unalope::Unalope( World * world, const Coordinate & _position ) : Enemy( world, _position, 10000.0 )
{
    space( Polygon::rectangle( 16.0, 16.0 ) );

    eye_info( 4, 1, 240 );

    sight_range( SIGHT_RANGE );
    alert_range( ALERT_RANGE );
}

void Unalope::update( )
{
    Enemy::update( );
}

void Unalope::render( )
{
    Enemy::render( );

    // draw( FUR_COLOR, BODY );

    draw( EAR_COLOR, EAR );
    draw( EAR_COLOR, EAR_MIRRORED );

    draw( INNER_EAR_COLOR, INNER_EAR );
    draw( INNER_EAR_COLOR, INNER_EAR_MIRRORED );

    draw( FUR_COLOR, HEAD );

    draw( NOSE_COLOR, NOSE );

    // draw_eyes( Coordinate( -24.0, 0.0 ), Coordinate( 24.0, 0.0 ) );
}