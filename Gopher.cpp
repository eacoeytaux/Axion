#include "Gopher.hpp"

using mtmercy::Gopher;
using mtmercy::DirtBall;

namespace
{
cdec GRAVITY_RATIO = 0.125;

cPlanc SIGHT_RANGE = METER * 10.0;
cPlanc ALERT_RANGE = SIGHT_RANGE * 0.75;

cuint RELOAD_TIME = 100;

cPlanc HEAD_RADIUS = 12.0;
cPlanc HEAD_X_STRETCH = 1.1;

cPlanc BODY_WIDTH = HEAD_RADIUS * 2.0 * 0.8;
cPlanc BODY_HEIGHT = HEAD_RADIUS * 2.0;

cPlanc INNER_FUR_WIDTH = BODY_WIDTH * 0.8;
cPlanc INNER_FUR_HEIGHT = BODY_HEIGHT * 0.8;

cPlanc CHEEK_RADIUS = HEAD_RADIUS * 0.3;

cPlanc NOSE_RADIUS = HEAD_RADIUS * 0.25;
const Vector NOSE_OFFSET = VectorX( CHEEK_RADIUS );

cPlanc TEETH_WIDTH = NOSE_RADIUS * 0.9;
cPlanc TEETH_LENGTH = HEAD_RADIUS * 0.64;

cPlanc EAR_RADIUS = HEAD_RADIUS * 0.3;
cPlanc EAR_INNER_RADIUS = EAR_RADIUS * 0.5;
cPlanc EAR_PLACEMENT_DISTANCE = HEAD_RADIUS;
const Angle EAR_ANGLE_FRONT = RIGHT * 1.5;
const Angle EAR_ANGLE_BACK = RIGHT * 0.5;

cPlanc EYE_SPACING_UP = NOSE_RADIUS * 1.25;
cPlanc EYE_SPACING_SIDE = NOSE_RADIUS * 1.75;
cPlanc EYE_RADIUS = 1.0;
cuint BLINK_DURATION = 4;
const Span<uint> BLINK_WAIT = { 360, 480 };

const Color EYE_COLOR = BLACK;
const Color TEETH_COLOR = WHITE;
const Color NOSE_COLOR = Color::rgb( 0x654321 );
const Color FUR_COLOR1 = Color::rgb( 0xA0522D );
const Color FUR_COLOR2 = Color::rgb( 0xF4A460 );
}

Gopher::Gopher( Room * room, Coordinate cref _position ) : Enemy( room, _position, 10000.0 )
{
    space( Polygon::square( 16.0 ) );

    eye_info( EYE_RADIUS, BLINK_DURATION, BLINK_WAIT, EYE_COLOR );

    sight_range( SIGHT_RANGE );
    alert_range( ALERT_RANGE );

    state( EXPOSED );
}

void Gopher::update( )
{
    Enemy::update( );

    m_reload_timer.tick( );

    if( state( ) == EXPOSED )
    {
        if( has_target( ) )
        {
            facing_left( target( )->position( ).x( ) < position( ).x( ) );

            if( !m_reload_timer.remaining( ) )
            {
                room( )->add_object( new DirtBall( room( ), 5.0, position( ), VectorA( Angle( position( ), target( )->position( ) ), 5.0 ) ) );
                m_reload_timer.reset( RELOAD_TIME );
            }
        }
    }
}

void Gopher::render( )
{
    Enemy::render( );

    if( state( ) == EXPOSED )
    {
        draw( FUR_COLOR1, Polygon::rectangle( BODY_WIDTH, BODY_HEIGHT ) );

        Polygon inner_fur = Polygon::circle( half( INNER_FUR_WIDTH ) ).stretch( Vector( 0.0, INNER_FUR_HEIGHT / INNER_FUR_WIDTH ) );
        draw( FUR_COLOR2, inner_fur );

        Vector head_offset( 0.0, half( BODY_HEIGHT ) );
        Drawing head_drawing;

        head_drawing.draw( FUR_COLOR1, Polygon::circle( EAR_RADIUS, VectorA( EAR_ANGLE_BACK, EAR_PLACEMENT_DISTANCE ) ) );
        head_drawing.draw( FUR_COLOR2, Polygon::circle( EAR_INNER_RADIUS, VectorA( EAR_ANGLE_BACK, EAR_PLACEMENT_DISTANCE ) ) );

        head_drawing.draw( FUR_COLOR1, Polygon::circle( HEAD_RADIUS ).stretch( Vector( HEAD_X_STRETCH, 0.0 ) ) );

        head_drawing.draw( FUR_COLOR1, Polygon::circle( EAR_RADIUS, VectorA( EAR_ANGLE_FRONT, EAR_PLACEMENT_DISTANCE ) ) );
        head_drawing.draw( FUR_COLOR2, Polygon::circle( EAR_INNER_RADIUS, VectorA( EAR_ANGLE_FRONT, EAR_PLACEMENT_DISTANCE ) ) );

        head_drawing.draw( TEETH_COLOR, Polygon::rectangle( TEETH_WIDTH, TEETH_LENGTH, NOSE_OFFSET + Vector( 0.0, -half( TEETH_LENGTH ) ) ) );

        head_drawing.draw( FUR_COLOR2, Polygon::circle( CHEEK_RADIUS, NOSE_OFFSET + VectorA( RIGHT * 2.5, CHEEK_RADIUS ) ) );
        head_drawing.draw( FUR_COLOR2, Polygon::circle( CHEEK_RADIUS, NOSE_OFFSET + VectorA( RIGHT * 3.5, CHEEK_RADIUS ) ) );

        head_drawing.draw( NOSE_COLOR, Polygon::circle( NOSE_RADIUS, NOSE_OFFSET ) );

        draw( head_drawing.move( head_offset ) );

        draw_eyes( NOSE_OFFSET + Vector( EYE_SPACING_SIDE, EYE_SPACING_UP ) + head_offset, NOSE_OFFSET + Vector( -EYE_SPACING_SIDE, EYE_SPACING_UP ) + head_offset );

        if( facing_left( ) )
        {
            mirror_y( );
        }
    }
}

DirtBall::DirtBall( Room * room, Planc cref _radius, Coordinate cref _position, Vector cref _velocity ) : Object( room, _position, _velocity )
{
    #if defined ( AXN_DEBUG )
    draw_debug = true;
    #endif

    space( Polygon::rectangle( _radius, _radius ) );
    position( _position );

    solid( true );
    interactive( true );

    gravity_ratio( GRAVITY_RATIO );
    terrain_boundaries( false );

    velocity( _velocity );

    persist_render( true );

    draw( WHITE, Polygon::circle( _radius ) );
}
