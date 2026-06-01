#include "mt/environment/plants/PineTree.hpp"

var_const( Angle MAX_SWAY = ( Angle::fraction( 64.0 ) ) );
var_const( uint SWAY_PERIOD = ( 40 ) );
var_const( Planc TRUNK_HEIGHT = ( 72.0 ) );
var_const( Planc TRUNK_THICKNESS = ( 16.0 ) );
var_const( Planc SUB_TRUNK_HEIGHT_INITIAL = ( 101.0 ) );
var_const( Planc SUB_TRUNK_HEIGHT_RATIO = ( 0.75 ) );
var_const( Span<uint> LEAVES_COUNT = { 3, 3 } );
var_const( Angle LEAVES_SPAN = ( Angle::fraction( 8.0 ) ) );
var_const( Planc LEAVES_HEIGHT_BASE_LENGTH = ( 172.0 ) );
var_const( Planc LEAVES_SHRINK_RATIO = ( 0.64 ) );
var_const( Planc LEAVES_BORDER_WIDTH = ( 4.0 ) );

var_const( Span<uint> SPIKE_COUNT = { 4, 5 } );
var_const( uint SPIKE_SHRINK_RATE = ( 1 ) );
var_const( dec SPIKE_INNER_RATIO = ( 0.9 ) );
var_const( Planc SPIKE_OFFSET_INNER = ( 3.0 ) );
var_const( Planc SPIKE_OFFSET_OUTER = ( 6.0 ) );

var_const( Color TRUNK_COLOR = ( Color::rgb( 0xA56405 ) ) );
var_const( Color TRUNK_BORDER_COLOR = ( Color::rgb( 0x804800 ) ) );
var_const( Color LEAVES_COLOR = ( Color::rgb( 0x00C000 ) ) );
var_const( Color LEAVES_BORDER_COLOR = ( Color::rgb( 0x008000 ) ) );

PineTree::PineTree( ptr<Room> in_room, Point cref in_root, dec in_z ) : Object( in_room, in_root, in_z )
{
    background( true );

    needs_render_always( true );

    dec shrink_ratio = LEAVES_SHRINK_RATIO;

    Drawing ref trunk = m_leave_bunches_and_trunk.insert_back( );

    Point base;
    Vector trunk_vector( Vector( base, base + VectorA( RIGHT, TRUNK_HEIGHT ) ) );
    trunk.draw( TRUNK_BORDER_COLOR, Line( trunk_vector ), TRUNK_THICKNESS + x2( LEAVES_BORDER_WIDTH ) );
    trunk.draw( TRUNK_COLOR, Line( trunk_vector ), TRUNK_THICKNESS );

    m_leave_bunches_rotation_points.insert_back( base );

    Point last_top = trunk_vector.destination( );

    uint leaves_count = Random::rint( LEAVES_COUNT );
    for_range( i, leaves_count )
    {
        Vector sub_trunk( last_top, last_top + VectorA( RIGHT, SUB_TRUNK_HEIGHT_INITIAL * shrink_ratio ) );

        varray<Point> leaf_bunch_pts;
        leaf_bunch_pts.insert_back( ORIGIN );

        Angle leaves_angle = -half( LEAVES_SPAN ) - RIGHT;
        leaf_bunch_pts.insert_back( VectorA( leaves_angle, ( LEAVES_HEIGHT_BASE_LENGTH + Random::negated( Random::rdec( SPIKE_OFFSET_OUTER ) ) ) * shrink_ratio ) );

        uint spike_count = Random::rint( SPIKE_COUNT ) - ( SPIKE_SHRINK_RATE * i );
        Angle leaves_sub_angle = ( LEAVES_SPAN / ( spike_count + 1 ) ) / 2.0;

        do_count( spike_count )
        {
            leaves_angle += leaves_sub_angle;
            leaf_bunch_pts.insert_back( VectorA( leaves_angle, LEAVES_HEIGHT_BASE_LENGTH * SPIKE_INNER_RATIO * shrink_ratio ) + VectorA( Random::rAngle( ), SPIKE_OFFSET_INNER * shrink_ratio ) );

            leaves_angle += leaves_sub_angle;
            leaf_bunch_pts.insert_back( VectorA( leaves_angle, LEAVES_HEIGHT_BASE_LENGTH * shrink_ratio ) + VectorA( Random::rAngle( ), SPIKE_OFFSET_OUTER * shrink_ratio ) );
        }

        leaves_angle += leaves_sub_angle;
        leaf_bunch_pts.insert_back( VectorA( leaves_angle, LEAVES_HEIGHT_BASE_LENGTH * SPIKE_INNER_RATIO * shrink_ratio ) + VectorA( Random::rAngle( ), SPIKE_OFFSET_INNER * shrink_ratio ) );

        leaf_bunch_pts.insert_back( VectorA( half( LEAVES_SPAN ) - RIGHT, ( LEAVES_HEIGHT_BASE_LENGTH + Random::rdec( SPIKE_OFFSET_OUTER, -SPIKE_OFFSET_OUTER ) ) * shrink_ratio ) );

        Polygon leaf_bunch = Polygon( leaf_bunch_pts );
        leaf_bunch += last_top + sub_trunk;

        m_leave_bunches_rotation_points.insert_back( last_top );

        Drawing ref leaves = m_leave_bunches_and_trunk.insert_back( );

        Polygon leaf_border = leaf_bunch.expanded( LEAVES_BORDER_WIDTH );
        leaves.draw( LEAVES_BORDER_COLOR, leaf_border );
        leaves.draw( LEAVES_COLOR, leaf_bunch );

        last_top = ( sub_trunk * SUB_TRUNK_HEIGHT_RATIO ).destination( );
        shrink_ratio *= LEAVES_SHRINK_RATIO;
    }
}

void PineTree::render( )
{
    Object::render( );

    Angle d_sway_angle = Angle( sin( ( dec( room( )->age( ) ) / dec( SWAY_PERIOD ) ) * MAX_SWAY.radians( ) ) );
    Angle sway_angle = d_sway_angle;

    Vector offset;
    for_range( i, m_leave_bunches_and_trunk.size( ) )
    {
        Drawing drawing = m_leave_bunches_and_trunk[ i ];

        Point pre_position = m_leave_bunches_rotation_points[ i ];
        Point post_position = pre_position;
        post_position.rotate( d_sway_angle * i, i ? m_leave_bunches_rotation_points[ i - 1 ] : ORIGIN );

        Vector d_sway = post_position - pre_position;
        offset += d_sway;

        drawing.rotate( sway_angle, post_position );
        drawing.move( offset );
        draw( drawing );

        sway_angle += d_sway_angle;
    }
}
