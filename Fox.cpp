#include "Fox.hpp"

using mtmercy::Fox;

Fox::Fox( World * world, const Coordinate & _position ) : Mob( world, _position )
{
    space( Rectangle( 32.0, 32.0 ) );
}

const Fox & Fox::render( ) const
{
    Mob::render( );

    Drawing fox;

    Color c = RED;
    Polygon body( { Coordinate( 70.0, 30.0 ), Coordinate( 0.0, 50.0 ), Coordinate( 0.0, 0.0 ), Coordinate( 70.0, 0.0 ) } );
    fox.draw( c, body );

    draw( fox );

    return *this;
}

Fox & Fox::update( )
{
    Object::update( );

    // todo

    return *this;
}
