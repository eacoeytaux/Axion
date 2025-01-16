#include "Porkupine.hpp"

#include "Player.hpp"

using mtmercy::Porkupine;

namespace
{
Span<uint> QUILL_COUNT = { 16, 24 };
Angle QUILL_FAR = PI;
Angle QUILL_CLOSE = PI / 5.0;
}

Porkupine::Porkupine( Room * room, Coordinate cref _position ) : Enemy( room, _position, 10000.0 )
{
    space( FixedRectangle( 25.0, 25.0 ) );

    uint quill_count = Random::rint( QUILL_COUNT );

    Angle d_angle = ( QUILL_FAR - QUILL_CLOSE ) / quill_count;

    for_range( i, quill_count + 1 )
    {
        m_quills.insert_back( Line( ORIGIN, Vector::A( QUILL_CLOSE + ( d_angle * i ), METER ) ) );
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
        Vector offset = Vector::A( quill.angle( ) + RIGHT_ANGLE, 2.5 );
        draw( RED, Polygon( { quill.c2( ), offset, -offset } ) );
    }
}
