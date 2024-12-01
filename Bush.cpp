#include "Bush.hpp"
#include "World.hpp"

using mtmercy::Bush;

namespace
{
const Planc BASE_WIDTH = 33.0;
const Planc LEAF_LENGTH = 45.0;
const Span<uint> LEAF_COUNT = { 5, 7 };
const Angle LEAF_BASE = RIGHT_ANGLE / 4.0;

const Color BUSH_COLOR = GREEN;
} // namespace

Bush::Bush( World * world, const Coordinate & _root, const dec _z ) : Object( world, _root )
{
    background( true );

    persist_render( true );

    z( _z );

    Drawing bush_drawing;

    Coordinate base_left( -half( BASE_WIDTH ), 0.0 );
    Coordinate base_right( half( BASE_WIDTH ), 0.0 );

    Coordinate tip = VectorA( LEAF_BASE, LEAF_LENGTH );

    int leaf_count = Random::rint( LEAF_COUNT );
    Angle leaf_delta = ( ( PI - ( LEAF_BASE * 2.0 ) ) / (dec)leaf_count );

    for_range( i, leaf_count + 1 )
    {
        bush_drawing.draw( BUSH_COLOR, Polygon( { base_right,
                                                  VectorA( LEAF_BASE + ( leaf_delta * (dec)i ), LEAF_LENGTH ),
                                                  base_left } ) );
    }

    draw( bush_drawing );
}
