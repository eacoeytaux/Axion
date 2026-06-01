#include "mt/player/Climber.hpp"

#include "axn/world/Terrain.hpp"
#include "mt/player/Arrow.hpp"
#include "mt/environment/misc/Dust.hpp"

var_const( Planc HEIGHT = ( METER * 1.85 ) );
var_const( Planc WIDTH = ( HEIGHT / 2.5 ) );
var_const( Planc WIDTH_GROUND = ( WIDTH * 0.25 ) );

var_const( Planc DEFAULT_SPEED_DASH = ( 2.5 ) );
var_const( Planc DEFAULT_SPEED_GROUND = ( 1.0 ) );
var_const( Planc DEFAULT_SPEED_AIR = ( 0.25 ) );

var_const( uint EXTRA_JUMP_COUNT = ( 2 ) );
var_const( Planc JUMP_STRENGTH = ( METER / 7.0 ) );
var_const( dec JUMP_DEGRATION_RATIO = ( 0.9 ) );
var_const( ticks JUMP_MAX_DURATION = ( 5.0 ) );
var_const( ticks JUMP_RESET_WAIT_TIME = ( 7.0 ) ); // starts after jump duration ends
var_const( ticks COYOTE_JUMP_TIME = ( 3.0 ) );

var_const( ticks DASH_DURATION = ( 3.9 ) );
var_const( ticks DASH_RESET_WAIT_TIME = ( 7.9 ) ); // starts after jump duration ends

var_const( Planc LIGHT_SIGHT = ( HEIGHT * 3.0 ) );

var_const( ticks STEP_DURATION = ( 4.0 ) );
var_const( Planc STEP_DISTANCE = ( 4.0 ) );
var_const( Planc STEP_HEIGHT = ( 2.0 ) );

// crossbox / hook

var_const( Planc DEFAULT_ARROW_LAUNCH_SPEED = ( 16.0 ) );
var_const( dec ARROW_VELOCITY_DAMPEN = ( 0.25 ) );

var_const( Planc DEFAULT_HOOK_LAUNCH_SPEED = ( 32.0 ) );
var_const( uint RELOAD_TIME = ( 20 ) );

var_const( Planc DUST_MINIMUM_SPEED = ( METER / 6.0 ) );
var_const( Span<dec> DUST_VELOCITY_DAMPEN_RATIO = { 0.075, 0.15 } );
var_const( Span<Planc> DUST_RISE = { 0.25, 0.75 } );
var_const( Span<uint> DUST_LANDING_COUNT = { 16, 32 } );
var_const( Span<Angle> DUST_LANDING_ANGLE = { A0, Angle::fraction( 12.0 ) } );
var_const( Span<uint> DUST_WALKING_COUNT = { 3, 7 } );
var_const( Span<ticks> DUST_WALKING_TIME = { 0.0, 3.0 } );

var_const( Planc HEAD_RADIUS = ( HEIGHT / 3.9 ) );
var_const( Planc HEAD_OFFSET_Y = ( HEAD_RADIUS * 0.05 ) );
var_const( Planc FACE_RADIUS = ( HEAD_RADIUS * 0.75 ) );
var_const( Planc EYE_SPACING = ( FACE_RADIUS * 0.55 ) );
var_const( Planc EYE_RADIUS = ( 1.5 ) );
var_const( Planc EYEBROW_LENGTH = ( EYE_RADIUS * 4.33 ) );
var_const( Planc EYEBROW_SPACING_X = ( EYE_RADIUS * ( 2.0 / 3.0 ) ) );
var_const( Planc EYEBROW_SPACING_Y = ( EYE_RADIUS * ( 5.0 / 3.0 ) ) );
var_const( Planc EYEBROW_DEAD_SPACING_Y = ( EYE_RADIUS ) );
var_const( Planc EAR_RADIUS = ( 3.0 ) );

var_const( Planc BEARD_WIDTH = ( FACE_RADIUS * 2.0 ) );
var_const( Planc BEARD_HEIGHT = ( BEARD_WIDTH * 0.5 ) );
var_const( Planc MUSTACHE_WIDTH = ( FACE_RADIUS ) );
var_const( Planc MUSTACHE_HEIGHT = ( MUSTACHE_WIDTH / 3.0 ) );

var_const( Planc BEARD_OFFSET = ( HEAD_RADIUS * 0.75 ) );
var_const( Planc MUSTACHE_OFFSET = ( BEARD_OFFSET * 0.5 ) );
var_const( Planc MOHAWK_WIDTH = ( HEAD_RADIUS * 0.666 ) );
var_const( Planc MOHAWK_HEIGHT = ( MOHAWK_WIDTH * 0.7 ) );
var_const( Planc MOHAWK_OFFSET = ( MOHAWK_HEIGHT * 0.5 ) );

var_const( Planc BODY_HEIGHT = ( HEIGHT / 3.0 ) );
var_const( Planc LEG_HEIGHT = ( HEIGHT / 4.0 ) );

var_const( Planc BODY_WIDTH = ( HEIGHT / 2.0 ) );

var_const( Planc PANTS_WIDTH_UPPER = ( BODY_WIDTH - 6.0 ) );
var_const( Planc PANTS_WIDTH_LOWER = ( PANTS_WIDTH_UPPER * 0.5 ) );
var_const( Planc PANTS_HEIGHT = ( BODY_HEIGHT / 2.0 ) );

var_const( Planc FRONT_UPPER_ARM_LENGTH = ( BODY_HEIGHT * 0.8 ) );
var_const( Planc FRONT_LOWER_ARM_LENGTH = ( FRONT_UPPER_ARM_LENGTH * 0.8 ) );

var_const( Planc BACK_UPPER_ARM_LENGTH = ( BODY_HEIGHT * 0.5 ) );
var_const( Planc BACK_LOWER_ARM_LENGTH = ( BACK_UPPER_ARM_LENGTH ) );

var_const( Planc FRONT_ARM_WIDTH = ( HEIGHT / 6.0 ) );
var_const( Planc FRONT_HAND_WIDTH = ( FRONT_ARM_WIDTH ) );

var_const( Planc BACK_ARM_WIDTH = ( FRONT_ARM_WIDTH * 0.9 ) );
var_const( Planc BACK_HAND_WIDTH = ( BACK_ARM_WIDTH ) );

var_const( Planc FRONT_HAND_CROSSBOW_OFFSET_DISTANCE = ( METER * 0.375 ) );
var_const( Angle FRONT_HAND_CROSSBOW_OFFSET_ANGLE = ( RIGHT ) );

var_const( Planc BACK_HAND_CROSSBOW_OFFSET_DISTANCE = ( METER * 0.75 ) );
var_const( Angle BACK_HAND_CROSSBOW_OFFSET_ANGLE = ( Angle::fraction( 32.0 ) ) );

var_const( Planc FINGER_WIDTH = ( FRONT_ARM_WIDTH / 3.0 ) );
var_const( Planc FINGER_LENGTH_OUT = ( FRONT_ARM_WIDTH * 0.72 ) );
var_const( Planc FINGER_LENGTH_DOWN = ( FRONT_ARM_WIDTH * 0.64 ) );

var_const( uint ELBOW_OFFSET_INDEX = ( 0 ) ); // always zero
var_const( uint HAND_OFFSET_INDEX = ( ELBOW_OFFSET_INDEX + 1 ) );
var_const( uint FINGER_OFFSET_INDEX = ( HAND_OFFSET_INDEX + 1 ) );

var_const( Angle FRONT_ARM_RAGDOLL_ROT_UPPER_LIMIT = ( PI * ( 2.0 / 3.0 ) ) );
var_const( Angle FRONT_ARM_RAGDOLL_ROT_LOWER_LIMIT = ( A0 ) );
var_const( Angle BACK_ARM_RAGDOLL_ROT_LIMIT = ( RIGHT * 1.5 ) );

var_const( Planc SHOULDER_RADIUS = ( FRONT_ARM_WIDTH * 0.5 ) );

var_const( Planc LEG_WIDTH = ( HEIGHT / 5.25 ) );

var_const( Planc BOOT_TOP_WIDTH = ( LEG_WIDTH * 1.3 ) );
var_const( Planc BOOT_TOP_HEIGHT = ( BOOT_TOP_WIDTH * 0.66 ) );
var_const( Planc BOOT_BASE_WIDTH = ( LEG_WIDTH * 1.6 ) );
var_const( Planc BOOT_BASE_HEIGHT = ( BOOT_BASE_WIDTH * 0.3 ) );
var_const( Planc BOOT_SOLE_HEIGHT = ( 1.0 ) ); // todo make relative
var_const( Planc BOOT_SOLE_SUB_WIDTH = ( 3.0 ) );
var_const( Planc BOOT_SOLE_SUB_GAP_WIDTH = ( 1.0 ) );
var_const( Planc BOOT_SOLE_SUB_HEIGHT = ( 0.75 ) );

// todo make relative
var_const( Planc BELT_THICKNESS = ( 3.0 ) );
var_const( Planc BELT_BUCKLE_WIDTH = ( 5.0 ) );

var_const( ticks BOB_PERIOD = ( 20.0 ) );
var_const( Planc HEAD_BOB_DISTANCE = ( 1.0 ) );
var_const( Planc SHOULDER_BOB_DISTANCE = ( 0.8 ) );
var_const( dec SHOULDER_BOB_OFFSET = ( 0.75 ) );

var_const( ticks BLINK_DURATION = ( 8.0 ) );
var_const( Span<ticks> BLINK_WAIT = { 240.0, 480.0 } );

var_const( Planc CROSSBOW_LENGTH = ( METER * 0.9375 ) );
var_const( Polygon CROSSBOW_SHAPE = Polygon( {
    { 0.0, 0.0 },
    { -CROSSBOW_LENGTH / 3.0, -CROSSBOW_LENGTH / 3.0 },
    { -CROSSBOW_LENGTH / 1.5, -CROSSBOW_LENGTH / 3.0 },
    { -CROSSBOW_LENGTH / 1.5, -CROSSBOW_LENGTH / 1.5 },
    { CROSSBOW_LENGTH / 3.0, -CROSSBOW_LENGTH / 6.0 },
    { CROSSBOW_LENGTH / 1.2, -CROSSBOW_LENGTH / 6.0 },
    { CROSSBOW_LENGTH, 0.0 } } ) );

var_const( Color CROSSBOW_COLOR = ( Color::rgb( 0xA54B23 ) ) );

var_const( Angle HOOK_ANGLE = ( RIGHT ) );
var_const( Planc HOOK_LENGTH = ( METER ) );
var_const( Planc HOOK_THICKNESS = ( HOOK_LENGTH / 6.0 ) );
var_const( Planc HOOK_TIP_LENGTH = ( HOOK_THICKNESS * 0.5 ) );
var_const( Color HOOK_COLOR = ( Color::rgb( 0x9C9C9C ) ) );

var_const( Planc ROPE_WIDTH = ( 4.0 ) );
var_const( Color ROPE_BASE_COLOR = ( Color::rgb( 0xDAA420 ) ) );
var_const( Color ROPE_ALT_COLOR = ( Color::rgb( 0xB9870F ) ) );

var_const( Planc PATH_LENGTH = ( METER * 25.0 ) );
var_const( dec PATH_ALPHA_START = ( 0.8 ) );

var_const( Planc ARROW_PATH_LENGTH = ( PATH_LENGTH ) );
var_const( Color HOOK_PATH_COLOR = ( CYAN ) );
var_const( dec ARROW_PATH_ALPHA_START = ( PATH_ALPHA_START ) );
var_const( Color ARROW_PATH_COLOR = ( RED ) );

var_const( Planc HOOK_PATH_LENGTH = ( PATH_LENGTH ) );
var_const( dec HOOK_PATH_ALPHA_START = ( PATH_ALPHA_START ) );

var_const( Span<ticks> PULSE_DURATION = { 16.0, 32.0 } );

var_const( dec SCREEN_FADE_DISTANCE = ( 0.5 ) );

var_const( Color BASE_COLOR = ( RED ) );
var_const( Color TRANSPARENT_COLOR = ( Color::transparent( BASE_COLOR ) ) );
var_const( dec BASE_ALPHA = ( 0.5 ) );

section( HEALTH_HUD )
{
    var_const( dec HEALTH_THRESHOLD_UPPER = ( 0.333 ) );
    var_const( dec HEALTH_THRESHOLD_LOWER = ( 0.1 ) );

    var_const( uint HUD_BORDER_COUNT = ( 3 ) );
    var_const( Planc HUD_BORDER_WIDTH = ( 4.0 ) );

    var_const( dec HEALTH_BAR_WIDTH = ( 0.8 ) );
    var_const( dec HEALTH_BAR_HEIGHT = ( 0.04 ) );

    var_const( dec HEALTH_BAR_RED_START = ( 0.1 ) );
    var_const( dec HEALTH_BAR_YELLOW_START = ( 0.5 ) );
}

section( JUMP_HUD )
{
    var_const( dec JUMP_BAR_RADIUS = ( HEALTH_BAR_HEIGHT * 0.8 ) );
    var_const( dec JUMP_BAR_SPACING_X = ( JUMP_BAR_RADIUS * 0.0 ) );
    var_const( dec JUMP_BAR_SPACING_Y = ( JUMP_BAR_RADIUS * 0.8 ) );
}

section( AIM_HUD )
{
    var_const( dec AIM_DISPLAY_RATIO = ( 0.6 ) );

    var_const( Planc AIM_DISPLAY_RADIUS = ( 200.0 ) );
    var_const( Planc AIM_DISPLAY_THICKNESS = ( 5.0 ) );
    var_const( Angle AIM_DISPLAY_BREADTH = ( Angle::fraction( 7.5 ) ) );

    var_const( Planc AIM_DISPLAY_POINTER_LENGTH = ( 15.0 ) );
    var_const( Angle AIM_DISPLAY_POINTER_ANGLE = ( Angle::fraction( 13.0 ) ) );

    var_const( Color AIM_DISPLAY_COLOR = ( WHITE ) );
    var_const( Color AIM_DISPLAY_BORDER_COLOR = ( BLACK ) );
    var_const( Planc AIM_DISPLAY_BORDER_THICKNESS = ( 2.0 ) );

    var_const( ticks AIM_DISPLAY_DURATION = ( 60.0 ) );
    var_const( ticks AIM_DISPLAY_FADE_DURATION = ( 24.0 ) );
}

Climber::~Climber( )
{
    camera( )->remove_hud_element( &m_healthbar );
    camera( )->remove_hud_element( &m_jumpbar );
    camera( )->remove_hud_element( &m_aim_hud );
    camera( )->remove_screen_effect( &m_low_health_effect );
}

Climber::Climber( uint in_player_number, ptr<Room> in_room, Point cref in_position ) : Player( in_player_number, in_room, in_position ),
m_hook( in_room, this ),
m_healthbar( this ),
m_jumpbar( this ),
m_aim_hud( this ),
m_low_health_effect( this )
{
    needs_render_always( true );

    interactive( true );
    solid( true );

    track_position( 32 ); // todo

    space( Polygon( { Point( half( WIDTH ), half( HEIGHT ) ),
                      Point( -half( WIDTH ), half( HEIGHT ) ),
                      Point( -half( WIDTH_GROUND ), -half( HEIGHT ) ),
                      Point( half( WIDTH_GROUND ), -half( HEIGHT ) ) } ) );

    eye_info( EYE_RADIUS, BLINK_DURATION, BLINK_WAIT );

    m_outfit = MOHAWK;

    m_movement_speed_dash = DEFAULT_SPEED_DASH;
    m_movement_speed_ground = DEFAULT_SPEED_GROUND;
    m_movement_speed_air = DEFAULT_SPEED_AIR;

    extra_jumps( EXTRA_JUMP_COUNT );

    m_hook_reload_timer.reset( 0 );
    m_arrow_reload_timer.reset( 0 );

    m_dust_timer.reset( 0 );

    m_arrow_feather_color = player_color( );

    camera( )->add_hud_element( &m_healthbar );
    camera( )->add_hud_element( &m_jumpbar );
    camera( )->add_hud_element( &m_aim_hud );
    camera( )->add_screen_effect( &m_low_health_effect );
}

void Climber::die( )
{
    m_dead_aim_angle = aim_angle( );

    m_dead_face = RagDollLimb( this, face_center( ) );

    m_dead_face.add_limb( aim_angle( ) );

    m_dead_arm_front = RagDollLimb( this, shoulder( true ) );

    m_dead_arm_front.add_limb( Angle( shoulder( true ), elbow( true ) ), arm_length_upper( true ) );
    m_dead_arm_front.add_limb( Angle( elbow( true ), hand( true ) ), arm_length_lower( true ) );
    m_dead_arm_front.add_limb( aim_angle( ) );

    if( aiming_right( ) )
    {
        m_dead_arm_front.limb_ccw_limit( HAND_OFFSET_INDEX, FRONT_ARM_RAGDOLL_ROT_UPPER_LIMIT );
        m_dead_arm_front.limb_cw_limit( HAND_OFFSET_INDEX, FRONT_ARM_RAGDOLL_ROT_LOWER_LIMIT );
    }
    else
    {
        m_dead_arm_front.limb_cw_limit( HAND_OFFSET_INDEX, FRONT_ARM_RAGDOLL_ROT_UPPER_LIMIT );
        m_dead_arm_front.limb_ccw_limit( HAND_OFFSET_INDEX, FRONT_ARM_RAGDOLL_ROT_LOWER_LIMIT );
    }

    m_dead_arm_back = RagDollLimb( this, shoulder( false ) );

    m_dead_arm_back.add_limb( Angle( shoulder( false ), elbow( false ) ), arm_length_upper( false ) );
    m_dead_arm_back.add_limb( Angle( elbow( false ), hand( false ) ), arm_length_lower( false ) );

    m_dead_arm_back.limb_ccw_limit( HAND_OFFSET_INDEX, BACK_ARM_RAGDOLL_ROT_LIMIT );
    m_dead_arm_back.limb_cw_limit( HAND_OFFSET_INDEX, BACK_ARM_RAGDOLL_ROT_LIMIT );

    clear_input( );

    Player::die( );
}

void Climber::trim_velocity( )
{
    Player::trim_velocity( );

    // rope adjustment
    if( m_hook.taut( ) )
    {
        Vector v = velocity( );
        Vector rope = m_hook.rope( );

        Angle d = ( rope.angle( ) - v.angle( ) ).truncated( );

        // todo align with gravity
        if( ( d >= RIGHT ) && ( d <= ( TAU - RIGHT ) ) )
        {
            Vector v_flat = v.flattened( rope.angle( ) + RIGHT );

            if( v_flat.has_magnitude( ) )
            {
                Planc m = v_flat.magnitude( );

                Angle a( dec( m / rope.magnitude( ) ) );
                a = negated( a, ( d >= PI ) );

                Vector rope_flipped( rope.destination( ), rope.origin( ) );
                rope_flipped.rotate( a );

                velocity( Vector( position( ), rope_flipped.destination( ) ) );
            }
        }
    }
}

void Climber::update( )
{
    if( !alive( ) )
    {
        m_dead_face.update( );
        m_dead_arm_front.update( );
        m_dead_arm_back.update( );
    }
    else
    {
        heal( 0.01 ); // todo

        if( m_aim_display_timer.tick( ) )
        {
            m_aim_display_fade_timer.tick( );
        }

        m_hook_reload_timer.tick( );
        m_arrow_reload_timer.tick( );

        m_dust_timer.tick( );

        if( grounded( ) )
        {
            m_coyote_jump_timer.reset( COYOTE_JUMP_TIME );
        }
        else
        {
            m_coyote_jump_timer.tick( );
        }

        Vector cref v = velocity( );
        if( v.has_magnitude( ) )
        {
            if( v.magnitude( ) >= DUST_MINIMUM_SPEED )
            {
                if( grounded( ) )
                {
                    if( m_dust_timer.completed( ) )
                    {
                        m_dust_timer.reset( Random::rdec( DUST_WALKING_TIME ) );

                        uint dust_count = Random::rint( DUST_WALKING_COUNT );
                        do_count( dust_count )
                        {
                            // room( )->add_object( new Dust( room( ),
                            //                               position( ) + foot( true ),
                            //                               ( velocity( ) * Random::rdec( DUST_VELOCITY_DAMPEN_RATIO ) ) + VectorY( Random::rPlanc( DUST_RISE ) ),
                            //                               room( )->terrain( )->dust_color( ) ) );
                        }
                    }
                }
            }
        }

        if( m_launching_hook )
        {
            if( m_hook_reload_timer.completed( ) )
            {
                if( m_hook.loaded( ) )
                {
                    // static SoundClip twang( "twang.wav" );
                    // twang.play( 0.5 );

                    m_hook.launch( VectorA( aim_angle( ), DEFAULT_HOOK_LAUNCH_SPEED ) + ( velocity( ) * ARROW_VELOCITY_DAMPEN ) );
                }
            }
        }
        else
        {
            m_hook.retract( );
        }

        if( m_hook.hooked( ) )
        {
            if( looking_up( ) )
            {
                m_hook.shorten_rope( 3.0 ); // todo
            }

            if( looking_down( ) )
            {
                m_hook.extend_rope( 5.0 ); // todo
            }
        }

        if( m_launching_arrow )
        {
            if( m_arrow_reload_timer.completed( ) )
            {
                // static SoundClip twang( "twang.wav" );
                // twang.play( 0.5 );

                Angle aim = aim_angle( ) + aim_shake( );
                Arrow * arrow = new Arrow( Arrow::tip( room( ), position( ), VectorA( aim, DEFAULT_ARROW_LAUNCH_SPEED ) + ( velocity( ) * ARROW_VELOCITY_DAMPEN ), m_arrow_feather_color ) );
                arrow->position( arrow->position( ) + VectorA( aim_angle( ), arrow->length( ) ) );

                room( )->add_object( arrow );

                m_arrow_feather_color = Random::rColor( );

                m_arrow_reload_timer.reset( RELOAD_TIME );
            }
        }

        if( m_slashing )
        {
            // todo
        }
    }

    Player::update( );
    m_hook.update( );

    if( passing_terrain( ) )
    {
        passing_terrain( false );
    }
}

void Climber::update_velocity( )
{
    Player::update_velocity( );

    bool add_dust = false;
    bool add_clouds = false;

    Planc movement_speed = ( grounded( ) ? ( m_movement_speed_ground ) : ( m_movement_speed_air ) );

    if( !has_gravity( ) )
    { // can fly?
        if( looking_up( ) )
        {
            if( moving_right( ) )
            {
                add_velocity( VectorA( half( RIGHT ), movement_speed ) );
            }
            else if( moving_left( ) )
            {
                add_velocity( VectorA( half( RIGHT ) + RIGHT, movement_speed ) );
            }
            else
            {
                add_velocity( VectorY( movement_speed ) );
            }
        }
        else if( looking_down( ) )
        {
            if( moving_right( ) )
            {
                add_velocity( VectorA( -half( RIGHT ), movement_speed ) );
            }
            else if( moving_left( ) )
            {
                add_velocity( VectorA( -half( RIGHT ) - RIGHT, movement_speed ) );
            }
            else
            {
                add_velocity( -VectorY( movement_speed ) );
            }
        }
        else
        {
            if( moving_right( ) )
            {
                add_velocity( VectorX( movement_speed ) );
            }
            else if( moving_left( ) )
            {
                add_velocity( VectorX( -movement_speed ) );
            }
        }
    }
    else
    {
        if( m_hook.taut( ) )
        {
            movement_speed *= 2.0; // todo
        }

        Vector v;

        if( moving_right( ) )
        {
            v = VectorX( movement_speed );
        }
        else if( moving_left( ) )
        {
            v = VectorX( -movement_speed );
        }

        if( v.has_magnitude( ) )
        {
            if( grounded( ) )
            {
                Angle a1, a2;
                Angle va = v.angle( );

                Line bumper_line;
                varray<Line> bumper_lines = Object::ground( )->bumpers( terrain_hitbox( ) ).path( ).lines( );
                for_each( line, bumper_lines )
                {
                    if( line.on( position( ) ) )
                    {
                        bumper_line = line;
                        break;
                    }
                }

                if( bumper_line.vertical( ) )
                {
                    if( bumper_line.pt1( ).y( ) > bumper_line.pt2( ).y( ) )
                    {
                        if( is_pos( v.dx( ) ) )
                        {
                            v.rotate_to( RIGHT );
                        }
                        else
                        {
                            v.rotate_to( -RIGHT );
                        }
                    }
                    else
                    {
                        if( is_pos( v.dx( ) ) )
                        {
                            v.rotate_to( -RIGHT );
                        }
                        else
                        {
                            v.rotate_to( RIGHT );
                        }
                    }
                }
                else
                {
                    a1 = bumper_line.angle( );
                    a2 = bumper_line.angle( ).flipped( );

                    v.rotate_to( va.closest( { a1, a2 } ) );
                }
            }

            add_velocity( v );
        }

        if( jumping( ) )
        {
            if( can_jump( ) || is_pos( m_extra_jumps ) )
            {
                if( m_jump_reset_timer.completed( ) )
                {
                    m_jumping_timer.reset( JUMP_MAX_DURATION );
                    m_jump_reset_timer.reset( JUMP_RESET_WAIT_TIME + JUMP_MAX_DURATION );
                    m_jump_degradation = 0.0;

                    if( !grounded( ) )
                    {
                        m_extra_jumps--;

                        add_clouds = true;
                    }
                }
            }

            if( !m_jumping_timer.tick( ) )
            {
                add_velocity( ( VectorA( gravity( ).angle( ).flipped( ), JUMP_STRENGTH ) - gravity( ) ) * one_minus( m_jump_degradation ) );

                m_jump_degradation = one_minus( one_minus( m_jump_degradation ) * JUMP_DEGRATION_RATIO );

                ground( nullptr, Terrain::Bumper( ) );
            }
            else
            {
                jump( false );

                m_jump_reset_timer.reset( JUMP_RESET_WAIT_TIME );
            }
        }
        else
        {
            m_jump_reset_timer.tick( );
        }
    }

    if( !m_dashing_timer.tick( ) )
    {
        if( moving_right( ) )
        {
            add_velocity( VectorX( m_movement_speed_dash ) );
        }
        else if( moving_left( ) )
        {
            add_velocity( VectorX( -m_movement_speed_dash ) );
        }

        m_jump_reset_timer.reset( DASH_RESET_WAIT_TIME );
    }
    else
    {
        m_dash_reset_timer.tick( );
    }

    if( add_dust )
    {
        // todo
    }

    if( add_clouds )
    {
        Vector dust_velocity = ( velocity( ).dy( 0.0 ) * Random::rdec( DUST_VELOCITY_DAMPEN_RATIO ) );
        uint dust_count = Random::rint( DUST_LANDING_COUNT );
        for_range( i, dust_count )
        {
            Angle a = Random::rAngle( DUST_LANDING_ANGLE.min( ), DUST_LANDING_ANGLE.max( ) );
            if( ( i < half( dust_count ) ) ^ facing_left( ) )
            {
                a = ( PI - a );
            }

            room( )->add_particle( new Dust( room( ), ( position( ) ), ( dust_velocity + VectorA( a, Random::rPlanc( DUST_RISE ) ) ), WHITE ) );
        }
    }
}

dec Climber::check_movement( Vector cref in_velocity )
{
    if( in_velocity.has_magnitude( ) )
    {
        if( m_hook.hooked( ) && !m_hook.taut( ) )
        {
            Vector rope = m_hook.rope( );

            bool now_in_range = rope.destination( ).in_distance_range( position( ), hook( ).rope_length( ) );
            bool will_be_in_range = rope.destination( ).in_distance_range( position( ) + in_velocity, m_hook.rope_length( ) );

            if( now_in_range && !will_be_in_range )
            {
                Polygon rope_radius = Polygon::circle( hook( ).rope_length( ), rope.origin( ) );
                varray<Line> intersections = rope_radius.intersection( Line( position( ), position( ) + in_velocity ) );

                for_each( line, intersections )
                {
                    if( line.pt1( ) == center( ) )
                    {
                        return( line.length( ) / in_velocity.magnitude( ) );
                    }
                }
            }
        }
    }

    return( Player::check_movement( in_velocity ) );
}

dec Climber::air_resistance( ) const
{
    if( hook( ).taut( ) )
    {
        return( 0.01 ); // TODO
    }

    return( Player::air_resistance( ) );
}

Planc Climber::light_sight( ) const
{
    return( LIGHT_SIGHT );
}

bool Climber::opening_door( ) const { return( grounded( ) && looking_up( ) ); }

void Climber::movement_stop( )
{
    moving_right( false );
    moving_left( false );
    looking_up( false );
    looking_down( false );
}

bool Climber::moving_right( ) const { return( m_moving_right && !m_moving_left ); }
void Climber::moving_right( bool in_moving )
{
    if( in_moving )
    {
        if( alive( ) )
        {
            m_moving_right = true;

            moving_left( false );
        }
    }
    else
    {
        m_moving_right = false;
    }
}

bool Climber::moving_left( ) const { return( m_moving_left && !m_moving_right ); }
void Climber::moving_left( bool in_moving )
{
    if( in_moving )
    {
        if( alive( ) )
        {
            m_moving_left = true;

            moving_right( false );
        }
    }
    else
    {
        m_moving_left = false;
    }
}

bool Climber::looking_up( ) const { return( m_looking_up && !m_looking_down ); }
void Climber::looking_up( bool in_looking_up )
{
    if( in_looking_up )
    {
        if( alive( ) )
        {
            m_looking_up = true;

            looking_down( false );
        }
    }
    else
    {
        m_looking_up = false;
    }
}

bool Climber::looking_down( ) const { return( m_looking_down && !m_looking_up ); }
void Climber::looking_down( bool in_looking_down )
{
    if( in_looking_down )
    {
        if( alive( ) )
        {
            m_looking_down = true;

            looking_up( false );
        }
    }
    else
    {
        m_looking_down = false;
    }
}

bool Climber::dashing( ) const
{
    return( !m_dashing_timer.completed( ) );
}

bool Climber::jumping( ) const
{
    return( m_jumping );
}

bool Climber::can_jump( ) const
{
    return( grounded( ) || !m_coyote_jump_timer.completed( ) );
}

uint Climber::extra_jumps( ) const
{
    return( m_extra_jumps );
}

uint Climber::extra_jumps_max( ) const
{
    return( m_extra_jumps_max );
}

void Climber::ground( ptr<Terrain::Node> ground, Terrain::Bumper cref in_bumper )
{
    if( ground )
    {
        if( !grounded( ) )
        {
            Vector dust_velocity = ( velocity( ).dy( 0.0 ) * Random::rdec( DUST_VELOCITY_DAMPEN_RATIO ) );
            uint dust_count = Random::rint( DUST_LANDING_COUNT );
            for_range( i, dust_count )
            {
                Point pt = position( ) + foot( true ) + ( Vector( foot( true ), foot( false ) ) * ( dec( i ) / dec( dust_count ) ) );
                Angle a = ground->normal( ) + Random::rAngle( DUST_LANDING_ANGLE.min( ), DUST_LANDING_ANGLE.max( ) );
                // todo shouldn't start from normal not right?

                if( ( i < half( dust_count ) ) ^ facing_left( ) )
                {
                    a = ( PI - a );
                }

                // todo room( )->add_object( new Dust( room( ), pt, dust_velocity + VectorA( a, Random::rPlanc( DUST_RISE ) ), room( )->terrain( )->dust_color( ) ) );
            }

            if( velocity( ).flattened( ground->normal( ) ).magnitude( ) > 16.0 ) // todo set min bounce mag
            {
                add_velocity( VectorA( velocity( ).angle( ).reflect( ground->normal( ) ).flip( ), ( velocity( ).magnitude( ) ) ) - velocity( ) );
            }
            else
            {
                m_extra_jumps = m_extra_jumps_max;

                Object::ground( ground, in_bumper );
            }
        }
        else
        {
            Object::ground( ground, in_bumper );
        }
    }
    else
    {
        Object::ground( ground, in_bumper );
    }
}

void Climber::dash( bool in_dashing )
{
    if( in_dashing )
    {
        if( alive( ) && m_dashing_timer.completed( ) && m_dash_reset_timer.completed( ) )
        {
            m_dashing_timer.reset( DASH_DURATION );
        }
    }
    else
    {
        m_dashing_timer.complete( );
    }
}

void Climber::jump( bool in_jumping )
{
    if( in_jumping )
    {
        if( alive( ) )
        {
            if( looking_down( ) )
            {
                passing_terrain( true );
            }
            else
            {
                m_jumping = true;
            }
        }
    }
    else
    {
        m_jumping = false;
        m_jumping_timer.complete( );
    }
}

void Climber::extra_jumps( const uint in_jumps )
{
    m_extra_jumps_max = in_jumps;

    m_jumpbar = JumpHUD( this );
}

Angle Climber::aim_target( ) const
{
    return( m_aim_target );
}

Angle Climber::aim_angle( ) const
{
    return( alive( ) ? ( m_aim_angle ) : ( m_dead_arm_front.angle( FINGER_OFFSET_INDEX ) ) );
}

bool Climber::aiming_right( ) const
{
    return( !aiming_left( ) );
}

bool Climber::aiming_left( ) const
{
    return( ( aim_angle( ).truncated( ) > RIGHT ) && ( aim_angle( ).truncated( ) < ( PI + RIGHT ) ) );
}

bool Climber::aiming_up( ) const
{
    return( aim_angle( ).in_quadrant( Q1, false ) || aim_angle( ).in_quadrant( Q2, false ) || ( aim_angle( ) == RIGHT ) );
}

bool Climber::aiming_down( ) const
{
    return( !aiming_up( ) );
}

void Climber::aim( Angle cref in_aim_angle )
{
    return_if( dead( ) );

    if( m_aim_angle != in_aim_angle )
    {
        m_aim_target = in_aim_angle;
        m_aim_angle = in_aim_angle;

        m_aim_display_timer.reset( AIM_DISPLAY_DURATION );
        m_aim_display_fade_timer.reset( AIM_DISPLAY_FADE_DURATION );
    }
}

void Climber::aim_shake( Angle cref in_aim_shake_angle )
{
    m_aim_shake_angle = in_aim_shake_angle;
}

Angle Climber::aim_shake( ) const
{
    return( Random::negated( half( Random::rAngle( m_aim_shake_angle ) ) ) );
}

Angle Climber::aim_shake_range( ) const
{
    return( m_aim_shake_angle );
}

void Climber::launch_hook( )
{
    if( alive( ) )
    {
        m_launching_hook = true;
    }
}

void Climber::launch_arrow( )
{
    if( alive( ) )
    {
        m_launching_arrow = true;
    }
}

void Climber::release_hook( )
{
    if( alive( ) )
    {
        m_launching_hook = false;
    }
}

void Climber::release_arrow( )
{
    if( alive( ) )
    {
        m_launching_arrow = false;
    }
}

void Climber::release_bow( )
{
    release_hook( );
    release_arrow( );
}

Point Climber::head_center( ) const
{
    // todo make cycle
    return( PointY( BODY_HEIGHT + HEAD_OFFSET_Y + ( HEAD_BOB_DISTANCE * ( alive( ) ? sin( dec( age( ) ) / BOB_PERIOD ) : -1.0 ) ) ) );
}

Point Climber::face_center( ) const
{
    return( head_center( ) + VectorA( alive( ) ? aim_angle( ) : m_dead_arm_back.angle( 0 ), half( HEAD_RADIUS - FACE_RADIUS ) ) );
}

Point Climber::shoulder( bool in_front ) const
{
    // todo make cycle
    return( Point( negated(
        half( BODY_WIDTH - x2( SHOULDER_RADIUS ) ), ( aiming_left( ) ^ in_front ) ),
        half( BODY_HEIGHT - x2( SHOULDER_RADIUS ) ) + ( SHOULDER_BOB_DISTANCE * ( alive( ) ? sin( dec( age( ) + ( BOB_PERIOD * SHOULDER_BOB_OFFSET ) ) / BOB_PERIOD ) : -1.0 ) ) ) );
}

Point Climber::elbow( bool in_front ) const
{
    if( alive( ) )
    {
        return( Joint( shoulder( in_front ), hand( in_front ), arm_length_upper( in_front ), arm_length_lower( in_front ), !( aiming_left( ) ) ).joint( ) );
    }
    else
    {
        return( in_front ? m_dead_arm_front.position( ELBOW_OFFSET_INDEX ) : m_dead_arm_back.position( ELBOW_OFFSET_INDEX ) );
    }
}

Point Climber::hand( bool in_front ) const
{
    if( alive( ) )
    {
        return( -hand_to_crossbow_offset( in_front ) );
    }
    else
    {
        return( in_front ? m_dead_arm_front.position( HAND_OFFSET_INDEX ) : m_dead_arm_back.position( HAND_OFFSET_INDEX ) );
    }
}

Point Climber::hand_to_crossbow_offset( bool in_front ) const
{
    return( VectorA( aim_angle( ) - negated( in_front ? FRONT_HAND_CROSSBOW_OFFSET_ANGLE : BACK_HAND_CROSSBOW_OFFSET_ANGLE, aiming_left( ) ),
                     negated( in_front ? FRONT_HAND_CROSSBOW_OFFSET_DISTANCE : BACK_HAND_CROSSBOW_OFFSET_DISTANCE ) ) );
}

Planc Climber::arm_length_upper( bool in_front ) const
{
    return( in_front ? FRONT_UPPER_ARM_LENGTH : BACK_UPPER_ARM_LENGTH );
}

Planc Climber::arm_length_lower( bool in_front ) const
{
    return( in_front ? FRONT_LOWER_ARM_LENGTH : BACK_LOWER_ARM_LENGTH );
}

Planc Climber::arm_width( bool in_front ) const
{
    return( in_front ? FRONT_ARM_WIDTH : BACK_ARM_WIDTH );
}

Point Climber::hip( bool in_front ) const
{
    return( Point( negated( half( PANTS_WIDTH_UPPER ) - half( LEG_WIDTH ), ( facing_left( ) ^ in_front ) ), -half( BODY_HEIGHT ) ) );
}

Point Climber::foot( bool in_front ) const
{
    return( Point( hip( in_front ).x( ), -half( HEIGHT ) ) );
}

// -- GRAPHICS --

Climber::Outfit Climber::outfit( ) const
{
    return( m_outfit );
}

Color Climber::color( ColorPiece component ) const
{
    switch( component )
    {
        default:
        {
            return( WHITE );
        }

        b_case( SKIN ) :
        {
            switch( outfit( ) )
            {
                d_case( NO_OUTFIT ) :
                {
                    return( WHITE );
                }

                b_case( MOHAWK ) :
                {
                    return( Color::rgb( 0xFFE1BE ) );
                }
            }
        }

        b_case( HAIR ) :
        {
            switch( outfit( ) )
            {
                d_case( NO_OUTFIT ) :
                {
                    return( BLACK );
                }

                b_case( MOHAWK ) :
                {
                    return( RED );
                }
            }
        }

        b_case( HAIR_SHAVED ) :
        {
            switch( outfit( ) )
            {
                d_case( NO_OUTFIT ) :
                {
                    return( BLACK );
                    break;
                }

                b_case( MOHAWK ) :
                {
                    return( RED.a( 0.375 ) );
                }
            }
        }

        b_case( EYE ) :
        {
            return( BLACK );
        }

        b_case( UNDERSHIRT ) :
        {
            return( GRAY_LIGHT );
        }

        b_case( JACKET ) :
        {
            return( Color::rgb( 0x8C4B2D ) );
        }

        b_case( PANTS ) :
        {
            return( Color::rgb( 0xCD8741 ) );
        }

        b_case( BELT ) :
        {
            return( BLACK );
        }

        b_case( BELT_BUCKLE ) :
        {
            return( YELLOW );
        }

        b_case( BOOT ) :
        {
            return( BLACK );
        }

        b_case( BOOT_SOLE ) :
        {
            return( GRAY_MID );
        }

        b_case( BOOT_LACE ) :
        {
            return( WHITE );
        }
    }
}

#if defined( AXN_DEBUG )
Drawing Climber::debug_overlay( ) const
{
    Drawing debug_overlay;

    if( !m_hook.loaded( ) )
    {
        debug_overlay.draw( m_hook.debug_overlay( ).move( m_hook.position( ) - position( ) ) );
    }

    return( debug_overlay.draw( Player::debug_overlay( ) ) );
}
#endif

void Climber::render( )
{
    Player::render( );
    m_hook.render( );

    draw_rope( );
    draw_legs( );
    draw_torso( );
    draw_arm_back( );
    draw_head( );
    draw_hook( );
    draw_crossbow( );
    draw_arm_front( );
    draw_arrow( );
}

void Climber::draw_head( )
{
    // head
    draw( color( SKIN ), Polygon::circle( HEAD_RADIUS, head_center( ) ) );

    if( m_outfit == MOHAWK )
    {
        // shaved hair
        draw( color( HAIR_SHAVED ), Polygon( Arc::semi_ccw( HEAD_RADIUS, head_center( ) ).points( ) ) );
    }

    // ears
    draw( color( SKIN ), Polygon::circle( EAR_RADIUS, head_center( ) + VectorX( HEAD_RADIUS - EAR_RADIUS ) ) );
    draw( color( SKIN ), Polygon::circle( EAR_RADIUS, head_center( ) - VectorX( HEAD_RADIUS - EAR_RADIUS ) ) );

    // face
    draw( color( SKIN ), Polygon::circle( FACE_RADIUS, face_center( ) ) );

    // eyes
    draw_eyes( face_center( ) + VectorX( EYE_SPACING ), face_center( ) + VectorX( -EYE_SPACING ) );

    // eyebrows
    Planc y_offset = EYEBROW_SPACING_Y;
    if( dead( ) )
    {
        y_offset += EYEBROW_DEAD_SPACING_Y;
    }

    draw( color( HAIR ), Line( face_center( ) + Vector( +EYE_SPACING + half( EYEBROW_LENGTH - EYEBROW_SPACING_X ), y_offset ),
                               face_center( ) + Vector( +EYE_SPACING - half( EYEBROW_LENGTH + EYEBROW_SPACING_X ), y_offset ) ), EYE_RADIUS + EYE_RADIUS );
    draw( color( HAIR ), Line( face_center( ) + Vector( -EYE_SPACING + half( EYEBROW_LENGTH + EYEBROW_SPACING_X ), y_offset ),
                               face_center( ) + Vector( -EYE_SPACING - half( EYEBROW_LENGTH - EYEBROW_SPACING_X ), y_offset ) ), EYE_RADIUS + EYE_RADIUS );

    if( m_outfit == MOHAWK )
    {
        { // mohawk
            draw( color( HAIR ), Line( head_center( ) + VectorY( HEAD_RADIUS + ( MOHAWK_HEIGHT - MOHAWK_OFFSET ) ),
                                       face_center( ) + VectorY( FACE_RADIUS - ( MOHAWK_OFFSET ) ) ), MOHAWK_WIDTH );
        }

        { // beard
            draw( color( HAIR ), Polygon::rectangle( BEARD_WIDTH, BEARD_HEIGHT, face_center( ) - VectorY( BEARD_OFFSET ) ) );
            draw( color( HAIR ), Polygon::rectangle( MUSTACHE_WIDTH, MUSTACHE_HEIGHT, face_center( ) - VectorY( MUSTACHE_OFFSET ) ) );
        }
    }
}

void Climber::draw_torso( )
{
    Point waist = PointY( -half( BODY_HEIGHT ) );

    // pants base
    draw( color( PANTS ), Polygon( { waist + PointX( +half( PANTS_WIDTH_UPPER ) ),
                                     waist + PointX( -half( PANTS_WIDTH_UPPER ) ),
                                     waist + Point( -half( PANTS_WIDTH_LOWER ), -half( PANTS_HEIGHT ) ),
                                     waist + Point( +half( PANTS_WIDTH_LOWER ), -half( PANTS_HEIGHT ) ) } ) );

    // undershirt
    draw( color( UNDERSHIRT ), Polygon::rectangle( half( BODY_WIDTH ), BODY_HEIGHT ) );

    // belt
    draw( color( BELT ), Line( waist - VectorX( half( PANTS_WIDTH_UPPER ) + ( BELT_THICKNESS / 4.0 ) ),
                               waist + VectorX( half( PANTS_WIDTH_UPPER ) + ( BELT_THICKNESS / 4.0 ) ) ), dec( BELT_THICKNESS ) );

    draw( color( BELT_BUCKLE ), Polygon::rectangle( BELT_BUCKLE_WIDTH, BELT_BUCKLE_WIDTH, waist ) );

    // jacket
    draw( color( JACKET ), Polygon::circle( half( BACK_ARM_WIDTH ), shoulder( false ) ) );
    draw( color( JACKET ), Polygon::rectangle( BODY_WIDTH / 3.0, BODY_HEIGHT - SHOULDER_RADIUS, Point( BODY_WIDTH / 3.5, -half( SHOULDER_RADIUS ) ) ) );
    draw( color( JACKET ), Polygon::rectangle( BODY_WIDTH / 3.0, BODY_HEIGHT - SHOULDER_RADIUS, Point( -BODY_WIDTH / 3.5, -half( SHOULDER_RADIUS ) ) ) );
}

void Climber::draw_legs( )
{
    Drawing boot_drawing;

    Planc offset_base_x = half( BOOT_BASE_WIDTH - BOOT_TOP_WIDTH );
    Polygon boot_sole = Polygon::rectangle( BOOT_BASE_WIDTH, BOOT_SOLE_HEIGHT, Point( offset_base_x, -half( BOOT_SOLE_HEIGHT ) ) );

    boot_drawing.draw( color( BOOT_SOLE ), boot_sole );
    {
        Polygon boot_sub_sole = Polygon::rectangle( BOOT_SOLE_SUB_WIDTH, BOOT_SOLE_SUB_HEIGHT, Point( offset_base_x - half( BOOT_BASE_WIDTH ) + half( BOOT_SOLE_SUB_WIDTH ), -BOOT_SOLE_SUB_HEIGHT - half( BOOT_SOLE_HEIGHT ) ) );
        boot_drawing.draw( color( BOOT_SOLE ), boot_sub_sole );
        for_range( i, floor( half( half( BOOT_BASE_WIDTH ) ) ) )
        {
            Polygon boot_sub_sole = Polygon::rectangle( BOOT_SOLE_SUB_WIDTH, BOOT_SOLE_SUB_HEIGHT, Point( offset_base_x + half( BOOT_BASE_WIDTH ) - half( BOOT_SOLE_SUB_WIDTH ) - ( ( BOOT_SOLE_SUB_WIDTH + BOOT_SOLE_SUB_GAP_WIDTH ) * Planc( i ) ), -BOOT_SOLE_SUB_HEIGHT - half( BOOT_SOLE_HEIGHT ) ) );
            boot_drawing.draw( color( BOOT_SOLE ), boot_sub_sole );
        }
    }

    Polygon boot_shin = Polygon::rectangle( BOOT_TOP_WIDTH, BOOT_TOP_HEIGHT, PointY( half( BOOT_TOP_HEIGHT ) ) );
    Polygon boot_base = Polygon::rectangle( BOOT_BASE_WIDTH, BOOT_BASE_HEIGHT, Point( offset_base_x, half( BOOT_BASE_HEIGHT ) ) );

    boot_drawing.draw( color( BOOT ), boot_shin );
    boot_drawing.draw( color( BOOT ), boot_base );

    Line lace1 = Line( Point( 5.0, 7.5 ), Point( -3.0, 7.5 ) );
    Line lace2 = Line( Point( 5.0, 6.0 ), Point( -3.0, 6.0 ) );

    boot_drawing.draw( color( BOOT_LACE ), lace1, 1.0 );
    boot_drawing.draw( color( BOOT_LACE ), lace2, 1.0 );

    // todo turn in Cycles

    Cycle foot_cycle_x = Cycle( STEP_DURATION, STEP_DISTANCE );
    Cycle foot_cycle_y = Cycle( STEP_DURATION, STEP_HEIGHT, Angle::fraction( 0.75 ) );

    Planc foot_offset_x = foot_cycle_x.at( age( ) );
    Planc foot_offset_y = foot_cycle_y.at( age( ) );;
    Planc foot_offset_y_back = max( -foot_offset_y, P0 );
    Planc foot_offset_y_front = max( foot_offset_y, P0 );

    { // back leg
        Point hip = Climber::hip( false );
        Point foot = Climber::foot( false );

        if( grounded( ) )
        {
            if( moving_right( ) ^ moving_left( ) )
            {
                if( moving_right( ) )
                {
                    foot += Vector( foot_offset_x, foot_offset_y_back );
                }
                else if( moving_left( ) )
                {
                    foot += Vector( -foot_offset_x, foot_offset_y_back );
                }
            }
        }
        else if( alive( ) && !god( ) )
        {
            foot += VectorY( LEG_HEIGHT / 4.0 ); // todo
        }

        Drawing boot_drawing_back = boot_drawing;

        if( facing_left( ) )
        {
            boot_drawing_back.mirror_y( );
        }

        boot_drawing_back.move( foot );

        draw( color( PANTS ), Line( hip, foot ), LEG_WIDTH );
        draw( boot_drawing_back );
    }

    { // front leg
        Point hip = Climber::hip( true );
        Point foot = Climber::foot( true );

        if( grounded( ) )
        {
            if( moving_right( ) ^ moving_left( ) )
            {
                if( moving_right( ) )
                {
                    foot += Vector( -foot_offset_x, foot_offset_y_front );
                }
                else if( moving_left( ) )
                {
                    foot += Vector( foot_offset_x, foot_offset_y_front );
                }
            }
        }

        Drawing boot_drawing_front = boot_drawing;

        if( facing_left( ) )
        {
            boot_drawing_front.mirror_y( );
        }

        boot_drawing_front.move( foot );

        draw( color( PANTS ), Line( hip, foot ), LEG_WIDTH );
        draw( boot_drawing_front );
    }
}

void Climber::draw_arm( bool in_front )
{
    draw( color( JACKET ), Polygon::circle( SHOULDER_RADIUS, shoulder( in_front ) ) );
    draw( color( JACKET ), Polygon::circle( half( arm_width( in_front ) ), elbow( in_front ) ) );

    draw( color( JACKET ), Line( shoulder( in_front ), elbow( in_front ) ), arm_width( in_front ) );
    draw( color( JACKET ), Line( elbow( in_front ), hand( in_front ) ), arm_width( in_front ) );

    draw_hand( in_front );
}

void Climber::draw_hand( bool in_front )
{
    if( in_front )
    {
        bool finger_down = ( ( m_arrow_reload_timer.remaining( ) > half( RELOAD_TIME ) ) || ( m_hook_reload_timer.remaining( ) > half( RELOAD_TIME ) ) );

        Point hand = Climber::hand( true );

        Point finger_base = hand + VectorA( aim_angle( ) + negated( RIGHT, aiming_left( ) ), half( FRONT_ARM_WIDTH - FINGER_WIDTH ) );
        Point finger_tip = finger_base + VectorA( aim_angle( ), finger_down ? FINGER_LENGTH_DOWN : FINGER_LENGTH_OUT );

        draw( color( SKIN ), Polygon::circle( half( FRONT_HAND_WIDTH ), hand ) );
        draw( color( SKIN ), Line( finger_base, finger_tip ), FINGER_WIDTH );
    }
    else
    {
        draw( color( SKIN ), Polygon::circle( half( BACK_HAND_WIDTH ), hand( false ) ) );
    }
}

void Climber::draw_rope( )
{
    Point hook_base = hook( ).hook_base( ) - position( );

    Vector rope_vector( hook_base );
    Line rope( rope_vector );

    // draw rope base
    draw( ROPE_BASE_COLOR, rope, ROPE_WIDTH );

    { // draw rope detail coils
        Vector rope_chunk = VectorA( rope_vector.angle( ), ROPE_WIDTH );
        for_range( i, half( rope_vector.magnitude( ) / rope_chunk.magnitude( ) ) )
        {
            Polygon rope_strip_rect = Polygon::rectangle( ROPE_WIDTH, ROPE_WIDTH, hook_base - ( rope_chunk * ( i + 1 ) * 2 ), rope_chunk.angle( ) );
            draw( ROPE_ALT_COLOR, rope_strip_rect );
        }
    }

    #if defined( AXN_DEBUG )
    // if( Debug::active )
    {
        if( hook( ).taut( ) )
        {
            draw( RED.a( 0.5 ), rope, ROPE_WIDTH );
        }
    }
    #endif
}

void Climber::draw_hook( )
{
    draw( Drawing( m_hook ).move( hook( ).loaded( ) ? ( hand( true ) + hand_to_crossbow_offset( true ) ) : ORIGIN ) );
}

void Climber::draw_arrow( )
{
    Arrow arrow = Arrow::tip( room( ), position( ), VectorA( aim_angle( ), DEFAULT_ARROW_LAUNCH_SPEED ), m_arrow_feather_color );

    Vector offset = VectorA( aim_angle( ), arrow.length( ) ) + ( hand( true ) + hand_to_crossbow_offset( true ) );

    #if defined( AXN_DEBUG )
    if( Debug::active && Settings::get( Settings::DEBUG_PHYSICS ) )
    {
        if( alive( ) )
        {
            draw( arrow.trajection_drawing( ARROW_PATH_LENGTH, ARROW_PATH_COLOR, ARROW_PATH_ALPHA_START ).move( offset ) );
        }
    }
    #endif

    arrow.render( );

    draw( Drawing( arrow ).move( offset ) );
}

void Climber::draw_crossbow( )
{
    Polygon crossbow = CROSSBOW_SHAPE;

    if( aiming_left( ) ) { crossbow.mirror_x( ); }

    crossbow.rotate( aim_angle( ) );
    crossbow.move( hand( true ) + hand_to_crossbow_offset( true ) );

    draw( CROSSBOW_COLOR, crossbow );
}

Climber::HealthHUD::HealthHUD( ptr<Climber> in_climber ) : HUD( 0.5, half( HEALTH_BAR_HEIGHT ), HEALTH_BAR_WIDTH, HEALTH_BAR_HEIGHT ), m_climber( in_climber )
{
    static_once{ Assert( HEALTH_BAR_RED_START < HEALTH_BAR_YELLOW_START ); }
}

void Climber::HealthHUD::render( ptr<Camera> in_camera )
{
    HUD::render( in_camera );

    if( ptr<Climber> climber = m_climber )
    {
        FixedRectangle cref rect = bounds( in_camera );

        Planc width = rect.width( );
        Planc height = rect.height( );
        Point center = rect.center( );

        // border
        for_range( i, HUD_BORDER_COUNT )
        {
            draw( ( ( ( i % 2 ) ^ !( HUD_BORDER_COUNT % 2 ) ) ? WHITE : BLACK ), Polygon::rectangle( width, height, center ) );

            width -= HUD_BORDER_WIDTH;
            height -= HUD_BORDER_WIDTH;
        }

        draw( BLACK, Polygon::rectangle( width, height, center ) );

        if( climber->alive( ) )
        {
            dec health_percentage = climber->health( ) / climber->max_health( );
            Assert( in_range_0_to_1( health_percentage, true ) );

            Color health_color;

            if( climber->god( ) )
            {
                health_color = CYAN;
            }
            else if( is_one( health_percentage ) )
            {
                health_color = GREEN;
            }
            else if( health_percentage >= HEALTH_BAR_YELLOW_START )
            {
                health_color = ColorSlider( YELLOW, GREEN ).color_at( ( health_percentage - HEALTH_BAR_YELLOW_START ) * inverse( one_minus( HEALTH_BAR_YELLOW_START ) ) );
            }
            else if( health_percentage >= HEALTH_BAR_RED_START )
            {
                health_color = ColorSlider( RED, YELLOW ).color_at( ( health_percentage - HEALTH_BAR_RED_START ) * inverse( one_minus( HEALTH_BAR_RED_START ) ) );
            }
            else
            {
                health_color = RED;
            }

            draw( health_color, Polygon::rectangle( width * health_percentage, height,
                                                    center - VectorX( half( width * one_minus( health_percentage ) ) ) ) );
        }
    }
}

Climber::JumpHUD::JumpHUD( ptr<Climber> in_climber ) : HUD( 0.5, ( HEALTH_BAR_HEIGHT + JUMP_BAR_RADIUS + JUMP_BAR_SPACING_Y ), ( ( x2( JUMP_BAR_RADIUS ) * ( in_climber->extra_jumps_max( ) + 1 ) ) + ( JUMP_BAR_SPACING_X /* * in_climber->extra_jumps_max( ) */ ) ), x2( JUMP_BAR_RADIUS ) ), m_climber( in_climber ) { }

void Climber::JumpHUD::render( ptr<Camera> in_camera )
{
    HUD::render( in_camera );

    if( ptr<Climber> climber = m_climber )
    {
        uint jumps_max = climber->extra_jumps_max( ) + 1;
        uint jumps = climber->god( ) ? ( jumps_max ) : ( climber->can_jump( ) ? ( jumps_max ) : ( climber->extra_jumps( ) ) );

        FixedRectangle cref rect = bounds( in_camera );

        for_range( i, jumps_max )
        {
            Planc radius = half( rect.height( ) );
            Point center = rect.center( );

            if( jumps_max > 1 )
            {
                center += VectorX( radius + ( ( ( rect.width( ) - x2( radius ) ) / ( jumps_max - 1 ) ) * i ) - half( rect.width( ) ) );
            }

            // border
            for_range( i, HUD_BORDER_COUNT )
            {
                draw( ( ( ( i % 2 ) ^ !( HUD_BORDER_COUNT % 2 ) ) ? WHITE : BLACK ), Polygon::circle( radius, center ) );

                radius -= half( HUD_BORDER_WIDTH );
            }

            draw( BLACK, Polygon::circle( radius, center ) );

            if( i < jumps ) // jump available
            {
                draw( WHITE, Polygon::circle( radius, center ) );
            }
        }
    }
}

Climber::AimHUD::AimHUD( ptr<Climber> in_climber ) : HUD( 0.5, 0.5, AIM_DISPLAY_RATIO, AIM_DISPLAY_RATIO ), m_climber( in_climber ) { }

void Climber::AimHUD::render( ptr<Camera> in_camera )
{
    HUD::render( in_camera );

    if( ptr<Climber> climber = m_climber )
    {
        dec a = 0.0;

        if( !climber->aim_display_timer( ).completed( ) )
        {
            a = 1.0;
        }
        else if( !climber->aim_display_fade_timer( ).completed( ) )
        {
            a = climber->aim_display_fade_timer( ).percentage( );
        }

        Angle aim_angle = climber->aim_angle( );

        Planc aim_radius = half( min( bounds( in_camera ).width( ), bounds( in_camera ).height( ) ) ) - AIM_DISPLAY_POINTER_LENGTH;

        Planc aim_thickness = AIM_DISPLAY_THICKNESS;
        Planc aim_pointer_length = AIM_DISPLAY_POINTER_LENGTH;
        Planc aim_border_thickness = AIM_DISPLAY_BORDER_THICKNESS;

        Point aim_tip = VectorA( aim_angle, aim_radius + aim_pointer_length );
        Point aim_side_1 = Circle( aim_radius ).intersection( VectorA( aim_angle.flipped( ) + half( AIM_DISPLAY_POINTER_ANGLE ), aim_radius, aim_tip ) ).pt1( );
        Point aim_side_2 = Circle( aim_radius ).intersection( VectorA( aim_angle.flipped( ) - half( AIM_DISPLAY_POINTER_ANGLE ), aim_radius, aim_tip ) ).pt1( );

        Polygon aim_pointer( { aim_side_1, aim_tip, aim_side_2 } );

        Point arc_tip = VectorA( AIM_DISPLAY_BREADTH, aim_radius );
        Point arc_inner = VectorX( aim_radius - aim_thickness );
        Point arc_mid_inner = midpoint( arc_inner, arc_tip );
        Planc inner_offset = -Line( arc_mid_inner, arc_mid_inner + VectorA( Angle( arc_inner, arc_tip ) + RIGHT, aim_radius + aim_thickness ) ).x( 0.0 );

        Crescent aim_crescent( Circle( aim_radius ), Circle( aim_radius - aim_thickness + inner_offset, VectorA( aim_angle.flipped( ), inner_offset ) ) );

        Vector offset( ( climber->position( ) - in_camera->center( ) ) * in_camera->zoom( ) );

        aim_pointer += offset;
        aim_crescent += offset;

        // aim.draw( AIM_DISPLAY_BORDER_COLOR.a( a * a ), Polygon( aim_pointer ).expanded( aim_border_thickness, Polygon::POINTED ) );
        // aim.draw( AIM_DISPLAY_BORDER_COLOR.a( a * a ), Polygon( aim_crescent ).expanded( aim_border_thickness, Polygon::ROUNDED ) );

        draw( AIM_DISPLAY_COLOR.a( a ), Polygon( aim_pointer ) );
        draw( AIM_DISPLAY_COLOR.a( a ), Polygon( aim_crescent ) );
    }
}

Climber::LowHealthAlertEffect::LowHealthAlertEffect( ptr<Climber> in_climber ) : m_climber( in_climber ) { }

void Climber::LowHealthAlertEffect::render( ptr<Camera> in_camera )
{
    ScreenVisual::render( in_camera );

    Assert( HEALTH_THRESHOLD_UPPER > HEALTH_THRESHOLD_LOWER );

    if( ptr<Climber> climber = m_climber )
    {
        if( climber->alive( ) && ( climber->health_percentage( ) <= HEALTH_THRESHOLD_UPPER ) )
        {
            dec alpha_health = 1.0;
            if( climber->health_percentage( ) > HEALTH_THRESHOLD_LOWER )
            {
                alpha_health = one_minus( ( climber->health_percentage( ) - HEALTH_THRESHOLD_LOWER ) / ( HEALTH_THRESHOLD_UPPER - HEALTH_THRESHOLD_LOWER ) );
            }

            // todo cycle?
            dec alpha_time = BASE_ALPHA * abs( sin( dec( climber->age( ) ) / dec( PULSE_DURATION.min( ) + ( PULSE_DURATION.range( ) * climber->health_percentage( ) ) ) ) );
            Color color = BASE_COLOR.a( alpha_health * alpha_time );

            FixedRectangle screen_bounds = in_camera->bounds( false );
            dec effect_distance = SCREEN_FADE_DISTANCE * min( screen_bounds.width( ), screen_bounds.height( ) );

            FixedRectangle clear_bounds( screen_bounds.width( ) - effect_distance, screen_bounds.height( ) - effect_distance, screen_bounds.center( ) );

            draw( { color, color, TRANSPARENT_COLOR, TRANSPARENT_COLOR }, Polygon( { screen_bounds.top_right( ), screen_bounds.top_left( ), clear_bounds.top_left( ), clear_bounds.top_right( ) } ) );
            draw( { color, color, TRANSPARENT_COLOR, TRANSPARENT_COLOR }, Polygon( { screen_bounds.top_left( ), screen_bounds.bottom_left( ), clear_bounds.bottom_left( ), clear_bounds.top_left( ) } ) );
            draw( { color, color, TRANSPARENT_COLOR, TRANSPARENT_COLOR }, Polygon( { screen_bounds.bottom_left( ), screen_bounds.bottom_right( ), clear_bounds.bottom_right( ), clear_bounds.bottom_left( ) } ) );
            draw( { color, color, TRANSPARENT_COLOR, TRANSPARENT_COLOR }, Polygon( { screen_bounds.bottom_right( ), screen_bounds.top_right( ), clear_bounds.top_right( ), clear_bounds.bottom_right( ) } ) );
        }
    }
}

void Climber::input( ptr<Input> in_input )
{
    if( cast_as( key_input, KeyInput, in_input ) )
    {
        KeyInput::Key key = key_input->key;
        KeyInput::Dynamic dynamic = key_input->dynamic;

        bool pressed = dynamic == KeyInput::PRESSED;
        bool held = dynamic == KeyInput::HELD;
        bool down = pressed || held;

        switch( key )
        {
            default:
            {

            }

            b_case( ' ' ) :
            {
                jump( down );
            }

            b_case( 'w' ) :
            {
                looking_up( down );
            }

            b_case( 's' ) :
            {
                looking_down( down );
            }

            b_case( 'a' ) :
            {
                moving_left( down );
            }

            b_case( 'd' ) :
            {
                moving_right( down );
            }

            b_case( 'q' ) : a_case( 'e' ) :
            {
                if( pressed )
                {
                    dash( pressed );
                }
            }

            #if defined( AXN_DEBUG_CONTROLS )

            b_case( 'g' ) :
            {
                if( pressed )
                {
                    god( !god( ) );
                }
            }

            b_case( 'h' ) :
            {
                if( down )
                {
                    heal_full( );
                }
            }

            b_case( 'k' ) :
            {
                if( pressed )
                {
                    if( Settings::shifty( ) )
                    {
                        if( dead( ) )
                        {
                            revive( );
                        }
                    }
                    else
                    {
                        if( alive( ) && !god( ) )
                        {
                            die( );
                        }
                    }
                }
            }

            #endif
        }
    }
    else if( cast_as( mouse_input, MouseInput, in_input ) )
    {
        MouseInput::Button button = mouse_input->button;
        MouseInput::Dynamic dynamic = mouse_input->dynamic;

        bool move = dynamic == MouseInput::MOVE;
        bool pressed = dynamic == MouseInput::PRESSED;
        bool released = dynamic == MouseInput::RELEASED;

        if( move )
        {
            if( button == MouseInput::NO_BUTTON )
            {
                // todo should this happen here?
                aim( Angle( position( ), camera( )->cursor_world_position( ) ) );
            }
        }
        else
        {
            if( pressed )
            {
                if( button == MouseInput::LEFT_BUTTON )
                {
                    launch_arrow( );
                }
                else if( button == MouseInput::RIGHT_BUTTON )
                {
                    launch_hook( );
                }
            }
            else if( released )
            {
                if( button == MouseInput::LEFT_BUTTON )
                {
                    release_arrow( );
                }
                else if( button == MouseInput::RIGHT_BUTTON )
                {
                    release_hook( );
                }
            }
        }
    }
    else if( cast_as( controller_input, ControllerInput, in_input ) )
    {
        if( cast_as( button_input, ControllerButtonInput, controller_input ) )
        {
            ControllerButtonInput::Button button = button_input->button;
            ControllerButtonInput::Dynamic dynamic = button_input->dynamic;

            bool pressed = dynamic == ControllerButtonInput::PRESSED;

            switch( button )
            {
                default:
                {
                    break;
                }

                b_case( ControllerButtonInput::L1_BUTTON ) : a_case( ControllerButtonInput::RIGHT_DOWN_BUTTON ) :
                {
                    if( alive( ) )
                    {
                        jump( pressed );
                    }
                }

                b_case ControllerButtonInput::R1_BUTTON:
                {
                    if( pressed )
                    {
                        launch_arrow( );
                    }
                    else
                    {
                        release_arrow( );
                    }
                }

                b_case ControllerButtonInput::R2_BUTTON:
                {
                    if( pressed )
                    {
                        launch_hook( );
                    }
                    else
                    {
                        release_hook( );
                    }
                }

                b_case ControllerButtonInput::LEFT_UP_BUTTON:
                {
                    looking_up( pressed );
                }

                b_case ControllerButtonInput::LEFT_DOWN_BUTTON :
                {
                    looking_down( pressed );
                }

                b_case ControllerButtonInput::LEFT_LEFT_BUTTON :
                {
                    moving_left( pressed );
                }

                b_case ControllerButtonInput::LEFT_RIGHT_BUTTON :
                {
                    moving_right( pressed );
                }
            }
        }
        else if( cast_as( joystick_input, ControllerJoystickInput, controller_input ) )
        {
            if( joystick_input->joystick == ControllerJoystickInput::LEFT_JOYSTICK )
            {
                if( joystick_input->dead_zone )
                {
                    movement_stop( );
                }
                else
                {
                    Vector cref v = joystick_input->vector;

                    if( is_pos( v.dx( ) ) )
                    {
                        moving_right( true );
                    }
                    else if( is_neg( v.dx( ) ) )
                    {
                        moving_left( true );
                    }

                    if( is_pos( v.dy( ) ) )
                    {
                        looking_up( true );
                    }
                    else if( is_neg( v.dy( ) ) )
                    {
                        looking_down( true );
                    }
                }
            }
            else if( joystick_input->joystick == ControllerJoystickInput::RIGHT_JOYSTICK )
            {
                if( !joystick_input->dead_zone )
                {
                    aim( joystick_input->vector.angle( ) );
                }
            }
        }
    }
}

void Climber::clear_input( )
{
    m_slashing = false;

    release_bow( );

    moving_left( false );
    moving_right( false );

    looking_up( false );
    looking_down( false );

    jump( false );
}
