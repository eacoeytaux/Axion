#include "Bush.hpp"
#include "World.hpp"

using mtmercy::Bush;

namespace
{

cPlanc BASE_WIDTH = 33.0;
cPlanc LEAF_LENGTH = 45.0;
const Span<uint> LEAF_COUNT = { 5, 7 };
cAngle LEAF_BASE = RIGHT / 4.0;
cPlanc LEAF_TIP_OFFSET = 5.0;

cColor BUSH_COLOR = GREEN;

} // namespace

Bush::Bush( Room * room, Coordinate cref _root, cdec _z ) : Object( room, _root, _z )
{
    background( true );

    persist_render( true );

    Drawing bush_drawing;

    Coordinate base_left( -half( BASE_WIDTH ), 0.0 );
    Coordinate base_right( half( BASE_WIDTH ), 0.0 );

    int leaf_count = Random::rint( LEAF_COUNT );
    Angle leaf_delta = ( ( PI - ( LEAF_BASE * 2.0 ) ) / (dec)leaf_count );

    for_range( i, leaf_count + 1 )
    {
        bush_drawing.draw( BUSH_COLOR, Polygon( { base_right,
                                                  VectorA( LEAF_BASE + ( leaf_delta * (dec)i ), LEAF_LENGTH ) + VectorA( Random::rAngle( ), Random::rPlanc( LEAF_TIP_OFFSET ) ),
                                                  base_left } ) );
    }

    draw( bush_drawing );
}
