#include "Waterfall.hpp"
#include "World.hpp"

using mtmercy::Waterfall;

namespace
{

cuint RIPPLE_PAUSE = 25;
cPlanc RIPPLE_HEIGHT = 12.0;
cPlanc RIPPLE_TIP_HEIGHT = 8.0;
const Span<Planc> RIPPLE_RADIUS = { 5.0, 20.0 };
cuint RIPPLE_EDGE_COUNT = 8;
const dec RIPPLE_ALPHA_GROWTH = 0.01;
cdec RIPPLE_GRAVITY_RATIO = 5.0;

cPlanc FOAM_X_SPEED_MAX = 2.0;
const Span<Planc> FOAM_Y_SPEED = { 3.0, 4.0 };
cPlanc FOAM_SMALL_Y_SPEED_RATIO = 1.01;
const Span<Planc> FOAM_RADIUS_START = { 5.0, 20.0 };
const Span<Planc> FOAM_SMALL_RADIUS_START = { 2.5, 7.5 };
cPlanc FOAM_RADIUS_MIN = 0.1;
cPlanc FOAM_RADIUS_SHRINK = 0.25;
cPlanc FOAM_ALPHA_START = 1.0;
cPlanc FOAM_ALPHA_SHRINK = 0.05;
cuint FOAM_EDGE_COUNT = 12;
const Span<uint> FOAM_NEW_COUNT = { 0, 4 };
const Span<uint> FOAM_SMALL_NEW_COUNT = { 3, 7 };
const dec FOAM_COLOR_TOP_ALPHA = 0.75;
cdec FOAM_GRAVITY_RATIO = 0.75;

const Color WATER_COLOR = CYAN;
const Color FOAM_COLOR = WHITE;

} // namespace

Waterfall::Waterfall( Room * room, Coordinate cref _bottom, cPlanc _width, cPlanc _height, cdec _z ) : Object( room, _bottom )
{
    background( true );

    needs_render_always( true );

    z( _z );

    space( Polygon::rectangle( _width, _height ) );

    no_gravity( );

    m_ripple_countdown.reset( RIPPLE_PAUSE );
}

void Waterfall::render( )
{
    Object::render( );

    Polygon bounds = Polygon::rectangle( width( ), height( ), Coordinate( 0.0, half( height( ) ) ) );

    draw( WATER_COLOR, bounds );
    draw( { FOAM_COLOR.a( 0.0 ), FOAM_COLOR.a( 0.0 ), FOAM_COLOR.a( FOAM_COLOR_TOP_ALPHA ), FOAM_COLOR.a( FOAM_COLOR_TOP_ALPHA ) }, bounds );

    add_bound( bounds );
    for_each( ripple, m_ripples ) { draw( FOAM_COLOR.a( ripple.alpha ), ripple.polygon + VectorY( ripple.height ) ); }
    clear_bounds( );

    if( m_create_foam )
    {
        for_each( foam, m_foam ) { draw( FOAM_COLOR.a( Settings::get( Settings::GRAPHICS_DETAIL_ALPHA ) ? foam.alpha : 1.0 ), Polygon::equilateral( FOAM_EDGE_COUNT, foam.radius, foam.position ) ); }
    }
}

void Waterfall::update( )
{
    Object::update( );

    { // ripples
        if( m_ripple_countdown.tick( ) )
        {
            m_ripple_countdown.reset( );

            Planc center_x = -half( Random::rPlanc( RIPPLE_RADIUS ) ) - half( width( ) );
            Planc radius = Random::rPlanc( RIPPLE_RADIUS );

            Path ripple_path;

            while( ( center_x - radius ) < half( width( ) ) )
            {
                Coordinate c( center_x, RIPPLE_TIP_HEIGHT * abs( center_x / half( width( ) ) ) );
                Arc arc = Arc::ccw( c, radius, PI, 0.0 );

                if( ripple_path.lines( ).size( ) )
                {
                    ripple_path += Line( ripple_path.lines( ).back( ).c2( ), c - VectorX( radius ) );
                }

                ripple_path += arc.path( RIPPLE_EDGE_COUNT );

                center_x += radius;
                radius = Random::rPlanc( RIPPLE_RADIUS );
                center_x += radius;
            }

            Ripple & ripple = m_ripples.insert_back( );

            ripple.polygon = Polygon( ripple_path.points( ) + ( ripple_path + VectorY( RIPPLE_HEIGHT ) ).points( ).reversed( ) );
            ripple.height = ( height( ) + RIPPLE_HEIGHT + RIPPLE_RADIUS.max( ) );
        }

        for_each( ripple, m_ripples )
        {
            ripple.height += ( GRAVITY * RIPPLE_GRAVITY_RATIO ).dy( );
            ripple.alpha = min( ripple.alpha + RIPPLE_ALPHA_GROWTH, 1.0 );
        }
        m_ripples.remove_if( [ ] ( Ripple cref ripple )
        {
            return ( is_neg( ripple.height ) );
        } );
    }

    if( m_create_foam )
    {
        uint new_foam_count = Random::rint( FOAM_NEW_COUNT );
        while( new_foam_count-- )
        {
            Foam & foam = m_foam.insert_back( Foam( ) );
            foam.position = VectorX( Random::negated( Random::rdec( half( width( ) ) ) ) );
            foam.movement = Vector( Random::negated( Random::rPlanc( FOAM_X_SPEED_MAX ) ), Random::rPlanc( FOAM_Y_SPEED ) );
            foam.radius = Random::rPlanc( FOAM_RADIUS_START );
            foam.alpha = FOAM_ALPHA_START;
        }

        uint new_foam_small_count = Random::rint( FOAM_SMALL_NEW_COUNT );
        while( new_foam_small_count-- )
        {
            Foam & foam = m_foam.insert_back( Foam( ) );
            foam.position = VectorX( Random::negated( Random::rdec( half( width( ) ) ) ) );
            foam.movement = Vector( Random::negated( Random::rPlanc( FOAM_X_SPEED_MAX ) ), Random::rPlanc( FOAM_Y_SPEED ) * FOAM_SMALL_Y_SPEED_RATIO );
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
        m_foam.remove_if( [ ] ( Foam cref foam )
        {
            return ( !is_pos( foam.alpha ) || ( foam.radius <= FOAM_RADIUS_MIN ) || ( foam.position.y( ) < -foam.radius ) );
        } );
    }
}
