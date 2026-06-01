#include "mt/environment/water/Waterfall.hpp"

var_const( ticks RIPPLE_PAUSE = ( 25.0 ) );
var_const( Planc RIPPLE_HEIGHT = ( 12.0 ) );
var_const( Planc RIPPLE_TIP_HEIGHT = ( 8.0 ) );
var_const( Span<Planc> RIPPLE_RADIUS = { 5.0, 20.0 } );
var_const( uint RIPPLE_EDGE_COUNT = ( 8 ) );
var_const( dec RIPPLE_ALPHA_START = ( 0.1 ) );
var_const( dec RIPPLE_ALPHA_GROWTH = ( 0.01 ) );
var_const( dec RIPPLE_GRAVITY_RATIO = ( 5.0 ) );

var_const( Planc FOAM_X_SPEED_MAX = ( 2.0 ) );
var_const( Span<Planc> FOAM_Y_SPEED = { 3.0, 4.0 } );
var_const( Planc FOAM_SMALL_Y_SPEED_RATIO = ( 1.01 ) );
var_const( Span<Planc> FOAM_RADIUS_START = { 5.0, 20.0 } );
var_const( Span<Planc> FOAM_SMALL_RADIUS_START = { 2.5, 7.5 } );
var_const( Planc FOAM_RADIUS_MIN = ( 0.1 ) );
var_const( Planc FOAM_RADIUS_SHRINK = ( 0.25 ) );
var_const( Planc FOAM_ALPHA_START = ( 1.0 ) );
var_const( Planc FOAM_ALPHA_SHRINK = ( 0.05 ) );
var_const( uint FOAM_EDGE_COUNT = ( 24 ) );
var_const( Span<uint> FOAM_NEW_COUNT = { 0, 4 } );
var_const( Span<uint> FOAM_SMALL_NEW_COUNT = { 3, 7 } );
var_const( dec FOAM_COLOR_TOP_ALPHA = ( 0.75 ) );
var_const( dec FOAM_GRAVITY_RATIO = ( 0.75 ) );

var_const( Color WATER_COLOR = ( CYAN ) );
var_const( Color FOAM_COLOR = ( WHITE ) );

Waterfall::Waterfall( ptr<Room> in_room, Point cref in_bottom, Planc cref in_width, Planc cref in_height, dec in_z ) : Object( in_room, in_bottom, in_z )
{
    background( true );

    needs_render_always( true );

    stationary( true );

    space( Polygon::rectangle( in_width, in_height ) );

    m_ripple_countdown.reset( RIPPLE_PAUSE );
}

void Waterfall::render( )
{
    Object::render( );

    Polygon bounds = Polygon::rectangle( width( ), height( ), Point( 0.0, half( height( ) ) ) );

    draw( WATER_COLOR, bounds );
    draw( { Color::transparent( FOAM_COLOR ),
            Color::transparent( FOAM_COLOR ),
            FOAM_COLOR.a( FOAM_COLOR_TOP_ALPHA ),
            FOAM_COLOR.a( FOAM_COLOR_TOP_ALPHA ) }, bounds );

    Drawing ripples;
    ripples.add_bounds( bounds );
    for_each( ripple, m_ripples ) { ripples.draw( FOAM_COLOR.a( ripple.alpha ), ripple.polygon + VectorY( ripple.height ) ); }
    draw( ripples );

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
                Point pt( center_x, RIPPLE_TIP_HEIGHT * abs( center_x / half( width( ) ) ) );
                Arc arc = Arc::semi_ccw( radius, pt, PI );

                if( ripple_path.lines( ).size( ) )
                {
                    ripple_path += Line( ripple_path.lines( ).back( ).pt2( ), pt - VectorX( radius ) );
                }

                ripple_path += arc.path( RIPPLE_EDGE_COUNT );

                center_x += radius;
                radius = Random::rPlanc( RIPPLE_RADIUS );
                center_x += radius;
            }

            Ripple ref ripple = m_ripples.insert_back( );

            ripple.polygon = Polygon( ripple_path.points( ) + ( ripple_path + VectorY( RIPPLE_HEIGHT ) ).points( ).reversed( ) );
            ripple.height = height( ) + RIPPLE_HEIGHT + RIPPLE_RADIUS.max( );
            ripple.alpha = RIPPLE_ALPHA_START;
        }

        for_each( ripple, m_ripples )
        {
            ripple.height += ( gravity( ) * RIPPLE_GRAVITY_RATIO ).dy( );
            ripple.alpha = min( ripple.alpha + RIPPLE_ALPHA_GROWTH, 1.0 );
        }
        m_ripples.remove_if( [ ] ( Ripple cref in_ripple )
        {
            return( is_neg( in_ripple.height ) );
        } );
    }

    if( m_create_foam )
    {
        uint new_foam_count = Random::rint( FOAM_NEW_COUNT );
        while( new_foam_count-- )
        {
            Foam ref foam = m_foam.insert_back( Foam( ) );

            foam.position = VectorX( Random::negated( Random::rdec( half( width( ) ) ) ) );
            foam.movement = Vector( Random::negated( Random::rPlanc( FOAM_X_SPEED_MAX ) ), Random::rPlanc( FOAM_Y_SPEED ) );
            foam.radius = Random::rPlanc( FOAM_RADIUS_START );
            foam.alpha = FOAM_ALPHA_START;
        }

        uint new_foam_small_count = Random::rint( FOAM_SMALL_NEW_COUNT );
        while( new_foam_small_count-- )
        {
            Foam ref foam = m_foam.insert_back( Foam( ) );

            foam.position = VectorX( Random::negated( Random::rdec( half( width( ) ) ) ) );
            foam.movement = Vector( Random::negated( Random::rPlanc( FOAM_X_SPEED_MAX ) ), Random::rPlanc( FOAM_Y_SPEED ) * FOAM_SMALL_Y_SPEED_RATIO );
            foam.radius = Random::rPlanc( FOAM_SMALL_RADIUS_START );
            foam.alpha = FOAM_ALPHA_START;
        }

        for_each( foam, m_foam )
        {
            foam.position += foam.movement;
            foam.movement += gravity( ) * FOAM_GRAVITY_RATIO;
            foam.radius -= FOAM_RADIUS_SHRINK;
            foam.alpha -= FOAM_ALPHA_SHRINK;
        }
        m_foam.remove_if( [ ] ( Foam cref in_foam )
        {
            return( !is_pos( in_foam.alpha ) || ( in_foam.radius <= FOAM_RADIUS_MIN ) || ( in_foam.position.y( ) < -in_foam.radius ) );
        } );
    }
}
