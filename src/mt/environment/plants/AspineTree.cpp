#include "mt/environment/plants/AspineTree.hpp"

var_const( dec TRUNK_LEAF_OFFSET = ( 0.95 ) );
var_const( Span<Planc> TRUNK_HEIGHT = { 300.0, 500.0 } );
var_const( Span<Planc> TRUNK_BASE_WIDTH = { 8.0, 12.0 } );
var_const( Angle TRUNK_SWAY_MAX = ( Angle::fraction( 64.0 ) ) );

var_const( Span<uint> BRANCH_COUNT = { 3, 5 } );
var_const( Span<dec> BRANCH_BASE_HEIGHT_RATIO = { 0.3, 0.333 } );
var_const( Span<dec> BRANCH_LENGTH_RATIO = { 0.333, 0.444444 } );
var_const( Angle BRANCH_OFFSET = ( Angle::fraction( 8.0 ) ) );

var_const( uint LEAF_TOP_COUNT = ( 10 ) );
var_const( uint LEAF_BOTTOM_COUNT = ( 5 ) );
var_const( Planc LEAF_TOP_START = ( 1.0 ) );
var_const( Planc LEAF_BOTTOM_START = ( 0.8 ) );
var_const( Planc LEAF_TOP_TOP_D = ( -0.025 ) );
var_const( Planc LEAF_TOP_BOTTOM_D = ( -0.06 ) );
var_const( Planc LEAF_BOTTOM_TOP_D = ( 0.01 ) );
var_const( Planc LEAF_BOTTOM_BOTTOM_D = ( -0.0075 ) );
var_const( Planc LEAF_OFFSET_1 = ( 5.0 ) );
var_const( Planc LEAF_OFFSET_2 = ( 2.0 ) );

var_const( Planc LEAF_TIP_LENGTH = ( 7.5 ) );
var_const( Planc LEAF_CROSS_LENGTH = ( 7.5 ) );
var_const( Planc LEAF_BASE_LENGTH = ( 3.0 ) );
var_const( Span<dec> LEAF_SCALE = { 0.8, 1.1 } );
var_const( dec LEAF_GRAVITY = ( 0.025 ) );

var_const( Span<dec> LEAF_FALL_DISTANCE = { 0.1, 0.9 } );
var_const( Span<ticks> LEAF_COUNTDOWN = { 50.0, 300.0 } );

var_const( Color TRUNK_COLOR = ( WHITE ) );
var_const( Color TRUNK_MARK_COLOR = ( BLACK ) );
var_const( ColorSlider LEAF_COLOR = { RED, Color::rgb( 1.0, 0.7, 0.0 ) } );

Drawing draw_trunk( Color cref in_leaf_color, bool in_draw_leaves, Planc cref in_length, Planc cref in_base_width, uint in_branch_count, Point ref leaf_base, Point ref leaf_side1, Point ref leaf_side2 );

Drawing draw_trunk( Color cref in_leaf_color, bool in_draw_leaves, Planc cref in_length, Planc cref in_base_width, uint in_branch_count, Point ref leaf_base, Point ref leaf_side1, Point ref leaf_side2 )
{
    Drawing tree_drawing;

    Drawing trunk_drawing;

    Vector trunk = VectorY( in_length );
    Point top = trunk.destination( );
    Point base_left = PointX( -half( in_base_width ) );
    Point base_right = PointX( half( in_base_width ) );

    dec branch_height = Random::rdec( BRANCH_BASE_HEIGHT_RATIO );

    if( in_draw_leaves )
    {
        Point top;
        Point bottom;
        Point side1;
        Point side2;

        dec leaves_top = LEAF_TOP_START;
        dec leaves_bottom = LEAF_BOTTOM_START;

        for_range( i, LEAF_TOP_COUNT )
        {
            // todo
            top = VectorA( RIGHT, ( in_length * leaves_top ) + Random::negated( Random::rPlanc( LEAF_OFFSET_1 ) ) );
            bottom = VectorA( RIGHT, ( in_length * leaves_bottom ) + Random::negated( Random::rPlanc( LEAF_OFFSET_1 ) ) );
            side1 = bottom + VectorA( BRANCH_OFFSET, ( half( in_length ) * one_minus( leaves_bottom ) ) + Random::rPlanc( LEAF_OFFSET_2 ) );
            side2 = bottom + VectorA( ( PI - BRANCH_OFFSET ), ( half( in_length ) * one_minus( leaves_bottom ) ) + Random::rPlanc( LEAF_OFFSET_2 ) );

            tree_drawing.draw( in_leaf_color,
                               Polygon( { top,
                                          side1,
                                          bottom,
                                          side2 } ) );

            leaves_top += LEAF_TOP_TOP_D;
            leaves_bottom += LEAF_TOP_BOTTOM_D;
        }

        leaf_base = bottom;
        leaf_side1 = side1;
        leaf_side2 = side2;

        for_range( i, LEAF_BOTTOM_COUNT )
        {
            top = VectorA( RIGHT, in_length * leaves_top + Random::negated( Random::rPlanc( LEAF_OFFSET_1 ) ) );
            bottom = VectorA( RIGHT, in_length * leaves_bottom + Random::negated( Random::rPlanc( LEAF_OFFSET_1 ) ) );
            side1 = bottom + VectorA( BRANCH_OFFSET, ( half( in_length ) * one_minus( leaves_bottom ) * ( dec( LEAF_BOTTOM_COUNT - i ) / dec( LEAF_BOTTOM_COUNT ) ) ) + Random::rPlanc( LEAF_OFFSET_2 ) );
            side2 = bottom + VectorA( ( PI - BRANCH_OFFSET ), ( half( in_length ) * one_minus( leaves_bottom ) * ( dec( LEAF_BOTTOM_COUNT - i ) / dec( LEAF_BOTTOM_COUNT ) ) ) + Random::rPlanc( LEAF_OFFSET_2 ) );

            tree_drawing.draw( in_leaf_color,
                               Polygon( { top,
                                          side1,
                                          bottom,
                                          side2 } ) );

            leaves_top += LEAF_BOTTOM_TOP_D;
            leaves_bottom += LEAF_BOTTOM_BOTTOM_D;
        }
    }

    Polygon trunk_base = Polygon::circle( half( in_base_width ) );
    Polygon trunk_polygon( { base_right, Vector( top ) * TRUNK_LEAF_OFFSET, base_left } );

    trunk_drawing.draw( TRUNK_COLOR, trunk_base );
    trunk_drawing.draw( TRUNK_COLOR, trunk_polygon );

    do_count( in_branch_count )
    {
        dec branch_remaining = one_minus( branch_height );

        Point branch_base = Vector( trunk ).magnitude( trunk.magnitude( ) * branch_height );

        bool add_left;
        bool add_right;
        bool add_both = Random::rbool( 0 ); // todo
        if( add_both )
        {
            add_left = true;
            add_right = true;
        }
        else
        {
            add_left = Random::rbool( );
            add_right = !add_left;
        }

        lambdaf( draw_branch )( bool in_left )
        {
            Drawing branch_drawing = draw_trunk( in_leaf_color, false, in_length * Random::rdec( BRANCH_LENGTH_RATIO ) * branch_remaining, in_base_width * branch_remaining, 0, leaf_base, leaf_side1, leaf_side2 );

            branch_drawing.rotate( in_left ? BRANCH_OFFSET : -BRANCH_OFFSET );
            branch_drawing.move( Vector( branch_base ) );
            tree_drawing.draw( branch_drawing );
        };

        if( add_left )
        {
            draw_branch( true );
        }

        if( add_right )
        {
            draw_branch( false );
        }

        branch_height += branch_remaining * Random::rdec( BRANCH_BASE_HEIGHT_RATIO );
    }

    trunk_drawing.add_bounds( trunk_polygon );

    branch_height = 0.0;
    while( branch_height < 1.0 )
    {
        // dec branch_remaining = one_minus( branch_height );
        branch_height += Random::rdec( 0.02, 0.1 );

        // todo constants
        trunk_drawing.draw( TRUNK_MARK_COLOR.a( Random::rdec( 0.25, 0.75 ) ), Polygon::equilateral( 4, Random::rPlanc( 0.1, 0.3 ) * in_base_width, Vector( trunk ).magnitude( trunk.magnitude( ) * branch_height ) + VectorA( trunk.angle( ) + Random::negated( Random::rAngle( RIGHT ) ), Random::rPlanc( in_base_width ) ), Random::negated( Random::rAngle( Angle::fraction( 16.0 ) ) ) ) );
    }

    tree_drawing.draw( trunk_drawing );

    return( tree_drawing );

};

AspineTree::AspineTree( ptr<Room> in_room, Point cref in_root, const dec in_z ) : Object( in_room, in_root, in_z )
{
    background( true );

    persist_render( true );

    stationary( true );

    m_leaf_drop_countdown1.reset( Random::rdec( LEAF_COUNTDOWN.max( ) ) );
    m_leaf_drop_countdown2.reset( Random::rdec( LEAF_COUNTDOWN.max( ) ) );

    m_leaf_color = LEAF_COLOR.color_at( Random::rdec( 1.0 ) );

    Angle sway = Random::negated( Random::rAngle( TRUNK_SWAY_MAX ) );

    Planc height = Random::rPlanc( TRUNK_HEIGHT );
    Planc base_width = Random::rPlanc( TRUNK_BASE_WIDTH );
    uint branch_count = Random::rint( BRANCH_COUNT );

    Drawing trunk = draw_trunk( m_leaf_color, true, height, base_width, branch_count, m_leaf_base, m_leaf_side1, m_leaf_side2 );
    trunk.rotate( sway );
    draw( trunk );

    m_leaf_base.rotate( sway );
    m_leaf_side1.rotate( sway );
    m_leaf_side2.rotate( sway );
}

void AspineTree::update( )
{
    lambdaf( update_leaf_countdown )( Countdown ref in_countdown, bool in_side )
    {
        if( in_countdown.tick( ) )
        {
            in_countdown.reset( Random::rdec( LEAF_COUNTDOWN ) );
            room( )->add_object( new Leaf( room( ), m_leaf_color, ( Vector( m_leaf_base, ( in_side ? m_leaf_side1 : m_leaf_side2 ) ) * Random::rdec( LEAF_FALL_DISTANCE ) ) + position( ) ) );
        }
    };

    update_leaf_countdown( m_leaf_drop_countdown1, true );
    update_leaf_countdown( m_leaf_drop_countdown2, false );
}

AspineTree::Leaf::Leaf( ptr<Room> in_room, Color cref in_color, Point cref in_center, dec in_z ) : Object( in_room, in_center, in_z ), m_color( in_color )
{
    background( true );

    needs_render_always( true );

    gravity_scale( LEAF_GRAVITY );

    m_scale = Random::rdec( LEAF_SCALE );
    m_offset = Random::rAngle( );
}

void AspineTree::Leaf::render( )
{
    Object::render( );

    Planc offset_x = sin( age( ) * 0.05 ) * 5.0;
    Planc cross_length = LEAF_CROSS_LENGTH * sin( age( ) * 0.05 );

    Polygon leaf( { PointY( LEAF_TIP_LENGTH ), PointX( -half( cross_length ) ), PointY( -LEAF_BASE_LENGTH ), PointX( half( cross_length ) ) } );

    leaf.rotate( Angle( age( ) * 0.05 ) + m_offset );

    draw( m_color, leaf + VectorX( offset_x ) );
}

void AspineTree::Leaf::ground( ptr<Terrain::Node> ground, Terrain::Bumper cref in_bumper )
{
    Object::ground( ground, in_bumper );

    if( ground )
    {
        mark_to_delete( );
    }
}
