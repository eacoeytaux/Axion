#include "mt/mobs/enemies/Wisp.hpp"

var_const( Planc MOVE_SPEED = ( 0.01 ) );

var_const( Planc HEAD_RADIUS = ( 10.0 ) );
var_const( Planc FACE_RADIUS = ( HEAD_RADIUS * 0.8 ) );
var_const( Planc FACE_OFFSET_DISTANCE = ( ( HEAD_RADIUS - FACE_RADIUS ) * 0.5 ) );

var_const( dec EYE_OFFSET_RATIO = ( 0.5 ) );
var_const( dec EYE_OFFSET = ( FACE_RADIUS * EYE_OFFSET_RATIO ) );
var_const( Angle EYE_OFFSET_ANGLE = ( Angle::fraction( 12.0 ) ) );
var_const( Planc EYE_RADIUS = ( ( one_minus( EYE_OFFSET_RATIO ) * 0.4 ) * FACE_RADIUS ) );

var_const( Planc MOUTH_OFFSET = ( FACE_RADIUS * 0.75 ) );
var_const( Planc MOUTH_RADIUS = ( FACE_RADIUS * 0.625 ) );
var_const( Color COLOR = ( BLACK ) );

var_const( Span<ticks> FLAME_PAUSE = { 5.0, 8.0 } );
var_const( Planc FLAME_RADIUS = ( 16.0 ) );
var_const( Planc FLAME_RADIUS_MIN = ( 0.1 ) );
var_const( Planc FLAME_SPEED = ( 1.0 ) );
var_const( Planc FLAME_SHRINK_RATE = ( 0.4 ) );
var_const( Planc FLAME_EXTINGUISHED_SHRINK_RATE = ( 2.5 ) );
var_const( Planc FLAME_ALPHA_SHRINK_RATE = ( 0.025 ) );
var_const( Angle FLAME_DEVIATION = ( Angle::fraction( 8.0 ) ) );
var_const( dec FLAME_WIND_RESISTANCE_RATIO = ( 0.5 ) );
var_const( dec FLAME_MOVEMENT_RESISTANCE_RATIO = ( 0.9 ) );
var_const( Span<dec> FLAME_BASE_RADIUS_RATIO = { 1.0, 1.1 } );
var_const( dec FLAME_BASE_OFFSET = ( 0.01 ) );
var_const( Planc FLAME_LIGHT_DISTANCE = ( FLAME_RADIUS * 2.0 ) );
var_const( Color FLAME_COLOR = ( CYAN ) );

var_const( uint DAMAGE = ( 10.0 ) );

var_const( Planc BULLET_RADIUS_START = ( 0.25 ) );
var_const( Planc BULLET_RADIUS_MAX = ( 4.0 ) );
var_const( Planc BULLET_RADIUS_GROWTH = ( 0.5 ) );
var_const( Planc BULLET_FLAME_RATIO = ( 1.1 ) );
var_const( Planc BULLET_SPEED = ( 2.5 ) );
var_const( ticks BULLET_RELOAD_TIME = ( 120.0 ) );
var_const( ticks BULLET_DELETION_TIME = ( 12.0 ) );

Wisp::Wisp( ptr<Room> in_room, Point cref in_position ) : Enemy( in_room, in_position ), m_fire( in_room, in_position )
{
    needs_render_always( true );

    terrain_bound( true );
    no_gravity( );

    space( Polygon::square( HEAD_RADIUS * sqrt( 2.0 ) ) );

    hurt_display_duration( 3 );

    // todo
    sight_range( 300.0 );
    alert_range( 200.0 );

    m_reload_timer.duration( BULLET_RELOAD_TIME );

    m_fire.flame_color( FLAME_COLOR );
    m_fire.flame_pause( FLAME_PAUSE );
    m_fire.flame_radius( FLAME_RADIUS );
    m_fire.flame_radius_min( FLAME_RADIUS_MIN );
    m_fire.flame_speed( FLAME_SPEED );
    m_fire.flame_shrink( FLAME_SHRINK_RATE );
    m_fire.flame_extinguished_shrink( FLAME_EXTINGUISHED_SHRINK_RATE );
    m_fire.flame_alpha_shrink( FLAME_ALPHA_SHRINK_RATE );
    m_fire.flame_deviation( FLAME_DEVIATION );
    m_fire.wind_resistance_ratio( FLAME_WIND_RESISTANCE_RATIO );
    m_fire.movement_resistance_ratio( FLAME_MOVEMENT_RESISTANCE_RATIO );
    m_fire.light_distance( FLAME_LIGHT_DISTANCE );
    m_fire.flame_base_radius_ratio( FLAME_BASE_RADIUS_RATIO );
    m_fire.flame_base_max_offset_ratio( FLAME_BASE_OFFSET );
    m_fire.enable_base_flame( true );

    m_fire.subscribe_to_movement( this );
}

void Wisp::render( )
{
    Enemy::render( );

    m_fire.render( );
    draw( m_fire );

    Polygon eye1 = Polygon::circle( EYE_RADIUS, VectorA( ( EYE_OFFSET_ANGLE ), EYE_OFFSET ) ) + face_offset( );
    Polygon eye2 = Polygon::circle( EYE_RADIUS, VectorA( ( PI - EYE_OFFSET_ANGLE ), EYE_OFFSET ) ) + face_offset( );
    Polygon mouth = Polygon( Arc::semi_ccw( MOUTH_RADIUS, PointY( -MOUTH_OFFSET ) ).points( ) ) + face_offset( );

    if( alive( ) )
    {
        draw( FLAME_COLOR, Polygon::circle( HEAD_RADIUS ) );
    }

    Drawing mask;

    mask.draw( COLOR, Polygon::circle( HEAD_RADIUS ) );

    mask.add_hole( eye1 );
    mask.add_hole( eye2 );
    mask.add_hole( mouth );

    mask.rotate( m_rotation );

    draw( mask );
}

void Wisp::update( )
{
    m_fire.update( );

    clear_light_sources( );
    add_light_sources( m_fire.light_sources( ) );

    if( alive( ) )
    {
        m_reload_timer.tick( );

        if( has_target( ) )
        {
            Vector v = ( target( )->position( ) - position( ) );

            if( v.magnitude( ) > ( velocity( ).magnitude( ) + MOVE_SPEED ) )
            {
                add_velocity( VectorA( v.angle( ), min( MOVE_SPEED, v.magnitude( ) ) ) );
            }
            else
            {
                add_velocity( -velocity( ) * 0.9 ); // todo
            }

            if( m_reload_timer.completed( ) )
            {
                m_reload_timer.reset( );
                room( )->add_object( new Bullet( room( ), ( position( ) + face_offset( ) - VectorY( MOUTH_OFFSET - half( MOUTH_RADIUS ) ) ), VectorA( v.angle( ), BULLET_SPEED ) ) );
            }
        }

        if( grounded( ) )
        {
            clear_ground( );
        }
    }

    Enemy::update( );

    if( dead( ) && grounded( ) )
    {
        // todo rather than dx, check ground direction
        // todo rotation_distance( ) instead of width( )
        m_rotation_speed = TAU * Random::negated( velocity( ).magnitude( ) /
                                                  Circle::circumference( width( ) ),
                                                  !is_neg( velocity( ).dx( ) ) );
    }

    m_rotation_speed *= one_minus( friction_resistance( ) );
    m_rotation += m_rotation_speed;

    for_each( object, movement_subscribers( ) )
    {
        object->rotate( m_rotation_speed, position( ) );
        // object->move( object->position( ).rotate( m_rotation_speed, position( ) ) - object->position( ) ) );
    }
}

void Wisp::die( )
{
    Enemy::die( );

    normal_gravity( );

    m_rotation_speed = killing_blow( ).force( ).angle( ).min_distance( Angle( killing_blow( ).force( ).origin( ), position( ) ) );

    m_fire.extinguish( );
}

void Wisp::move( Vector cref in_velocity )
{
    Enemy::move( in_velocity );
}

Vector Wisp::face_offset( ) const
{
    if( alive( ) && has_target( ) )
    {
        return( VectorA( Angle( position( ), target( )->position( ) ), FACE_OFFSET_DISTANCE ) );
    }
    else
    {
        return( V0 );
    }
}

Wisp::Bullet::Bullet( ptr<Room> in_room, Point cref in_position, Vector cref in_velocity ) : Projectile( in_room, DAMAGE, in_position, in_velocity ), m_fire( in_room, in_position )
{
    #if defined( AXN_DEBUG )
    draw_debug = true;
    #endif

    needs_render_always( true );

    no_gravity( );
    no_air_resistance( );

    m_radius = BULLET_RADIUS_START;
    m_radius_max = BULLET_RADIUS_MAX;

    solid( m_radius == m_radius_max );
    interactive( m_radius == m_radius_max );

    m_fire.flame_color( FLAME_COLOR );
    m_fire.flame_pause( FLAME_PAUSE );
    m_fire.flame_radius( FLAME_RADIUS * ( BULLET_FLAME_RATIO * ( BULLET_RADIUS_MAX / HEAD_RADIUS ) ) );
    m_fire.flame_radius_min( FLAME_RADIUS_MIN );
    m_fire.flame_speed( FLAME_SPEED );
    m_fire.flame_shrink( FLAME_SHRINK_RATE );
    m_fire.flame_extinguished_shrink( FLAME_EXTINGUISHED_SHRINK_RATE );
    m_fire.flame_alpha_shrink( FLAME_ALPHA_SHRINK_RATE );
    m_fire.flame_deviation( FLAME_DEVIATION );
    m_fire.wind_resistance_ratio( FLAME_WIND_RESISTANCE_RATIO );
    m_fire.movement_resistance_ratio( FLAME_MOVEMENT_RESISTANCE_RATIO );
    m_fire.light_distance( FLAME_LIGHT_DISTANCE );
    m_fire.flame_base_radius_ratio( FLAME_BASE_RADIUS_RATIO );
    m_fire.flame_base_max_offset_ratio( FLAME_BASE_OFFSET );
    m_fire.enable_base_flame( true );

    m_fire.subscribe_to_movement( this );
}

void Wisp::Bullet::render( )
{
    Projectile::render( );

    Color color = COLOR;

    #if defined( AXN_DEBUG )
    if( Debug::active && Settings::get( Settings::DEBUG_DAMAGE ) )
    {
        if( interactive( ) )
        {
            color = RED; // todo should be in projectile somehow
        }
    }
    #endif

    if( marked_to_delete( ) )
    {
        color.a( to_deletion_countdown( ).percentage( ) );
    }

    m_fire.render( );
    draw( m_fire ); // todo alpha-ize fire

    draw( color, Polygon::circle( m_radius ) );
}

void Wisp::Bullet::update( )
{
    Projectile::update( );

    if( m_radius != m_radius_max )
    {
        m_radius += BULLET_RADIUS_GROWTH;

        if( m_radius >= m_radius_max )
        {
            m_radius = m_radius_max;
            activate( true );
        }

        m_fire.flame_radius( FLAME_RADIUS * ( BULLET_FLAME_RATIO * ( m_radius / HEAD_RADIUS ) ) );
    }

    m_fire.update( );

    clear_light_sources( );
    add_light_sources( m_fire.light_sources( ) );
}

void Wisp::Bullet::activate( bool in_active )
{
    Projectile::activate( in_active );

    if( !in_active )
    {
        mark_to_delete( BULLET_DELETION_TIME );

        m_fire.extinguish( );
    }
}
