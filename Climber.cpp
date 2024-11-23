#include "Climber.hpp"
#include "Terrain.hpp"
#include "Arrow.hpp"
#include "Dust.hpp"

using mtmercy::Climber;

namespace
{
const Planc HEIGHT = METER * 1.85;
const Planc WIDTH = HEIGHT / 2.5;

const Planc DEFAULT_SPEED = 2.0;
const Planc TURN_SPEED = 3.0;
const Planc JUMP_STRENGTH = METER / 4.0;
const dec JUMP_DEGRATION_RATIO = 0.9;
const uint JUMP_MAX_DURATION = 15;
const uint JUMP_RESET_WAIT_TIME = 0;

const Planc LIGHT_SIGHT = HEIGHT * 3.0;

// crossbox / hook
const Planc DEFAULT_ARROW_LAUNCH_SPEED = 32.0;
const Planc DEFAULT_ROPE_GROWTH_SPEED = 33.0;
const Planc DEFAULT_ROPE_RETRACT_SPEED = 44.0;
const uint RELOAD_TIME = 10;

const Planc DUST_MINIMUM_SPEED = METER / 12.0;
const dec DUST_VELOCITY_DAMPEN_RATIO = 0.1;
const Span<Planc> DUST_RISE = { 0.5, 1.2 };
const Span<uint> DUST_LANDING_COUNT = { 9, 15 };
const Span<dec> DUST_LANDING_ANGLE = { 0.0, RIGHT_ANGLE / 3. };
const Span<uint> DUST_TIME = { 0, 5 };

const Planc HEAD_RADIUS = HEIGHT / 3.9;
const Planc HEAD_Y_OFFSET = HEAD_RADIUS * 0.05;
const Planc FACE_RADIUS = HEAD_RADIUS * 0.75;
const Planc EYE_SPACING = FACE_RADIUS * 0.55;
const Planc EYE_RADIUS = 1.5;
const Planc NOSE_WIDTH = 3.5;
const Planc NOSE_HEIGHT = 8.0;
const Planc NOSE_RADIUS = 1.5;
const Planc NOSE_OFFSET_Y = -3.0;
const Planc EAR_RADIUS = 3.0;

const Planc BODY_HEIGHT = HEIGHT / 3.0;
const Planc LEG_HEIGHT = HEIGHT / 4.0;

const Planc BODY_WIDTH = HEIGHT / 2.0;

const Planc PANTS_WIDTH_UPPER = BODY_WIDTH - 6.0;
const Planc PANTS_WIDTH_LOWER = PANTS_WIDTH_UPPER * 0.5;
const Planc PANTS_HEIGHT = BODY_HEIGHT / 2.0;
const Planc FRONT_ARM_WIDTH = HEIGHT / 6.0;
const Planc BACK_ARM_WIDTH = FRONT_ARM_WIDTH * 0.9;
const Planc SHOULDER_RADIUS = FRONT_ARM_WIDTH.half( );
const Planc FINGER_WIDTH = FRONT_ARM_WIDTH / 3.0;
const Planc FINGER_LENGTH_OUT = FRONT_ARM_WIDTH * 0.72;
const Planc FINGER_LENGTH_DOWN = FRONT_ARM_WIDTH * 0.64;
const Planc UPPER_ARM_LENGTH = BODY_HEIGHT * 0.8;
const Planc LOWER_ARM_LENGTH = UPPER_ARM_LENGTH * 0.8;
const Planc LEG_WIDTH = HEIGHT / 5.25;
const Planc BOOT_TOP_WIDTH = LEG_WIDTH * 1.3;
const Planc BOOT_TOP_HEIGHT = BOOT_TOP_WIDTH * 0.66;
const Planc BOOT_BASE_WIDTH = LEG_WIDTH * 1.6;
const Planc BOOT_BASE_HEIGHT = BOOT_BASE_WIDTH * 0.3;
const Planc BOOT_SOLE_HEIGHT = 1.0;
const Planc BOOT_SOLE_SUB_WIDTH = 3.0;
const Planc BOOT_SOLE_SUB_GAP_WIDTH = 1.0;
const Planc BOOT_SOLE_SUB_HEIGHT = 0.75;

const Planc BELT_THICKNESS = 3.0;
const Planc BELT_BUCKLE_WIDTH = 5.0;

const uint BOB_PERIOD = 10;
const Planc HEAD_BOB_DISTANCE = 1.0;
const Planc SHOULDER_BOB_DISTANCE = 0.8;
const dec SHOULDER_BOB_OFFSET = 0.75;

static uint BLINK_DURATION = 3;
static Span<uint> BLINK_WAIT = { 120, 240 };

const Color CROSSBOW_COLOR = Color::rgb( 0xA54B23 );

const Planc HOOK_LENGTH = 38.0;
const Planc HOOK_THICKNESS = 5.0;
const Planc HOOK_TIP_LENGTH = 2.5;
const Angle HOOK_ANGLE = RIGHT_ANGLE;
const Color HOOK_COLOR = Color::rgb( 0x9C9C9C );

const Planc ROPE_WIDTH = 4.0;
const Color ROPE_BASE_COLOR = Color::rgb( 0xDAA420 );
const Color ROPE_ALT_COLOR = Color::rgb( 0xB9870F );

const Planc HOOK_PATH_LENGTH = METER * 25;
const dec HOOK_PATH_ALPHA_START = 0.8;
const Color HOOK_PATH_COLOR = WHITE;

const Planc ARROW_PATH_LENGTH = HOOK_PATH_LENGTH;
const dec ARROW_PATH_ALPHA_START = HOOK_PATH_ALPHA_START;
const Color ARROW_PATH_COLOR = RED;
} // namespace

Climber::~Climber( )
{
    world( )->camera( )->remove_hud_element( &m_healthbar );
    world( )->camera( )->remove_screen_effect( &m_low_health_effect );
}

Climber::Climber( World * world, const Coordinate & _position )
    : Player( world, _position ),
    m_hook( world, this ),
    m_healthbar( this ),
    m_low_health_effect( this )
{
    interactive( true );
    
    needs_render_always( true );

    space( Rectangle( WIDTH, HEIGHT ) );
    
    eye_info( EYE_RADIUS, BLINK_DURATION, BLINK_WAIT );
    
    m_skin = MOHAWK;

    m_movement_speed = DEFAULT_SPEED;

    m_reload_timer.reset( 0 );
    m_dust_timer.reset( 0 );

    m_arrow_feather_color = Random::rColor( );

    world->camera( )->add_hud_element( &m_healthbar );
    world->camera( )->add_screen_effect( &m_low_health_effect );
}

void Climber::update( )
{
    Player::update( );

    //heal( 0.1 );

    m_reload_timer.tick( );
    m_dust_timer.tick( );

    const Vector & v = velocity( );
    if( v.has_magnitude( ) )
    {
        if( v.magnitude( ) >= DUST_MINIMUM_SPEED )
        {
            if( Object::ground( ) )
            {
                if( !m_dust_timer.remaining( ) )
                {
                    world( )->add_object( new Dust( world( ), position( ) + front_foot( ), ( velocity( ) * DUST_VELOCITY_DAMPEN_RATIO ) + VectorY( Random::rPlanc( DUST_RISE ) ), world( )->terrain( )->dust_color( ) ) );
                    m_dust_timer.reset( Random::rint( DUST_TIME ) );
                }
            }
        }
    }

    if( m_firing_arrow || m_firing_hook )
    {
        if( !m_reload_timer.remaining( ) )
        {
            if( m_firing_arrow )
            {
                // static SoundClip twang( "twang.wav" );
                // twang.play( 0.5 );

                Angle aim = aim_angle( ) + aim_shake( );
                Arrow * arrow = new Arrow( world( ), position( ), VectorA( aim, DEFAULT_ARROW_LAUNCH_SPEED ) + velocity( ), m_arrow_feather_color );
                arrow->position( arrow->position( ) + VectorA( aim_angle( ), arrow->length( ) ) );

                world( )->add_object( arrow );

                m_arrow_feather_color = Random::rColor( );
            }
            else if( m_firing_hook )
            {
                m_hook.fire( VectorA( aim_angle( ), DEFAULT_ROPE_GROWTH_SPEED ) + velocity( ) );
            }
            m_reload_timer.reset( RELOAD_TIME );
        }
    }
    else
    {
        if( !m_firing_hook )
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
    if( !gravity_ratio( ) )
    { // can fly?
        if( m_looking_up )
        {
            if( m_moving_right && !m_moving_left )
            {
                add_velocity( VectorA( Angle( half( RIGHT_ANGLE_1 ) ), m_movement_speed ) - VectorX( m_movement_speed ) );
            }
            else if( m_moving_left && !m_moving_right )
            {
                add_velocity( VectorA( Angle( half( RIGHT_ANGLE_3 ) ), m_movement_speed ) - VectorX( -m_movement_speed ) );
            }
            else
            {
                add_velocity( VectorY( m_movement_speed ) );
            }
        }
        if( m_looking_down )
        {
            if( m_moving_right && !m_moving_left )
            {
                add_velocity( VectorA( Angle( -half( RIGHT_ANGLE_1 ) ), m_movement_speed ) - VectorX( m_movement_speed ) );
            }
            else if( m_moving_left && !m_moving_right )
            {
                add_velocity( VectorA( Angle( -half( RIGHT_ANGLE_3 ) ), m_movement_speed ) - VectorX( -m_movement_speed ) );
            }
            else
            {
                add_velocity( VectorY( -m_movement_speed ) );
            }
        }
    }

    if( m_moving_right )
    {
        add_velocity( VectorX( m_movement_speed ) );
    }
    if( m_moving_left )
    {
        add_velocity( VectorX( -m_movement_speed ) );
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
            add_velocity( ( VectorY( JUMP_STRENGTH ) - GRAVITY ) * ( 1.0 - m_jump_degradation ) );
            m_jump_degradation = 1.0 - ( ( 1.0 - m_jump_degradation ) * JUMP_DEGRATION_RATIO );
            ground( nullptr );
        }
    }
    else
    {
        m_jumping_timer.reset( 0 );
    }

    velocity( velocity( ) * 0.9 );
}

Planc Climber::light_sight( ) const
{
    return LIGHT_SIGHT;
}

bool Climber::moving_right( ) const
{
    return m_moving_right && !m_moving_left;
}

void Climber::moving_right( const bool _moving )
{
    m_moving_right = _moving;
}

bool Climber::moving_left( ) const
{
    return m_moving_left && !m_moving_right;
}

void Climber::moving_left( const bool _moving )
{
    m_moving_left = _moving;
}

bool Climber::looking_up( ) const
{
    return m_looking_up && !m_looking_down;
}

void Climber::looking_up( const bool _looking )
{
    m_looking_up = _looking;
}

bool Climber::looking_down( ) const
{
    return m_looking_down && !m_looking_up;
}

void Climber::looking_down( const bool _looking )
{
    m_looking_down = _looking;
}

void Climber::jump( const bool _jumping )
{
    m_jumping = _jumping;
}

void Climber::ground( TerrainEdge * ground )
{
    if( ground && !Object::ground( ) )
    {
        Vector dust_velocity = ( velocity( ).dy( 0.0 ) * DUST_VELOCITY_DAMPEN_RATIO );
        uint dust_count = Random::rint( DUST_LANDING_COUNT );
        for_range( i, dust_count )
        {
            bool left = ( i < half( dust_count ) );

            Coordinate back_foot = left ? left_foot( ) : right_foot( );
            Coordinate front_foot = left ? right_foot( ) : left_foot( );
            Coordinate foot = position( ) + back_foot + ( Vector( back_foot, front_foot ) * ( (dec)i / (dec)dust_count ) );

            Angle angle = ground->vector( ).angle( ) + Random::rAngle( DUST_LANDING_ANGLE.min( ), DUST_LANDING_ANGLE.max( ) ) + ( left ? ZERO : PI );

            world( )->add_object( new Dust( world( ), foot, dust_velocity + VectorA( angle, Random::rPlanc( DUST_RISE ) ), world( )->terrain( )->dust_color( ) ) );
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

void Climber::aim( const Angle & _aim_angle )
{
    m_aim_angle = _aim_angle;
}

void Climber::aim_shake( const Angle & _aim_shake_angle )
{
    m_aim_shake_angle = _aim_shake_angle;
}

Angle Climber::aim_shake( ) const
{
    return Random::negative( half( m_aim_shake_angle ) );
}

Angle Climber::aim_shake_range( ) const
{
    return m_aim_shake_angle;
}

void Climber::fire_hook( )
{
    m_firing_hook = true;
}

void Climber::fire_arrow( )
{
    m_firing_arrow = true;
}

void Climber::release_bow( )
{
    m_firing_hook = false;
    m_firing_arrow = false;
}

Coordinate Climber::head_center( ) const
{
    return Coordinate( 0.0, BODY_HEIGHT + HEAD_Y_OFFSET + ( HEAD_BOB_DISTANCE * sin( (dec)age( ) / BOB_PERIOD ) ) );
}

Coordinate Climber::face_center( ) const
{
    return head_center( ) + VectorA( aim_angle( ), ( HEAD_RADIUS - FACE_RADIUS ) / 2.0 );
}

Coordinate Climber::front_shoulder( ) const
{
    Coordinate shoulder = Coordinate( -half( BODY_WIDTH - ( SHOULDER_RADIUS * 2.0 ) ),
                                      half( BODY_HEIGHT - ( SHOULDER_RADIUS * 2.0 ) ) + ( SHOULDER_BOB_DISTANCE * sin( (dec)( age( ) + ( BOB_PERIOD * SHOULDER_BOB_OFFSET ) ) / BOB_PERIOD ) ) );

    if( aiming_left( ) )
    {
        shoulder.mirror_y( );
    }

    return shoulder;
}

Coordinate Climber::back_shoulder( ) const
{
    Coordinate shoulder = front_shoulder( );
    shoulder.mirror_y( );
    return shoulder;
}

Coordinate Climber::front_hand( ) const
{
    Coordinate hand = VectorA( aim_angle( ) - negative( RIGHT_ANGLE, aiming_left( ) ), 12.0 );
    return hand;
}

Coordinate Climber::back_hand( ) const
{
    Coordinate hand = VectorA( aim_angle( ) - negative( Angle( TAU / 32.0 ), aiming_left( ) ), 24.0 );
    return hand;
}

Coordinate Climber::front_hip( ) const
{
    return Coordinate( negative( LEG_WIDTH.half( ) - PANTS_WIDTH_UPPER.half( ), facing_left( ) ), -BODY_HEIGHT.half( ) );
}

Coordinate Climber::back_hip( ) const
{
    return Coordinate( negative( PANTS_WIDTH_UPPER.half( ) - LEG_WIDTH.half( ), facing_left( ) ), -BODY_HEIGHT.half( ) );
}

Coordinate Climber::front_foot( ) const
{
    return Coordinate( front_hip( ).x( ), -HEIGHT.half( ) );
}

Coordinate Climber::back_foot( ) const
{
    return Coordinate( back_hip( ).x( ), -HEIGHT.half( ) );
}

Coordinate Climber::left_shoulder( ) const
{
    return ( facing_left( ) ? back_shoulder( ) : front_shoulder( ) );
}

Coordinate Climber::right_shoulder( ) const
{
    return ( facing_right( ) ? back_shoulder( ) : front_shoulder( ) );
}

Coordinate Climber::left_hand( ) const
{
    return ( facing_left( ) ? back_hand( ) : front_hand( ) );
}

Coordinate Climber::right_hand( ) const
{
    return ( facing_right( ) ? back_hand( ) : front_hand( ) );
}

Coordinate Climber::left_hip( ) const
{
    return ( facing_left( ) ? front_hip( ) : back_hip( ) );
}

Coordinate Climber::right_hip( ) const
{
    return ( facing_right( ) ? front_hip( ) : back_hip( ) );
}

Coordinate Climber::left_foot( ) const
{
    return ( facing_left( ) ? front_foot( ) : back_foot( ) );
}

Coordinate Climber::right_foot( ) const
{
    return ( facing_right( ) ? front_foot( ) : back_foot( ) );
}

// -- GRAPHICS --

Climber::Skin Climber::skin( ) const
{
    return m_skin;
}

Color Climber::color( ColorPiece component ) const
{
    if( hurt_display( ).remaining( ) )
    {
        return RED;
    }
    else
    {
        switch( component )
        {
            case SKIN:
                switch( skin( ) )
                {
                    // case:
                    //     return Color::rgb( 0x9B5032 );
                    //     break;
                    case MOHAWK :
                        return Color::rgb( 0xFFE1BE );
                        break;
                    case NO_SKIN :
                    default :
                        return WHITE;
                        break;
                }
                break;
            case HAIR:
                switch( skin( ) )
                {
                    case MOHAWK :
                        return RED;
                        break;
                    case NO_SKIN :
                    default :
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
    // draw( m_hook.projectile_drawing( VectorA( aim_angle( ), DEFAULT_ROPE_GROWTH_SPEED ), HOOK_PATH_LENGTH, HOOK_PATH_COLOR, HOOK_PATH_ALPHA_START ) );
    draw( m_hook );

    draw_legs( );
    draw_torso( );
    draw_back_arm( );
    draw_back_hand( );
    draw_head( );
    draw_crossbow( );
    draw_front_arm( );
    draw_front_hand( );
    draw_arrow( );
}

void Climber::draw_head( )
{
    // head
    draw( color( SKIN ), Circle( HEAD_RADIUS, head_center( ) ) );

    if( m_skin == MOHAWK )
    {
        // shaved hair
        varray<Coordinate> top_half_coordinates = Circle( HEAD_RADIUS, head_center( ) ).coordinates( );
        uint count = ( top_half_coordinates.size( ) / 2 ) - 1;
        for_range( i, count ) top_half_coordinates.erase( floor( top_half_coordinates.size( ) / 2.0 ) );
        draw( color( HAIR ).a( 0.375 ), Polygon( top_half_coordinates ) );
    }

    // ears
    draw( color( SKIN ), Circle( EAR_RADIUS, head_center( ) + VectorX( HEAD_RADIUS - EAR_RADIUS ) ) );
    draw( color( SKIN ), Circle( EAR_RADIUS, head_center( ) - VectorX( HEAD_RADIUS - EAR_RADIUS ) ) );

    // face
    draw( color( SKIN ), Circle( FACE_RADIUS, face_center( ) ) );

    // eyes
    draw_eyes( face_center( ) + Vector( EYE_SPACING, 0.0 ), face_center( ) + Vector( -EYE_SPACING, 0.0 ) );
    
    if( m_skin == MOHAWK )
    {
        { // mohawk
            Coordinate top = head_center( ) + VectorY( HEAD_RADIUS + 3.0 );
            Coordinate top_right = top + Vector( 5.0, 0.0 );
            Coordinate top_left = top + Vector( -5.0, 0.0 );

            Coordinate bottom = face_center( ) + VectorY( FACE_RADIUS - 4.0 );
            Coordinate bottom_right = bottom + Vector( 5.0, 0.0 );
            Coordinate bottom_left = bottom + Vector( -5.0, 0.0 );

            draw( color( HAIR ), Line( top, bottom ), 10.0 );
        }

        { // beard
            draw( color( HAIR ), Rectangle( FACE_RADIUS * 2.0, 12.0, face_center( ) + VectorY( -12.0 ) ) );
            draw( color( HAIR ), Rectangle( 12.0, 4.0, face_center( ) + VectorY( -6.0 ) ) );
        }
    }
}

void Climber::draw_torso( )
{
    Coordinate waist = Coordinate( 0.0, -BODY_HEIGHT.half( ) );

    // pants base
    draw( color( PANTS ), Polygon( { waist + Coordinate( PANTS_WIDTH_UPPER.half( ), 0.0 ),
                                                  waist + Coordinate( -PANTS_WIDTH_UPPER.half( ), 0.0 ),
                                                  waist + Coordinate( -PANTS_WIDTH_LOWER.half( ), -PANTS_HEIGHT.half( ) ),
                                                  waist + Coordinate( PANTS_WIDTH_LOWER.half( ), -PANTS_HEIGHT.half( ) ) } ) );

    // undershirt
    draw( color( UNDERSHIRT ), Rectangle( BODY_WIDTH.half( ), BODY_HEIGHT ) );

    // belt
    draw( color( BELT ), Line( waist - VectorX( PANTS_WIDTH_UPPER.half( ) + ( BELT_THICKNESS / 4.0 ) ),
                                            waist + VectorX( PANTS_WIDTH_UPPER.half( ) + ( BELT_THICKNESS / 4.0 ) ) ), (dec)BELT_THICKNESS );
    draw( color( BELT_BUCKLE ), Rectangle( BELT_BUCKLE_WIDTH, BELT_BUCKLE_WIDTH, waist ) );

    // jacket
    draw( color( JACKET ), Circle( BACK_ARM_WIDTH.half( ), back_shoulder( ) ) );
    draw( color( JACKET ), Rectangle( BODY_WIDTH / 3.0, BODY_HEIGHT - SHOULDER_RADIUS, Coordinate( BODY_WIDTH / 3.5, -half( SHOULDER_RADIUS ) ) ) );
    draw( color( JACKET ), Rectangle( BODY_WIDTH / 3.0, BODY_HEIGHT - SHOULDER_RADIUS, Coordinate( -BODY_WIDTH / 3.5, -half( SHOULDER_RADIUS ) ) ) );
}

void Climber::draw_legs( )
{
    Drawing boot_drawing;

    Planc x_offset_base = half( BOOT_BASE_WIDTH - BOOT_TOP_WIDTH );
    Polygon boot_sole = Rectangle( BOOT_BASE_WIDTH, BOOT_SOLE_HEIGHT, Coordinate( x_offset_base, -BOOT_SOLE_HEIGHT.half( ) ) );
    boot_drawing.draw( color( BOOT_SOLE ), boot_sole );
    {
        Polygon boot_sub_sole = Rectangle( BOOT_SOLE_SUB_WIDTH, BOOT_SOLE_SUB_HEIGHT, Coordinate( x_offset_base - BOOT_BASE_WIDTH.half( ) + BOOT_SOLE_SUB_WIDTH.half( ), -BOOT_SOLE_SUB_HEIGHT - BOOT_SOLE_HEIGHT.half( ) ) );
        boot_drawing.draw( color( BOOT_SOLE ), boot_sub_sole );
        for_range( i, floor( BOOT_BASE_WIDTH.half( ).half( ) ) )
        {
            Polygon boot_sub_sole = Rectangle( BOOT_SOLE_SUB_WIDTH, BOOT_SOLE_SUB_HEIGHT, Coordinate( x_offset_base + BOOT_BASE_WIDTH.half( ) - BOOT_SOLE_SUB_WIDTH.half( ) - ( ( BOOT_SOLE_SUB_WIDTH + BOOT_SOLE_SUB_GAP_WIDTH ) * (Planc)i ), -BOOT_SOLE_SUB_HEIGHT - BOOT_SOLE_HEIGHT.half( ) ) );
            boot_drawing.draw( color( BOOT_SOLE ), boot_sub_sole );
        }
    }

    Polygon boot_shin = Rectangle( BOOT_TOP_WIDTH, BOOT_TOP_HEIGHT, Coordinate( 0.0, BOOT_TOP_HEIGHT.half( ) ) );
    Polygon boot_base = Rectangle( BOOT_BASE_WIDTH, BOOT_BASE_HEIGHT, Coordinate( x_offset_base, BOOT_BASE_HEIGHT.half( ) ) );
    boot_drawing.draw( color( BOOT ), boot_shin );
    boot_drawing.draw( color( BOOT ), boot_base );

    Line lace_1 = Line( Coordinate( 5.0, 7.5 ), Coordinate( -3.0, 7.5 ) );
    Line lace_2 = Line( Coordinate( 5.0, 6.0 ), Coordinate( -3.0, 6.0 ) );
    boot_drawing.draw( color( BOOT_LACE ), lace_1, 1.0 );
    boot_drawing.draw( color( BOOT_LACE ), lace_2, 1.0 );

    Planc foot_offset_x = ( sin( world( )->age( ) / 2.0 ) * 4.0 );
    Planc foot_offset_y = ( cos( world( )->age( ) / 2.0 ) * 2.0 );
    Planc foot_offset_y_back = max( -foot_offset_y, (Planc)0.0 );
    Planc foot_offset_y_front = max( foot_offset_y, (Planc)0.0 );

    // back leg
    {
        Coordinate hip = back_hip( );
        Coordinate foot = back_foot( );

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
            foot += VectorY( LEG_HEIGHT / 4.0 );
        }

        Coordinate knee = foot + VectorY( LEG_HEIGHT * ( 2.0 / 3.0 ) );

        Drawing boot_drawing_back = boot_drawing;
        if( facing_left( ) )
        {
            boot_drawing_back.mirror_y( );
        }
        boot_drawing_back.move( foot );

        draw( color( PANTS ), Line( hip, foot ), LEG_WIDTH );
        // draw( pants_color( ), Line( hip, knee ), LEG_WIDTH );
        // draw( pants_color( ), Circle( LEG_WIDTH.half( ), knee ) );
        // draw( pants_color( ), Line( knee, foot ), LEG_WIDTH );

        draw( boot_drawing_back );
    }

    // front leg
    {
        Coordinate hip = front_hip( );
        Coordinate foot = front_foot( );

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

        Coordinate knee = foot + VectorY( LEG_HEIGHT * ( 2.0 / 3.0 ) );

        Drawing boot_drawing_front = boot_drawing;
        if( facing_left( ) )
        {
            boot_drawing_front.mirror_y( );
        }
        boot_drawing_front.move( foot );

        draw( color( PANTS ), Line( hip, foot ), LEG_WIDTH );
        // draw( pants_color( ), Line( hip, knee ), LEG_WIDTH );
        // draw( pants_color( ), Circle( LEG_WIDTH.half( ), knee ) );
        // draw( pants_color( ), Line( knee, foot ), LEG_WIDTH );

        draw( boot_drawing_front );
    }
}

void Climber::draw_front_arm( )
{
    const Coordinate shoulder = front_shoulder( );
    const Coordinate hand = front_hand( );

    Joint lower_arm = Joint( shoulder, UPPER_ARM_LENGTH, LOWER_ARM_LENGTH, hand, !( aiming_left( ) ) );
    Joint upper_arm = Joint( shoulder, UPPER_ARM_LENGTH, LOWER_ARM_LENGTH, hand, !( aiming_left( ) ) );

    draw( color( JACKET ), Circle( SHOULDER_RADIUS, shoulder ) );

    draw( color( JACKET ), Line( lower_arm.c1( ), lower_arm.joint( ) ), (dec)FRONT_ARM_WIDTH );
    draw( color( JACKET ), Circle( (dec)FRONT_ARM_WIDTH.half( ), upper_arm.joint( ) ) );
    draw( color( JACKET ), Line( upper_arm.joint( ), upper_arm.c2( ) ), (dec)FRONT_ARM_WIDTH );
}

void Climber::draw_back_arm( )
{
    // draw( color( JACKET ), Line( back_shoulder( ), back_hand( ) ), (dec)BACK_ARM_WIDTH );
    
    const Coordinate shoulder = back_shoulder( );
    const Coordinate hand = back_hand( );

    Joint lower_arm = Joint( shoulder, UPPER_ARM_LENGTH, LOWER_ARM_LENGTH, hand, !( aiming_left( ) ) );
    Joint upper_arm = Joint( shoulder, UPPER_ARM_LENGTH, LOWER_ARM_LENGTH, hand, !( aiming_left( ) ) );

    draw( color( JACKET ), Circle( SHOULDER_RADIUS, shoulder ) );

    draw( color( JACKET ), Line( lower_arm.c1( ), lower_arm.joint( ) ), (dec)BACK_ARM_WIDTH );
    draw( color( JACKET ), Circle( (dec)BACK_ARM_WIDTH.half( ), upper_arm.joint( ) ) );
    draw( color( JACKET ), Line( upper_arm.joint( ), upper_arm.c2( ) ), (dec)BACK_ARM_WIDTH );
}

void Climber::draw_front_hand( )
{
    bool finger_down = ( m_reload_timer.remaining( ) <= ( RELOAD_TIME / 2 ) );

    Coordinate hand = front_hand( );

    Coordinate finger_base = hand + VectorA( aim_angle( ) + negative( RIGHT_ANGLE, aiming_left( ) ), FRONT_ARM_WIDTH.half( ) - FINGER_WIDTH.half( ) );
    Coordinate finger_tip = finger_base + VectorA( aim_angle( ), finger_down ? FINGER_LENGTH_OUT : FINGER_LENGTH_DOWN );

    draw( color( SKIN ), Circle( FRONT_ARM_WIDTH.half( ), hand ) );
    draw( color( SKIN ), Line( finger_base, finger_tip ), FINGER_WIDTH );
}

void Climber::draw_back_hand( )
{
    draw( color( SKIN ), Circle( BACK_ARM_WIDTH.half( ), back_hand( ) ) );
}

void Climber::draw_crossbow( )
{
    Polygon crossbow_polygon = Polygon( { { 6.0, 0.0 }, Coordinate( 0.0, 0.0 ), Coordinate( -2.0, -2.0 ), Coordinate( -4.0, -2.0 ), Coordinate( -4.0, -4.0 ), Coordinate( 1.0, -1.0 ), Coordinate( 5.0, -1.0 ) } ).scale( 5.0 );

    if( aiming_left( ) )
    {
        crossbow_polygon.mirror_x( );
    }

    crossbow_polygon.rotate( aim_angle( ) );

    draw( CROSSBOW_COLOR, crossbow_polygon );
}

void Climber::draw_arrow( )
{
    Arrow arrow( world( ), position( ), VectorA( aim_angle( ), DEFAULT_ARROW_LAUNCH_SPEED ), m_arrow_feather_color );
    arrow.render( );

    Vector v = VectorA( aim_angle( ), arrow.length( ) );

    draw( arrow.projectile_drawing( ARROW_PATH_LENGTH, ARROW_PATH_COLOR, ARROW_PATH_ALPHA_START ).move( v ) );
    draw( Drawing( arrow ).move( v ) );
}

// todo make variable
Climber::HealthBar::HealthBar( Climber * climber ) : HeadUpDisplay( 0.5, 0.015, 1.0, 0.03 ), m_climber( climber ) { }

void Climber::HealthBar::render( Camera * camera )
{
    HeadUpDisplay::render( camera );

    const Planc HEALTH_BAR_BORDER_WIDTH = 4.0;

    const dec RED_START = 0.1;
    const dec YELLOW_START = 0.5;
    static_once( ) { Assert( RED_START < YELLOW_START ); }

    if( Climber * climber = m_climber )
    {
        const FixedRectangle & rect = bounds( camera );
        Planc width = rect.width( ) - ( HEALTH_BAR_BORDER_WIDTH * 3.0 );
        Planc height = rect.height( ) - ( HEALTH_BAR_BORDER_WIDTH * 3.0 );
        Coordinate center = rect.center( );

        draw( BLACK, Rectangle( width + ( HEALTH_BAR_BORDER_WIDTH * 3.0 ), height + ( HEALTH_BAR_BORDER_WIDTH * 3.0 ), center ) );
        draw( WHITE, Rectangle( width + ( HEALTH_BAR_BORDER_WIDTH * 2.0 ), height + ( HEALTH_BAR_BORDER_WIDTH * 2.0 ), center ) );
        draw( BLACK, Rectangle( width + ( HEALTH_BAR_BORDER_WIDTH * 1.0 ), height + ( HEALTH_BAR_BORDER_WIDTH * 1.0 ), center ) );

        draw( BLACK, Rectangle( width, height, center ) );

        if( climber->alive( ) )
        {
            dec health_percentage = climber->health( ) / climber->max_health( );
            Assert( in_range( health_percentage, ZERO, ONE, true ) );

            Color health_color;
            if( climber->god( ) )
            {
                health_color = CYAN;
            }
            else if( health_percentage == ONE )
            {
                health_color = GREEN;
            }
            else if( health_percentage >= YELLOW_START )
            {
                health_color = ColorSlider( YELLOW, GREEN ).color_at( ( health_percentage - YELLOW_START ) * inverse( ONE - YELLOW_START ) );
            }
            else if( health_percentage >= RED_START )
            {
                health_color = ColorSlider( RED, YELLOW ).color_at( ( health_percentage - RED_START ) * inverse( ONE - RED_START ) );
            }
            else
            {
                health_color = RED;
            }

            draw( health_color, Rectangle( width * health_percentage, height,
                                           center - VectorX( half( width * ( ONE - health_percentage ) ) ) ) );
        }
    }
}

Climber::LowHealthAlertEffect::LowHealthAlertEffect( Climber * climber ) : m_climber( climber ) { }

void Climber::LowHealthAlertEffect::render( Camera * camera )
{
    ScreenEffect::render( camera );

    const dec PULSE_DURATION = 15.0;
    const dec SCREEN_FADE_DISTANCE = 0.5;

    const Color BASE_COLOR = RED;
    const Color TRANSPARENT_COLOR = BASE_COLOR.a( ZERO );
    const dec BASE_ALPHA = 0.5;

    const dec HEALTH_THRESHOLD_UPPER = 0.333;
    const dec HEALTH_THRESHOLD_LOWER = 0.1;
    
    Assert( HEALTH_THRESHOLD_UPPER > HEALTH_THRESHOLD_LOWER );

    if( m_climber->health_percentage( ) <= HEALTH_THRESHOLD_UPPER )
    {
        dec alpha_health = ONE;
        if( m_climber->health_percentage( ) > HEALTH_THRESHOLD_LOWER )
        {
            alpha_health = ONE - ( ( m_climber->health_percentage( ) - HEALTH_THRESHOLD_LOWER ) / ( HEALTH_THRESHOLD_UPPER - HEALTH_THRESHOLD_LOWER ) );
        }

        dec alpha_time = BASE_ALPHA * abs( sin( (dec)( (dec)m_climber->age( ) / PULSE_DURATION ) ) );
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
            case ' ' :
            {
                if( down )
                {
                    jump( true );
                }
                else
                {
                    jump( false );
                }
                break;
            }
            case 'w' :
            {
                if( down )
                {
                    looking_up( true );
                }
                else
                {
                    looking_up( false );
                }
                break;
            }
            case 's' :
            {
                if( down )
                {
                    looking_down( true );
                }
                else
                {
                    looking_down( false );
                }
                break;
            }
            case 'a' :
            {
                if( down )
                {
                    moving_left( true );
                }
                else
                {
                    moving_left( false );
                }
                break;
            }
            case 'd' :
            {
                if( down )
                {
                    moving_right( true );
                }
                else
                {
                    moving_right( false );
                }
                break;
            }
            case 'g' :
            {
                if( pressed )
                {
                    god( !god( ) );
                }
                break;
            }
            default :
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
                    fire_arrow( );
                }
                else if( button == MouseInput::RIGHT_BUTTON )
                {
                    fire_hook( );
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

            if( ( button == ControllerButtonInput::L1_BUTTON ) || ( button == ControllerButtonInput::RIGHT_DOWN_BUTTON ) )
            {
                jump( dynamic == ControllerButtonInput::PRESSED );
            }

            if( button == ControllerButtonInput::R1_BUTTON )
            {
                if( dynamic == ControllerButtonInput::PRESSED )
                {
                    fire_arrow( );
                }
                else
                {
                    release_bow( );
                }
            }

            if( button == ControllerButtonInput::R2_BUTTON )
            {
                if( dynamic == ControllerButtonInput::PRESSED )
                {
                    fire_hook( );
                }
                else
                {
                    release_bow( );
                }
            }

            if( button == ControllerButtonInput::LEFT_UP_BUTTON )
            {
                if( dynamic == ControllerButtonInput::PRESSED )
                {
                    looking_up( true );
                }
                else
                {
                    looking_up( false );
                }
            }

            if( button == ControllerButtonInput::LEFT_DOWN_BUTTON )
            {
                if( dynamic == ControllerButtonInput::PRESSED )
                {
                    looking_down( true );
                }
                else
                {
                    looking_down( false );
                }
            }

            if( button == ControllerButtonInput::LEFT_LEFT_BUTTON )
            {
                if( dynamic == ControllerButtonInput::PRESSED )
                {
                    moving_left( true );
                }
                else
                {
                    moving_left( false );
                }
            }

            if( button == ControllerButtonInput::LEFT_RIGHT_BUTTON )
            {
                if( dynamic == ControllerButtonInput::PRESSED )
                {
                    moving_right( true );
                }
                else
                {
                    moving_right( false );
                }
            }
        }

        if( ControllerJoystickInput * joystick_input = dynamic_cast<ControllerJoystickInput *>( controller_input ) )
        {
            if( joystick_input->joystick == ControllerJoystickInput::LEFT_JOYSTICK )
            {
                //if( joystick_input->direction == ControllerJoystickInput::LEFT )
                //{
                //    moving_left( true );
                //}
                //else if( joystick_input->direction == ControllerJoystickInput::RIGHT )
                //{
                //    moving_right( true );
                //}
                //else if( joystick_input->direction == ControllerJoystickInput::UP )
                //{
                //    looking_up( true );
                //}
                //else if( joystick_input->direction == ControllerJoystickInput::DOWN )
                //{
                //    looking_down( true );
                //}
                //else
                //{
                //    moving_left( false );
                //    moving_right( false );
                //    looking_up( false );
                //    looking_down( false );
                //}
                //}

            if( joystick_input->joystick == ControllerJoystickInput::RIGHT_JOYSTICK )
            {
                aim( joystick_input->vector.angle( ) );
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
