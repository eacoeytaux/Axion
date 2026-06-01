#include "mt/mobs/enemies/Porkupine.hpp"

#include "axn/world/Player.hpp"

var_const( Planc WIDTH = ( 25.0 ) );
var_const( Planc HEIGHT = ( WIDTH ) );

var_const( Planc QUILL_OFFSET = ( 2.5 ) );
var_const( Span<uint> QUILL_COUNT = { 16, 24 } );
var_const( Angle QUILL_FAR = ( Angle::fraction( 2.0 ) ) );
var_const( Angle QUILL_CLOSE = ( Angle::fraction( 10.0 ) ) );

var_const( Color QUILL_COLOR = ( YELLOW ) );

Porkupine::Porkupine( ptr<Room> in_room, Point cref in_position ) : Enemy( in_room, in_position )
{
    space( FixedRectangle( WIDTH, HEIGHT ) );

    uint quill_count = Random::rint( QUILL_COUNT );

    Angle d_angle = ( QUILL_FAR - QUILL_CLOSE ) / quill_count;

    for_range( i, quill_count + 1 )
    {
        m_quills.insert_back( Line( ORIGIN, VectorA( QUILL_CLOSE + ( d_angle * i ), METER ) ) );
    }
}

void Porkupine::update( )
{
    Enemy::update( );
}

void Porkupine::render( )
{
    Enemy::render( );

    for_each( quill, m_quills )
    {
        Vector offset = VectorA( quill.angle( ) + RIGHT, QUILL_OFFSET );
        draw( QUILL_COLOR, Polygon( { quill.pt2( ), quill.pt1( ) + offset, quill.pt1( ) - offset } ) );
    }
}
