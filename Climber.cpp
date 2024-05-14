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
const double JUMP_DEGRATION_RATIO = 0.9;
const uint JUMP_MAX_DURATION = 15;
const uint JUMP_RESET_WAIT_TIME = 0;

const Planc LIGHT_SIGHT = HEIGHT * 3.0;

// crossbox / hook
const Planc DEFAULT_ARROW_LAUNCH_SPEED = 32.0;
const Planc DEFAULT_ROPE_MAX_LENGTH = HEIGHT * 50.0;
const Planc DEFAULT_ROPE_GROWTH_SPEED = 33.0;
const Planc DEFAULT_ROPE_RETRACT_SPEED = 44.0;
const uint RELOAD_TIME = 10;

const Planc DUST_MINIMUM_SPEED = METER / 12.0;
const double DUST_VELOCITY_DAMPEN_RATIO = 0.1;
const Span<Planc> DUST_RISE = { 0.5, 1.2 };
const Span<uint> DUST_LANDING_COUNT = { 9, 15 };
const Span<double> DUST_LANDING_ANGLE = { 0.0, RIGHT_ANGLE / 3. };
const Span<uint> DUST_TIME = { 0, 5 };

const Planc HEAD_RADIUS = HEIGHT / 3.9;
const Planc HEAD_Y_OFFSET = HEAD_RADIUS * 0.05;
const Planc FACE_RADIUS = HEAD_RADIUS * 0.75;
const Planc EYE_SPACING = FACE_RADIUS * 0.55;
const Planc EYE_WIDTH = 3.0;
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
const double SHOULDER_BOB_OFFSET = 0.75;

const Color CROSSBOW_COLOR = Color::rgb( 0xA54B23 );

const Planc HOOK_LENGTH = 38.0;
const Planc HOOK_THICKNESS = 5.0;
const Planc HOOK_TIP_LENGTH = 2.5;
const Angle HOOK_ANGLE = RIGHT_ANGLE;
const Color HOOK_COLOR = Color::rgb( 0x9C9C9C );

const Planc ROPE_WIDTH = 4.0;
const Color ROPE_BASE_COLOR = Color::rgb( 0xDAA420 );
const Color ROPE_ALT_COLOR = Color::rgb( 0xB9870F );
} // namespace

Climber::Climber( World * world, const Coordinate & _position ) : Player( world, _position ), m_hook( world, this )
{
    space( Polygon::rectangle( WIDTH, HEIGHT ) );

    m_skin = MOHAWK;

    m_movement_speed = DEFAULT_SPEED;

    m_reload_timer.reset( 0 );
    m_dust_timer.reset( 0 );

    m_arrow_feather_color = Random::rColor( );
}

Climber & Climber::update( )
{
    Player::update( );

    heal( 0.1 );

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
        drawing_dirty( true );
        if( !m_reload_timer.remaining( ) )
        {
            if( m_firing_arrow )
            {
                // static SoundClip twang( "twang.wav" );
                // twang.play( 0.5 );

                Angle shake = aim_shake( );
                Arrow * arrow = new Arrow( world( ), position( ), VectorA( ( aim_angle( ) + shake ), DEFAULT_ARROW_LAUNCH_SPEED ) + velocity( ), m_arrow_feather_color );
                arrow->position( position( ) + VectorA( aim_angle( ), arrow->length( ) ) );

                world( )->add_object( arrow );

                m_arrow_feather_color = Random::rColor( );
            }
            else if( m_firing_hook )
            {
                m_hook.fire( VectorA( aim_angle( ), DEFAULT_ROPE_GROWTH_SPEED ) );
            }
            m_reload_timer.reset( RELOAD_TIME );
        }
    }
    else
    {
        if( !m_firing_hook )
        {
            drawing_dirty( true );
            m_hook.retract( );
        }

        if( m_slashing )
        {
            drawing_dirty( true );
        }
    }

    m_hook.update( );

    return *this;
}

Climber & Climber::update_velocity( )
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

    Player::update_velocity( );

    if( Object::ground( ) )
        m_jump_reset_timer.tick( );

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

    return *this;
}

Planc Climber::light_sight( ) const
{
    return LIGHT_SIGHT;
}

bool Climber::moving_right( ) const
{
    return m_moving_right && !m_moving_left;
}

Climber & Climber::moving_right( const bool _moving )
{
    m_moving_right = _moving;
    drawing_dirty( true );
    return *this;
}

bool Climber::moving_left( ) const
{
    return m_moving_left && !m_moving_right;
}

Climber & Climber::moving_left( const bool _moving )
{
    m_moving_left = _moving;
    drawing_dirty( true );
    return *this;
}

bool Climber::looking_up( ) const
{
    return m_looking_up && !m_looking_down;
}

Climber & Climber::looking_up( const bool _looking )
{
    m_looking_up = _looking;
    drawing_dirty( true );
    return *this;
}

bool Climber::looking_down( ) const
{
    return m_looking_down && !m_looking_up;
}

Climber & Climber::looking_down( const bool _looking )
{
    m_looking_down = _looking;
    drawing_dirty( true );
    return *this;
}

Climber & Climber::jump( const bool _jumping )
{
    m_jumping = _jumping;
    drawing_dirty( true );
    return *this;
}

Climber & Climber::ground( TerrainEdge * ground )
{
    if( ground && !Object::ground( ) )
    {
        Vector dust_velocity = ( velocity( ).dy( 0.0 ) * DUST_VELOCITY_DAMPEN_RATIO );
        uint dust_count = Random::rint( DUST_LANDING_COUNT );
        for_range( dust_count )
        {
            bool left = i < half( dust_count );
            Coordinate foot = position( ) + left_foot( ) + ( Vector( left_foot( ), right_foot( ) ) * ( (double)i / (double)dust_count ) );
            Angle angle = ground->vector( ).angle( ) + Random::rAngle( DUST_LANDING_ANGLE.min( ), DUST_LANDING_ANGLE.max( ) );
            if( !left )
            {
                angle.flip( );
            }
            world( )->add_object( new Dust( world( ), foot, dust_velocity + VectorA( angle, Random::rPlanc( DUST_RISE ) ), world( )->terrain( )->dust_color( ) ) );
        }
    }

    Object::ground( ground );
    return *this;
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

Angle Climber::aim_shake( ) const
{
    return Random::flipped( Random::rAngle( ) / 64.0 ); // todo
}

Climber & Climber::aim( const Angle & _aim_angle )
{
    m_aim_angle = _aim_angle;
    drawing_dirty( true );
    return *this;
}

Climber & Climber::fire_hook( )
{
    m_firing_hook = true;
    drawing_dirty( true );
    return *this;
}

Climber & Climber::fire_arrow( )
{
    m_firing_arrow = true;
    drawing_dirty( true );
    return *this;
}

Climber & Climber::release_bow( )
{
    m_firing_hook = false;
    m_firing_arrow = false;
    drawing_dirty( true );
    return *this;
}

Coordinate Climber::head_center( ) const
{
    return Coordinate( 0.0, BODY_HEIGHT + HEAD_Y_OFFSET + ( HEAD_BOB_DISTANCE * sin( (double)age( ) / BOB_PERIOD ) ) );
}

Coordinate Climber::face_center( ) const
{
    return head_center( ) + VectorA( aim_angle( ), ( HEAD_RADIUS - FACE_RADIUS ) / 2.0 );
}

Coordinate Climber::front_shoulder( ) const
{
    Coordinate shoulder = Coordinate( -half( BODY_WIDTH - ( SHOULDER_RADIUS * 2.0 ) ), half( BODY_HEIGHT - ( SHOULDER_RADIUS * 2.0 ) ) + ( SHOULDER_BOB_DISTANCE * sin( (double)( age( ) + ( BOB_PERIOD * SHOULDER_BOB_OFFSET ) ) / BOB_PERIOD ) ) );
    if( aiming_left( ) )
        shoulder.mirror_y( );
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
    Coordinate hand = VectorA( aim_angle( ) - flipped( RIGHT_ANGLE, aiming_left( ) ), 12.0 );
    return hand;
}

Coordinate Climber::back_hand( ) const
{
    Coordinate hand = VectorA( aim_angle( ) - flipped( Angle( TAU / 16.0 ), aiming_left( ) ), 24.0 );
    return hand;
}

Coordinate Climber::front_hip( ) const
{
    return Coordinate( flipped( LEG_WIDTH.half( ) - PANTS_WIDTH_UPPER.half( ), facing_left( ) ), -BODY_HEIGHT.half( ) );
}

Coordinate Climber::back_hip( ) const
{
    return Coordinate( flipped( PANTS_WIDTH_UPPER.half( ) - LEG_WIDTH.half( ), facing_left( ) ), -BODY_HEIGHT.half( ) );
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
    return ( !facing_left( ) ? back_shoulder( ) : front_shoulder( ) );
}

Coordinate Climber::left_hand( ) const
{
    return ( facing_left( ) ? back_hand( ) : front_hand( ) );
}

Coordinate Climber::right_hand( ) const
{
    return ( !facing_left( ) ? back_hand( ) : front_hand( ) );
}

Coordinate Climber::left_hip( ) const
{
    return ( facing_left( ) ? front_hip( ) : back_hip( ) );
}

Coordinate Climber::right_hip( ) const
{
    return ( !facing_left( ) ? front_hip( ) : back_hip( ) );
}

Coordinate Climber::left_foot( ) const
{
    return ( facing_left( ) ? front_foot( ) : back_foot( ) );
}

Coordinate Climber::right_foot( ) const
{
    return ( !facing_left( ) ? front_foot( ) : back_foot( ) );
}

// -- GRAPHICS --

Climber::Skin Climber::skin( ) const
{
    return m_skin;
}

Color Climber::skin_color( ) const
{
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
}

Color Climber::hair_color( ) const
{
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
}

Color Climber::eye_color( ) const
{
    switch( skin( ) )
    {
        case MOHAWK :
        case NO_SKIN :
        default :
            return BLACK;
            break;
    }
}

Color Climber::undershirt_color( ) const
{
    switch( skin( ) )
    {
        case MOHAWK :
            return GRAY_LIGHT;
            break;
        case NO_SKIN :
        default :
            return WHITE;
            break;
    }
}

Color Climber::jacket_color( ) const
{
    switch( skin( ) )
    {
        case MOHAWK :
            return Color::rgb( 0x8C4B2D );
            break;
        case NO_SKIN :
        default :
            return RED;
            break;
    }
}

Color Climber::pants_color( ) const
{
    switch( skin( ) )
    {
        case MOHAWK :
            return Color::rgb( 0xCD8741 );
            break;
        case NO_SKIN :
        default :
            return BLUE;
            break;
    }
}

Color Climber::belt_color( ) const
{
    switch( skin( ) )
    {
        case MOHAWK :
        case NO_SKIN :
        default :
            return BLACK;
            break;
    }
}

Color Climber::belt_buckle_color( ) const
{
    switch( skin( ) )
    {
        case MOHAWK :
        case NO_SKIN :
        default :
            return YELLOW;
            break;
    }
}

Color Climber::boot_color( ) const
{
    switch( skin( ) )
    {
        case MOHAWK :
        case NO_SKIN :
        default :
            return BLACK;
            break;
    }
}

Color Climber::boot_sole_color( ) const
{
    switch( skin( ) )
    {
        case MOHAWK :
        case NO_SKIN :
        default :
            return GRAY_MID;
            break;
    }
}

Color Climber::boot_lace_color( ) const
{
    switch( skin( ) )
    {
        case MOHAWK :
        case NO_SKIN :
        default :
            return WHITE;
            break;
    }
}

const Climber & Climber::render( ) const
{
    Player::render( );

    m_hook.render( );
    draw( m_hook.drawing( ) );

    render_legs( );
    render_torso( );
    render_back_arm( );
    render_head( );
    render_back_hand( );
    render_crossbow( );
    render_front_arm( );
    render_front_hand( );
    render_arrow( );

    return *this;
}

void Climber::render_head( ) const
{
    static_setup( Drawing, head_drawing )
    {

        // head
        head_drawing.draw( skin_color( ), Polygon::circle( HEAD_RADIUS ) );

        if( m_skin == MOHAWK )
        {
            // shaved hair
            varray<Coordinate> top_half_coordinates = Polygon::circle( HEAD_RADIUS ).coordinates( );
            uint count = ( top_half_coordinates.size( ) / 2 ) - 1;
            // TODO wtf?
            for_range( count ) top_half_coordinates.remove( floor( top_half_coordinates.size( ) / 2.f ) );
            Polygon top_half = Polygon( top_half_coordinates );
            head_drawing.draw( hair_color( ).a( 0.375 ), top_half );
        }

        // ears
        head_drawing.draw( skin_color( ), Polygon::circle( EAR_RADIUS, VectorX( -HEAD_RADIUS + EAR_RADIUS ) ) );
        head_drawing.draw( skin_color( ), Polygon::circle( EAR_RADIUS, VectorX( HEAD_RADIUS - EAR_RADIUS ) ) );
    }

    draw( Drawing( head_drawing ).move( Vector( head_center( ) ) ) );

    // face
    draw( skin_color( ), Polygon::circle( FACE_RADIUS, face_center( ) ) );

    if( m_skin == MOHAWK )
    {
        { // mohawk
            Coordinate top = head_center( ) + VectorY( HEAD_RADIUS + 3.0 );
            Coordinate top_right = top + Vector( 5.0, 0.0 );
            Coordinate top_left = top + Vector( -5.0, 0.0 );

            Coordinate bottom = face_center( ) + VectorY( FACE_RADIUS - 4.0 );
            Coordinate bottom_right = bottom + Vector( 5.0, 0.0 );
            Coordinate bottom_left = bottom + Vector( -5.0, 0.0 );

            draw( hair_color( ), Line( top, bottom ), 10.0 );
        }

        { // beard
            draw( hair_color( ), Polygon::rectangle( FACE_RADIUS * 2.0, 12.0, face_center( ) + VectorY( -12.0 ) ) );
            draw( hair_color( ), Polygon::rectangle( 12.0, 4.0, face_center( ) + VectorY( -6.0 ) ) );
        }
    }

    // nose
    // draw( skin_color_secondary( ), Polygon::circle( NOSE_RADIUS, face_center( ) + Vector( -half( NOSE_WIDTH ), NOSE_OFFSET_Y - half( NOSE_HEIGHT ) + NOSE_RADIUS ) ) );
    // draw( skin_color_secondary( ), Polygon::circle( NOSE_RADIUS, face_center( ) + Vector( half( NOSE_WIDTH ), NOSE_OFFSET_Y - half( NOSE_HEIGHT ) + NOSE_RADIUS ) ) );
    // draw( { skin_color( ), skin_color( ), skin_color_secondary( ), skin_color_secondary( ) }, Polygon::rectangle( NOSE_WIDTH, NOSE_HEIGHT, face_center( ) + VectorY( NOSE_OFFSET_Y ) ) );
    // draw( skin_color_secondary( ), Polygon::circle( NOSE_WIDTH, face_center( ) + VectorY( NOSE_OFFSET_Y ) ) );

    // eyes
    if( m_blink_wait.remaining( ) && m_blink_wait.remaining( ) )
    {
        draw( eye_color( ), Polygon::rectangle( EYE_WIDTH, EYE_WIDTH, face_center( ) + Vector( EYE_SPACING, 0.0 ) ) );
        draw( eye_color( ), Polygon::rectangle( EYE_WIDTH, EYE_WIDTH, face_center( ) + Vector( -EYE_SPACING, 0.0 ) ) );
    }
    else
    {
        draw( eye_color( ), Polygon::rectangle( EYE_WIDTH, 1.0, face_center( ) + Vector( EYE_SPACING, -1.0 ) ) );
        draw( eye_color( ), Polygon::rectangle( EYE_WIDTH, 1.0, face_center( ) + Vector( -EYE_SPACING, -1.0 ) ) );
    }
}

void Climber::render_torso( ) const
{
    Coordinate waist = Coordinate( 0.0, -BODY_HEIGHT.half( ) );

    Drawing body_drawing;

    // pants base
    body_drawing.draw( pants_color( ), Polygon( { waist + Coordinate( PANTS_WIDTH_UPPER.half( ), 0.0 ),
                                                  waist + Coordinate( -PANTS_WIDTH_UPPER.half( ), 0.0 ),
                                                  waist + Coordinate( -PANTS_WIDTH_LOWER.half( ), -PANTS_HEIGHT.half( ) ),
                                                  waist + Coordinate( PANTS_WIDTH_LOWER.half( ), -PANTS_HEIGHT.half( ) ) } ) );

    // undershirt
    body_drawing.draw( undershirt_color( ), Polygon::rectangle( BODY_WIDTH.half( ), BODY_HEIGHT ) );

    // belt
    body_drawing.draw( belt_color( ), Line( waist + Vector( PANTS_WIDTH_UPPER.half( ), 0.0 ), waist + Vector( -PANTS_WIDTH_UPPER.half( ) ) ), (double)BELT_THICKNESS );
    body_drawing.draw( belt_buckle_color( ), Polygon::rectangle( BELT_BUCKLE_WIDTH, BELT_BUCKLE_WIDTH, waist ) );

    // jacket
    body_drawing.draw( jacket_color( ), Polygon::circle( BACK_ARM_WIDTH.half( ), back_shoulder( ) ) );
    body_drawing.draw( jacket_color( ), Polygon::rectangle( BODY_WIDTH / 3.0, BODY_HEIGHT - SHOULDER_RADIUS, Coordinate( BODY_WIDTH / 3.5, -half( SHOULDER_RADIUS ) ) ) );
    body_drawing.draw( jacket_color( ), Polygon::rectangle( BODY_WIDTH / 3.0, BODY_HEIGHT - SHOULDER_RADIUS, Coordinate( -BODY_WIDTH / 3.5, -half( SHOULDER_RADIUS ) ) ) );

    draw( body_drawing );
}

void Climber::render_legs( ) const
{
    static_setup( Drawing, boot_drawing )
    {
        Planc x_offset_base = half( BOOT_BASE_WIDTH - BOOT_TOP_WIDTH );
        Polygon boot_sole = Polygon::rectangle( BOOT_BASE_WIDTH, BOOT_SOLE_HEIGHT, Coordinate( x_offset_base, -BOOT_SOLE_HEIGHT.half( ) ) );
        boot_drawing.draw( boot_sole_color( ), boot_sole );
        {
            Polygon boot_sub_sole = Polygon::rectangle( BOOT_SOLE_SUB_WIDTH, BOOT_SOLE_SUB_HEIGHT, Coordinate( x_offset_base - BOOT_BASE_WIDTH.half( ) + BOOT_SOLE_SUB_WIDTH.half( ), -BOOT_SOLE_SUB_HEIGHT - BOOT_SOLE_HEIGHT.half( ) ) );
            boot_drawing.draw( boot_sole_color( ), boot_sub_sole );
            for_range( floor( BOOT_BASE_WIDTH.half( ).half( ) ) )
            {
                Polygon boot_sub_sole = Polygon::rectangle( BOOT_SOLE_SUB_WIDTH, BOOT_SOLE_SUB_HEIGHT, Coordinate( x_offset_base + BOOT_BASE_WIDTH.half( ) - BOOT_SOLE_SUB_WIDTH.half( ) - ( ( BOOT_SOLE_SUB_WIDTH + BOOT_SOLE_SUB_GAP_WIDTH ) * (Planc)i ), -BOOT_SOLE_SUB_HEIGHT - BOOT_SOLE_HEIGHT.half( ) ) );
                boot_drawing.draw( boot_sole_color( ), boot_sub_sole );
            }
        }

        Polygon boot_shin = Polygon::rectangle( BOOT_TOP_WIDTH, BOOT_TOP_HEIGHT, Coordinate( 0.0, BOOT_TOP_HEIGHT.half( ) ) );
        Polygon boot_base = Polygon::rectangle( BOOT_BASE_WIDTH, BOOT_BASE_HEIGHT, Coordinate( x_offset_base, BOOT_BASE_HEIGHT.half( ) ) );
        boot_drawing.draw( boot_color( ), boot_shin );
        boot_drawing.draw( boot_color( ), boot_base );

        Line lace_1 = Line( Coordinate( 5.0, 7.5 ), Coordinate( -3.0, 7.5 ) );
        Line lace_2 = Line( Coordinate( 5.0, 6.0 ), Coordinate( -3.0, 6.0 ) );
        boot_drawing.draw( boot_lace_color( ), lace_1, 1.0 );
        boot_drawing.draw( boot_lace_color( ), lace_2, 1.0 );
    }

    Planc foot_offset_x = ( sin( world( )->age( ) / 2.0 ) * 4.0 );
    Planc foot_offset_y = ( cos( world( )->age( ) / 2.0 ) * 2.0 );
    Planc foot_offset_y_back = max( -foot_offset_y, (Planc)0.0 );
    Planc foot_offset_y_front = max( foot_offset_y, (Planc)0.0 );

    // back leg
    {
        Coordinate hip = back_hip( );
        Coordinate foot = back_foot( );

        if( Object::ground( ) && ( moving_left( ) ^ moving_right( ) ) )
        {
            if( moving_left( ) )
                foot += Vector( foot_offset_x, foot_offset_y_back );
            else if( moving_right( ) )
                foot += Vector( -foot_offset_x, foot_offset_y_back );
        }

        Coordinate knee = foot + VectorY( LEG_HEIGHT * ( 2.0 / 3.0 ) );

        Drawing boot_drawing_back = boot_drawing;
        if( facing_left( ) )
            boot_drawing_back.mirror_y( );
        boot_drawing_back.move( foot );

        draw( pants_color( ), Line( hip, foot ), LEG_WIDTH );
        // draw( pants_color( ), Line( hip, knee ), LEG_WIDTH );
        // draw( pants_color( ), Polygon::circle( LEG_WIDTH.half( ), knee ) );
        // draw( pants_color( ), Line( knee, foot ), LEG_WIDTH );

        draw( boot_drawing_back );
    }

    // front leg
    {
        Coordinate hip = front_hip( );
        Coordinate foot = front_foot( );

        if( Object::ground( ) && ( moving_left( ) ^ moving_right( ) ) )
        {
            if( moving_left( ) )
                foot += Vector( -foot_offset_x, foot_offset_y_front );
            else if( moving_right( ) )
                foot += Vector( foot_offset_x, foot_offset_y_front );
        }

        Coordinate knee = foot + VectorY( LEG_HEIGHT * ( 2.0 / 3.0 ) );

        Drawing boot_drawing_front = boot_drawing;
        if( facing_left( ) )
            boot_drawing_front.mirror_y( );
        boot_drawing_front.move( foot );

        draw( pants_color( ), Line( hip, foot ), LEG_WIDTH );
        // draw( pants_color( ), Line( hip, knee ), LEG_WIDTH );
        // draw( pants_color( ), Polygon::circle( LEG_WIDTH.half( ), knee ) );
        // draw( pants_color( ), Line( knee, foot ), LEG_WIDTH );

        draw( boot_drawing_front );
    }
}

void Climber::render_front_arm( ) const
{
    Coordinate shoulder = front_shoulder( );
    Coordinate hand = front_hand( );

    Joint lower_arm = Joint( shoulder, UPPER_ARM_LENGTH, LOWER_ARM_LENGTH, hand, !( aiming_left( ) ) );
    Joint upper_arm = Joint( shoulder, UPPER_ARM_LENGTH, LOWER_ARM_LENGTH, hand, !( aiming_left( ) ) );

    draw( jacket_color( ), Polygon::circle( SHOULDER_RADIUS, shoulder ) );

    draw( jacket_color( ), Line( lower_arm.c1( ), lower_arm.joint( ) ), (double)FRONT_ARM_WIDTH );
    draw( jacket_color( ), Polygon::circle( FRONT_ARM_WIDTH.half( ), upper_arm.joint( ) ) );
    draw( jacket_color( ), Line( upper_arm.joint( ), upper_arm.c2( ) ), (double)FRONT_ARM_WIDTH );
}

void Climber::render_back_arm( ) const
{
    draw( jacket_color( ), Line( back_shoulder( ), back_hand( ) ), (double)BACK_ARM_WIDTH );
}

void Climber::render_front_hand( ) const
{
    bool finger_down = ( m_reload_timer.remaining( ) <= ( RELOAD_TIME / 2 ) );

    Coordinate hand = front_hand( );

    Coordinate finger_base = hand + VectorA( aim_angle( ) + flipped( RIGHT_ANGLE, aiming_left( ) ), FRONT_ARM_WIDTH.half( ) - FINGER_WIDTH.half( ) );
    Coordinate finger_tip = finger_base + VectorA( aim_angle( ), finger_down ? FINGER_LENGTH_OUT : FINGER_LENGTH_DOWN );

    draw( skin_color( ), Polygon::circle( FRONT_ARM_WIDTH.half( ), hand ) );
    draw( skin_color( ), Line( finger_base, finger_tip ), FINGER_WIDTH );
}

void Climber::render_back_hand( ) const
{
    draw( skin_color( ), Polygon::circle( BACK_ARM_WIDTH.half( ), back_hand( ) ) );
}

void Climber::render_crossbow( ) const
{
    Polygon crossbow_polygon = Polygon( { { 6.0, 0.0 }, Coordinate( 0.0, 0.0 ), Coordinate( -2.0, -2.0 ), Coordinate( -4.0, -2.0 ), Coordinate( -4.0, -4.0 ), Coordinate( 1.0, -1.0 ), Coordinate( 5.0, -1.0 ) } ).scale( 5.0 );
    Line crossbow_handle = Line( Coordinate( 21.5, 0.0 ), Coordinate( 21.5, -9.0 ) );

    if( aiming_left( ) )
    {
        crossbow_polygon.mirror_x( );
        crossbow_handle.mirror_x( );
    }

    crossbow_polygon.rotate( aim_angle( ) );
    crossbow_handle.rotate( aim_angle( ) );

    draw( CROSSBOW_COLOR, crossbow_handle, (double)4.0 );
    draw( CROSSBOW_COLOR, crossbow_polygon );
}

void Climber::render_arrow( ) const
{
    Arrow arrow( world( ), position( ), VectorA( aim_angle( ), 1.0 ), m_arrow_feather_color );
    arrow.render( );
    arrow.position( position( ) + VectorA( aim_angle( ), arrow.length( ) ) );
    Drawing arrow_drawing = arrow.drawing( );
    arrow_drawing.move( VectorA( aim_angle( ), arrow.length( ) ) );

    draw( arrow_drawing );
}

Climber & Climber::input( Input * _input )
{
    if( KeyInput * key_input = dynamic_cast<KeyInput *>( _input ) )
    {
        KeyInput::KEY key = key_input->key;
        KeyInput::DYNAMIC dynamic = key_input->dynamic;

        bool pressed = ( dynamic == KeyInput::PRESSED );
        bool held = ( dynamic == KeyInput::HELD );

        bool down = ( pressed || held );

        switch( key )
        {
            case ' ' :
            {
                if( down )
                    jump( true );
                else
                    jump( false );
                break;
            }
            case 'w' :
            {
                if( down )
                    looking_up( true );
                else
                    looking_up( false );
                break;
            }
            case 's' :
            {
                if( down )
                    looking_down( true );
                else
                    looking_down( false );
                break;
            }
            case 'a' :
            {
                if( down )
                    moving_left( true );
                else
                    moving_left( false );
                break;
            }
            case 'd' :
            {
                if( down )
                    moving_right( true );
                else
                    moving_right( false );
                break;
            }
            case 'g' :
            {
                if( pressed )
                    god( !god( ), 666 );
                break;
            }
            default :
            {
                break;
            }
        }
        return *this;
    }

    if( MouseInput * mouse_input = dynamic_cast<MouseInput *>( _input ) )
    {
        MouseInput::BUTTON button = mouse_input->button;
        MouseInput::DYNAMIC dynamic = mouse_input->dynamic;

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
        return *this;
    }

    if( ControllerInput * controller_input = dynamic_cast<ControllerInput *>( _input ) )
    {
        if( ControllerButtonInput * button_input = dynamic_cast<ControllerButtonInput *>( controller_input ) )
        {
            ControllerButtonInput::BUTTON button = button_input->button;
            ControllerButtonInput::DYNAMIC dynamic = button_input->dynamic;

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
                if( joystick_input->direction == ControllerJoystickInput::LEFT )
                {
                    moving_left( true );
                }
                else if( joystick_input->direction == ControllerJoystickInput::RIGHT )
                {
                    moving_right( true );
                }
                else if( joystick_input->direction == ControllerJoystickInput::UP )
                {
                    looking_up( true );
                }
                else if( joystick_input->direction == ControllerJoystickInput::DOWN )
                {
                    looking_down( true );
                }
                else
                {
                    moving_left( false );
                    moving_right( false );
                    looking_up( false );
                    looking_down( false );
                }
            }

            if( joystick_input->joystick == ControllerJoystickInput::RIGHT_JOYSTICK )
            {
                aim( joystick_input->vector.angle( ) );
            }
        }
    }

    return *this;
}

Climber & Climber::clear_input( )
{
    release_bow( );
    moving_left( false );
    moving_right( false );
    looking_up( false );
    looking_down( false );
    jump( false );
    return *this;
}

Climber::Hook::Hook( World * world, const Climber * owner ) : Object( world ), m_owner( owner )
{
    Assert( m_owner, "owner cannot be null" );

    solid( true );

    position( m_owner->position( ) );

    m_state = LOADED;
    m_rope_retract_speed = DEFAULT_ROPE_RETRACT_SPEED;
    m_max_rope_length = DEFAULT_ROPE_MAX_LENGTH;
}

const Climber::Hook & Climber::Hook::render( ) const
{
    Object::render( );

    Coordinate tip = hook_tip( ) - m_owner->position( );
    Coordinate base = hook_base( ) - m_owner->position( );

    Vector rod( base, tip );
    rod.magnitude( rod.magnitude( ) - ( HOOK_THICKNESS * HOOK_TIP_LENGTH ) );

    Angle hook_angle( base, tip );

    draw( HOOK_COLOR, Polygon::circle( HOOK_THICKNESS, base ) );
    draw( HOOK_COLOR, Line( rod.origin( ), rod ), HOOK_THICKNESS );
    draw( HOOK_COLOR,
          Polygon::triangle( tip,
                             tip + VectorA( hook_angle + RIGHT_ANGLE, HOOK_THICKNESS * HOOK_TIP_LENGTH.half( ) ) - VectorA( hook_angle, HOOK_THICKNESS * HOOK_TIP_LENGTH ),
                             tip + VectorA( hook_angle - RIGHT_ANGLE, HOOK_THICKNESS * HOOK_TIP_LENGTH.half( ) ) - VectorA( hook_angle, HOOK_THICKNESS * HOOK_TIP_LENGTH ) ) );

    { // rope
        Vector rope_vector = hook_base( ) - m_owner->position( );

        // draw rope base
        draw( ROPE_BASE_COLOR, Line( rope_vector.origin( ), rope_vector ), ROPE_WIDTH );

        { // draw rope detail coils
            Vector rope_chunk = VectorA( rope_vector.angle( ), ROPE_WIDTH );
            for_range( (uint)half( rope_vector.magnitude( ) / rope_chunk.magnitude( ) ) )
            {
                Polygon rope_strip_rect = Polygon::rectangle( ROPE_WIDTH, ROPE_WIDTH, base - ( rope_chunk * ( i + 1 ) * 2 ), rope_chunk.angle( ) );
                draw( ROPE_ALT_COLOR, rope_strip_rect );
            }
        }
    }

    return *this;
}

Climber::Hook & Climber::Hook::update( )
{
    Object::update( );

    return *this;
}

Climber::Hook & Climber::Hook::update_velocity( )
{
    if( state( ) == LOADED )
    {
        ground( nullptr );
        gravity_ratio( 0.0 );
        velocity( ZERO_VECTOR );
        m_angle = m_owner->aim_angle( );
        position( m_owner->position( ) + VectorA( m_angle, HOOK_LENGTH ) );
    }
    else
    {
        gravity_ratio( 0.5 );
        if( state( ) == FIRING )
        {
            m_rope_length = m_owner->position( ).distance( position( ) );

            if( Object::ground( ) )
            {
                m_state = HOOKED;
                velocity( ZERO_VECTOR );
            }
            else if( m_rope_length > m_max_rope_length )
            {
                m_state = RETRACTING;
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
                m_state = LOADED;
        }
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

    return *this;
}

Climber::Hook & Climber::Hook::ground( TerrainEdge * ground )
{
    Object::ground( ground );
    return *this;
}

Coordinate Climber::Hook::hook_tip( ) const
{
    return position( );
}

Coordinate Climber::Hook::hook_base( ) const
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

Climber::Hook & Climber::Hook::fire( const Vector & _launch_speed )
{
    switch( state( ) )
    {
        case LOADED :
            velocity( _launch_speed );
            m_state = FIRING;
            // static SoundClip twang( "twang.wav" );
            // twang.play( );
            break;
        case FIRING :
        case HOOKED :
        case RETRACTING :
            break;
        default :
            // Assert( "invalid_state" );
            break;
    }

    return *this;
}

Climber::Hook & Climber::Hook::retract( )
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
            m_state = RETRACTING;
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

    return *this;
}

Climber::Hook & Climber::Hook::load( )
{
    ground( nullptr );
    m_state = LOADED;
    return *this;
}

Climber::Hook::HookState Climber::Hook::state( ) const
{
    return m_state;
}
