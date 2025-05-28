#include "Wisp.hpp"

#include "World.hpp"
#include "Terrain.hpp"

using mtmercy::Wisp;

namespace
{

const Planc HEAD_RADIUS = 10.0;
const dec FACE_RADIUS_RATIO = 0.8;
const Planc FACE_RADIUS = FACE_RADIUS_RATIO * HEAD_RADIUS;
const dec FACE_RADIUS_OFFSET_RATIO = 0.5;
const Planc FACE_OFFSET_DISTANCE = FACE_RADIUS_OFFSET_RATIO * ( HEAD_RADIUS - FACE_RADIUS );
const dec EYE_OFFSET = 0.5;
cAngle EYE_OFFSET_ANGLE = ( TAU / 12.0 );
const Planc EYE_RADIUS = ( ( 1.0 - EYE_OFFSET ) * 0.4 ) * FACE_RADIUS;;
const dec MOUTH_OFFSET_RATIO = 0.75;
const Planc MOUTH_OFFSET = MOUTH_OFFSET_RATIO * FACE_RADIUS;
const dec MOUTH_RADIUS_RATIO = 0.625;
const Planc MOUTH_RADIUS = MOUTH_RADIUS_RATIO * FACE_RADIUS;
cColor COLOR = BLACK;

const Span<uint> FLAME_PAUSE = { 5, 8 };
const Planc FLAME_RADIUS = 16.0;
const Planc FLAME_RADIUS_MIN = 0.1;
const Planc FLAME_SPEED = 1.0;
const Planc FLAME_SHRINK_RATE = 0.4;
const Planc FLAME_ALPHA_SHRINK_RATE = 0.025;
cAngle FLAME_DEVIATION = ( RIGHT / 2.0 );
const dec FLAME_WIND_RESISTANCE_RATIO = 0.5;
const dec FLAME_MOVEMENT_RESISTANCE_RATIO = 0.9;
const Span<dec> FLAME_BASE_RADIUS_RATIO = { 1.0, 1.1 };
const dec FLAME_BASE_OFFSET = 0.01;
const Planc FLAME_LIGHT_DISTANCE = ( FLAME_RADIUS * 2.0 );
cColor FLAME_COLOR = CYAN;

const Planc BULLET_RADIUS = 4.0;
const Planc BULLET_FLAME_RATIO = 1.1;

} // namespace

Wisp::Wisp( Room * room, const Coordinate & _position ) : Enemy( room, _position ), m_fire( room, _position )
{
    needs_render_always( true );

    no_gravity( );

    space( Polygon::square( HEAD_RADIUS ) );

    sight_range( 300.0 );
    alert_range( 200.0 );

    m_reload_timer.reset( 0 );

    m_fire.flame_color( FLAME_COLOR );
    m_fire.flame_pause( FLAME_PAUSE );
    m_fire.flame_radius( FLAME_RADIUS );
    m_fire.flame_radius_min( FLAME_RADIUS_MIN );
    m_fire.flame_speed( FLAME_SPEED );
    m_fire.flame_shrink( FLAME_SHRINK_RATE );
    m_fire.flame_alpha_shrink( FLAME_ALPHA_SHRINK_RATE );
    m_fire.flame_deviation( FLAME_DEVIATION );
    m_fire.wind_resistance_ratio( FLAME_WIND_RESISTANCE_RATIO );
    m_fire.movement_resistance_ratio( FLAME_MOVEMENT_RESISTANCE_RATIO );
    m_fire.light_distance( FLAME_LIGHT_DISTANCE );
    m_fire.flame_base_radius_ratio( FLAME_BASE_RADIUS_RATIO );
    m_fire.flame_base_max_offset_ratio( FLAME_BASE_OFFSET );
    m_fire.enable_base_flame( true );
}

void Wisp::render( )
{
    Enemy::render( );

    m_fire.render( );
    draw( m_fire );

    Vector face_offset;

    if( has_target( ) )
    {
        Angle viewing_angle = Angle( position( ), target( )->position( ) );
        face_offset = VectorA( viewing_angle, FACE_OFFSET_DISTANCE );
    }

    draw( COLOR, Polygon::circle( HEAD_RADIUS ) );
    draw( COLOR, Polygon::circle( FACE_RADIUS, face_offset ) );

    draw( FLAME_COLOR, Polygon::circle( EYE_RADIUS, VectorA( EYE_OFFSET_ANGLE, EYE_OFFSET * FACE_RADIUS ) + face_offset ) );
    draw( FLAME_COLOR, Polygon::circle( EYE_RADIUS, VectorA( PI - EYE_OFFSET_ANGLE, EYE_OFFSET * FACE_RADIUS ) + face_offset ) );

    draw( FLAME_COLOR, Polygon( Arc::ccw( CoordinateY( -MOUTH_OFFSET ), MOUTH_RADIUS, 0.0, PI ).path( ).points( ) ) + face_offset );
}

void Wisp::update( )
{
    Enemy::update( );

    m_fire.update( );

    clear_light_sources( );
    add_light_sources( m_fire.light_sources( ) );

    if( m_reload_timer.tick( ) )
    {
        if( has_target( ) )
        {
            m_reload_timer.reset( 60 );
            room( )->add_object( new Bullet( room( ), position( ), VectorA( Angle( position( ), target( )->position( ) ), 10.0 ) ) );
        }
    }
}

void Wisp::move( Vector cref _velocity )
{
    Enemy::move( _velocity );

    m_fire.move( _velocity );
}

Wisp::Bullet::Bullet( Room * room, const Coordinate & _position, const Vector & _velocity ) : Object( room, _position, _velocity ), m_fire( room, _position )
{
    needs_render_always( true );

    no_gravity( );

    interactive( true );

    solid( true );

    air_resistance_ratio( 0.0 );

    m_fire.flame_color( FLAME_COLOR );
    m_fire.flame_pause( FLAME_PAUSE );
    m_fire.flame_radius( FLAME_RADIUS * ( BULLET_FLAME_RATIO * ( BULLET_RADIUS / HEAD_RADIUS ) ) );
    m_fire.flame_radius_min( FLAME_RADIUS_MIN );
    m_fire.flame_speed( FLAME_SPEED );
    m_fire.flame_shrink( FLAME_SHRINK_RATE );
    m_fire.flame_alpha_shrink( FLAME_ALPHA_SHRINK_RATE );
    m_fire.flame_deviation( FLAME_DEVIATION );
    m_fire.wind_resistance_ratio( FLAME_WIND_RESISTANCE_RATIO );
    m_fire.movement_resistance_ratio( FLAME_MOVEMENT_RESISTANCE_RATIO );
    m_fire.light_distance( FLAME_LIGHT_DISTANCE );
    m_fire.flame_base_radius_ratio( FLAME_BASE_RADIUS_RATIO );
    m_fire.flame_base_max_offset_ratio( FLAME_BASE_OFFSET );
    m_fire.enable_base_flame( true );
}

void Wisp::Bullet::render( )
{
    Object::render( );

    m_fire.render( );
    draw( m_fire );

    draw( COLOR, Polygon::circle( BULLET_RADIUS ) );
}

void Wisp::Bullet::update( )
{
    Object::update( );

    if( ground( ) )
    {
        m_fire.extinguish( );
    }

    m_fire.update( );

    clear_light_sources( );
    add_light_sources( m_fire.light_sources( ) );
}

void Wisp::die( )
{
    Mob::die( );
    
    gravity_ratio( 1.0 );
}

void Wisp::Bullet::move( Vector cref _velocity )
{
    Object::move( _velocity );

    m_fire.move( _velocity );
}

bool Wisp::Bullet::collide( Object * object )
{
    Object::collide( object );

    if( object->interactive( ) && !dynamic_cast<Bullet *>( object ) && !dynamic_cast<Wisp *>( object ) )
    {
        if( Mob * mob = dynamic_cast<Mob *>( object ) )
        {
            mob->hurt( Damage( 10.0 ) );
        }

        subscribe_to_movement( object );
        stationary( true );
        //interactive( false );
        solid( false );

        mark_to_delete( );

        return true;
    }

    return false;
}
