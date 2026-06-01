#include "mt/environment/misc/Sign.hpp"

#include "axn/world/Player.hpp"

var_const( Planc POST_HEIGHT = ( METER * 1.5 ) );
var_const( Planc POST_THICKNESS = ( 5.0 ) );

var_const( Planc POST_GROUND_EXTENSION = ( POST_HEIGHT / 8.0 ) );
var_const( Planc POST_GROUND_SPIKE_LENGTH = ( POST_GROUND_EXTENSION ) );

var_const( Angle MAX_POST_SWAY = ( Angle::fraction( 64.0 ) ) );

var_const( Planc BOARD_WIDTH = ( POST_HEIGHT * 0.8 ) );
var_const( Planc BOARD_HEIGHT = ( BOARD_WIDTH * 0.75 ) );

var_const( Planc BOARD_OFFSET = ( POST_HEIGHT / 16.0 ) );

var_const( Planc INK_THICKNESS = ( 0.5 ) );
var_const( Planc INK_LETTER_SPACING = ( INK_THICKNESS * 3.0 ) );
var_const( Planc INK_LETTER_HEIGHT = ( INK_LETTER_SPACING * 2.0 ) );
var_const( Planc INK_ROW_HEIGHT = ( INK_LETTER_HEIGHT + ( INK_LETTER_SPACING * 0.5 ) ) );
var_const( Planc INK_LETTER_SCATTER = ( INK_LETTER_SPACING * 1.2 ) );

var_const( Span<uint> INK_GAP = { 0, 6 } );

var_const( dec INK_OFFSET = ( 0.4 ) );

var_const( Angle MAX_INK_SWAY = ( Angle::fraction( 32.0 ) ) );

var_const( Color INK_COLOR = ( BLACK ) );
var_const( Color WOOD_COLOR = ( mtmercy::WOOD_COLOR_1 ) );

var_const( Planc MESSAGE_WIDTH = ( 100.0 ) );
var_const( Planc MESSAGE_HEIGHT = ( 50.0 ) );

var_const( Planc MESSAGE_OFFSET = ( POST_HEIGHT * 0.75 ) );

var_const( Planc MESSAGE_TIP_WIDTH = ( POST_THICKNESS * 3.0 ) );
var_const( Planc MESSAGE_TIP_HEIGHT = ( MESSAGE_TIP_WIDTH * 0.75 ) );

var_const( Planc BORDER_THICKNESS = ( 4.0 ) );

var_const( Color MESSAGE_BOX_COLOR = ( BLACK.a( 0.25 ) ) );
var_const( Color MESSAGE_BORDER_COLOR = ( WHITE.a( 0.25 ) ) );

Sign::Sign( ptr<Room> in_room, Point cref in_base ) : Object( in_room, in_base )
{
    #if defined( AXN_DEBUG )
    draw_debug = true;
    #endif

    interactive( true );

    stationary( true );

    needs_render_always( true );

    Angle post_sway = ( Random::rAngle( -MAX_POST_SWAY, MAX_POST_SWAY ) );
    Angle post_angle = ( gravity( ).angle( ).flipped( ) + post_sway );

    Point sign_center = ( VectorA( post_angle, ( POST_HEIGHT - BOARD_OFFSET - half( BOARD_HEIGHT ) ) ) );

    position( in_base + sign_center );

    space( Polygon::square( min( BOARD_WIDTH, BOARD_HEIGHT ) ) );

    {
        { // post

            m_sign_drawing.draw( WOOD_COLOR, Line( ( VectorA( post_angle.flipped( ), POST_GROUND_EXTENSION ) ),
                                                   ( VectorA( post_angle, POST_HEIGHT ) ) ), POST_THICKNESS );

            m_sign_drawing.draw( WOOD_COLOR, Polygon::isosceles( ( VectorA( post_angle.flipped( ), ( POST_GROUND_EXTENSION + POST_GROUND_SPIKE_LENGTH ) ) ),
                                                                 ( VectorA( post_angle.flipped( ), POST_GROUND_EXTENSION ) ), POST_THICKNESS ) );
        }

        { // sign board

            m_sign_drawing.draw( WOOD_COLOR, Polygon::rectangle( BOARD_WIDTH, BOARD_HEIGHT, sign_center ).rotate( post_sway, sign_center ) );

            lambdaf( text_drawing )( Point cref in_start, Point cref in_end )
            {
                Drawing text;

                uint letter_count = ( in_start.distance_to( in_end ) / INK_LETTER_SPACING );

                Vector letter_spacing = Vector( in_start, in_end );
                letter_spacing.magnitude( letter_spacing.magnitude( ) / dec( letter_count ) );

                Point letter_prev = in_start + VectorA( Random::rAngle( ), Random::rPlanc( INK_LETTER_SCATTER ) );

                uint gap = Random::rint( INK_GAP );

                uint gap_start = Random::rint( gap );
                uint gap_end = gap - gap_start;

                for_range( i, letter_count )
                {
                    break_if( i > ( letter_count - gap_end ) );

                    Point letter_next = ( letter_spacing * ( i + 1 ) ) + VectorY( half( Random::rPlanc( -INK_LETTER_HEIGHT, INK_LETTER_HEIGHT ) ) ) + VectorA( Random::rAngle( ), Random::rPlanc( INK_LETTER_SCATTER ) );

                    if( i >= gap_start )
                    {
                        text.draw( INK_COLOR, Line( letter_prev, letter_next ), INK_THICKNESS );
                    }

                    letter_prev = letter_next;
                }

                return( text );
            };


            uint row_count = max( 1, int( ( ( BOARD_HEIGHT * one_minus( INK_OFFSET ) ) / INK_ROW_HEIGHT ) - ( Random::rbool( ) ? 1 : 0 ) ) );

            Point row_center = sign_center - ( VectorA( post_angle, INK_ROW_HEIGHT ) * half( row_count ) );

            for_range( i, row_count )
            {
                m_sign_drawing.draw( text_drawing( row_center + VectorA( post_angle + RIGHT, half( BOARD_WIDTH ) * one_minus( INK_OFFSET ) ),
                                                   row_center + VectorA( post_angle - RIGHT, half( BOARD_WIDTH ) * one_minus( INK_OFFSET ) ) ).rotate( Random::rAngle( -MAX_INK_SWAY, MAX_INK_SWAY ), row_center ) );

                row_center += VectorA( post_angle, INK_ROW_HEIGHT );
            }
        }

        m_sign_drawing.move( -sign_center );
    }

    { // message

        m_sign_message.draw( MESSAGE_BORDER_COLOR, Polygon::rectangle( MESSAGE_WIDTH, MESSAGE_HEIGHT, VectorY( half( MESSAGE_HEIGHT ) + MESSAGE_OFFSET + MESSAGE_TIP_HEIGHT ) ) );
        m_sign_message.draw( MESSAGE_BORDER_COLOR, Polygon::isosceles( VectorY( MESSAGE_OFFSET ), VectorY( MESSAGE_OFFSET + MESSAGE_TIP_HEIGHT ), MESSAGE_TIP_WIDTH ) );

        m_sign_message.draw( MESSAGE_BOX_COLOR, Polygon::rectangle( MESSAGE_WIDTH - x2( BORDER_THICKNESS ), MESSAGE_HEIGHT - x2( BORDER_THICKNESS ), VectorY( half( MESSAGE_HEIGHT ) + MESSAGE_OFFSET + MESSAGE_TIP_HEIGHT ) ) );
        m_sign_message.draw( MESSAGE_BOX_COLOR, Polygon::isosceles( VectorY( MESSAGE_OFFSET + ( BORDER_THICKNESS * sqrt( 2.0 ) ) ), VectorY( MESSAGE_OFFSET + BORDER_THICKNESS + MESSAGE_TIP_HEIGHT ), ( MESSAGE_TIP_WIDTH - BORDER_THICKNESS ) ) );
    }
}

void Sign::render( )
{
    Object::render( );

    draw( m_sign_drawing );

    if( m_reading )
    {
        // draw( m_sign_message );
    }
}

bool Sign::collide( ptr<Object> in_object )
{
    if( cast( Player, in_object ) )
    {
        m_reading = true;
    }

    return( false );
}
