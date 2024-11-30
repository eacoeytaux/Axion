#include "Porcupine.hpp"

#include "Player.hpp"

using mtmercy::Porcupine;

namespace
{
    Span<uint> QUILL_COUNT = { 16, 24 };
    Angle QUILL_FAR = PI;
    Angle QUILL_CLOSE = PI / 5.0;
}

Porcupine::Porcupine( World * world, const Coordinate & _position ) : Enemy( world, _position, 10000.0 )
{
    uint quill_count = Random::rint( QUILL_COUNT );
 
    Angle d_angle = ( QUILL_FAR - QUILL_CLOSE ) / quill_count;
    
    for_range( i, quill_count + ONE )
    {
        m_quills.insert_back( Line( ORIGIN, VectorA( QUILL_CLOSE + ( d_angle * i ), METER ) ) );
    }
}

void Porcupine::update( )
{
    Enemy::update( );
}

void Porcupine::render( )
{
    Enemy::render( );

    for_each( quill, m_quills )
    {
        VectorA offset( quill.angle( ) + RIGHT_ANGLE, 2.5 );
        draw( RED, Polygon( { quill.c2( ), offset, -offset } ) );
    }
}
