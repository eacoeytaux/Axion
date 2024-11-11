#include "Stalagmite.hpp"

using mtmercy::Stalagmite;

Stalagmite::Stalagmite( World * world, const Coordinate & _base, Planc _height, dec _z, const Color & _color ) : Object( world, _base )
{
    background( true );
    
    persist_render( true );

    stationary( true );

    z( _z );
    
    draw( _color, Polygon( { Coordinate( 0, _height ), Coordinate( -half( _height ), 0), Coordinate( half( _height ), 0 ) } ) );
}
