#include "Stalate.hpp"

using mtmercy::Stalate;

namespace
{

const Color COLOR = GRAY_MID;

}

Stalate::Stalate( Room * room, const bool _hanging, const Coordinate & _base, Planc cref _width, Planc cref _height, dec _z ) : Object( room, _base, _z )
{
    background( true );
    
    persist_render( true );

    stationary( true );
    
     draw( COLOR, Polygon( { CoordinateY( _height ), CoordinateX( -_width ), CoordinateX( _width ) } ) );
}
