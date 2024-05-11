#include "AspenTree.hpp"

#include "World.hpp"

using mtmercy::AspenTree;

namespace
{
const Span<Planc> TRUNK_HEIGHT = { 300.0, 500.0 };
const Span<Planc> TRUNK_BASE_WIDTH = { 8.0, 12.0 };
const Angle TRUNK_SWAY_MAX = RIGHT_ANGLE / 16.0;

const Span<uint> BRANCH_COUNT = { 3 };
const Span<double> BRANCH_BASE_HEIGHT = { 0.3, 0.333 };
const Span<double> BRANCH_LENGTH = { 0.333, 0.4444 };
const Angle BRANCH_OFFSET = RIGHT_ANGLE / 2.0;

const Color TRUNK_COLOR = WHITE;
const Color TRUNK_MARK_COLOR = BLACK;
const Color LEAF_COLOR = RED;
} // namespace

Drawing draw_trunk( const bool _draw_leaves, const Planc & _length, const Planc & _base_width, const uint _branch_count );
Drawing draw_trunk( const bool _draw_leaves, const Planc & _length, const Planc & _base_width, const uint _branch_count )
{
    Drawing tree_drawing;

    Vector trunk = Vector( 0, _length );
    Coordinate top = trunk.destination( );
    Coordinate base_left( -half( _base_width ), 0 );
    Coordinate base_right( half( _base_width ), 0 );

    double branch_height = Random::rdouble( BRANCH_BASE_HEIGHT );

    if( _draw_leaves )
    {
        double leaves_top = 1.f;
        double leaves_bottom = 0.8f;

        for_range( 10 )
        {
            // todo
            Coordinate top = VectorA( RIGHT_ANGLE, _length * leaves_top + Random::rPlanc( -5.0, 5.0 ) );
            Coordinate bottom = VectorA( RIGHT_ANGLE, _length * leaves_bottom + Random::rPlanc( -5.0, 5.0 ) );

            tree_drawing.draw( LEAF_COLOR,
                               Shape( { top,
                                        bottom + VectorA( BRANCH_OFFSET, ( _length * 0.5 * ( 1.0 - leaves_bottom ) ) + Random::rPlanc( 2.0 ) ),
                                        bottom,
                                        bottom + VectorA( ( PI - BRANCH_OFFSET ), ( _length * 0.5 * ( 1.0 - leaves_bottom ) ) + Random::rPlanc( 2.0 ) ) } ) );

            leaves_top -= 0.025;
            leaves_bottom -= 0.06;
        }

        for_range( 5 )
        {
            Coordinate top = VectorA( RIGHT_ANGLE, _length * leaves_top + Random::rPlanc( -5.0, 5.0 ) );
            Coordinate bottom = VectorA( RIGHT_ANGLE, _length * leaves_bottom + Random::rPlanc( -5.0, 5.0 ) );

            tree_drawing.draw( LEAF_COLOR,
                               Shape( { top,
                                        bottom + VectorA( BRANCH_OFFSET, ( _length * 0.5 * ( 1.0 - leaves_bottom ) * ( (double)( 5 - i ) / 5.0 ) ) + Random::rPlanc( 2.0 ) ),
                                        bottom,
                                        bottom + VectorA( ( PI - BRANCH_OFFSET ), ( _length * 0.5 * ( 1.0 - leaves_bottom ) * ( (double)( 5 - i ) / 5.0 ) ) + Random::rPlanc( 2.0 ) ) } ) );

            leaves_top += 0.01;
            leaves_bottom -= 0.01;
        }
    }

    tree_drawing.draw( TRUNK_COLOR, Shape( { base_right,
                                             top,
                                             base_left } ) );

    for_range( _branch_count )
    {
        double branch_remaining = 1.0 - branch_height;

        Coordinate branch_base = Vector( trunk ).magnitude( trunk.magnitude( ) * branch_height );

        bool both = Random::rbool( 0 ); // todo
        bool left = both;
        bool right = both;
        if( !both )
        {
            if( Random::rbool( ) )
                left = true;
            else
                right = true;
        }

        if( left )
        {
            Drawing branch_drawing1 = draw_trunk( false, _length * Random::rdouble( BRANCH_LENGTH ) * branch_remaining, _base_width * branch_remaining, 0 );

            branch_drawing1.rotate( BRANCH_OFFSET );
            branch_drawing1.move( Vector( branch_base ) );
            tree_drawing.draw( branch_drawing1 );
        }

        if( right )
        {
            Drawing branch_drawing2 = draw_trunk( false, _length * Random::rdouble( BRANCH_LENGTH ) * branch_remaining, _base_width * branch_remaining, 0 );

            branch_drawing2.rotate( -BRANCH_OFFSET );
            branch_drawing2.move( Vector( branch_base ) );
            tree_drawing.draw( branch_drawing2 );
        }

        branch_height += branch_remaining * Random::rdouble( BRANCH_BASE_HEIGHT );
    }

    branch_height = 0.0;
    while( branch_height < 1.0 )
    {
        // double branch_remaining = ( 1.0 - branch_height );
        branch_height += Random::rdouble( 0.02, 0.1 );

        // tree_drawing.draw( TRUNK_MARK_COLOR, Shape::circle( half( _base_width ) * branch_remaining * 0.333, Vector( trunk ).magnitude( trunk.magnitude( ) * branch_height ) ) );
    }

    return tree_drawing;
};

AspenTree::AspenTree( World * world, const Coordinate & _root, const double _z ) : Object( world, _root )
{
    persist_drawing( true );

    stationary( true );

    z( _z );

    Angle sway = Random::rAngle( -TRUNK_SWAY_MAX, TRUNK_SWAY_MAX );

    Planc height = Random::rPlanc( TRUNK_HEIGHT );
    Planc base_width = Random::rPlanc( TRUNK_BASE_WIDTH );
    uint branch_count = Random::rint( BRANCH_COUNT );

    draw( draw_trunk( true, height, base_width, branch_count ).rotate( sway ) );
}
