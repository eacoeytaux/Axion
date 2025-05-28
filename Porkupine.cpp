#include "Porkupine.hpp"

#include "Player.hpp"

using mtmercy::Porkupine;

namespace
{

cPlanc WIDTH = 25.0;
cPlanc HEIGHT = WIDTH;

cPlanc QUILL_OFFSET = 2.5;
const Span<uint> QUILL_COUNT = { 16, 24 };
cAngle QUILL_FAR = ( TAU / 2.0 );
cAngle QUILL_CLOSE = ( TAU / 10.0 );

cColor QUILL_COLOR = YELLOW;
}

Porkupine::Porkupine( Room * room, Coordinate cref _position ) : Enemy( room, _position )
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
        draw( QUILL_COLOR, Polygon( { quill.c2( ), quill.c1( ) + offset, quill.c1( ) - offset } ) );
    }
}
