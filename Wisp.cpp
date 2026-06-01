#include "Wisp.hpp"

#include "World.hpp"
#include "Terrain.hpp"

using mtmercy::Wisp;

namespace
{

cPlanc HEAD_RADIUS = 10.0;
cPlanc FACE_RADIUS = HEAD_RADIUS * 0.8;
cPlanc FACE_OFFSET_DISTANCE = ( HEAD_RADIUS - FACE_RADIUS ) * 0.5;

cdec EYE_OFFSET = 0.5;
cAngle EYE_OFFSET_ANGLE = ( TAU / 12.0 );
cPlanc EYE_RADIUS = ( ( 1.0 - EYE_OFFSET ) * 0.4 ) * FACE_RADIUS;;

cPlanc MOUTH_OFFSET = FACE_RADIUS * 0.75;
cPlanc MOUTH_RADIUS = FACE_RADIUS * 0.625;
cColor COLOR = BLACK;

const Span<uint> FLAME_PAUSE = { 5, 8 };
cPlanc FLAME_RADIUS = 16.0;
cPlanc FLAME_RADIUS_MIN = 0.1;
cPlanc FLAME_SPEED = 1.0;
cPlanc FLAME_SHRINK_RATE = 0.4;
cPlanc FLAME_ALPHA_SHRINK_RATE = 0.025;
cAngle FLAME_DEVIATION = ( RIGHT / 2.0 );
cdec FLAME_WIND_RESISTANCE_RATIO = 0.5;
cdec FLAME_MOVEMENT_RESISTANCE_RATIO = 0.9;
const Span<dec> FLAME_BASE_RADIUS_RATIO = { 1.0, 1.1 };
cdec FLAME_BASE_OFFSET = 0.01;
cPlanc FLAME_LIGHT_DISTANCE = ( FLAME_RADIUS * 2.0 );
cColor FLAME_COLOR = CYAN;

cPlanc BULLET_RADIUS = 4.0;
cPlanc BULLET_FLAME_RATIO = 1.1;
cPlanc BULLET_SPEED = 1.0;
cuint BULLET_RELOAD_TIME = 60.0;

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
    //draw( m_fire );

    Vector face_offset;

    if( has_target( ) )
    {
        Angle viewing_angle = Angle( position( ), target( )->position( ) );
        face_offset = VectorA( viewing_angle, FACE_OFFSET_DISTANCE );
    }

    draw( COLOR, Polygon::circle( HEAD_RADIUS ) );

    erase( Polygon::circle( EYE_RADIUS, VectorA( EYE_OFFSET_ANGLE, EYE_OFFSET * FACE_RADIUS ) + face_offset ) );
    erase( Polygon::circle( EYE_RADIUS, VectorA( PI - EYE_OFFSET_ANGLE, EYE_OFFSET * FACE_RADIUS ) + face_offset ) );

    erase( Polygon( Arc::semi_ccw( MOUTH_RADIUS, CoordinateY( -MOUTH_OFFSET ) ).path( ).points( ) ) + face_offset );
}

void Wisp::update( )
{
    Enemy::update( );

    m_fire.update( );

    clear_light_sources( );
    add_light_sources( m_fire.light_sources( ) );

    if( alive( ) )
    {
        if( m_reload_timer.tick( ) )
        {
            if( has_target( ) )
            {
                m_reload_timer.reset( BULLET_RELOAD_TIME );
                room( )->add_object( new Bullet( room( ), position( ), VectorA( Angle( position( ), target( )->position( ) ), BULLET_SPEED ) ) );
            }
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
    no_air_resistance( );

    interactive( true );

    solid( true );

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

    normal_gravity( );

    m_fire.extinguish( );
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
            mob->damage( Damage( 10.0 ) ); // todo
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
