#include "Climber.hpp"
#include "Terrain.hpp"
#include "Arrow.hpp"
#include "Dust.hpp"

using mtmercy::Climber;

namespace
{

cPlanc HEIGHT = ( METER * 1.85 );
cPlanc WIDTH = ( HEIGHT / 2.5 );
cPlanc WIDTH_GROUND = ( WIDTH * 0.25 );

cPlanc DEFAULT_SPEED_GROUND = ( 1.0 );
cPlanc DEFAULT_SPEED_AIR = ( 0.25 );
cPlanc JUMP_STRENGTH = ( METER / 7.0 );
cdec JUMP_DEGRATION_RATIO = ( 0.9 );
cuint JUMP_MAX_DURATION = ( 5 );
cuint JUMP_RESET_WAIT_TIME = ( 0 );

cPlanc LIGHT_SIGHT = ( HEIGHT * 3.0 );

// crossbox / hook
cPlanc DEFAULT_ARROW_LAUNCH_SPEED = ( 16.0 );
cdec ARROW_VELOCITY_DAMPEN = ( 0.25 );
cPlanc DEFAULT_HOOK_LAUNCH_SPEED = ( 32.0 );
cPlanc DEFAULT_ROPE_GROWTH_SPEED = ( 16.0 );
cPlanc DEFAULT_ROPE_RETRACT_SPEED = ( 32.0 );
cuint RELOAD_TIME = ( 20 );

cPlanc DUST_MINIMUM_SPEED = ( METER / 6.0 );
const Span<dec> DUST_VELOCITY_DAMPEN_RATIO = { 0.075, 0.15 };
const Span<Planc> DUST_RISE = { 0.25, 0.75 };
const Span<uint> DUST_LANDING_COUNT = { 16, 32 };
const Span<dec> DUST_LANDING_ANGLE = { 0.0, RIGHT / 3.0 };
const Span<uint> DUST_WALKING_COUNT = { 3, 7 };
const Span<uint> DUST_WALKING_TIME = { 0, 3 };

cPlanc HEAD_RADIUS = ( HEIGHT / 3.9 );
cPlanc HEAD_OFFSET_Y = ( HEAD_RADIUS * 0.05 );
cPlanc FACE_RADIUS = ( HEAD_RADIUS * 0.75 );
cPlanc EYE_SPACING = ( FACE_RADIUS * 0.55 );
cPlanc EYE_RADIUS = ( 1.5 );
cPlanc EYEBROW_LENGTH = ( EYE_RADIUS * 4.33 );
cPlanc EYEBROW_SPACING_X = ( EYE_RADIUS * ( 2.0 / 3.0 ) );
cPlanc EYEBROW_SPACING_Y = ( EYE_RADIUS * ( 5.0 / 3.0 ) );
cPlanc EYEBROW_DEAD_SPACING_Y = ( EYE_RADIUS );
cPlanc EAR_RADIUS = ( 3.0 );

cPlanc BEARD_WIDTH = ( FACE_RADIUS * 2.0 );
cPlanc BEARD_HEIGHT = ( BEARD_WIDTH * 0.5 );
cPlanc MUSTACHE_WIDTH = ( FACE_RADIUS );
cPlanc MUSTACHE_HEIGHT = ( MUSTACHE_WIDTH / 3.0 );

cPlanc BEARD_OFFSET = ( HEAD_RADIUS * 0.75 );
cPlanc MUSTACHE_OFFSET = ( BEARD_OFFSET * 0.5 );
cPlanc MOHAWK_WIDTH = ( HEAD_RADIUS * 0.666 );
cPlanc MOHAWK_HEIGHT = ( MOHAWK_WIDTH * 0.7 );
cPlanc MOHAWK_OFFSET = ( MOHAWK_HEIGHT * 0.5 );

cPlanc BODY_HEIGHT = ( HEIGHT / 3.0 );
cPlanc LEG_HEIGHT = ( HEIGHT / 4.0 );

cPlanc BODY_WIDTH = ( HEIGHT / 2.0 );

cPlanc PANTS_WIDTH_UPPER = ( BODY_WIDTH - 6.0 );
cPlanc PANTS_WIDTH_LOWER = ( PANTS_WIDTH_UPPER * 0.5 );
cPlanc PANTS_HEIGHT = ( BODY_HEIGHT / 2.0 );

cPlanc FRONT_UPPER_ARM_LENGTH = ( BODY_HEIGHT * 0.8 );
cPlanc FRONT_LOWER_ARM_LENGTH = ( FRONT_UPPER_ARM_LENGTH * 0.8 );

cPlanc BACK_UPPER_ARM_LENGTH = ( BODY_HEIGHT * 0.5 );
cPlanc BACK_LOWER_ARM_LENGTH = ( BACK_UPPER_ARM_LENGTH );

cPlanc FRONT_ARM_WIDTH = ( HEIGHT / 6.0 );
cPlanc FRONT_HAND_WIDTH = ( FRONT_ARM_WIDTH );

cPlanc BACK_ARM_WIDTH = ( FRONT_ARM_WIDTH * 0.9 );
cPlanc BACK_HAND_WIDTH = ( BACK_ARM_WIDTH );

cPlanc FRONT_HAND_CROSSBOW_OFFSET_DISTANCE = ( METER * 0.375 );
cAngle FRONT_HAND_CROSSBOW_OFFSET_ANGLE = ( RIGHT );

cPlanc BACK_HAND_CROSSBOW_OFFSET_DISTANCE = ( METER * 0.75 );
cAngle BACK_HAND_CROSSBOW_OFFSET_ANGLE = ( TAU / 32.0 );

cPlanc FINGER_WIDTH = ( FRONT_ARM_WIDTH / 3.0 );
cPlanc FINGER_LENGTH_OUT = ( FRONT_ARM_WIDTH * 0.72 );
cPlanc FINGER_LENGTH_DOWN = ( FRONT_ARM_WIDTH * 0.64 );

cuint ELBOW_OFFSET_INDEX = ( 0 ); // always zero
cuint HAND_OFFSET_INDEX = ( ELBOW_OFFSET_INDEX + 1 );
cuint FINGER_OFFSET_INDEX = ( HAND_OFFSET_INDEX + 1 );

cAngle FRONT_ARM_RAGDOLL_ROT_UPPER_LIMIT = ( PI * ( 2.0 / 3.0 ) );
cAngle FRONT_ARM_RAGDOLL_ROT_LOWER_LIMIT = ( A0 );
cAngle BACK_ARM_RAGDOLL_ROT_LIMIT = ( RIGHT * 1.5 );

cPlanc SHOULDER_RADIUS = ( FRONT_ARM_WIDTH * 0.5 );

cPlanc LEG_WIDTH = ( HEIGHT / 5.25 );

cPlanc BOOT_TOP_WIDTH = ( LEG_WIDTH * 1.3 );
cPlanc BOOT_TOP_HEIGHT = ( BOOT_TOP_WIDTH * 0.66 );
cPlanc BOOT_BASE_WIDTH = ( LEG_WIDTH * 1.6 );
cPlanc BOOT_BASE_HEIGHT = ( BOOT_BASE_WIDTH * 0.3 );
cPlanc BOOT_SOLE_HEIGHT = ( 1.0 ); // todo make relative
cPlanc BOOT_SOLE_SUB_WIDTH = ( 3.0 );
cPlanc BOOT_SOLE_SUB_GAP_WIDTH = ( 1.0 );
cPlanc BOOT_SOLE_SUB_HEIGHT = ( 0.75 );

// todo make relative
cPlanc BELT_THICKNESS = ( 3.0 );
cPlanc BELT_BUCKLE_WIDTH = ( 5.0 );

cuint BOB_PERIOD = ( 20 );
cPlanc HEAD_BOB_DISTANCE = ( 1.0 );
cPlanc SHOULDER_BOB_DISTANCE = ( 0.8 );
cdec SHOULDER_BOB_OFFSET = ( 0.75 );

cuint BLINK_DURATION = ( 8 );
const Span<uint> BLINK_WAIT = { 240, 480 };

cPlanc CROSSBOW_LENGTH = ( METER * 0.9375 );
cPolygon CROSSBOW_SHAPE = Polygon( { { 0.0, 0.0 },
                                     { -CROSSBOW_LENGTH / 3.0, -CROSSBOW_LENGTH / 3.0 },
                                     { -CROSSBOW_LENGTH / 1.5, -CROSSBOW_LENGTH / 3.0 },
                                     { -CROSSBOW_LENGTH / 1.5, -CROSSBOW_LENGTH / 1.5 },
                                     { CROSSBOW_LENGTH / 3.0, -CROSSBOW_LENGTH / 6.0 },
                                     { CROSSBOW_LENGTH / 1.2, -CROSSBOW_LENGTH / 6.0 },
                                     { CROSSBOW_LENGTH, 0.0 } } );

cColor CROSSBOW_COLOR = ( Color::rgb( 0xA54B23 ) );

cAngle HOOK_ANGLE = ( RIGHT );
cPlanc HOOK_LENGTH = ( METER );
cPlanc HOOK_THICKNESS = ( HOOK_LENGTH / 6.0 );
cPlanc HOOK_TIP_LENGTH = ( HOOK_THICKNESS * 0.5 );
cColor HOOK_COLOR = ( Color::rgb( 0x9C9C9C ) );

cPlanc ROPE_WIDTH = ( 4.0 );
cColor ROPE_BASE_COLOR = ( Color::rgb( 0xDAA420 ) );
cColor ROPE_ALT_COLOR = ( Color::rgb( 0xB9870F ) );

cPlanc PATH_LENGTH = ( METER * 25.0 );
cdec PATH_ALPHA_START = ( 0.8 );

cPlanc ARROW_PATH_LENGTH = ( PATH_LENGTH );
cdec ARROW_PATH_ALPHA_START = ( PATH_ALPHA_START );
cColor ARROW_PATH_COLOR = ( RED );

cPlanc HOOK_PATH_LENGTH = ( PATH_LENGTH );
cdec HOOK_PATH_ALPHA_START = ( PATH_ALPHA_START );
cColor HOOK_PATH_COLOR = ( WHITE );

cdec HEALTH_BAR_WIDTH = ( 1.0 );
cdec HEALTH_BAR_HEIGHT = ( 0.04 );
cPlanc HEALTH_BAR_BORDER_WIDTH = ( 4.0 );

cdec HEALTH_BAR_RED_START = ( 0.1 );
cdec HEALTH_BAR_YELLOW_START = ( 0.5 );

} // namespace

Climber::~Climber( )
{
    room( )->camera( )->remove_hud_element( &m_healthbar );
    room( )->camera( )->remove_screen_effect( &m_low_health_effect );
}

Climber::Climber( cuint _player_number, Room * room, Coordinate cref _position )
    : Player( _player_number, room, _position ),
    m_hook( room, this ),
    m_healthbar( this ),
    m_low_health_effect( this )
{
    interactive( true );

    needs_render_always( true );

    track_position( 32 );

    space( Polygon::equilateral( 7, half( HEIGHT ) ).rotate( 0.1 ) );

    //space( Polygon( { Coordinate( half( WIDTH ), half( HEIGHT ) ),
    //                  Coordinate( -half( WIDTH ), half( HEIGHT ) ),
    //                  Coordinate( -half( WIDTH_GROUND ), -half( HEIGHT ) ),
    //                  Coordinate( half( WIDTH_GROUND ), -half( HEIGHT ) ) } ) );

    eye_info( EYE_RADIUS, BLINK_DURATION, BLINK_WAIT );

    m_skin = MOHAWK;

    m_movement_speed_ground = DEFAULT_SPEED_GROUND;
    m_movement_speed_air = DEFAULT_SPEED_AIR;

    m_hook_reload_timer.reset( 0 );
    m_arrow_reload_timer.reset( 0 );

    m_dust_timer.reset( 0 );

    m_arrow_feather_color = Random::rColor( );

    room->camera( )->add_hud_element( &m_healthbar );
    room->camera( )->add_screen_effect( &m_low_health_effect );
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

    release_arrow( );

    Player::die( );
}

void Climber::update( )
{
    Player::update( );

    if( alive( ) )
    {
        heal( 0.01 ); // todo
    }
    else
    {
        m_dead_face.update( );
        m_dead_arm_front.update( );
        m_dead_arm_back.update( );
    }

    m_hook_reload_timer.tick( );
    m_arrow_reload_timer.tick( );

    m_dust_timer.tick( );

    if( passing_terrain( ) )
    {
        passing_terrain( false );
    }

    Vector cref v = velocity( );
    if( v.has_magnitude( ) )
    {
        if( v.magnitude( ) >= DUST_MINIMUM_SPEED )
        {
            if( Object::ground( ) )
            {
                if( !m_dust_timer.remaining( ) )
                {
                    m_dust_timer.reset( Random::rint( DUST_WALKING_TIME ) );

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
        if( !m_hook_reload_timer.remaining( ) )
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
        if( m_looking_up )
        {
            m_hook.shorten_rope( 1.0 ); // todo
        }

        if( m_looking_down )
        {
            m_hook.extend_rope( 1.0 ); // todo
        }
    }

    if( m_launching_arrow )
    {
        if( !m_arrow_reload_timer.remaining( ) )
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

    }

    m_hook.update( );
}

void Climber::update_velocity( )
{
    Planc movement_speed = Object::ground( ) ? m_movement_speed_ground : m_movement_speed_air;

    if( !has_gravity( ) )
    { // can fly?
        if( m_looking_up )
        {
            if( m_moving_right && !m_moving_left )
            {
                add_velocity( VectorA( half( RIGHT ), movement_speed ) );
            }
            else if( m_moving_left && !m_moving_right )
            {
                add_velocity( VectorA( half( RIGHT ) + RIGHT, movement_speed ) );
            }
            else
            {
                add_velocity( VectorY( movement_speed ) );
            }
        }
        if( m_looking_down )
        {
            if( m_moving_right && !m_moving_left )
            {
                add_velocity( VectorA( -half( RIGHT ), movement_speed ) );
            }
            else if( m_moving_left && !m_moving_right )
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
            if( m_moving_right )
            {
                add_velocity( VectorX( movement_speed ) );
            }
            if( m_moving_left )
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

        if( m_moving_right )
        {
            add_velocity( VectorX( movement_speed ) );
        }
        if( m_moving_left )
        {
            add_velocity( VectorX( -movement_speed ) );
        }
    }

    Player::update_velocity( );

    // rope adjustment
    if( m_hook.taut( ) )
    {
        Vector v = velocity( );
        Vector rope = m_hook.rope( );

        Angle d = ( rope.angle( ) - v.angle( ) ).truncated( );

        // todo align with gravity
        if( ( d >= RIGHT ) && ( d <= ( TAU - RIGHT ) ) )
        {
            Vector v_flat = v;
            v_flat.flatten( rope.angle( ) + RIGHT );

            if( v_flat.has_magnitude( ) )
            {
                Planc m = v_flat.magnitude( );

                Angle a( (dec)( m / rope.magnitude( ) ) );
                a = negate( a, ( d >= PI ) );

                Vector rope_flipped( rope.destination( ), rope.origin( ) );
                rope_flipped.rotate( a );

                velocity( Vector( position( ), rope_flipped.destination( ) ) );
            }
        }
    }

    if( Object::ground( ) )
    {
        m_jump_reset_timer.tick( );
    }

    if( m_jumping )
    {
        if( !m_jump_reset_timer.remaining( ) && Object::ground( ) )
        {
            m_jumping_timer.reset( JUMP_MAX_DURATION );
            m_jump_reset_timer.reset( JUMP_RESET_WAIT_TIME );
            m_jump_degradation = 0.0;
        }

        if( !m_jumping_timer.tick( ) )
        {
            add_velocity( ( VectorA( gravity( ).angle( ).flipped( ), JUMP_STRENGTH ) - gravity( ) ) * ( 1.0 - m_jump_degradation ) );

            m_jump_degradation = 1.0 - ( ( 1.0 - m_jump_degradation ) * JUMP_DEGRATION_RATIO );

            ground( nullptr, Terrain::Bumper( ) );
        }
        else
        {
            m_jumping = false;
        }
    }
    else
    {
        m_jumping_timer.reset( 0 );
    }
}

dec Climber::check_movement( Vector cref _velocity )
{
    if( _velocity.has_magnitude( ) )
    {
        if( m_hook.hooked( ) && !m_hook.taut( ) )
        {
            Vector rope = m_hook.rope( );

            bool now_in_range = rope.destination( ).in_distance_range( position( ), hook( ).rope_length( ) );
            bool will_be_in_range = rope.destination( ).in_distance_range( position( ) + _velocity, m_hook.rope_length( ) );

            if( now_in_range && !will_be_in_range )
            {
                Polygon rope_radius = Polygon::circle( hook( ).rope_length( ), rope.origin( ) );
                varray<Line> intersections = rope_radius.intersection( Line( position( ), position( ) + _velocity ) );

                for_each( line, intersections )
                {
                    if( line.c1( ) == center( ) )
                    {
                        return ( line.length( ) / _velocity.magnitude( ) );
                    }
                }
            }
        }
    }

    return Player::check_movement( _velocity );
}

dec Climber::air_resistance( ) const
{
    if( hook( ).taut( ) )
    {
        return 0.01; // TODO
    }

    return Player::air_resistance( );
}

Planc Climber::light_sight( ) const
{
    return LIGHT_SIGHT;
}

void Climber::movement_stop( )
{
    moving_right( false );
    moving_left( false );
    looking_up( false );
    looking_down( false );
}

bool Climber::moving_right( ) const
{
    return m_moving_right && !m_moving_left;
}

void Climber::moving_right( cbool _moving )
{
    if( _moving )
    {
        if( alive( ) )
        {
            m_moving_right = true;
        }
    }
    else
    {
        m_moving_right = false;
    }
}

bool Climber::moving_left( ) const
{
    return m_moving_left && !m_moving_right;
}

void Climber::moving_left( cbool _moving )
{
    if( _moving )
    {
        if( alive( ) )
        {
            m_moving_left = true;
        }
    }
    else
    {
        m_moving_left = false;
    }
}

bool Climber::looking_up( ) const
{
    return m_looking_up && !m_looking_down;
}

void Climber::looking_up( cbool _looking )
{
    if( _looking )
    {
        if( alive( ) )
        {
            m_looking_up = true;
        }
    }
    else
    {
        m_looking_up = false;
    }
}

bool Climber::looking_down( ) const
{
    return m_looking_down && !m_looking_up;
}

void Climber::looking_down( cbool _looking )
{
    if( _looking )
    {
        if( alive( ) )
        {
            m_looking_down = true;
        }
    }
    else
    {
        m_looking_down = false;
    }
}

void Climber::jump( cbool _jumping )
{
    if( _jumping )
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
    }
}

void Climber::ground( Terrain::Node * ground, Terrain::Bumper cref _bumper )
{
    if( ground && !Object::ground( ) )
    {
        Vector dust_velocity = ( velocity( ).dy( 0.0 ) * Random::rdec( DUST_VELOCITY_DAMPEN_RATIO ) );
        uint dust_count = Random::rint( DUST_LANDING_COUNT );
        for_range( i, dust_count )
        {
            Coordinate c = position( ) + foot( true ) + ( Vector( foot( true ), foot( false ) ) * ( (dec)i / (dec)dust_count ) );
            Angle a = ground->normal( ) + Random::rAngle( DUST_LANDING_ANGLE.min( ), DUST_LANDING_ANGLE.max( ) );
            // todo shouldn't start from normal not right?

            if( ( i < half( dust_count ) ) ^ facing_left( ) )
            {
                a = ( PI - a );
            }

            // todo room( )->add_object( new Dust( room( ), c, dust_velocity + VectorA( a, Random::rPlanc( DUST_RISE ) ), room( )->terrain( )->dust_color( ) ) );
        }
    }

    Object::ground( ground, _bumper );
}

Angle Climber::aim_angle( ) const
{
    return ( alive( ) ? m_aim_angle : m_dead_arm_front.angle( FINGER_OFFSET_INDEX ) );
}

bool Climber::aiming_right( ) const
{
    return ( !aiming_left( ) );
}

bool Climber::aiming_left( ) const
{
    Angle a = alive( ) ? aim_angle( ) : m_dead_aim_angle;
    return ( ( a.truncated( ) > RIGHT ) && ( a.truncated( ) < ( PI + RIGHT ) ) );
}

bool Climber::aiming_up( ) const
{
    return ( aim_angle( ).in_quadrant( Q1 ) || aim_angle( ).in_quadrant( Q2 ) || ( aim_angle( ) == RIGHT ) );
}

bool Climber::aiming_down( ) const
{
    return ( !aiming_up( ) );
}

void Climber::aim( Angle cref _aim_angle )
{
    m_aim_angle = _aim_angle;
}

void Climber::aim_shake( Angle cref _aim_shake_angle )
{
    m_aim_shake_angle = _aim_shake_angle;
}

Angle Climber::aim_shake( ) const
{
    return Random::negated( half( Random::rAngle( m_aim_shake_angle ) ) );
}

Angle Climber::aim_shake_range( ) const
{
    return m_aim_shake_angle;
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

Coordinate Climber::head_center( ) const
{
    // todo make cycle
    return CoordinateY( BODY_HEIGHT + HEAD_OFFSET_Y + ( HEAD_BOB_DISTANCE * ( alive( ) ? sin( (dec)age( ) / BOB_PERIOD ) : -1.0 ) ) );
}

Coordinate Climber::face_center( ) const
{
    return head_center( ) + VectorA( alive( ) ? aim_angle( ) : m_dead_arm_back.angle( 0 ), half( HEAD_RADIUS - FACE_RADIUS ) );
}

Coordinate Climber::shoulder( cbool _front ) const
{
    return Coordinate( negate( half( BODY_WIDTH - ( SHOULDER_RADIUS * 2.0 ) ), ( aiming_left( ) ^ _front ) ),
                       half( BODY_HEIGHT - ( SHOULDER_RADIUS * 2.0 ) ) + ( SHOULDER_BOB_DISTANCE * ( alive( ) ? sin( (dec)( age( ) + ( BOB_PERIOD * SHOULDER_BOB_OFFSET ) ) / BOB_PERIOD ) : -1.0 ) ) );
}

Coordinate Climber::elbow( cbool _front ) const
{
    if( alive( ) )
    {
        return Joint( shoulder( _front ), hand( _front ), arm_length_upper( _front ), arm_length_lower( _front ), !( aiming_left( ) ) ).joint( );
    }
    else
    {
        return ( _front ? m_dead_arm_front.position( ELBOW_OFFSET_INDEX ) : m_dead_arm_back.position( ELBOW_OFFSET_INDEX ) );
    }
}

Coordinate Climber::hand( cbool _front ) const
{
    if( alive( ) )
    {
        return -hand_to_crossbow_offset( _front );
    }
    else
    {
        return ( _front ? m_dead_arm_front.position( HAND_OFFSET_INDEX ) : m_dead_arm_back.position( HAND_OFFSET_INDEX ) );
    }
}

Coordinate Climber::hand_to_crossbow_offset( cbool _front ) const
{
    return VectorA( aim_angle( ) - negate( _front ? FRONT_HAND_CROSSBOW_OFFSET_ANGLE : BACK_HAND_CROSSBOW_OFFSET_ANGLE, aiming_left( ) ),
                    negate( _front ? FRONT_HAND_CROSSBOW_OFFSET_DISTANCE : BACK_HAND_CROSSBOW_OFFSET_DISTANCE ) );
}

Planc Climber::arm_length_upper( cbool _front ) const
{
    return ( _front ? FRONT_UPPER_ARM_LENGTH : BACK_UPPER_ARM_LENGTH );
}

Planc Climber::arm_length_lower( cbool _front ) const
{
    return ( _front ? FRONT_LOWER_ARM_LENGTH : BACK_LOWER_ARM_LENGTH );
}

Planc Climber::arm_width( cbool _front ) const
{
    return ( _front ? FRONT_ARM_WIDTH : BACK_ARM_WIDTH );
}

Coordinate Climber::hip( cbool _front ) const
{
    return Coordinate( negate( half( PANTS_WIDTH_UPPER ) - half( LEG_WIDTH ), ( facing_left( ) ^ _front ) ), -half( BODY_HEIGHT ) );
}

Coordinate Climber::foot( cbool _front ) const
{
    return Coordinate( hip( _front ).x( ), -half( HEIGHT ) );
}

// -- GRAPHICS --

Climber::Skin Climber::skin( ) const
{
    return m_skin;
}

Color Climber::color( ColorPiece component ) const
{
    switch( component )
    {
        case SKIN:
            switch( skin( ) )
            {
                // case:
                //     return Color::rgb( 0x9B5032 );
                //     break;
                case MOHAWK:
                    return Color::rgb( 0xFFE1BE );
                    break;
                case NO_SKIN:
                default:
                    return WHITE;
                    break;
            }
            break;
        case HAIR:
            switch( skin( ) )
            {
                case MOHAWK:
                    return RED;
                    break;
                case NO_SKIN:
                default:
                    return BLACK;
                    break;
            }
            break;
        case HAIR_SHAVED:
            switch( skin( ) )
            {
                case MOHAWK:
                    return RED.a( 0.375 );
                    break;
                case NO_SKIN:
                default:
                    return BLACK;
                    break;
            }
            break;
        case EYE:
            return BLACK;
            break;
        case UNDERSHIRT:
            return GRAY_LIGHT;
            break;
        case JACKET:
            return Color::rgb( 0x8C4B2D );
            break;
        case PANTS:
            return Color::rgb( 0xCD8741 );
            break;
        case BELT:
            return BLACK;
            break;
        case BELT_BUCKLE:
            return YELLOW;
            break;
        case BOOT:
            return BLACK;
            break;
        case BOOT_SOLE:
            return GRAY_MID;
            break;
        case BOOT_LACE:
            return WHITE;
            break;
        default:
            return WHITE;
            break;
    }
}

#if defined ( AXN_DEBUG )
Drawing Climber::debug_overlay( ) const
{
    Drawing debug_overlay;

    if( !m_hook.loaded( ) )
    {
        debug_overlay.draw( m_hook.debug_overlay( ).move( m_hook.position( ) - position( ) ) );
    }

    debug_overlay.draw( Player::debug_overlay( ) );

    return debug_overlay;
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

    if( m_skin == MOHAWK )
    {
        // shaved hair
        draw( color( HAIR_SHAVED ), Polygon( Arc::semi_ccw( HEAD_RADIUS, head_center( ) ).path( ).points( ) ) );
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

    draw( color( HAIR ), Line( face_center( ) + Vector( EYE_SPACING + half( EYEBROW_LENGTH - EYEBROW_SPACING_X ), y_offset ),
                               face_center( ) + Vector( EYE_SPACING - half( EYEBROW_LENGTH + EYEBROW_SPACING_X ), y_offset ) ), EYE_RADIUS + EYE_RADIUS );
    draw( color( HAIR ), Line( face_center( ) + Vector( -EYE_SPACING + half( EYEBROW_LENGTH + EYEBROW_SPACING_X ), y_offset ),
                               face_center( ) + Vector( -EYE_SPACING - half( EYEBROW_LENGTH - EYEBROW_SPACING_X ), y_offset ) ), EYE_RADIUS + EYE_RADIUS );

    if( m_skin == MOHAWK )
    {
        { // mohawk
            Coordinate top = head_center( ) + VectorY( HEAD_RADIUS + ( MOHAWK_HEIGHT - MOHAWK_OFFSET ) );
            Coordinate bottom = face_center( ) + VectorY( FACE_RADIUS - MOHAWK_OFFSET );
            draw( color( HAIR ), Line( top, bottom ), MOHAWK_WIDTH );
        }

        { // beard
            draw( color( HAIR ), Polygon::rectangle( BEARD_WIDTH, BEARD_HEIGHT, face_center( ) - VectorY( BEARD_OFFSET ) ) );
            draw( color( HAIR ), Polygon::rectangle( MUSTACHE_WIDTH, MUSTACHE_HEIGHT, face_center( ) - VectorY( MUSTACHE_OFFSET ) ) );
        }
    }
}

void Climber::draw_torso( )
{
    Coordinate waist = CoordinateY( -half( BODY_HEIGHT ) );

    // pants base
    draw( color( PANTS ), Polygon( { waist + CoordinateX( half( PANTS_WIDTH_UPPER ) ),
                                     waist + CoordinateX( -half( PANTS_WIDTH_UPPER ) ),
                                     waist + Coordinate( -half( PANTS_WIDTH_LOWER ), -half( PANTS_HEIGHT ) ),
                                     waist + Coordinate( half( PANTS_WIDTH_LOWER ), -half( PANTS_HEIGHT ) ) } ) );

    // undershirt
    draw( color( UNDERSHIRT ), Polygon::rectangle( half( BODY_WIDTH ), BODY_HEIGHT ) );

    // belt
    draw( color( BELT ), Line( waist - VectorX( half( PANTS_WIDTH_UPPER ) + ( BELT_THICKNESS / 4.0 ) ),
                               waist + VectorX( half( PANTS_WIDTH_UPPER ) + ( BELT_THICKNESS / 4.0 ) ) ), (dec)BELT_THICKNESS );
    draw( color( BELT_BUCKLE ), Polygon::rectangle( BELT_BUCKLE_WIDTH, BELT_BUCKLE_WIDTH, waist ) );

    // jacket
    draw( color( JACKET ), Polygon::circle( half( BACK_ARM_WIDTH ), shoulder( false ) ) );
    draw( color( JACKET ), Polygon::rectangle( BODY_WIDTH / 3.0, BODY_HEIGHT - SHOULDER_RADIUS, Coordinate( BODY_WIDTH / 3.5, -half( SHOULDER_RADIUS ) ) ) );
    draw( color( JACKET ), Polygon::rectangle( BODY_WIDTH / 3.0, BODY_HEIGHT - SHOULDER_RADIUS, Coordinate( -BODY_WIDTH / 3.5, -half( SHOULDER_RADIUS ) ) ) );
}

void Climber::draw_legs( )
{
    Drawing boot_drawing;

    Planc offset_base_x = half( BOOT_BASE_WIDTH - BOOT_TOP_WIDTH );
    Polygon boot_sole = Polygon::rectangle( BOOT_BASE_WIDTH, BOOT_SOLE_HEIGHT, Coordinate( offset_base_x, -half( BOOT_SOLE_HEIGHT ) ) );
    boot_drawing.draw( color( BOOT_SOLE ), boot_sole );
    {
        Polygon boot_sub_sole = Polygon::rectangle( BOOT_SOLE_SUB_WIDTH, BOOT_SOLE_SUB_HEIGHT, Coordinate( offset_base_x - half( BOOT_BASE_WIDTH ) + half( BOOT_SOLE_SUB_WIDTH ), -BOOT_SOLE_SUB_HEIGHT - half( BOOT_SOLE_HEIGHT ) ) );
        boot_drawing.draw( color( BOOT_SOLE ), boot_sub_sole );
        for_range( i, floor( half( half( BOOT_BASE_WIDTH ) ) ) )
        {
            Polygon boot_sub_sole = Polygon::rectangle( BOOT_SOLE_SUB_WIDTH, BOOT_SOLE_SUB_HEIGHT, Coordinate( offset_base_x + half( BOOT_BASE_WIDTH ) - half( BOOT_SOLE_SUB_WIDTH ) - ( ( BOOT_SOLE_SUB_WIDTH + BOOT_SOLE_SUB_GAP_WIDTH ) * (Planc)i ), -BOOT_SOLE_SUB_HEIGHT - half( BOOT_SOLE_HEIGHT ) ) );
            boot_drawing.draw( color( BOOT_SOLE ), boot_sub_sole );
        }
    }

    Polygon boot_shin = Polygon::rectangle( BOOT_TOP_WIDTH, BOOT_TOP_HEIGHT, CoordinateY( half( BOOT_TOP_HEIGHT ) ) );
    Polygon boot_base = Polygon::rectangle( BOOT_BASE_WIDTH, BOOT_BASE_HEIGHT, Coordinate( offset_base_x, half( BOOT_BASE_HEIGHT ) ) );
    boot_drawing.draw( color( BOOT ), boot_shin );
    boot_drawing.draw( color( BOOT ), boot_base );

    Line lace1 = Line( Coordinate( 5.0, 7.5 ), Coordinate( -3.0, 7.5 ) );
    Line lace2 = Line( Coordinate( 5.0, 6.0 ), Coordinate( -3.0, 6.0 ) );
    boot_drawing.draw( color( BOOT_LACE ), lace1, 1.0 );
    boot_drawing.draw( color( BOOT_LACE ), lace2, 1.0 );

    // todo turn in Cycles

    cuint STEP_DURATION = 4.0;
    cPlanc STEP_DISTANCE = 4.0;
    cPlanc STEP_HEIGHT = 2.0;

    Cycle foot_cycle_x = Cycle( STEP_DURATION, STEP_DISTANCE );
    Cycle foot_cycle_y = Cycle( STEP_DURATION, STEP_HEIGHT, 0.75 );

    Planc foot_offset_x = foot_cycle_x.at( age( ) );
    Planc foot_offset_y = foot_cycle_y.at( age( ) );;
    Planc foot_offset_y_back = max( -foot_offset_y, P0 );
    Planc foot_offset_y_front = max( foot_offset_y, P0 );

    { // back leg
        Coordinate hip = Climber::hip( false );
        Coordinate foot = Climber::foot( false );

        if( Object::ground( ) )
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
        Coordinate hip = Climber::hip( true );
        Coordinate foot = Climber::foot( true );

        if( Object::ground( ) )
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

void Climber::draw_arm( cbool _front )
{
    draw( color( JACKET ), Polygon::circle( SHOULDER_RADIUS, shoulder( _front ) ) );
    draw( color( JACKET ), Polygon::circle( half( arm_width( _front ) ), elbow( _front ) ) );

    draw( color( JACKET ), Line( shoulder( _front ), elbow( _front ) ), arm_width( _front ) );
    draw( color( JACKET ), Line( elbow( _front ), hand( _front ) ), arm_width( _front ) );

    draw_hand( _front );
}

void Climber::draw_hand( cbool _front )
{
    if( _front )
    {
        bool finger_down = ( ( m_arrow_reload_timer.remaining( ) > half( RELOAD_TIME ) ) || ( m_hook_reload_timer.remaining( ) > half( RELOAD_TIME ) ) );

        Coordinate hand = Climber::hand( true );

        Coordinate finger_base = hand + VectorA( aim_angle( ) + negate( RIGHT, aiming_left( ) ), half( FRONT_ARM_WIDTH - FINGER_WIDTH ) );
        Coordinate finger_tip = finger_base + VectorA( aim_angle( ), finger_down ? FINGER_LENGTH_DOWN : FINGER_LENGTH_OUT );

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
    Coordinate hook_base = hook( ).hook_base( ) - position( );

    Vector rope_vector( hook_base );
    Line rope( rope_vector );

    // draw rope base
    draw( ROPE_BASE_COLOR, rope, ROPE_WIDTH );

    { // draw rope detail coils
        Vector rope_chunk = VectorA( rope_vector.angle( ), ROPE_WIDTH );
        for_range( i, ( half( rope_vector.magnitude( ) / rope_chunk.magnitude( ) ) ) )
        {
            Polygon rope_strip_rect = Polygon::rectangle( ROPE_WIDTH, ROPE_WIDTH, hook_base - ( rope_chunk * ( i + 1 ) * 2 ), rope_chunk.angle( ) );
            draw( ROPE_ALT_COLOR, rope_strip_rect );
        }
    }

    #if defined ( AXN_DEBUG )
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

    #if defined ( AXN_DEBUG )
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

Climber::HealthBar::HealthBar( Climber * climber ) : HeadUpDisplay( half( HEALTH_BAR_WIDTH ), half( HEALTH_BAR_HEIGHT ), HEALTH_BAR_WIDTH, HEALTH_BAR_HEIGHT ), m_climber( climber )
{
    static_once( ) { Assert( HEALTH_BAR_RED_START < HEALTH_BAR_YELLOW_START ); }
}

void Climber::HealthBar::render( Camera * camera )
{
    HeadUpDisplay::render( camera );

    if( Climber * climber = m_climber )
    {
        FixedRectangle cref rect = bounds( camera );
        Planc width = rect.width( ) - ( HEALTH_BAR_BORDER_WIDTH * 3.0 );
        Planc height = rect.height( ) - ( HEALTH_BAR_BORDER_WIDTH * 3.0 );
        Coordinate center = rect.center( );

        draw( BLACK, Polygon::rectangle( width + ( HEALTH_BAR_BORDER_WIDTH * 3.0 ), height + ( HEALTH_BAR_BORDER_WIDTH * 3.0 ), center ) );
        draw( WHITE, Polygon::rectangle( width + ( HEALTH_BAR_BORDER_WIDTH * 2.0 ), height + ( HEALTH_BAR_BORDER_WIDTH * 2.0 ), center ) );
        draw( BLACK, Polygon::rectangle( width + ( HEALTH_BAR_BORDER_WIDTH * 1.0 ), height + ( HEALTH_BAR_BORDER_WIDTH * 1.0 ), center ) );

        draw( BLACK, Polygon::rectangle( width, height, center ) );

        if( climber->alive( ) )
        {
            dec health_percentage = climber->health( ) / climber->max_health( );
            Assert( in_range( health_percentage, 0.0, 1.0, true ) );

            Color health_color;
            if( climber->god( ) )
            {
                health_color = CYAN;
            }
            else if( health_percentage == 1.0 )
            {
                health_color = GREEN;
            }
            else if( health_percentage >= HEALTH_BAR_YELLOW_START )
            {
                health_color = ColorSlider( YELLOW, GREEN ).color_at( ( health_percentage - HEALTH_BAR_YELLOW_START ) * inverse( 1.0 - HEALTH_BAR_YELLOW_START ) );
            }
            else if( health_percentage >= HEALTH_BAR_RED_START )
            {
                health_color = ColorSlider( RED, YELLOW ).color_at( ( health_percentage - HEALTH_BAR_RED_START ) * inverse( 1.0 - HEALTH_BAR_RED_START ) );
            }
            else
            {
                health_color = RED;
            }

            draw( health_color, Polygon::rectangle( width * health_percentage, height,
                                                    center - VectorX( half( width * ( 1.0 - health_percentage ) ) ) ) );
        }
    }
}

Climber::LowHealthAlertEffect::LowHealthAlertEffect( Climber * climber ) : m_climber( climber ) { }

void Climber::LowHealthAlertEffect::render( Camera * camera )
{
    ScreenEffect::render( camera );

    // todo constants

    const Span<dec> PULSE_DURATION = { 16.0, 32.0 };

    cdec SCREEN_FADE_DISTANCE = 0.5;

    cColor BASE_COLOR = RED;
    cColor TRANSPARENT_COLOR = BASE_COLOR.a( 0.0 );
    cdec BASE_ALPHA = 0.5;

    cdec HEALTH_THRESHOLD_UPPER = 0.333;
    cdec HEALTH_THRESHOLD_LOWER = 0.1;

    Assert( HEALTH_THRESHOLD_UPPER > HEALTH_THRESHOLD_LOWER );

    if( m_climber->alive( ) && ( m_climber->health_percentage( ) <= HEALTH_THRESHOLD_UPPER ) )
    {
        dec alpha_health = 1.0;
        if( m_climber->health_percentage( ) > HEALTH_THRESHOLD_LOWER )
        {
            alpha_health = 1.0 - ( ( m_climber->health_percentage( ) - HEALTH_THRESHOLD_LOWER ) / ( HEALTH_THRESHOLD_UPPER - HEALTH_THRESHOLD_LOWER ) );
        }

        dec alpha_time = BASE_ALPHA * abs( sin( (dec)( (dec)m_climber->age( ) / ( PULSE_DURATION.min( ) + ( PULSE_DURATION.range( ) * m_climber->health_percentage( ) ) ) ) ) );
        Color color = BASE_COLOR.a( alpha_health * alpha_time );

        FixedRectangle screen_bounds = camera->bounds( ) - camera->center( );
        dec effect_distance = SCREEN_FADE_DISTANCE * min( screen_bounds.width( ), screen_bounds.height( ) );

        FixedRectangle clear_bounds( screen_bounds.width( ) - effect_distance, screen_bounds.height( ) - effect_distance, screen_bounds.center( ) );

        draw( { color, color, TRANSPARENT_COLOR, TRANSPARENT_COLOR }, Polygon( { screen_bounds.top_right( ), screen_bounds.top_left( ), clear_bounds.top_left( ), clear_bounds.top_right( ) } ) );
        draw( { color, color, TRANSPARENT_COLOR, TRANSPARENT_COLOR }, Polygon( { screen_bounds.top_left( ), screen_bounds.bottom_left( ), clear_bounds.bottom_left( ), clear_bounds.top_left( ) } ) );
        draw( { color, color, TRANSPARENT_COLOR, TRANSPARENT_COLOR }, Polygon( { screen_bounds.bottom_left( ), screen_bounds.bottom_right( ), clear_bounds.bottom_right( ), clear_bounds.bottom_left( ) } ) );
        draw( { color, color, TRANSPARENT_COLOR, TRANSPARENT_COLOR }, Polygon( { screen_bounds.bottom_right( ), screen_bounds.top_right( ), clear_bounds.top_right( ), clear_bounds.bottom_right( ) } ) );
    }
}

void Climber::input( Input * _input )
{
    if( KeyInput * key_input = dynamic_cast<KeyInput *>( _input ) )
    {
        KeyInput::Key key = key_input->key;
        KeyInput::Dynamic dynamic = key_input->dynamic;

        bool pressed = ( dynamic == KeyInput::PRESSED );
        bool held = ( dynamic == KeyInput::HELD );

        bool down = ( pressed || held );

        switch( key )
        {
            case ' ':
            {
                jump( down );
                break;
            }
            case 'w':
            {
                looking_up( down );
                break;
            }
            case 's':
            {
                looking_down( down );
                break;
            }
            case 'a':
            {
                moving_left( down );
                break;
            }
            case 'd':
            {
                moving_right( down );
                break;
            }
            #if defined ( AXN_DEBUG )
            case 'h':
            {
                if( held )
                {
                    heal_full( );
                }
                break;
            }
            case 'k':
            {
                if( pressed )
                {
                    if( Debug::shifty )
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
                break;
            }
            case 'g':
            {
                if( pressed )
                {
                    god( !god( ) );
                }
                break;
            }
            #endif
            default:
            {
                break;
            }
        }

        return;
    }

    if( MouseInput * mouse_input = dynamic_cast<MouseInput *>( _input ) )
    {
        MouseInput::Button button = mouse_input->button;
        MouseInput::Dynamic dynamic = mouse_input->dynamic;

        bool move = ( dynamic == MouseInput::MOVE );
        bool pressed = ( dynamic == MouseInput::PRESSED );
        bool released = ( dynamic == MouseInput::RELEASED );

        if( move )
        {
            if( button == MouseInput::NO_BUTTON )
            {
                // todo should this happen here?
                aim( Angle( position( ), world( )->camera( )->cursor_world_position( ) ) );
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

        return;
    }

    if( ControllerInput * controller_input = dynamic_cast<ControllerInput *>( _input ) )
    {
        if( ControllerButtonInput * button_input = dynamic_cast<ControllerButtonInput *>( controller_input ) )
        {
            ControllerButtonInput::Button button = button_input->button;
            ControllerButtonInput::Dynamic dynamic = button_input->dynamic;

            bool pressed = ( dynamic == ControllerButtonInput::PRESSED );

            switch( button )
            {
                default:
                {
                    break;
                }

                case ControllerButtonInput::L1_BUTTON:
                case ControllerButtonInput::RIGHT_DOWN_BUTTON:
                {
                    if( alive( ) )
                    {
                        jump( pressed );
                    }
                    break;
                }

                case ControllerButtonInput::R1_BUTTON:
                {
                    if( pressed )
                    {
                        launch_arrow( );
                    }
                    else
                    {
                        release_arrow( );
                    }
                    break;
                }

                case ControllerButtonInput::R2_BUTTON:
                {
                    if( pressed )
                    {
                        launch_hook( );
                    }
                    else
                    {
                        release_hook( );
                    }
                    break;
                }

                case ControllerButtonInput::LEFT_UP_BUTTON:
                {
                    looking_up( pressed );
                    break;
                }

                case ControllerButtonInput::LEFT_DOWN_BUTTON:
                {
                    looking_down( pressed );
                    break;
                }

                case ControllerButtonInput::LEFT_LEFT_BUTTON:
                {
                    moving_left( pressed );
                    break;
                }

                case ControllerButtonInput::LEFT_RIGHT_BUTTON:
                {
                    moving_right( pressed );
                    break;
                }
            }
        }

        if( ControllerJoystickInput * joystick_input = dynamic_cast<ControllerJoystickInput *>( controller_input ) )
        {
            if( joystick_input->joystick == ControllerJoystickInput::LEFT_JOYSTICK )
            {
                if( joystick_input->dead_zone )
                {
                    movement_stop( );
                }
                else
                {
                    Vector v = joystick_input->vector;

                    if( is_pos( v.dx( ) ) )
                    {
                        moving_left( false );
                        moving_right( true );
                    }
                    else if( is_neg( v.dx( ) ) )
                    {
                        moving_left( true );
                        moving_right( false );
                    }

                    if( is_pos( v.dy( ) ) )
                    {
                        looking_up( true );
                        looking_down( false );
                    }
                    else if( is_neg( v.dy( ) ) )
                    {
                        looking_up( false );
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

        return;
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
