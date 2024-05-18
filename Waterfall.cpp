#include "Waterfall.hpp"

#include "World.hpp"

using mtmercy::Waterfall;

namespace
{
const Planc FOAM_X_SPEED_MAX = 4.0;
const Span<Planc> FOAM_Y_SPEED = { 3.0, 5.0 };
const Planc FOAM_SMALL_Y_SPEED_RATIO = 1.2;
const Span<Planc> FOAM_RADIUS_START = { 5.0, 20.0 };
const Span<Planc> FOAM_SMALL_RADIUS_START = { 2.5, 7.5 };
const Planc FOAM_RADIUS_MIN = 0.5;
const Planc FOAM_RADIUS_SHRINK = 0.5;
const Planc FOAM_ALPHA_START = 1.0;
const Planc FOAM_ALPHA_SHRINK = 0.05;
const dec FOAM_GRAVITY_RATIO = 0.2;
const uint FOAM_EDGE_COUNT = 16;
const Span<uint> FOAM_NEW_COUNT = { 0, 4 };
const Span<uint> FOAM_SMALL_NEW_COUNT = { 3, 7 };
const Color WATER_COLOR = CYAN;
const Color FOAM_COLOR = WHITE;
} // namespace

Waterfall::Waterfall( World * _world, const Coordinate & _bottom, const Planc _width, const Planc _height, const dec _z ) : StationaryObject( _world, _bottom )
{
    drawing_always_dirty( true );

    z( _z );

    space( Rectangle( _width, _height ) );
}

const Waterfall & Waterfall::render( ) const
{
    const Color bottom_color = Color( FOAM_COLOR, 0.75 );
    const Color top_color = Color( FOAM_COLOR, 0.0 );

    Object::render( );

    draw( WATER_COLOR, Rectangle( width( ), height( ), Coordinate( 0, height( ).half( ) ) ) );

    draw( { top_color, top_color, bottom_color, bottom_color }, Rectangle( width( ), height( ), Coordinate( 0, height( ).half( ) ) ) );

    for_each( foam, m_foam ) { draw( FOAM_COLOR.a( foam.alpha ), Polygon::equilateral( FOAM_EDGE_COUNT, foam.radius, foam.position ) ); }

    return *this;
}

Waterfall & Waterfall::update( )
{
    Object::update( );

    uint new_foam_count = Random::rint( FOAM_NEW_COUNT );
    while( new_foam_count-- )
    {
        Foam & foam = m_foam.insert_back( Foam( ) );
        foam.position = VectorX( Random::flipped( Random::rdec( width( ).half( ) ) ) );
        foam.movement = Vector( Random::rPlanc( -FOAM_X_SPEED_MAX, FOAM_X_SPEED_MAX ), Random::rPlanc( FOAM_Y_SPEED ) );
        foam.radius = Random::rPlanc( FOAM_RADIUS_START );
        foam.alpha = FOAM_ALPHA_START;
    }

    uint new_foam_small_count = Random::rint( FOAM_SMALL_NEW_COUNT );
    while( new_foam_small_count-- )
    {
        Foam & foam = m_foam.insert_back( Foam( ) );
        foam.position = VectorX( Random::flipped( Random::rdec( width( ).half( ) ) ) );
        foam.movement = Vector( Random::rPlanc( -FOAM_X_SPEED_MAX, FOAM_X_SPEED_MAX ), Random::rPlanc( FOAM_Y_SPEED ) * FOAM_SMALL_Y_SPEED_RATIO );
        foam.radius = Random::rPlanc( FOAM_SMALL_RADIUS_START );
        foam.alpha = FOAM_ALPHA_START;
    }

    for_each( foam, m_foam )
    {
        foam.radius -= FOAM_RADIUS_SHRINK;
        foam.movement += GRAVITY * FOAM_GRAVITY_RATIO;
        foam.position += foam.movement;
        foam.alpha -= FOAM_ALPHA_SHRINK;
    }
    m_foam.remove_if( []( const Foam & foam )
                      { return ( ( foam.alpha <= 0.0 ) || ( foam.radius <= FOAM_RADIUS_MIN ) || ( foam.position.y( ) < -foam.radius ) ); } );

    return *this;
}
