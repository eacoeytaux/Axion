#include "Boulder.hpp"

using mtmercy::Boulder;

namespace
{

const Span<Planc> WIDTH_RATIO = { 0.75, 0.85 };
const Span<Planc> TOP_WIDTH_RATIO = { 0.25, 0.35 };
const Span<Planc> OFFSET_RATIO = { 0.05, 0.1 };

cColor COLOR = GRAY_MID;

}

Boulder::Boulder( Room * room, const Coordinate & _base, Planc cref _scale, dec _z ) : Object( room, _base, _z )
{
    background( true );
    
    persist_render( true );

    stationary( true );
    
    draw( COLOR, Polygon( Arc::ccw( _base, _scale, 0.0, PI ).path( 21 ).points( ) ).stretch( VectorY( 2.5 ) ) );
}
