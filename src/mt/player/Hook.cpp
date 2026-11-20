#include "mt/player/Hook.hpp"

#include "mt/player/Climber.hpp"

// crossbox / hook
var_const( Planc DEFAULT_ROPE_MAX_LENGTH = ( METER * 10.0 ) );
var_const( Planc DEFAULT_ROPE_GROWTH_SPEED = ( 33.0 ) );
var_const( Planc DEFAULT_ROPE_RETRACT_SPEED = ( 64.0 ) );
var_const( dec LAUNCH_GRAVITY_SCALE = ( 0.25 ) );

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

    Assert( ( !is_null( owner( ) ) ), "owner cannot be null" );

    solid( false );
    interactive( false );

    position( owner( )->position( ) );

    state( LOADED );

    rope_growth_speed( DEFAULT_ROPE_GROWTH_SPEED );
    rope_retract_speed( DEFAULT_ROPE_RETRACT_SPEED );
    max_rope_length( DEFAULT_ROPE_MAX_LENGTH );
}

void Hook::render( )
{
    Object::render( );

    Point tip = hook_tip( ) - owner( )->position( );
    Point base = hook_base( ) - owner( )->position( );

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
            angle( velocity( ).angle( ) );
        }
        else if( state( ) == RETRACTING )
        {
            angle( velocity( ).angle( ) + PI );
        }
    }
}

void Hook::update_velocity( )
{
    Object::update_velocity( );

    if( state( ) == LAUNCHING )
    {
        gravity_scale( LAUNCH_GRAVITY_SCALE );
        rope_length( owner( )->position( ).distance_to( position( ) ) );

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
        velocity( VectorA( Angle( position( ), owner( )->position( ) ), rope_retract_speed( ) ) );

        rope_length( rope_length( ) - rope_retract_speed( ) );

        if( rope_length( ) <= HOOK_LENGTH )
        {
            state( LOADED );
        }
    }

    if( state( ) == LOADED ) // C! after RETRACTING, so a hook that finishes retracting is back in place this frame (it used to keep its retracting angle and speed for a frame, with its base already at the climber)
    {
        stow( );
    }
}

bool Hook::collide( ptr<Object> in_object )
{
    if( state( ) != LOADED )
    {
        Object::collide( in_object );

        if( state( ) == LAUNCHING )
        {
            if( in_object->interactive( ) && !cast( in_object, Hook ) )
            {
                if( cast_as( in_object, mob, Object ) )
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

void Hook::ground( Terrain::Bumper cref in_ground )
{
    Object::ground( in_ground );
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
    rope_length( min( rope_length( ) + in_length, max_rope_length( ) ) );
}

void Hook::shorten_rope( Planc cref in_length )
{
    Planc rope_length_start = rope_length( );

    rope_length( max( rope_length( ) - in_length, P0 ) );

    if( taut( ) )
    {
        owner( )->position( hook_base( ) + VectorA( rope( ).angle( ).flipped( ), rope_length( ) ) );
    }
}

Point Hook::hook_base( ) const
{
    if( state( ) == LOADED )
    {
        return( owner( )->position( ) );
    }
    else
    {
        return( hook_tip( ) - VectorA( angle( ), HOOK_LENGTH ) );
    }
}

Point Hook::hook_tip( ) const
{
    return( position( ) );
}

Vector Hook::rope( ) const
{
    return( Vector( owner( )->position( ), hook_base( ) ) );
}

bool Hook::taut( ) const
{
    return( hooked( ) && ( rope( ).magnitude( ) >= rope_length( ) ) );
}

bool Hook::hooked( ) const
{
    return( state( ) == HOOKED );
}

bool Hook::loaded( ) const
{
    return( state( ) == LOADED );
}

bool Hook::launching( ) const
{
    return( state( ) == LAUNCHING );
}

bool Hook::retracting( ) const
{
    return( state( ) == RETRACTING );
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

            // static Sound twang( "twang.wav" );
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

void Hook::stow( )
{
    state( LOADED );
    no_gravity( );
    clear_ground( );
    velocity( V0 );
    angle( owner( )->aim_angle( ) );
    position( owner( )->position( ) + VectorA( angle( ), HOOK_LENGTH ) );
}
