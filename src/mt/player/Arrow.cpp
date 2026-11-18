#include "mt/player/Arrow.hpp"

#include "axn/world/Mob.hpp"

var_const( dec DAMAGE = ( 10.0 ) );

var_const( dec GRAVITY = ( 0.5 ) );

var_const( dec LIFESPAN = ( 300.0 ) );
var_const( dec FADESPAN = ( 50.0 ) );

var_const( Planc HEAD_LENGTH_INNER = ( 4.0 ) );
var_const( Planc HEAD_LENGTH_OUTER = ( 6.0 ) );
var_const( Planc HEAD_WIDTH = ( 3.0 ) );
var_const( Planc HEAD_BUFFER = ( 1.0 ) );
var_const( Planc SHAFT_LENGTH = ( 30.0 ) );
var_const( Planc SHAFT_THICKNESS = ( 3.0 ) );
var_const( Planc FEATHER_HEIGHT = ( 8.0 ) );
var_const( Planc FEATHER_LENGTH = ( 12.0 ) );
var_const( Planc FEATHER_NOTCH_LENGTH = ( 4.0 ) );

var_const( Polygon HEAD( { Point( HEAD_BUFFER, 0.0 ),
                           Point( -HEAD_LENGTH_OUTER, -HEAD_WIDTH ),
                           Point( -HEAD_LENGTH_INNER, 0.0 ),
                           Point( -HEAD_LENGTH_OUTER, HEAD_WIDTH ) } ) );

var_const( Polygon FEATHER( { Point( 0.0, 0.0 ),
                              Point( -FEATHER_NOTCH_LENGTH, half( FEATHER_HEIGHT ) ),
                              Point( -FEATHER_LENGTH, half( FEATHER_HEIGHT ) ),
                              Point( -FEATHER_LENGTH + FEATHER_NOTCH_LENGTH, 0.0 ),
                              Point( -FEATHER_LENGTH, -half( FEATHER_HEIGHT ) ),
                              Point( -FEATHER_NOTCH_LENGTH, -half( FEATHER_HEIGHT ) ) } ) );

var_const( Color TIP_COLOR = ( GRAY_LIGHT ) );
var_const( Color SHAFT_COLOR = ( Color::rgb( 0x8B4513 ) ) );

Arrow Arrow::tip( ptr<Room> in_room, Point cref in_tip, Vector cref in_launch_speed, Color cref in_feather_color ) { return( Arrow( in_room, in_tip, in_launch_speed, in_feather_color ) ); }
Arrow Arrow::base( ptr<Room> in_room, Point cref in_base, Vector cref in_launch_speed, Color cref in_feather_color ) { return( Arrow( in_room, in_base + VectorA( in_launch_speed.angle( ), ( SHAFT_LENGTH + HEAD_LENGTH_INNER ) ), in_launch_speed, in_feather_color ) ); }

Arrow::Arrow( ptr<Room> in_room, Point cref in_tip, Vector cref in_launch_speed, Color cref in_feather_color ) : Projectile( in_room, DAMAGE, in_tip )
{
    #if defined( AXN_DEBUG )
    draw_debug = true;
    #endif

    mark_to_delete( LIFESPAN, FADESPAN );

    interactive( true );

    solid( true );

    gravity_scale( 0.5 );
    no_air_resistance( );

    velocity( in_launch_speed );

    angle( in_launch_speed.angle( ) );

    m_feather_color = in_feather_color;
}

void Arrow::render( )
{
    Object::render( );

    draw_shaft( );
    draw_head( );
    draw_feather( );

    Drawing::rotate( rotation( ) );
}

void Arrow::draw_head( )
{
    Polygon head = HEAD;

    head.rotate( angle( ) );

    draw( TIP_COLOR.a( to_deletion_countdown( ).percentage( ) ), head );
}

void Arrow::draw_shaft( )
{
    Point tip( -HEAD_LENGTH_INNER, 0.0 );
    Point back( -( SHAFT_LENGTH + HEAD_LENGTH_INNER ), 0.0 );

    tip.rotate( angle( ) );
    back.rotate( angle( ) );

    draw( SHAFT_COLOR.a( to_deletion_countdown( ).percentage( ) ), Line( tip, back ), SHAFT_THICKNESS );
}

void Arrow::draw_feather( )
{
    Polygon feather = FEATHER;

    feather.move( VectorX( -( SHAFT_LENGTH + HEAD_LENGTH_INNER ) + FEATHER_NOTCH_LENGTH ) );
    feather.rotate( angle( ) );

    draw( m_feather_color.a( to_deletion_countdown( ).percentage( ) ), feather );
}

void Arrow::update( )
{
    Object::update( );

    if( !grounded( ) )
    {
        needs_render( true );

        if( velocity( ).has_magnitude( ) )
        {
            angle( velocity( ).angle( ) );
        }
    }

    if( age( ) >= LIFESPAN )
    {
        needs_render( true );

        if( age( ) >= ( LIFESPAN + FADESPAN ) )
        {
            mark_to_delete( );
        }
    }
}

Angle Arrow::angle( ) const { return( m_angle ); }

void Arrow::angle( Angle cref in_angle )
{
    if( m_angle != in_angle )
    {
        m_angle = in_angle;
        needs_render( true );
    }
}

dec Arrow::length( ) const { return( SHAFT_LENGTH + HEAD_LENGTH_INNER ); }

void Arrow::ground( ptr<Terrain::Node> ground, Terrain::Bumper cref in_bumper )
{
    Object::ground( ground, in_bumper );

    if( ground )
    {
        stationary( true );
        solid( false );
    }
}

void Arrow::react_to_movement( ptr<Object> in_object, Vector cref in_movement )
{
    Object::react_to_movement( in_object, in_movement );

    move( in_movement );
}
