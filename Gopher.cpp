#include "Gopher.hpp"

#include "Player.hpp"

using mtmercy::Gopher;
using mtmercy::DirtBall;

namespace
{
    const Planc SIGHT_RANGE = METER * 10.0;
    const Planc ALERT_RANGE = SIGHT_RANGE * 0.75;

    const uint RELOAD_TIME = 100;

    const Planc HEAD_RADIUS = 12.0;
    const Planc HEAD_X_STRETCH = 1.1;

    const Planc BODY_WIDTH = HEAD_RADIUS * 2.0 * 0.8;
    const Planc BODY_HEIGHT = HEAD_RADIUS * 2.0;

    const Planc INNER_FUR_WIDTH = BODY_WIDTH * 0.8;
    const Planc INNER_FUR_HEIGHT = BODY_HEIGHT * 0.8;

    const Planc CHEEK_RADIUS = HEAD_RADIUS * 0.3;

    const Planc NOSE_RADIUS = HEAD_RADIUS * 0.25;
    const Vector NOSE_OFFSET = VectorX( CHEEK_RADIUS );

    const Planc TEETH_WIDTH = NOSE_RADIUS * 0.9;
    const Planc TEETH_LENGTH = HEAD_RADIUS * 0.64;

    const Planc EAR_RADIUS = HEAD_RADIUS * 0.3;
    const Planc EAR_INNER_RADIUS = EAR_RADIUS * 0.5;
    const Planc EAR_PLACEMENT_DISTANCE = HEAD_RADIUS;
    const Angle EAR_ANGLE_FRONT = RIGHT_ANGLE * 1.5;
    const Angle EAR_ANGLE_BACK = RIGHT_ANGLE * 0.5;

    const Planc EYE_SPACING_UP = NOSE_RADIUS * 1.25;
    const Planc EYE_SPACING_SIDE = NOSE_RADIUS * 1.75;
    const Planc EYE_RADIUS = 1.0;
    const uint BLINK_DURATION = 4;
    const Span<uint> BLINK_WAIT = { 360, 480 };

    const Color EYE_COLOR = BLACK;
    const Color TEETH_COLOR = WHITE;
    const Color NOSE_COLOR = Color::rgb( 0x654321 );
    const Color FUR_COLOR1 = Color::rgb( 0xA0522D );
    const Color FUR_COLOR2 = Color::rgb( 0xF4A460 );
}

Gopher::Gopher( World * world, const Coordinate & _position ) : Enemy( world, _position, 10000.0 )
{
    space( Rectangle( 16.0, 16.0 ) );

    eye_info( EYE_RADIUS, BLINK_DURATION, BLINK_WAIT, EYE_COLOR );

    sight_range( SIGHT_RANGE );
    alert_range( ALERT_RANGE );

    state( EXPOSED );
}

void Gopher::update( )
{
    Enemy::update( );

    m_reload.tick( );

    if ( state( ) == EXPOSED )
    {
        if ( has_target( ) )
        {
            facing_left( target( )->position( ).x( ) < position( ).x( ) );

            if ( !m_reload.remaining( ) )
            {
                world( )->add_object( new DirtBall( world( ), 5.0, position( ), VectorA( Angle( position( ), target( )->position( ) ), 5.0 ) ) );
                m_reload.reset( RELOAD_TIME );
            }
        }
    }
}

void Gopher::render( )
{
    Enemy::render( );

    if ( state( ) == EXPOSED )
    {
        draw( FUR_COLOR1, Rectangle( BODY_WIDTH, BODY_HEIGHT ) );

        Polygon inner_fur = Circle( half( INNER_FUR_WIDTH ) ).stretch( Vector( ZERO, INNER_FUR_HEIGHT / INNER_FUR_WIDTH ) );
        draw( FUR_COLOR2, inner_fur );

        Vector head_offset( ZERO, half( BODY_HEIGHT ) );
        Drawing head_drawing;

        head_drawing.draw( FUR_COLOR1, Circle( EAR_RADIUS, VectorA( EAR_ANGLE_BACK, EAR_PLACEMENT_DISTANCE ) ) );
        head_drawing.draw( FUR_COLOR2, Circle( EAR_INNER_RADIUS, VectorA( EAR_ANGLE_BACK, EAR_PLACEMENT_DISTANCE ) ) );

        head_drawing.draw( FUR_COLOR1, Circle( HEAD_RADIUS ).stretch( Vector( HEAD_X_STRETCH, ZERO ) ) );

        head_drawing.draw( FUR_COLOR1, Circle( EAR_RADIUS, VectorA( EAR_ANGLE_FRONT, EAR_PLACEMENT_DISTANCE ) ) );
        head_drawing.draw( FUR_COLOR2, Circle( EAR_INNER_RADIUS, VectorA( EAR_ANGLE_FRONT, EAR_PLACEMENT_DISTANCE ) ) );

        head_drawing.draw( TEETH_COLOR, Rectangle( TEETH_WIDTH, TEETH_LENGTH, NOSE_OFFSET + Vector( ZERO, -half( TEETH_LENGTH ) ) ) );

        head_drawing.draw( FUR_COLOR2, Circle( CHEEK_RADIUS, NOSE_OFFSET + VectorA( RIGHT_ANGLE * 2.5, CHEEK_RADIUS ) ) );
        head_drawing.draw( FUR_COLOR2, Circle( CHEEK_RADIUS, NOSE_OFFSET + VectorA( RIGHT_ANGLE * 3.5, CHEEK_RADIUS ) ) );

        head_drawing.draw( NOSE_COLOR, Circle( NOSE_RADIUS, NOSE_OFFSET ) );

        draw( head_drawing.move( head_offset ) );

        draw_eyes( NOSE_OFFSET + Vector( EYE_SPACING_SIDE, EYE_SPACING_UP ) + head_offset, NOSE_OFFSET + Vector( -EYE_SPACING_SIDE, EYE_SPACING_UP ) + head_offset );

        if ( facing_left( ) )
        {
            mirror_y( );
        }
    }
}

DirtBall::DirtBall( World * world, const Planc & _radius, const Coordinate & _position, const Vector & _velocity ) : Object( world, _position, _velocity )
{
#ifdef AXN_DEBUG
    m_draw_debug = true;
#endif

    space( Rectangle( _radius, _radius ) );
    position( _position );

    solid( true );
    interactive( true );

    gravity_ratio( 0.125 );
    terrain_boundaries( false );

    velocity( _velocity );

    persist_render( true );

    draw( WHITE, Circle( _radius ) );
}
