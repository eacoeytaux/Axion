#include "Hook.hpp"

#include "Climber.hpp"

namespace
{
// crossbox / hook
const Planc DEFAULT_ROPE_MAX_LENGTH = METER * 10.0;
const Planc DEFAULT_ROPE_GROWTH_SPEED = 33.0;
const Planc DEFAULT_ROPE_RETRACT_SPEED = 44.0;

const Planc HOOK_LENGTH = 38.0;
const Planc HOOK_THICKNESS = 5.0;
const Planc HOOK_TIP_LENGTH = 2.5;
const Angle HOOK_ANGLE = RIGHT_ANGLE;
const Color HOOK_COLOR = Color::rgb( 0x9C9C9C );

const Planc ROPE_WIDTH = 4.0;
const Color ROPE_BASE_COLOR = Color::rgb( 0xDAA420 );
const Color ROPE_ALT_COLOR = Color::rgb( 0xB9870F );
} // namespace

using mtmercy::Hook;
Hook::Hook( World * world, const Climber * owner ) : Object( world ), m_owner( owner )
{
#ifdef AXN_DEBUG
    draw_debug = true;
#endif
    
    Assert( m_owner, "owner cannot be null" );

    solid( true );

    position( m_owner->position( ) );

    state( LOADED );
    m_rope_growth_speed = DEFAULT_ROPE_GROWTH_SPEED;
    m_rope_retract_speed = DEFAULT_ROPE_RETRACT_SPEED;
    m_max_rope_length = DEFAULT_ROPE_MAX_LENGTH;
}

void Hook::render( )
{
    Object::render( );

    Coordinate tip = hook_tip( ) - m_owner->position( );
    Coordinate base = hook_base( ) - m_owner->position( );

    Vector rod( base, tip );
    rod.magnitude( rod.magnitude( ) - ( HOOK_THICKNESS * HOOK_TIP_LENGTH ) );

    Angle hook_angle( base, tip );

    draw( HOOK_COLOR, Circle( HOOK_THICKNESS, base ) );
    draw( HOOK_COLOR, Line( rod.origin( ), rod ), HOOK_THICKNESS );
    draw( HOOK_COLOR, Triangle( tip,
                                tip + VectorA( hook_angle + RIGHT_ANGLE, HOOK_THICKNESS * HOOK_TIP_LENGTH.half( ) ) - VectorA( hook_angle, HOOK_THICKNESS * HOOK_TIP_LENGTH ),
                                tip + VectorA( hook_angle - RIGHT_ANGLE, HOOK_THICKNESS * HOOK_TIP_LENGTH.half( ) ) - VectorA( hook_angle, HOOK_THICKNESS * HOOK_TIP_LENGTH ) ) );

    Vector rope_vector = hook_base( ) - m_owner->position( );

    // draw rope base
    draw( ROPE_BASE_COLOR, Line( rope_vector.origin( ), rope_vector ), ROPE_WIDTH );

    { // draw rope detail coils
        Vector rope_chunk = VectorA( rope_vector.angle( ), ROPE_WIDTH );
        for_range( i, (uint)half( rope_vector.magnitude( ) / rope_chunk.magnitude( ) ) )
        {
            Polygon rope_strip_rect = Rectangle( ROPE_WIDTH, ROPE_WIDTH, base - ( rope_chunk * ( i + 1 ) * 2 ), rope_chunk.angle( ) );
            draw( ROPE_ALT_COLOR, rope_strip_rect );
        }
    }
    
#ifdef AXN_DEBUG
    // if( Debug::active )
    {
        if( m_rope_length >= m_max_rope_length )
        {
            draw( RED.a( 0.5 ), Line( rope_vector.origin( ), rope_vector ), ROPE_WIDTH );
        }
    }
#endif
}

void Hook::update( )
{
    Object::update( );
}

void Hook::update_velocity( )
{
    if( state( ) == LOADED )
    {
        ground( nullptr );
        no_gravity( );
        velocity( ZERO_VECTOR );
        m_angle = m_owner->aim_angle( );
        position( m_owner->position( ) + VectorA( m_angle, HOOK_LENGTH ) );
    }
    else if( state( ) == FIRING )
    {
        gravity_ratio( 0.5 );
        m_rope_length = m_owner->position( ).distance_to( position( ) );

        if( Object::ground( ) )
        {
            state( HOOKED );
            velocity( ZERO_VECTOR );
        }
        else if( m_rope_length > m_max_rope_length )
        {
            gravity_ratio( 0 );
            state( HOOKED );
            velocity( ZERO_VECTOR );
            
            // state( RETRACTING );
            // TODO adjust for overshot with remaining percentage
        }
    }
    else if( state( ) == RETRACTING )
    {
        stationary( false );
        ground( nullptr );
        velocity( VectorA( Angle( position( ), m_owner->position( ) ), m_rope_retract_speed ) );
        m_rope_length -= m_rope_retract_speed;
        if( m_rope_length <= HOOK_LENGTH )
        {
            state( LOADED );
        }
    }
    else if( state( ) == HOOKED )
    {
        m_rope_length = m_owner->position( ).distance_to( position( ) );
    }

    Object::update_velocity( );

    if( !Object::ground( ) )
    {
        if( state( ) == FIRING )
        {
            m_angle = velocity( ).angle( );
        }
        else if( state( ) == RETRACTING )
        {
            m_angle = velocity( ).angle( ) + PI;
        }
    }
}

void Hook::ground( TerrainEdge * ground )
{
    Object::ground( ground );
}

Coordinate Hook::hook_tip( ) const
{
    return position( );
}

Coordinate Hook::hook_base( ) const
{
    if( state( ) == LOADED )
    {
        return m_owner->position( );
    }
    else
    {
        return hook_tip( ) - VectorA( m_angle, HOOK_LENGTH );
    }
}

void Hook::fire( const Vector & _launch_speed )
{
    switch( state( ) )
    {
        case LOADED :
        {
            velocity( _launch_speed );
            state( FIRING );
            // static SoundClip twang( "twang.wav" );
            // twang.play( );
            break;
        }
        case FIRING :
        case HOOKED :
        case RETRACTING :
        {
            break;
        }
        default :
        {
            // Assert( "invalid_state" );
            break;
        }
    }
}

void Hook::retract( )
{
    switch( state( ) )
    {
        case LOADED :
        {
            break;
        }
        case FIRING :
        case HOOKED :
        {
            state( RETRACTING );
            break;
        }
        case RETRACTING :
        {
            break;
        }
        default :
        {
            // Assert( "invalid_state" );
            break;
        }
    }
}

void Hook::load( )
{
    ground( nullptr );
    state( LOADED );
}
