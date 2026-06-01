#include "mt/environment/plants/Bush.hpp"

var_const( Planc BASE_WIDTH = ( 33.0 ) );
var_const( Planc LEAF_LENGTH = ( 45.0 ) );
var_const( Span<uint> LEAF_COUNT = { 5, 7 } );
var_const( Angle LEAF_BASE = ( Angle::fraction( 16.0 ) ) );
var_const( Planc LEAF_TIP_OFFSET = ( 5.0 ) );

var_const( Color BUSH_COLOR = ( GREEN ) );

Bush::Bush( ptr<Room> in_room, Point cref in_root, dec in_z ) : Object( in_room, in_root, in_z )
{
    background( true );

    persist_render( true );

    Drawing bush_drawing;

    Point base_left( -half( BASE_WIDTH ), 0.0 );
    Point base_right( half( BASE_WIDTH ), 0.0 );

    int leaf_count = Random::rint( LEAF_COUNT );
    Angle leaf_delta = ( ( PI - x2( LEAF_BASE ) ) / dec( leaf_count ) );

    for_range( i, leaf_count + 1 )
    {
        bush_drawing.draw( BUSH_COLOR, Polygon( { base_right,
                                                  VectorA( LEAF_BASE + ( leaf_delta * i ), LEAF_LENGTH ) + VectorA( Random::rAngle( ), Random::rPlanc( LEAF_TIP_OFFSET ) ),
                                                  base_left } ) );
    }

    draw( bush_drawing );
}
