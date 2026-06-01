#include "Sign.hpp"

#include "Player.hpp"

using mtmercy::Sign;

namespace
{

cPlanc POST_HEIGHT = ( METER * 1.5 );
cPlanc POST_THICKNESS = ( 5.0 );

cPlanc POST_GROUND_EXTENSION = ( POST_HEIGHT / 8.0 );
cPlanc POST_GROUND_SPIKE_LENGTH = ( POST_GROUND_EXTENSION );

cAngle MAX_POST_SWAY = ( RIGHT / 16.0 );

cPlanc BOARD_WIDTH = ( POST_HEIGHT * 0.8 );
cPlanc BOARD_HEIGHT = ( BOARD_WIDTH * 0.75 );

cPlanc BOARD_OFFSET = ( POST_HEIGHT / 16.0 );

cPlanc INK_THICKNESS = ( 0.5 );
cPlanc INK_LETTER_SPACING = ( INK_THICKNESS * 3.0 );
cPlanc INK_LETTER_HEIGHT = ( INK_LETTER_SPACING * 2.0 );
cPlanc INK_ROW_HEIGHT = ( INK_LETTER_HEIGHT + ( INK_LETTER_SPACING * 0.5 ) );
cPlanc INK_LETTER_SCATTER = ( INK_LETTER_SPACING * 1.2 );

const Span<uint> INK_GAP = { 0, 6 };

cdec INK_OFFSET = ( 0.4 );

cAngle MAX_INK_SWAY = ( RIGHT / 8.0 );

cColor INK_COLOR = ( BLACK );
cColor WOOD_COLOR = ( mtmercy::Colors::WOOD_COLOR_1 );

cPlanc MESSAGE_WIDTH = ( 100.0 );
cPlanc MESSAGE_HEIGHT = ( 50.0 );

cPlanc MESSAGE_OFFSET = ( POST_HEIGHT * 0.75 );

cPlanc MESSAGE_TIP_WIDTH = ( POST_THICKNESS * 3.0 );
cPlanc MESSAGE_TIP_HEIGHT = ( MESSAGE_TIP_WIDTH * 0.75 );

cPlanc BORDER_THICKNESS = ( 4.0 );

cColor MESSAGE_BOX_COLOR = ( BLACK.a( 0.25 ) );
cColor MESSAGE_BORDER_COLOR = ( WHITE.a( 0.25 ) );

} // namespace

Sign::Sign( Room * room, Coordinate cref _base ) : Object( room, _base )
{
    #if defined ( AXN_DEBUG )
    draw_debug = true;
    #endif

    interactive( true );

    stationary( true );

    needs_render_always( true );

    Angle post_sway = ( Random::rAngle( -MAX_POST_SWAY, MAX_POST_SWAY ) );
    Angle post_angle = ( gravity( ).angle( ).flipped( ) + post_sway );

    Coordinate sign_center = ( _base + VectorA( post_angle, ( POST_HEIGHT - BOARD_OFFSET - half( BOARD_HEIGHT ) ) ) );

    position( sign_center );

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

            auto text_drawing = [ & ] ( Coordinate cref start, Coordinate cref end )
            {
                Drawing text;

                uint letter_count = ( start.distance_to( end ) / INK_LETTER_SPACING );

                Vector letter_spacing = Vector( start, end );
                letter_spacing.magnitude( letter_spacing.magnitude( ) / (dec)letter_count );

                Coordinate letter_prev = start + VectorA( Random::rAngle( ), Random::rPlanc( INK_LETTER_SCATTER ) );

                uint gap = Random::rint( INK_GAP );

                uint gap_start = Random::rint( gap );
                uint gap_end = gap - gap_start;

                for_range( i, letter_count )
                {
                    break_if( i > ( letter_count - gap_end ) );

                    Coordinate letter_next = ( letter_spacing * ( i + 1 ) ) + VectorY( half( Random::rPlanc( -INK_LETTER_HEIGHT, INK_LETTER_HEIGHT ) ) ) + VectorA( Random::rAngle( ), Random::rPlanc( INK_LETTER_SCATTER ) );

                    if( i >= gap_start )
                    {
                        text.draw( INK_COLOR, Line( letter_prev, letter_next ), INK_THICKNESS );
                    }

                    letter_prev = letter_next;
                }

                return text;
            };


            uint row_count = max( 1, (int)( ( ( BOARD_HEIGHT * ( 1.0 - INK_OFFSET ) ) / INK_ROW_HEIGHT ) - ( Random::rbool( ) ? 1 : 0 ) ) );

            Coordinate row_center = sign_center - ( VectorA( post_angle, INK_ROW_HEIGHT ) * half( row_count ) );

            for_range( i, row_count )
            {
                m_sign_drawing.draw( text_drawing( row_center + VectorA( post_angle + RIGHT, half( BOARD_WIDTH ) * ( 1.0 - INK_OFFSET ) ),
                                                   row_center + VectorA( post_angle - RIGHT, half( BOARD_WIDTH ) * ( 1.0 - INK_OFFSET ) ) ).rotate( Random::rAngle( -MAX_INK_SWAY, MAX_INK_SWAY ), row_center ) );

                row_center += VectorA( post_angle, INK_ROW_HEIGHT );
            }
        }

        m_sign_drawing.move( -sign_center );
    }

    { // message

        m_sign_message.draw( MESSAGE_BORDER_COLOR, Polygon::rectangle( MESSAGE_WIDTH, MESSAGE_HEIGHT, VectorY( half( MESSAGE_HEIGHT ) + MESSAGE_OFFSET + MESSAGE_TIP_HEIGHT ) ) );
        m_sign_message.draw( MESSAGE_BORDER_COLOR, Polygon::isosceles( VectorY( MESSAGE_OFFSET ), VectorY( MESSAGE_OFFSET + MESSAGE_TIP_HEIGHT ), MESSAGE_TIP_WIDTH ) );

        m_sign_message.draw( MESSAGE_BOX_COLOR, Polygon::rectangle( MESSAGE_WIDTH - ( BORDER_THICKNESS * 2.0 ), MESSAGE_HEIGHT - ( BORDER_THICKNESS * 2.0 ), VectorY( half( MESSAGE_HEIGHT ) + MESSAGE_OFFSET + MESSAGE_TIP_HEIGHT ) ) );
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

bool Sign::collide( Object * object )
{
    if( dynamic_cast<Player *>( object ) )
    {
        m_reading = true;
    }

    return false;
}
