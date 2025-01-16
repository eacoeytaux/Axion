#include "Climber.hpp"
#include "Terrain.hpp"
#include "Arrow.hpp"
#include "Dust.hpp"

using mtmercy::Climber;

namespace
{
cPlanc HEIGHT = METER * 1.85;
cPlanc WIDTH = HEIGHT / 2.5;

cPlanc DEFAULT_SPEED_GROUND = 1.0;
cPlanc DEFAULT_SPEED_AIR = 0.25;
cPlanc JUMP_STRENGTH = METER / 7.0;
cdec JUMP_DEGRATION_RATIO = 0.9;
cuint JUMP_MAX_DURATION = 8;
cuint JUMP_RESET_WAIT_TIME = 0;

cPlanc LIGHT_SIGHT = HEIGHT * 3.0;

// crossbox / hook
cPlanc DEFAULT_ARROW_LAUNCH_SPEED = 16.0;
cdec ARROW_VELOCITY_DAMPEN = 0.25;
cPlanc DEFAULT_HOOK_LAUNCH_SPEED = 16.0;
cPlanc DEFAULT_ROPE_GROWTH_SPEED = 16.0;
cPlanc DEFAULT_ROPE_RETRACT_SPEED = 32.0;
cuint RELOAD_TIME = 20;

cPlanc DUST_MINIMUM_SPEED = METER / 6.0;
cdec DUST_VELOCITY_DAMPEN_RATIO = 0.1;
const Span<Planc> DUST_RISE = { 0.25, 0.75 };
const Span<uint> DUST_LANDING_COUNT = { 9, 15 };
const Span<dec> DUST_LANDING_ANGLE = { 0.0, RIGHT_ANGLE / 3.0 };
const Span<uint> DUST_WALKING_COUNT = { 1, 3 };
const Span<uint> DUST_WALKING_TIME = { 0, 5 };

cPlanc HEAD_RADIUS = HEIGHT / 3.9;
cPlanc HEAD_Y_OFFSET = HEAD_RADIUS * 0.05;
cPlanc FACE_RADIUS = HEAD_RADIUS * 0.75;
cPlanc EYE_SPACING = FACE_RADIUS * 0.55;
cPlanc EYE_RADIUS = 1.5;
cPlanc NOSE_WIDTH = 3.5;
cPlanc NOSE_HEIGHT = 8.0;
cPlanc NOSE_RADIUS = 1.5;
cPlanc NOSE_OFFSET_Y = -3.0;
cPlanc EAR_RADIUS = 3.0;

cPlanc BODY_HEIGHT = HEIGHT / 3.0;
cPlanc LEG_HEIGHT = HEIGHT / 4.0;

cPlanc BODY_WIDTH = HEIGHT / 2.0;

cPlanc PANTS_WIDTH_UPPER = BODY_WIDTH - 6.0;
cPlanc PANTS_WIDTH_LOWER = PANTS_WIDTH_UPPER * 0.5;
cPlanc PANTS_HEIGHT = BODY_HEIGHT / 2.0;

cPlanc FRONT_ARM_WIDTH = HEIGHT / 6.0;
cPlanc FRONT_HAND_WIDTH = FRONT_ARM_WIDTH;
cPlanc FRONT_HAND_OFFSET = METER * ( 3.0 / 8.0 );

cPlanc BACK_ARM_WIDTH = FRONT_ARM_WIDTH * 0.9;
cPlanc BACK_HAND_WIDTH = BACK_ARM_WIDTH;
cPlanc BACK_HAND_OFFSET = METER * ( 3.0 / 4.0 );

cPlanc FINGER_WIDTH = FRONT_ARM_WIDTH / 3.0;
cPlanc FINGER_LENGTH_OUT = FRONT_ARM_WIDTH * 0.72;
cPlanc FINGER_LENGTH_DOWN = FRONT_ARM_WIDTH * 0.64;

cPlanc UPPER_ARM_LENGTH = BODY_HEIGHT * 0.8;
cPlanc LOWER_ARM_LENGTH = UPPER_ARM_LENGTH * 0.8;

cPlanc SHOULDER_RADIUS = half( FRONT_ARM_WIDTH );

cPlanc LEG_WIDTH = HEIGHT / 5.25;

cPlanc BOOT_TOP_WIDTH = LEG_WIDTH * 1.3;
cPlanc BOOT_TOP_HEIGHT = BOOT_TOP_WIDTH * 0.66;
cPlanc BOOT_BASE_WIDTH = LEG_WIDTH * 1.6;
cPlanc BOOT_BASE_HEIGHT = BOOT_BASE_WIDTH * 0.3;
cPlanc BOOT_SOLE_HEIGHT = 1.0;
cPlanc BOOT_SOLE_SUB_WIDTH = 3.0;
cPlanc BOOT_SOLE_SUB_GAP_WIDTH = 1.0;
cPlanc BOOT_SOLE_SUB_HEIGHT = 0.75;

cPlanc BELT_THICKNESS = 3.0;
cPlanc BELT_BUCKLE_WIDTH = 5.0;

cuint BOB_PERIOD = 20;
cPlanc HEAD_BOB_DISTANCE = 1.0;
cPlanc SHOULDER_BOB_DISTANCE = 0.8;
cdec SHOULDER_BOB_OFFSET = 0.75;

static uint BLINK_DURATION = 6;
static Span<uint> BLINK_WAIT = { 240, 480 };

const Color CROSSBOW_COLOR = Color::rgb( 0xA54B23 );

cPlanc HOOK_LENGTH = 38.0;
cPlanc HOOK_THICKNESS = 5.0;
cPlanc HOOK_TIP_LENGTH = 2.5;
const Angle HOOK_ANGLE = RIGHT_ANGLE;
const Color HOOK_COLOR = Color::rgb( 0x9C9C9C );

cPlanc ROPE_WIDTH = 4.0;
const Color ROPE_BASE_COLOR = Color::rgb( 0xDAA420 );
const Color ROPE_ALT_COLOR = Color::rgb( 0xB9870F );

cPlanc HOOK_PATH_LENGTH = METER * 25;
cdec HOOK_PATH_ALPHA_START = 0.8;
const Color HOOK_PATH_COLOR = WHITE;

cPlanc ARROW_PATH_LENGTH = HOOK_PATH_LENGTH;
cdec ARROW_PATH_ALPHA_START = HOOK_PATH_ALPHA_START;
const Color ARROW_PATH_COLOR = RED;
} // namespace

Climber::~Climber( )
{
    room( )->camera( )->remove_hud_element( &m_healthbar );
    room( )->camera( )->remove_screen_effect( &m_low_health_effect );
}

Climber::Climber( Room * room, Coordinate cref _position )
    : Player( room, _position ),
    m_hook( room, this ),
    m_healthbar( this ),
    m_low_health_effect( this )
{
    interactive( true );

    needs_render_always( true );

    space( Polygon::rectangle( WIDTH, HEIGHT ) );

    eye_info( EYE_RADIUS, BLINK_DURATION, BLINK_WAIT );

    m_skin = MOHAWK;

    m_movement_speed_ground = DEFAULT_SPEED_GROUND;
    m_movement_speed_air = DEFAULT_SPEED_AIR;

    m_reload_timer.reset( 0 );
    m_dust_timer.reset( 0 );

    m_arrow_feather_color = Random::rColor( );

    room->camera( )->add_hud_element( &m_healthbar );
    room->camera( )->add_screen_effect( &m_low_health_effect );
}

void Climber::update( )
{
    Player::update( );

    if( alive( ) )
    {
        heal( 0.01 );
    }

    m_reload_timer.tick( );
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
                    uint dust_count = Random::rint( DUST_WALKING_COUNT );
                    do_count( dust_count )
                    {
                        room( )->add_object( new Dust( room( ), position( ) + foot( true ), ( velocity( ) * DUST_VELOCITY_DAMPEN_RATIO ) + Vector::Y( Random::rPlanc( DUST_RISE ) ), room( )->terrain( )->dust_color( ) ) );
                    }
                    m_dust_timer.reset( Random::rint( DUST_WALKING_TIME ) );
                }
            }
        }
    }

    if( m_launching_arrow || m_launching_hook )
    {
        if( !m_reload_timer.remaining( ) )
        {
            if( m_launching_arrow )
            {
                // static SoundClip twang( "twang.wav" );
                // twang.play( 0.5 );

                Angle aim = aim_angle( ) + aim_shake( );
                Arrow * arrow = new Arrow( Arrow::base( room( ), position( ), Vector::A( aim, DEFAULT_ARROW_LAUNCH_SPEED ) + ( velocity( ) * ARROW_VELOCITY_DAMPEN ), m_arrow_feather_color ) );
                arrow->position( arrow->position( ) + Vector::A( aim_angle( ), arrow->length( ) ) );

                room( )->add_object( arrow );

                m_arrow_feather_color = Random::rColor( );

                m_launching_arrow = false;
            }
            else if( m_launching_hook )
            {
                m_hook.fire( Vector::A( aim_angle( ), DEFAULT_HOOK_LAUNCH_SPEED ) + ( velocity( ) * ARROW_VELOCITY_DAMPEN ) );
            }
            m_reload_timer.reset( RELOAD_TIME );
        }
    }
    else
    {
        if( !m_launching_hook )
        {
            m_hook.retract( );
        }

        if( m_slashing )
        {
        }
    }

    m_hook.update( );
}

void Climber::update_velocity( )
{
    cPlanc _movement_speed = Object::ground( ) ? m_movement_speed_ground : m_movement_speed_air;

    if( !gravity_ratio( ) )
    { // can fly?
        if( m_looking_up )
        {
            if( m_moving_right && !m_moving_left )
            {
                add_velocity( Vector::A( Angle( half( RIGHT_ANGLE_1 ) ), _movement_speed ) - Vector::X( _movement_speed ) );
            }
            else if( m_moving_left && !m_moving_right )
            {
                add_velocity( Vector::A( Angle( half( RIGHT_ANGLE_3 ) ), _movement_speed ) - Vector::X( -_movement_speed ) );
            }
            else
            {
                add_velocity( Vector::Y( _movement_speed ) );
            }
        }
        if( m_looking_down )
        {
            if( m_moving_right && !m_moving_left )
            {
                add_velocity( Vector::A( Angle( -half( RIGHT_ANGLE_1 ) ), _movement_speed ) - Vector::X( _movement_speed ) );
            }
            else if( m_moving_left && !m_moving_right )
            {
                add_velocity( Vector::A( Angle( -half( RIGHT_ANGLE_3 ) ), _movement_speed ) - Vector::X( -_movement_speed ) );
            }
            else
            {
                add_velocity( Vector::Y( -_movement_speed ) );
            }
        }
    }

    if( m_moving_right )
    {
        add_velocity( Vector::X( _movement_speed ) );
    }
    if( m_moving_left )
    {
        add_velocity( Vector::X( -_movement_speed ) );
    }

    if( ( m_hook.state( ) == Hook::HOOKED ) && ( m_hook.rope_length( ) > m_hook.max_rope_length( ) ) )
    {
        Vector rope_pull( position( ), m_hook.hook_base( ) );
        rope_pull.magnitude( m_hook.rope_length( ) - m_hook.max_rope_length( ) );

        add_velocity( rope_pull );
    }

    Player::update_velocity( );

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
            add_velocity( ( Vector::Y( JUMP_STRENGTH ) - GRAVITY ) * ( 1.0 - m_jump_degradation ) );
            m_jump_degradation = 1.0 - ( ( 1.0 - m_jump_degradation ) * JUMP_DEGRATION_RATIO );
            ground( nullptr );
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
    m_moving_right = _moving;
}

bool Climber::moving_left( ) const
{
    return m_moving_left && !m_moving_right;
}

void Climber::moving_left( cbool _moving )
{
    m_moving_left = _moving;
}

bool Climber::looking_up( ) const
{
    return m_looking_up && !m_looking_down;
}

void Climber::looking_up( cbool _looking )
{
    m_looking_up = _looking;
}

bool Climber::looking_down( ) const
{
    return m_looking_down && !m_looking_up;
}

void Climber::looking_down( cbool _looking )
{
    m_looking_down = _looking;
}

void Climber::jump( cbool _jumping )
{
    if( _jumping && looking_down( ) )
    {
        passing_terrain( true );
    }
    else
    {
        m_jumping = _jumping;
    }
}

void Climber::ground( TerrainEdge * ground )
{
    if( ground && !Object::ground( ) )
    {
        Vector dust_velocity = ( velocity( ).dy( 0.0 ) * DUST_VELOCITY_DAMPEN_RATIO );
        uint dust_count = Random::rint( DUST_LANDING_COUNT );
        for_range( i, dust_count )
        {
            bool b = ( i < half( dust_count ) );

            Coordinate c = position( ) + foot( b ) + ( Vector( foot( b ), foot( !b ) ) * ( (dec)i / (dec)dust_count ) );

            Angle angle = ground->vector( ).angle( ) + Random::rAngle( DUST_LANDING_ANGLE.min( ), DUST_LANDING_ANGLE.max( ) ) + ( b ? 0.0 : PI );

            room( )->add_object( new Dust( room( ), c, dust_velocity + Vector::A( angle, Random::rPlanc( DUST_RISE ) ), room( )->terrain( )->dust_color( ) ) );
        }
    }

    Object::ground( ground );
}

Angle Climber::aim_angle( ) const
{
    return m_aim_angle;
}

bool Climber::aiming_right( ) const
{
    return !aiming_left( );
}

bool Climber::aiming_left( ) const
{
    return ( aim_angle( ).in_quadrant( Q2 ) || aim_angle( ).in_quadrant( Q3 ) || ( aim_angle( ) == RIGHT_ANGLE_2 ) );
}

bool Climber::aiming_up( ) const
{
    return ( aim_angle( ).in_quadrant( Q1 ) || aim_angle( ).in_quadrant( Q2 ) || ( aim_angle( ) == RIGHT_ANGLE_1 ) );
}

bool Climber::aiming_down( ) const
{
    return !aiming_up( );
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
    return Random::negate( half( m_aim_shake_angle ) );
}

Angle Climber::aim_shake_range( ) const
{
    return m_aim_shake_angle;
}

void Climber::launch_hook( )
{
    m_launching_hook = true;
}

void Climber::launch_arrow( )
{
    m_launching_arrow = true;
}

void Climber::release_bow( )
{
    m_launching_hook = false;
    m_launching_arrow = false;
}

Coordinate Climber::head_center( ) const
{
    return Coordinate( 0.0, BODY_HEIGHT + HEAD_Y_OFFSET + ( HEAD_BOB_DISTANCE * sin( (dec)age( ) / BOB_PERIOD ) ) );
}

Coordinate Climber::face_center( ) const
{
    return head_center( ) + Vector::A( aim_angle( ), ( HEAD_RADIUS - FACE_RADIUS ) / 2.0 );
}

Coordinate Climber::shoulder( cbool _front ) const
{
    return Coordinate( negate( half( BODY_WIDTH - ( SHOULDER_RADIUS * 2.0 ) ), ( aiming_left( ) ^ _front ) ),
                       half( BODY_HEIGHT - ( SHOULDER_RADIUS * 2.0 ) ) + ( SHOULDER_BOB_DISTANCE * sin( (dec)( age( ) + ( BOB_PERIOD * SHOULDER_BOB_OFFSET ) ) / BOB_PERIOD ) ) );
}

Coordinate Climber::hand( cbool _front ) const
{
    return Vector::A( aim_angle( ) - negate( _front ? RIGHT_ANGLE : ( TAU / 32.0 ), aiming_left( ) ), _front ? FRONT_HAND_OFFSET : BACK_HAND_OFFSET );
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

#ifdef AXN_DEBUG
Drawing Climber::debug_overlay( ) const
{
    Drawing debug_overlay;

    if( m_hook.state( ) != Hook::LOADED )
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

    draw( m_hook );

    draw_legs( );
    draw_torso( );
    draw_arm( false );
    draw_hand( false );
    draw_head( );
    draw_crossbow( );
    draw_arm( true );
    draw_hand( true );
    draw_arrow( );
}

void Climber::draw_head( )
{
    // head
    draw( color( SKIN ), Polygon::circle( HEAD_RADIUS, head_center( ) ) );

    if( m_skin == MOHAWK )
    {
        // shaved hair
        varray<Coordinate> top_half_coordinates = Polygon::circle( HEAD_RADIUS, head_center( ) ).coordinates( );
        uint count = ( top_half_coordinates.size( ) / 2 ) - 1;
        for_range( i, count ) top_half_coordinates.remove_index( half( top_half_coordinates.size( ) ) );
        draw( color( HAIR ).a( 0.375 ), Polygon( top_half_coordinates ) );
    }

    // ears
    draw( color( SKIN ), Polygon::circle( EAR_RADIUS, head_center( ) + Vector::X( HEAD_RADIUS - EAR_RADIUS ) ) );
    draw( color( SKIN ), Polygon::circle( EAR_RADIUS, head_center( ) - Vector::X( HEAD_RADIUS - EAR_RADIUS ) ) );

    // face
    draw( color( SKIN ), Polygon::circle( FACE_RADIUS, face_center( ) ) );

    // eyes
    draw_eyes( face_center( ) + Vector( EYE_SPACING, 0.0 ), face_center( ) + Vector( -EYE_SPACING, 0.0 ) );

    // eyebrows
    draw( color( HAIR ), Line( face_center( ) + Vector( EYE_SPACING + 3.0, 2.1 ), face_center( ) + Vector( EYE_SPACING - 4.0, 2.1 ) ), EYE_RADIUS * 2.0 );
    draw( color( HAIR ), Line( face_center( ) + Vector( -EYE_SPACING + 4.0, 2.1 ), face_center( ) + Vector( -EYE_SPACING - 3.0, 2.1 ) ), EYE_RADIUS * 2.0 );

    if( m_skin == MOHAWK )
    {
        { // mohawk
            Coordinate top = head_center( ) + Vector::Y( HEAD_RADIUS + 3.0 );
            Coordinate top_right = top + Vector( 5.0, 0.0 );
            Coordinate top_left = top + Vector( -5.0, 0.0 );

            Coordinate bottom = face_center( ) + Vector::Y( FACE_RADIUS - 4.0 );
            Coordinate bottom_right = bottom + Vector( 5.0, 0.0 );
            Coordinate bottom_left = bottom + Vector( -5.0, 0.0 );

            draw( color( HAIR ), Line( top, bottom ), 10.0 );
        }

        { // beard
            draw( color( HAIR ), Polygon::rectangle( FACE_RADIUS * 2.0, 12.0, face_center( ) + Vector::Y( -12.0 ) ) );
            draw( color( HAIR ), Polygon::rectangle( 12.0, 4.0, face_center( ) + Vector::Y( -6.0 ) ) );
        }
    }
}

void Climber::draw_torso( )
{
    Coordinate waist = Coordinate( 0.0, -half( BODY_HEIGHT ) );

    // pants base
    draw( color( PANTS ), Polygon( { waist + Coordinate( half( PANTS_WIDTH_UPPER ), 0.0 ),
                                                  waist + Coordinate( -half( PANTS_WIDTH_UPPER ), 0.0 ),
                                                  waist + Coordinate( -half( PANTS_WIDTH_LOWER ), -half( PANTS_HEIGHT ) ),
                                                  waist + Coordinate( half( PANTS_WIDTH_LOWER ), -half( PANTS_HEIGHT ) ) } ) );

    // undershirt
    draw( color( UNDERSHIRT ), Polygon::rectangle( half( BODY_WIDTH ), BODY_HEIGHT ) );

    // belt
    draw( color( BELT ), Line( waist - Vector::X( half( PANTS_WIDTH_UPPER ) + ( BELT_THICKNESS / 4.0 ) ),
                               waist + Vector::X( half( PANTS_WIDTH_UPPER ) + ( BELT_THICKNESS / 4.0 ) ) ), (dec)BELT_THICKNESS );
    draw( color( BELT_BUCKLE ), Polygon::rectangle( BELT_BUCKLE_WIDTH, BELT_BUCKLE_WIDTH, waist ) );

    // jacket
    draw( color( JACKET ), Polygon::circle( half( BACK_ARM_WIDTH ), shoulder( false ) ) );
    draw( color( JACKET ), Polygon::rectangle( BODY_WIDTH / 3.0, BODY_HEIGHT - SHOULDER_RADIUS, Coordinate( BODY_WIDTH / 3.5, -half( SHOULDER_RADIUS ) ) ) );
    draw( color( JACKET ), Polygon::rectangle( BODY_WIDTH / 3.0, BODY_HEIGHT - SHOULDER_RADIUS, Coordinate( -BODY_WIDTH / 3.5, -half( SHOULDER_RADIUS ) ) ) );
}

void Climber::draw_legs( )
{
    Drawing boot_drawing;

    Planc x_offset_base = half( BOOT_BASE_WIDTH - BOOT_TOP_WIDTH );
    Polygon boot_sole = Polygon::rectangle( BOOT_BASE_WIDTH, BOOT_SOLE_HEIGHT, Coordinate( x_offset_base, -half( BOOT_SOLE_HEIGHT ) ) );
    boot_drawing.draw( color( BOOT_SOLE ), boot_sole );
    {
        Polygon boot_sub_sole = Polygon::rectangle( BOOT_SOLE_SUB_WIDTH, BOOT_SOLE_SUB_HEIGHT, Coordinate( x_offset_base - half( BOOT_BASE_WIDTH ) + half( BOOT_SOLE_SUB_WIDTH ), -BOOT_SOLE_SUB_HEIGHT - half( BOOT_SOLE_HEIGHT ) ) );
        boot_drawing.draw( color( BOOT_SOLE ), boot_sub_sole );
        for_range( i, floor( half( half( BOOT_BASE_WIDTH ) ) ) )
        {
            Polygon boot_sub_sole = Polygon::rectangle( BOOT_SOLE_SUB_WIDTH, BOOT_SOLE_SUB_HEIGHT, Coordinate( x_offset_base + half( BOOT_BASE_WIDTH ) - half( BOOT_SOLE_SUB_WIDTH ) - ( ( BOOT_SOLE_SUB_WIDTH + BOOT_SOLE_SUB_GAP_WIDTH ) * (Planc)i ), -BOOT_SOLE_SUB_HEIGHT - half( BOOT_SOLE_HEIGHT ) ) );
            boot_drawing.draw( color( BOOT_SOLE ), boot_sub_sole );
        }
    }

    Polygon boot_shin = Polygon::rectangle( BOOT_TOP_WIDTH, BOOT_TOP_HEIGHT, Coordinate( 0.0, half( BOOT_TOP_HEIGHT ) ) );
    Polygon boot_base = Polygon::rectangle( BOOT_BASE_WIDTH, BOOT_BASE_HEIGHT, Coordinate( x_offset_base, half( BOOT_BASE_HEIGHT ) ) );
    boot_drawing.draw( color( BOOT ), boot_shin );
    boot_drawing.draw( color( BOOT ), boot_base );

    Line lace1 = Line( Coordinate( 5.0, 7.5 ), Coordinate( -3.0, 7.5 ) );
    Line lace2 = Line( Coordinate( 5.0, 6.0 ), Coordinate( -3.0, 6.0 ) );
    boot_drawing.draw( color( BOOT_LACE ), lace1, 1.0 );
    boot_drawing.draw( color( BOOT_LACE ), lace2, 1.0 );

    Planc foot_offset_x = ( sin( room( )->age( ) / 4.0 ) * 4.0 );
    Planc foot_offset_y = ( cos( room( )->age( ) / 4.0 ) * 2.0 );
    Planc foot_offset_y_back = max( -foot_offset_y, P0 );
    Planc foot_offset_y_front = max( foot_offset_y, P0 );

    { // back leg
        Coordinate hip = Climber::hip( false );
        Coordinate foot = Climber::foot( false );

        if( Object::ground( ) )
        {
            if( moving_left( ) ^ moving_right( ) )
            {
                if( moving_left( ) )
                {
                    foot += Vector( foot_offset_x, foot_offset_y_back );
                }
                else if( moving_right( ) )
                {
                    foot += Vector( -foot_offset_x, foot_offset_y_back );
                }
            }
        }
        else if( !god( ) )
        {
            foot += Vector::Y( LEG_HEIGHT / 4.0 );
        }

        Coordinate knee = foot + Vector::Y( LEG_HEIGHT * ( 2.0 / 3.0 ) );

        Drawing boot_drawing_back = boot_drawing;
        if( facing_left( ) )
        {
            boot_drawing_back.mirror_y( );
        }
        boot_drawing_back.move( foot );

        draw( color( PANTS ), Line( hip, foot ), LEG_WIDTH );
        // draw( pants_color( ), Line( hip, knee ), LEG_WIDTH );
        // draw( pants_color( ), Polygon::circle( half( LEG_WIDTH ), knee ) );
        // draw( pants_color( ), Line( knee, foot ), LEG_WIDTH );

        draw( boot_drawing_back );
    }

    { // front leg
        Coordinate hip = Climber::hip( true );
        Coordinate foot = Climber::foot( true );

        if( Object::ground( ) )
        {
            if( moving_left( ) ^ moving_right( ) )
            {
                if( moving_left( ) )
                {
                    foot += Vector( -foot_offset_x, foot_offset_y_front );
                }
                else if( moving_right( ) )
                {
                    foot += Vector( foot_offset_x, foot_offset_y_front );
                }
            }
        }

        Coordinate knee = foot + Vector::Y( LEG_HEIGHT * ( 2.0 / 3.0 ) );

        Drawing boot_drawing_front = boot_drawing;
        if( facing_left( ) )
        {
            boot_drawing_front.mirror_y( );
        }
        boot_drawing_front.move( foot );

        draw( color( PANTS ), Line( hip, foot ), LEG_WIDTH );
        // draw( pants_color( ), Line( hip, knee ), LEG_WIDTH );
        // draw( pants_color( ), Polygon::circle( half( LEG_WIDTH ), knee ) );
        // draw( pants_color( ), Line( knee, foot ), LEG_WIDTH );

        draw( boot_drawing_front );
    }
}

void Climber::draw_arm( cbool _front )
{
    const Coordinate shoulder = Climber::shoulder( _front );
    const Coordinate hand = Climber::hand( _front );

    Joint arm = Joint( shoulder, hand, UPPER_ARM_LENGTH, LOWER_ARM_LENGTH, !( aiming_left( ) ) );

    draw( color( JACKET ), Polygon::circle( SHOULDER_RADIUS, shoulder ) );

    draw( color( JACKET ), Polygon::circle( half( _front ? FRONT_ARM_WIDTH : BACK_ARM_WIDTH ), arm.joint( ) ) );

    draw( color( JACKET ), arm.arm1( ), _front ? FRONT_ARM_WIDTH : BACK_ARM_WIDTH );
    draw( color( JACKET ), arm.arm2( ), _front ? FRONT_ARM_WIDTH : BACK_ARM_WIDTH );
}

void Climber::draw_hand( cbool _front )
{
    if( _front )
    {
        bool finger_down = ( m_reload_timer.remaining( ) <= ( RELOAD_TIME / 2 ) );

        Coordinate hand = Climber::hand( true );

        Coordinate finger_base = hand + Vector::A( aim_angle( ) + negate( RIGHT_ANGLE, aiming_left( ) ), half( FRONT_ARM_WIDTH - FINGER_WIDTH ) );
        Coordinate finger_tip = finger_base + Vector::A( aim_angle( ), finger_down ? FINGER_LENGTH_OUT : FINGER_LENGTH_DOWN );

        draw( color( SKIN ), Polygon::circle( half( FRONT_ARM_WIDTH ), hand ) );
        draw( color( SKIN ), Line( finger_base, finger_tip ), FINGER_WIDTH );
    }
    else
    {
        draw( color( SKIN ), Polygon::circle( half( BACK_ARM_WIDTH ), hand( false ) ) );
    }
}

void Climber::draw_crossbow( )
{
    Polygon crossbow_polygon = Polygon( { { 6.0, 0.0 }, Coordinate( 0.0, 0.0 ), Coordinate( -2.0, -2.0 ), Coordinate( -4.0, -2.0 ), Coordinate( -4.0, -4.0 ), Coordinate( 2.0, -1.0 ), Coordinate( 5.0, -1.0 ) } ).scale( 5.0 );

    if( aiming_left( ) )
    {
        crossbow_polygon.mirror_x( );
    }

    crossbow_polygon.rotate( aim_angle( ) );

    draw( CROSSBOW_COLOR, crossbow_polygon );
}

void Climber::draw_arrow( )
{
    Arrow arrow = Arrow::tip( room( ), position( ), Vector::A( aim_angle( ), DEFAULT_ARROW_LAUNCH_SPEED ), m_arrow_feather_color );
    arrow.render( );

    Vector v = Vector::A( aim_angle( ), arrow.length( ) );

    #ifdef AXN_DEBUG
    if( Debug::active && Object::draw_physics )
    {
        draw( arrow.trajection_drawing( ARROW_PATH_LENGTH, ARROW_PATH_COLOR, ARROW_PATH_ALPHA_START ).move( v ) );
    }
    #endif

    draw( Drawing( arrow ).move( v ) );
}

// todo make variable
Climber::HealthBar::HealthBar( Climber * climber ) : HeadUpDisplay( 0.5, 0.015, 1.0, 0.03 ), m_climber( climber ) { }

void Climber::HealthBar::render( Camera * camera )
{
    HeadUpDisplay::render( camera );

    cPlanc HEALTH_BAR_BORDER_WIDTH = 4.0;

    cdec RED_START = 0.1;
    cdec YELLOW_START = 0.5;
    static_once( ) { Assert( RED_START < YELLOW_START ); }

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
            else if( health_percentage >= YELLOW_START )
            {
                health_color = ColorSlider( YELLOW, GREEN ).color_at( ( health_percentage - YELLOW_START ) * inverse( 1.0 - YELLOW_START ) );
            }
            else if( health_percentage >= RED_START )
            {
                health_color = ColorSlider( RED, YELLOW ).color_at( ( health_percentage - RED_START ) * inverse( 1.0 - RED_START ) );
            }
            else
            {
                health_color = RED;
            }

            draw( health_color, Polygon::rectangle( width * health_percentage, height,
                                                    center - Vector::X( half( width * ( 1.0 - health_percentage ) ) ) ) );
        }
    }
}

Climber::LowHealthAlertEffect::LowHealthAlertEffect( Climber * climber ) : m_climber( climber ) { }

void Climber::LowHealthAlertEffect::render( Camera * camera )
{
    ScreenEffect::render( camera );

    const Span<dec> PULSE_DURATION = { 16.0, 32.0 };

    cdec SCREEN_FADE_DISTANCE = 0.5;

    const Color BASE_COLOR = RED;
    const Color TRANSPARENT_COLOR = BASE_COLOR.a( 0.0 );
    cdec BASE_ALPHA = 0.5;

    cdec HEALTH_THRESHOLD_UPPER = 0.333;
    cdec HEALTH_THRESHOLD_LOWER = 0.1;

    Assert( HEALTH_THRESHOLD_UPPER > HEALTH_THRESHOLD_LOWER );

    if( m_climber->health_percentage( ) <= HEALTH_THRESHOLD_UPPER )
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
            case 'g':
            {
                if( pressed )
                {
                    god( !god( ) );
                }
                break;
            }
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
            if( button != MouseInput::SCROLL_BUTTON )
            {
                aim( Angle( position( ), mouse_input->position ) );
            }
        }
        else
        {
            if( released )
            {
                release_bow( );
            }
            else if( pressed )
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
                    jump( pressed );
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
                        release_bow( );
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
                        release_bow( );
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

                    if( v.dx( ) > 0.0 )
                    {
                        moving_left( false );
                        moving_right( true );
                    }
                    else if( v.dx( ) < 0.0 )
                    {
                        moving_left( true );
                        moving_right( false );
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
    release_bow( );
    moving_left( false );
    moving_right( false );
    looking_up( false );
    looking_down( false );
    jump( false );
}

void Climber::hurt( dec health )
{
    Player::hurt( health );
}
