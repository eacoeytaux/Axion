#include "AspineTree.hpp"
#include "World.hpp"

using mtmercy::AspineTree;

namespace
{
const Span<Planc> TRUNK_HEIGHT = { 300.0, 500.0 };
const Span<Planc> TRUNK_BASE_WIDTH = { 8.0, 12.0 };
const Angle TRUNK_SWAY_MAX = RIGHT_ANGLE / 16.0;

const Span<uint> BRANCH_COUNT = { 3, 5 };
const Span<dec> BRANCH_BASE_HEIGHT = { 0.3, 0.333 };
const Span<dec> BRANCH_LENGTH = { 0.333, 0.4444 };
const Angle BRANCH_OFFSET = RIGHT_ANGLE / 2.0;

const Color TRUNK_COLOR = WHITE;
const Color TRUNK_MARK_COLOR = BLACK;
const Color LEAF_COLOR = RED;
} // namespace

namespace
{
Drawing draw_trunk( const bool _draw_leaves, const Planc & _length, const Planc & _base_width, const uint _branch_count );
Drawing draw_trunk( const bool _draw_leaves, const Planc & _length, const Planc & _base_width, const uint _branch_count )
{
    Drawing tree_drawing;

    Vector trunk = Vector( 0, _length );
    Coordinate top = trunk.destination( );
    Coordinate base_left( -half( _base_width ), 0 );
    Coordinate base_right( half( _base_width ), 0 );

    dec branch_height = Random::rdec( BRANCH_BASE_HEIGHT );

    if( _draw_leaves )
    {
        dec leaves_top = 1.0;
        dec leaves_bottom = 0.8;

        for_range( i, 10 )
        {
            // todo
            Coordinate top = Vector::A( RIGHT_ANGLE, _length * leaves_top + Random::rPlanc( -5.0, 5.0 ) );
            Coordinate bottom = Vector::A( RIGHT_ANGLE, _length * leaves_bottom + Random::rPlanc( -5.0, 5.0 ) );

            tree_drawing.draw( LEAF_COLOR,
                               Polygon( { top,
                                          bottom + Vector::A( BRANCH_OFFSET, ( _length * 0.5 * ( 1.0 - leaves_bottom ) ) + Random::rPlanc( 2.0 ) ),
                                          bottom,
                                          bottom + Vector::A( ( PI - BRANCH_OFFSET ), ( _length * 0.5 * ( 1.0 - leaves_bottom ) ) + Random::rPlanc( 2.0 ) ) } ) );

            leaves_top -= 0.025;
            leaves_bottom -= 0.06;
        }

        for_range( i, 5 )
        {
            Coordinate top = Vector::A( RIGHT_ANGLE, _length * leaves_top + Random::rPlanc( -5.0, 5.0 ) );
            Coordinate bottom = Vector::A( RIGHT_ANGLE, _length * leaves_bottom + Random::rPlanc( -5.0, 5.0 ) );

            tree_drawing.draw( LEAF_COLOR,
                               Polygon( { top,
                                          bottom + Vector::A( BRANCH_OFFSET, ( _length * 0.5 * ( 1.0 - leaves_bottom ) * ( (dec)( 5 - i ) / 5.0 ) ) + Random::rPlanc( 2.0 ) ),
                                          bottom,
                                          bottom + Vector::A( ( PI - BRANCH_OFFSET ), ( _length * 0.5 * ( 1.0 - leaves_bottom ) * ( (dec)( 5 - i ) / 5.0 ) ) + Random::rPlanc( 2.0 ) ) } ) );

            leaves_top += 0.01;
            leaves_bottom -= 0.01;
        }
    }

    Polygon trunk_base = Polygon::circle( half( _base_width ) );
    Polygon trunk_polygon( { base_right, top, base_left } );

    tree_drawing.draw( TRUNK_COLOR, trunk_base );
    tree_drawing.draw( TRUNK_COLOR, trunk_polygon );

    do_count( _branch_count )
    {
        dec branch_remaining = 1.0 - branch_height;

        Coordinate branch_base = Vector( trunk ).magnitude( trunk.magnitude( ) * branch_height );

        bool left;
        bool right;
        bool both = Random::rbool( 0 ); // todo
        if( both )
        {
            left = true;
            right = true;
        }
        else
        {
            left = Random::rbool( );
            right = !left;
        }

        auto draw_branch = [ & ] ( bool left )
        {
            Drawing branch_drawing = draw_trunk( false, _length * Random::rdec( BRANCH_LENGTH ) * branch_remaining, _base_width * branch_remaining, 0 );

            branch_drawing.rotate( left ? BRANCH_OFFSET : -BRANCH_OFFSET );
            branch_drawing.move( Vector( branch_base ) );
            tree_drawing.draw( branch_drawing );
        };

        if( left )
        {
            draw_branch( true );
        }

        if( right )
        {
            draw_branch( false );
        }

        branch_height += branch_remaining * Random::rdec( BRANCH_BASE_HEIGHT );
    }

    tree_drawing.add_bound( trunk_polygon );

    branch_height = 0.0;
    while( branch_height < 1.0 )
    {
        dec branch_remaining = ( 1.0 - branch_height );
        branch_height += Random::rdec( 0.02, 0.1 );

        tree_drawing.draw( TRUNK_MARK_COLOR.a( Random::rdec( 0.25, 0.75 ) ), Polygon::equilateral( 4, Random::rPlanc( 0.1, 0.3 ) * _base_width, Vector( trunk ).magnitude( trunk.magnitude( ) * branch_height ) + Vector::A( trunk.angle( ) + Random::rAngle( -RIGHT_ANGLE, RIGHT_ANGLE ), Random::rPlanc( _base_width ) ), Random::rAngle( -TAU / 16.0, TAU / 16.0 ) ) );
    }

    tree_drawing.clear_bounds( );

    return tree_drawing;
};
} // namespace

AspineTree::AspineTree( World * world, const Coordinate & _root, const dec _z ) : Object( world, _root )
{
    background( true );

    persist_render( true );

    stationary( true );

    z( _z );

    Angle sway = Random::rAngle( -TRUNK_SWAY_MAX, TRUNK_SWAY_MAX );

    Planc height = Random::rPlanc( TRUNK_HEIGHT );
    Planc base_width = Random::rPlanc( TRUNK_BASE_WIDTH );
    uint branch_count = Random::rint( BRANCH_COUNT );

    Drawing trunk = draw_trunk( true, height, base_width, branch_count );
    trunk.rotate( sway );
    draw( trunk );
}
