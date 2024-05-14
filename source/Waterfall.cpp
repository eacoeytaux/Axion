#include "Waterfall.hpp"

#include "World.hpp"

using mtmercy::Waterfall;

namespace
{
const Planc FOAM_X_SPEED = 5.0;
const Planc FOAM_Y_SPEED = 10.0;
const Span<Planc> FOAM_RADIUS_START = { 5.0, 20.0 };
const Planc FOAM_RADIUS_MIN = 2.0;
const Planc FOAM_RADIUS_SHRINK = 0.5;
const double FOAM_GRAVITY_RATIO = 0.5;
const uint FOAM_EDGE_COUNT = 16;
const Span<uint> FOAM_NEW_COUNT = { 0, 4 };

const Color WATER_COLOR = CYAN;
const Color FOAM_COLOR = WHITE;
} // namespace

Waterfall::Waterfall( World * _world, const Coordinate & _bottom, const Planc _width, const Planc _height, const double _z ) : StationaryObject( _world, _bottom )
{
    drawing_always_dirty( true );

    z( _z );

    space( Shape::rectangle( _width, _height ) );
}

const Waterfall & Waterfall::render( ) const
{
    const Color bottom_color = Color( FOAM_COLOR, 0.75 );
    const Color top_color = Color( FOAM_COLOR, 0.0 );

    Object::render( );

    draw( WATER_COLOR, Shape::rectangle( width( ), height( ), Coordinate( 0, height( ).half( ) ) ) );

    draw( { top_color, top_color, bottom_color, bottom_color }, Shape::rectangle( width( ), height( ), Coordinate( 0, height( ).half( ) ) ) );

    for_each( foam, m_foam ) { draw( FOAM_COLOR, Shape::equilateral( FOAM_EDGE_COUNT, foam.radius, foam.position ) ); }

    return *this;
}

Waterfall & Waterfall::update( )
{
    Object::update( );

    uint new_foam_count = Random::rint( FOAM_NEW_COUNT );
    while( new_foam_count-- )
    {
        Foam & foam = m_foam.insert_back( Foam( ) );
        foam.position = Vector( Random::flipped( Random::rdouble( width( ).half( ).half( ) ) ), 0.0 );
        foam.radius = Random::rdouble( FOAM_RADIUS_START );
        foam.movement = Vector( Random::rdouble( -FOAM_X_SPEED, FOAM_X_SPEED ), FOAM_Y_SPEED );
    }

    for_each( foam, m_foam )
    {
        foam.movement += GRAVITY * FOAM_GRAVITY_RATIO;
        foam.position += foam.movement;
        foam.radius -= FOAM_RADIUS_SHRINK;
    }
    m_foam.remove_if( [ & ]( Foam foam )
                      { return ( ( foam.radius <= FOAM_RADIUS_MIN ) || ( foam.position.y( ) < 0.0 ) ); } );

    return *this;
}
