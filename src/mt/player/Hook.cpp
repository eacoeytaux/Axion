#include "mt/player/Hook.hpp"

#include "mt/player/Climber.hpp"

// crossbox / hook
var_const( Planc DEFAULT_ROPE_MAX_LENGTH = ( METER * 10.0 ) );
var_const( Planc DEFAULT_ROPE_GROWTH_SPEED = ( 33.0 ) );
var_const( Planc DEFAULT_ROPE_RETRACT_SPEED = ( 64.0 ) );

var_const( Planc HOOK_LENGTH = ( 38.0 ) );
var_const( Planc HOOK_THICKNESS = ( 5.0 ) );
var_const( Planc HOOK_HOLE_THICKNESS = ( HOOK_THICKNESS * 0.5 ) );
var_const( Planc HOOK_TIP_LENGTH = ( 2.5 ) );
var_const( Angle HOOK_ANGLE = ( RIGHT ) );
var_const( Color HOOK_COLOR = ( Color::rgb( 0x9C9C9C ) ) );

var_const( Planc ROPE_WIDTH = ( 4.0 ) );
var_const( Color ROPE_BASE_COLOR = ( Color::rgb( 0xDAA420 ) ) );
var_const( Color ROPE_ALT_COLOR = ( Color::rgb( 0xB9870F ) ) );

Hook::Hook( ptr<Room> in_room, Climber * in_owner ) : Object( in_room ), m_owner( in_owner )
{
    #if defined( AXN_DEBUG )
    draw_debug = true;
    #endif

    Assert( m_owner, "owner cannot be null" );

    solid( false );
    interactive( false );

    position( m_owner->position( ) );

    state( LOADED );

    m_rope_growth_speed = DEFAULT_ROPE_GROWTH_SPEED;
    m_rope_retract_speed = DEFAULT_ROPE_RETRACT_SPEED;
    m_max_rope_length = DEFAULT_ROPE_MAX_LENGTH;
}

void Hook::render( )
{
    Object::render( );

    Point tip = hook_tip( ) - m_owner->position( );
    Point base = hook_base( ) - m_owner->position( );

    Vector rod( base, tip );
    rod.magnitude( rod.magnitude( ) - ( HOOK_THICKNESS * HOOK_TIP_LENGTH ) );

    Angle hook_angle( base, tip );

    draw( HOOK_COLOR, Line( rod.origin( ), rod ), HOOK_THICKNESS );
    draw( HOOK_COLOR, Polygon::circle( HOOK_THICKNESS, base ) );
    draw( HOOK_COLOR, Polygon::triangle( tip,
                                         tip + VectorA( hook_angle + RIGHT, HOOK_THICKNESS * half( HOOK_TIP_LENGTH ) ) - VectorA( hook_angle, HOOK_THICKNESS * HOOK_TIP_LENGTH ),
                                         tip + VectorA( hook_angle - RIGHT, HOOK_THICKNESS * half( HOOK_TIP_LENGTH ) ) - VectorA( hook_angle, HOOK_THICKNESS * HOOK_TIP_LENGTH ) ) );

    // erase( Polygon::circle( HOOK_HOLE_THICKNESS, base ) );
}

void Hook::update( )
{
    Object::update( );
}

void Hook::trim_velocity( )
{
    Object::trim_velocity( );

    // todo this isn't trimming...
    if( !grounded( ) )
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

void Hook::update_velocity( )
{
    Object::update_velocity( );

    if( state( ) == LOADED )
    {
        no_gravity( );
        clear_ground( );
        velocity( V0 );
        m_angle = m_owner->aim_angle( );
        position( m_owner->position( ) + VectorA( m_angle, HOOK_LENGTH ) );
    }
    else if( state( ) == LAUNCHING )
    {
        gravity_scale( 0.25 ); // todo
        m_rope_length = m_owner->position( ).distance_to( position( ) );

        if( grounded( ) )
        {
            state( HOOKED );
            velocity( V0 );
        }
        else if( rope( ).magnitude( ) >= max_rope_length( ) )
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
        clear_ground( );
        stationary( false );
        velocity( VectorA( Angle( position( ), m_owner->position( ) ), m_rope_retract_speed ) );

        m_rope_length -= m_rope_retract_speed;

        if( m_rope_length <= HOOK_LENGTH )
        {
            state( LOADED );
        }
    }
}

void Hook::ground( ptr<Terrain::Node> ground, Terrain::Bumper cref in_bumper )
{
    Object::ground( ground, in_bumper );
}

bool Hook::collide( ptr<Object> in_object )
{
    if( state( ) != LOADED )
    {
        Object::collide( in_object );

        if( state( ) == LAUNCHING )
        {
            if( in_object->interactive( ) && !cast( Hook, in_object ) )
            {
                if( cast_as( mob, Object, in_object ) )
                {
                    // mob->damage( Damage( 1000.0 ) ); // todo lol
                }

                // state( RETRACTING );

                return( true );
            }
        }
    }

    return( false );
}

Planc Hook::max_rope_length( ) const
{
    return( m_max_rope_length );
}

Planc Hook::rope_length( ) const
{
    return( m_rope_length );
}

void Hook::extend_rope( Planc cref in_length )
{
    m_rope_length = min( m_rope_length + in_length, max_rope_length( ) );
}

void Hook::shorten_rope( Planc cref in_length )
{
    Planc rope_length_start = m_rope_length;

    m_rope_length = max( m_rope_length - in_length, P0 );

    if( taut( ) )
    {
        m_owner->position( hook_base( ) + VectorA( rope( ).angle( ).flipped( ), rope_length( ) ) );
    }
}

Point Hook::hook_base( ) const
{
    if( state( ) == LOADED )
    {
        return( m_owner->position( ) );
    }
    else
    {
        return( hook_tip( ) - VectorA( m_angle, HOOK_LENGTH ) );
    }
}

Point Hook::hook_tip( ) const
{
    return( position( ) );
}

Vector Hook::rope( ) const
{
    return( Vector( m_owner->position( ), hook_base( ) ) );
}

bool Hook::taut( ) const
{
    return( hooked( ) && ( rope( ).magnitude( ) >= rope_length( ) ) );
}

bool Hook::hooked( ) const
{
    return( m_state == HOOKED );
}

bool Hook::loaded( ) const
{
    return( m_state == LOADED );
}

bool Hook::launching( ) const
{
    return( m_state == LOADED );
}

bool Hook::retracting( ) const
{
    return( m_state == RETRACTING );
}

void Hook::launch( Vector cref in_launch_speed )
{
    switch( state( ) )
    {
        default:
        {
            // Assert( "invalid_state" );
        }

        b_case( LOADED ) :
        {
            velocity( in_launch_speed );
            state( LAUNCHING );

            // static SoundClip twang( "twang.wav" );
            // twang.play( );
        }

        b_case( LAUNCHING ) : a_case( RETRACTING ) : a_case( HOOKED ) :
        {

        }
    }
}

void Hook::retract( )
{
    switch( state( ) )
    {
        default:
        {
            // Assert( "invalid_state" );
        }

        b_case( LOADED ) :
        {

        }

        b_case( LAUNCHING ) : a_case( HOOKED ) :
        {
            state( RETRACTING );
        }

        b_case( RETRACTING ) :
        {

        }
    }
}

void Hook::reload( )
{
    clear_ground( );
    state( LOADED );
}
