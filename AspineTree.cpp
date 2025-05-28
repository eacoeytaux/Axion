#include "AspineTree.hpp"
#include "World.hpp"

using mtmercy::AspineTree;

namespace
{

const dec TRUNK_LEAF_OFFSET = 0.95;
const Span<Planc> TRUNK_HEIGHT = { 300.0, 500.0 };
const Span<Planc> TRUNK_BASE_WIDTH = { 8.0, 12.0 };
cAngle TRUNK_SWAY_MAX = ( TAU / 64.0 );

const Span<uint> BRANCH_COUNT = { 3, 5 };
const Span<dec> BRANCH_BASE_HEIGHT_RATIO = { 0.3, 0.333 };
const Span<dec> BRANCH_LENGTH_RATIO = { 0.333, 0.4444 };
cAngle BRANCH_OFFSET = ( TAU / 8.0 );

const uint LEAF_TOP_COUNT = 10;
const uint LEAF_BOTTOM_COUNT = 5;
const Planc LEAF_TOP_START = 1.0;
const Planc LEAF_BOTTOM_START = 0.8;
const Planc LEAF_TOP_TOP_D = -0.025;
const Planc LEAF_TOP_BOTTOM_D = -0.06;
const Planc LEAF_BOTTOM_TOP_D = 0.01;
const Planc LEAF_BOTTOM_BOTTOM_D = -0.0075;
const Planc LEAF_OFFSET_1 = 5.0;
const Planc LEAF_OFFSET_2 = 2.0;

const Planc LEAF_TIP_LENGTH = 7.5;
const Planc LEAF_CROSS_LENGTH = 7.5;
const Planc LEAF_BASE_LENGTH = 3.0;
const Span<dec> LEAF_SCALE = { 0.8, 1.1 };

const dec LEAF_GRAVITY_RATIO = 0.1;
const dec LEAF_SWAY_RATE = 0.25;
const dec LEAF_SWAY_SCALE = 5.0;
const dec LEAF_SPIN_RATE = 0.3;
const dec LEAF_ROTATE_RATE = 0.25;

const Span<uint> LEAF_COUNTDOWN = { 25, 150 };
const Span<dec> LEAF_FALL_DISTANCE = { 0.1, 0.9 };

cColor TRUNK_COLOR = WHITE;
cColor TRUNK_MARK_COLOR = BLACK;
cColorSlider LEAF_COLOR = { RED, Color::rgb( 1.0, 0.7, 0.0 ) };

} // namespace

namespace
{

Drawing draw_trunk( Color cref leaf_color, cbool _draw_leaves, Planc cref _length, Planc cref _base_width, cuint _branch_count, Coordinate & leaf_base, Coordinate & leaf_side1, Coordinate & leaf_side2 );

Drawing draw_trunk( Color cref leaf_color, cbool _draw_leaves, Planc cref _length, Planc cref _base_width, cuint _branch_count, Coordinate & leaf_base, Coordinate & leaf_side1, Coordinate & leaf_side2 )
{
    Drawing tree_drawing;

    Vector trunk = Vector( 0, _length );
    Coordinate top = trunk.destination( );
    Coordinate base_left( -half( _base_width ), 0 );
    Coordinate base_right( half( _base_width ), 0 );

    dec branch_height = Random::rdec( BRANCH_BASE_HEIGHT_RATIO );

    if( _draw_leaves )
    {
        Coordinate top;
        Coordinate bottom;
        Coordinate side1;
        Coordinate side2;

        dec leaves_top = LEAF_TOP_START;
        dec leaves_bottom = LEAF_BOTTOM_START;

        for_range( i, LEAF_TOP_COUNT )
        {
            // todo
            top = VectorA( RIGHT, ( _length * leaves_top ) + Random::negated( Random::rPlanc( LEAF_OFFSET_1 ) ) );
            bottom = VectorA( RIGHT, ( _length * leaves_bottom ) + Random::negated( Random::rPlanc( LEAF_OFFSET_1 ) ) );
            side1 = bottom + VectorA( BRANCH_OFFSET, ( half( _length ) * ( 1.0 - leaves_bottom ) ) + Random::rPlanc( LEAF_OFFSET_2 ) );
            side2 = bottom + VectorA( ( PI - BRANCH_OFFSET ), ( half( _length ) * ( 1.0 - leaves_bottom ) ) + Random::rPlanc( LEAF_OFFSET_2 ) );

            tree_drawing.draw( leaf_color,
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
            top = VectorA( RIGHT, _length * leaves_top + Random::negated( Random::rPlanc( LEAF_OFFSET_1 ) ) );
            bottom = VectorA( RIGHT, _length * leaves_bottom + Random::negated( Random::rPlanc( LEAF_OFFSET_1 ) ) );
            side1 = bottom + VectorA( BRANCH_OFFSET, ( half( _length ) * ( 1.0 - leaves_bottom ) * ( (dec)( LEAF_BOTTOM_COUNT - i ) / (dec)LEAF_BOTTOM_COUNT ) ) + Random::rPlanc( LEAF_OFFSET_2 ) );
            side2 = bottom + VectorA( ( PI - BRANCH_OFFSET ), ( half( _length ) * ( 1.0 - leaves_bottom ) * ( (dec)( LEAF_BOTTOM_COUNT - i ) / (dec)LEAF_BOTTOM_COUNT ) ) + Random::rPlanc( LEAF_OFFSET_2 ) );

            tree_drawing.draw( leaf_color,
                               Polygon( { top,
                                          side1,
                                          bottom,
                                          side2 } ) );

            leaves_top += LEAF_BOTTOM_TOP_D;
            leaves_bottom += LEAF_BOTTOM_BOTTOM_D;
        }
    }

    Polygon trunk_base = Polygon::circle( half( _base_width ) );
    Polygon trunk_polygon( { base_right, Vector( top ) * TRUNK_LEAF_OFFSET, base_left } );

    tree_drawing.draw( TRUNK_COLOR, trunk_base );
    tree_drawing.draw( TRUNK_COLOR, trunk_polygon );

    do_count( _branch_count )
    {
        dec branch_remaining = 1.0 - branch_height;

        Coordinate branch_base = Vector( trunk ).magnitude( trunk.magnitude( ) * branch_height );

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

        auto draw_branch = [ & ] ( bool left )
        {
            Drawing branch_drawing = draw_trunk( leaf_color, false, _length * Random::rdec( BRANCH_LENGTH_RATIO ) * branch_remaining, _base_width * branch_remaining, 0, leaf_base, leaf_side1, leaf_side2 );

            branch_drawing.rotate( left ? BRANCH_OFFSET : -BRANCH_OFFSET );
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

    tree_drawing.bind( trunk_polygon );

    branch_height = 0.0;
    while( branch_height < 1.0 )
    {
        // dec branch_remaining = ( 1.0 - branch_height );
        branch_height += Random::rdec( 0.02, 0.1 );

        tree_drawing.draw( TRUNK_MARK_COLOR.a( Random::rdec( 0.25, 0.75 ) ), Polygon::equilateral( 4, Random::rPlanc( 0.1, 0.3 ) * _base_width, Vector( trunk ).magnitude( trunk.magnitude( ) * branch_height ) + VectorA( trunk.angle( ) + Random::negated( Random::rAngle( RIGHT ) ), Random::rPlanc( _base_width ) ), Random::negated( Random::rAngle( TAU / 16.0 ) ) ) );
    }

    tree_drawing.clear_bounds( );

    return tree_drawing;

};

} // namespace

AspineTree::AspineTree( Room * room, Coordinate cref _root, cdec _z ) : Object( room, _root, _z )
{
    background( true );

    persist_render( true );

    stationary( true );

    m_leaf_drop_countdown1.reset( Random::rint( LEAF_COUNTDOWN.max( ) ) );
    m_leaf_drop_countdown2.reset( Random::rint( LEAF_COUNTDOWN.max( ) ) );

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
    auto update_leaf_countdown = [ & ] ( Countdown & countdown, bool side )
    {
        if( countdown.tick( ) )
        {
            countdown.reset( Random::rint( LEAF_COUNTDOWN ) );
            room( )->add_object( new Leaf( room( ), m_leaf_color, ( Vector( m_leaf_base, ( side ? m_leaf_side1 : m_leaf_side2 ) ) * Random::rdec( LEAF_FALL_DISTANCE ) ) + position( ) ) );
        }
    };

    update_leaf_countdown( m_leaf_drop_countdown1, true );
    update_leaf_countdown( m_leaf_drop_countdown2, false );
}

AspineTree::Leaf::Leaf( Room * room, Color cref color, Coordinate cref _center, dec _z ) : Object( room, _center, _z ), m_color( color )
{
    background( true );

    needs_render_always( true );

    gravity_ratio( LEAF_GRAVITY_RATIO );
    
    m_scale = Random::rdec( LEAF_SCALE );
    m_offset = Random::rAngle( );
}

void AspineTree::Leaf::render( )
{
    Object::render( );

    Planc offset_x = sin( age( ) * LEAF_SWAY_RATE ) * LEAF_SWAY_SCALE;
    Planc cross_length = LEAF_CROSS_LENGTH * sin( age( ) * LEAF_SPIN_RATE );

    Polygon leaf( { CoordinateY( LEAF_TIP_LENGTH ), CoordinateX( -half( cross_length ) ), CoordinateY( -LEAF_BASE_LENGTH ), CoordinateX( half( cross_length ) ) } );
        
    leaf.scale( m_scale );
    leaf.rotate( Angle( age( ) * LEAF_ROTATE_RATE ) + m_offset );

    draw( m_color, leaf + VectorX( offset_x ) );
}

void AspineTree::Leaf::ground( TerrainEdge * terrain )
{
    Object::ground( terrain );

    if( terrain )
    {
        mark_to_delete( );
    }
}
