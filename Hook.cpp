#include "Hook.hpp"

#include "Climber.hpp"

namespace
{
// crossbox / hook
cPlanc DEFAULT_ROPE_MAX_LENGTH = METER * 10.0;
cPlanc DEFAULT_ROPE_GROWTH_SPEED = 33.0;
cPlanc DEFAULT_ROPE_RETRACT_SPEED = 64.0;

cPlanc HOOK_LENGTH = 38.0;
cPlanc HOOK_THICKNESS = 5.0;
cPlanc HOOK_TIP_LENGTH = 2.5;
cAngle HOOK_ANGLE = RIGHT;
cColor HOOK_COLOR = Color::rgb( 0x9C9C9C );

cPlanc ROPE_WIDTH = 4.0;
cColor ROPE_BASE_COLOR = Color::rgb( 0xDAA420 );
cColor ROPE_ALT_COLOR = Color::rgb( 0xB9870F );

} // namespace

using mtmercy::Hook;
Hook::Hook( Room * room, const Climber * owner ) : Object( room ), m_owner( owner )
{
    #if defined ( AXN_DEBUG )
    draw_debug = true;
    #endif

    Assert( m_owner, "owner cannot be null" );

    solid( true );
    interactive( true );

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

    draw( HOOK_COLOR, Line( rod.origin( ), rod ), HOOK_THICKNESS );
    draw( HOOK_COLOR, Polygon::circle( HOOK_THICKNESS, base ) );
    draw( HOOK_COLOR, Polygon::triangle( tip,
                                         tip + VectorA( hook_angle + RIGHT, HOOK_THICKNESS * half( HOOK_TIP_LENGTH ) ) - VectorA( hook_angle, HOOK_THICKNESS * HOOK_TIP_LENGTH ),
                                         tip + VectorA( hook_angle - RIGHT, HOOK_THICKNESS * half( HOOK_TIP_LENGTH ) ) - VectorA( hook_angle, HOOK_THICKNESS * HOOK_TIP_LENGTH ) ) );

    Vector rope_vector = hook_base( ) - m_owner->position( );
    Line rope( rope_vector );

    // draw rope base
    draw( ROPE_BASE_COLOR, rope, ROPE_WIDTH );

    { // draw rope detail coils
        Vector rope_chunk = VectorA( rope_vector.angle( ), ROPE_WIDTH );
        for_range( i, ( half( rope_vector.magnitude( ) / rope_chunk.magnitude( ) ) ) )
        {
            Polygon rope_strip_rect = Polygon::rectangle( ROPE_WIDTH, ROPE_WIDTH, base - ( rope_chunk * ( i + 1 ) * 2 ), rope_chunk.angle( ) );
            draw( ROPE_ALT_COLOR, rope_strip_rect );
        }
    }

    #if defined ( AXN_DEBUG )
    // if( Debug::active )
    {
        if( taut( ) )
        {
            draw( RED.a( 0.5 ), rope, ROPE_WIDTH );
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
        velocity( V0 );
        m_angle = m_owner->aim_angle( );
        position( m_owner->position( ) + VectorA( m_angle, HOOK_LENGTH ) );
    }
    else if( state( ) == LAUNCHING )
    {
        gravity_ratio( 0.25 );
        m_rope_length = m_owner->position( ).distance_to( position( ) );

        if( Object::ground( ) )
        {
            state( HOOKED );
            velocity( V0 );
        }
        else if( rope( ).magnitude( ) >= m_max_rope_length )
        {
            no_gravity( );
            state( HOOKED );
            velocity( V0 );

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
        if( state( ) == LAUNCHING )
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

bool Hook::collide( Object * object )
{
    if( state( ) != LOADED )
    {
        Object::collide( object );

        if( state( ) == LAUNCHING )
        {
            if( object->interactive( ) && !dynamic_cast<Hook *>( object ) )
            {
                if( Mob * mob = dynamic_cast<Mob *>( object ) )
                {
                    mob->hurt( Damage( 1000.0 ) ); // todo lol
                }

                state( RETRACTING );

                return true;
            }
        }
    }

    return false;
}

Planc Hook::max_rope_length( ) const
{
    return m_max_rope_length;
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

Coordinate Hook::hook_tip( ) const
{
    return position( );
}

Vector Hook::rope( ) const
{
    return Vector( m_owner->position( ), hook_base( ) );
}

bool Hook::taut( ) const
{
    return ( ( rope( ).magnitude( ) >= m_max_rope_length ) && !loaded( ) );
}

bool Hook::hooked( ) const
{
    return ( m_state == HOOKED );
}

bool Hook::loaded( ) const
{
    return ( m_state == LOADED );
}

bool Hook::launching( ) const
{
    return ( m_state == LOADED );
}

bool Hook::retracting( ) const
{
    return ( m_state == RETRACTING );
}

void Hook::launch( Vector cref _launch_speed )
{
    switch( state( ) )
    {
        case LOADED:
        {
            velocity( _launch_speed );
            state( LAUNCHING );
            // static SoundClip twang( "twang.wav" );
            // twang.play( );
            break;
        }
        case LAUNCHING:
        case HOOKED:
        case RETRACTING:
        {
            break;
        }
        default:
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
        case LOADED:
        {
            break;
        }
        case LAUNCHING:
        case HOOKED:
        {
            state( RETRACTING );
            break;
        }
        case RETRACTING:
        {
            break;
        }
        default:
        {
            // Assert( "invalid_state" );
            break;
        }
    }
}

void Hook::reload( )
{
    ground( nullptr );
    state( LOADED );
}
