#include "PineTree.hpp"
#include "World.hpp"

using mtmercy::PineTree;

namespace
{
const Angle MAX_SWAY = TAU / 64.0;
cuint SWAY_PERIOD = 40;
cPlanc TRUNK_HEIGHT = 72.0;
cPlanc TRUNK_THICKNESS = 16.0;
cPlanc SUB_TRUNK_HEIGHT_INITIAL = 101.0;
cPlanc SUB_TRUNK_HEIGHT_RATIO = 0.75;
const Span<uint> LEAVES_COUNT = { 3, 3 };
const Angle LEAVES_SPAN = TAU / 8.0;
cPlanc LEAVES_HEIGHT_BASE_LENGTH = 172.0;
cPlanc LEAVES_SHRINK_RATIO = 0.64;
cPlanc LEAVES_BORDER_WIDTH = 4.0;

const Span<uint> SPIKE_COUNT = { 4, 5 };
cuint SPIKE_SHRINK_RATE = 1;
cdec SPIKE_INNER_RATIO = 0.9;
cPlanc SPIKE_OFFSET_INNER = 3.0;
cPlanc SPIKE_OFFSET_OUTER = 6.0;

const Color TRUNK_COLOR = Color::rgb( 0xA56405 );
const Color TRUNK_BORDER_COLOR = Color::rgb( 0x804800 );
const Color LEAVES_COLOR = Color::rgb( 0x00C000 );
const Color LEAVES_BORDER_COLOR = Color::rgb( 0x008000 );
} // namespace

PineTree::PineTree( Room * room, Coordinate cref _root, cdec _z ) : Object( room, _root )
{
    background( true );

    needs_render_always( true );

    z( _z );

    dec shrink_ratio = LEAVES_SHRINK_RATIO;

    Drawing & trunk = m_leave_bunches_and_trunk.insert_back( );

    Coordinate base;
    Vector trunk_vector( Vector( base, base + Vector::A( RIGHT_ANGLE_1, TRUNK_HEIGHT ) ) );
    trunk.draw( TRUNK_BORDER_COLOR, Line( trunk_vector ), TRUNK_THICKNESS + LEAVES_BORDER_WIDTH * 2.0 );
    trunk.draw( TRUNK_COLOR, Line( trunk_vector ), TRUNK_THICKNESS );

    m_leave_bunches_rotation_points.insert_back( base );

    Coordinate last_top = trunk_vector.destination( );

    uint leaves_count = Random::rint( LEAVES_COUNT );
    for_range( i, leaves_count )
    {
        Vector sub_trunk( last_top, last_top + Vector::A( RIGHT_ANGLE_1, SUB_TRUNK_HEIGHT_INITIAL * shrink_ratio ) );

        varray<Coordinate> leaf_bunch_coords;
        leaf_bunch_coords.insert_back( ORIGIN );

        Angle leaves_angle = -half( LEAVES_SPAN ) + RIGHT_ANGLE_3;
        leaf_bunch_coords.insert_back( Vector::A( leaves_angle, ( LEAVES_HEIGHT_BASE_LENGTH + Random::rdec( -SPIKE_OFFSET_OUTER, SPIKE_OFFSET_OUTER ) ) * shrink_ratio ) );

        uint spike_count = Random::rint( SPIKE_COUNT ) - ( SPIKE_SHRINK_RATE * i );
        Angle leaves_sub_angle = ( LEAVES_SPAN / ( spike_count + 1 ) ) / 2.0;

        do_count( spike_count )
        {
            leaves_angle += leaves_sub_angle;
            leaf_bunch_coords.insert_back( Vector::A( leaves_angle, LEAVES_HEIGHT_BASE_LENGTH * SPIKE_INNER_RATIO * shrink_ratio ) + Vector::A( Random::rAngle( ), SPIKE_OFFSET_INNER * shrink_ratio ) );

            leaves_angle += leaves_sub_angle;
            leaf_bunch_coords.insert_back( Vector::A( leaves_angle, LEAVES_HEIGHT_BASE_LENGTH * shrink_ratio ) + Vector::A( Random::rAngle( ), SPIKE_OFFSET_OUTER * shrink_ratio ) );
        }

        leaves_angle += leaves_sub_angle;
        leaf_bunch_coords.insert_back( Vector::A( leaves_angle, LEAVES_HEIGHT_BASE_LENGTH * SPIKE_INNER_RATIO * shrink_ratio ) + Vector::A( Random::rAngle( ), SPIKE_OFFSET_INNER * shrink_ratio ) );

        leaf_bunch_coords.insert_back( Vector::A( half( LEAVES_SPAN ) + RIGHT_ANGLE_3, ( LEAVES_HEIGHT_BASE_LENGTH + Random::rdec( SPIKE_OFFSET_OUTER, -SPIKE_OFFSET_OUTER ) ) * shrink_ratio ) );

        Polygon leaf_bunch = Polygon( leaf_bunch_coords );
        leaf_bunch += last_top + sub_trunk;

        m_leave_bunches_rotation_points.insert_back( last_top );

        Drawing & leaves = m_leave_bunches_and_trunk.insert_back( );
        {
            // todo change to border
            Polygon leaf_border = Polygon::expand( Polygon( leaf_bunch ), LEAVES_BORDER_WIDTH );
            leaves.draw( LEAVES_BORDER_COLOR, leaf_border );
        }
        leaves.draw( LEAVES_COLOR, leaf_bunch );

        last_top = ( sub_trunk * SUB_TRUNK_HEIGHT_RATIO ).destination( );
        shrink_ratio *= LEAVES_SHRINK_RATIO;
    }
}

void PineTree::render( )
{
    Object::render( );

    Angle d_sway_angle = Angle( sin( ( (dec)room( )->age( ) / (dec)SWAY_PERIOD ) * MAX_SWAY.radians( ) ) );
    Angle sway_angle = d_sway_angle;

    Vector offset;
    for_range( i, m_leave_bunches_and_trunk.size( ) )
    {
        Drawing drawing = m_leave_bunches_and_trunk[ i ];

        Coordinate pre_position = m_leave_bunches_rotation_points[ i ];
        Coordinate post_position = pre_position;
        post_position.rotate( d_sway_angle * (dec)i, i ? m_leave_bunches_rotation_points[ i - 1 ] : ORIGIN );

        Vector d_sway = post_position - pre_position;
        offset += d_sway;

        drawing.rotate( sway_angle, post_position );
        drawing.move( offset );
        draw( drawing );

        sway_angle += d_sway_angle;
    }
}
