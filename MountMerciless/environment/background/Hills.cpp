#include "Hills.hpp"

#include "World.hpp"

using mtmercy::Hills;

// TODO
Color GRASS_COLOR = Color::rgb( 0x00E000 );
Color TREE_COLOR = Color::rgb( 0x00C000 );

Hills::Hills( World * world, const Planc _base_bottom, const double _z ) : Object( world )
{
    persist_drawing( true );

    z( _z );
    stationary( true );

    Shape tree( { Coordinate( 0.0, 3.0 ), Coordinate( -1.0, 0.0 ), Coordinate( 1.0, 0.0 ) } );
    tree.scale( 75.0 );

    Shape hill = Shape::circle( 1000.0, Coordinate( 0.0, -500.0 ) );

    Shape hill_temp = hill;
    hill.stretch( VectorX( 2.0 ) );

    m_hills.draw( GRASS_COLOR, hill );
}

const Hills & Hills::render( ) const
{
    Object::render( );

    draw( m_hills );

    return *this;
}
