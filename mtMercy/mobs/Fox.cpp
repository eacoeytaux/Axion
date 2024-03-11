#include "Fox.hpp"

AXN_NAMESPACES
using axn::reality::Fox;

Fox::Fox( World * world, const Coordinate & _position )
: Object( world, _position )
{
    space( Shape::rectangle( 32.f, 32.f ) );
    drawing_always_dirty(true);
}

const Fox & Fox::render( ) const {
    Object::render( );
    
    Drawing fox;
    
    Color color = CYAN;
    Shape body( { Coordinate( 70.f, 30.f ), Coordinate ( 0.f, 50.f ), Coordinate( 0.f, 0.f ), Coordinate( 70.f, 0.f ) } );
    fox.add( color, body );
    
    draw( fox );
    
    return *this;
}

Fox & Fox::update( ) {
    Object::update( );
    return *this;
}
