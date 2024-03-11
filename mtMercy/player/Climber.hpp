#ifndef Climber_hpp
#define Climber_hpp

#include "MountMerciless.hpp"

namespace mtmercy {

class Climber : public Player {
public:
    virtual ~Climber( ) { }
    Climber( World * world, const Coordinate & position );
    
    virtual const Climber & render( ) const override = 0;
    
    Climber & update( ) override;
    Climber & input( Input* ) override;
    
    bool moving_right( ) const;
    bool moving_left( ) const;
    bool looking_up( ) const;
    bool looking_down( ) const;
    
    Angle aim_angle( ) const;
    
protected:
    Climber & adjust_velocity( ) override;
    
    Climber & moving_right( bool );
    Climber & moving_left( bool );
    Climber & looking_up( bool );
    Climber & looking_down( bool );
    
    Climber & jump( bool );
    
    Climber & aim( const Angle & );
    
    Climber & fire_hook( );
    Climber & fire_arrow( );
    Climber & release_bow( );
    
    // all of these are offsets from position
    Coordinate head_center( ) const;
    Coordinate face_center( ) const;
    Coordinate front_shoulder( ) const;
    Coordinate front_hand( ) const;
    Coordinate back_shoulder( ) const;
    Coordinate back_hand( ) const;
    Coordinate front_hip( ) const;
    Coordinate front_foot( ) const;
    Coordinate back_hip( ) const;
    Coordinate back_foot( ) const;
    
protected:
    planc m_movement_speed;
    bool m_moving_right = false;
    bool m_moving_left = false;
    bool m_looking_up = false;
    bool m_looking_down = false;
    bool m_jumping = false;
    ufloat m_jump_degradation = 0.f;
    Counter m_jumping_timer;
    Counter m_jump_reset_timer;
    
    bool m_aiming = true;
    Angle m_aim_angle = 0.f;
    bool m_firing_hook = false;
    bool m_firing_arrow = false;
    Color m_arrow_feather_color;
    planc m_rope_max_length;
    planc m_hook_growth_speed;
    planc m_hook_retract_speed;
    
    Hook m_hook;
    
    Counter m_reload_timer;
    
// TODO move to cpp file
protected:
    const planc HEIGHT = METER * 2.f;
    const planc WIDTH = HEIGHT / 2.f;

    const planc DEFAULT_SPEED = 2.f;
    const planc JUMP_STRENGTH = METER / 4.f;
    const ufloat JUMP_DEGRATION_RATIO = 0.9f;
    const uint JUMP_MAX_DURATION = 5;
    const uint JUMP_RESET_WAIT_TIME = 0;

    // crossbox / hook
    const planc DEFAULT_ROPE_MAX_LENGTH = HEIGHT * 3.f;
    const planc DEFAULT_HOOK_GROWTH_SPEED = 20.f;
    const planc DEFAULT_HOOK_RETRACT_SPEED = 30.f;
    const uint RELOAD_TIME = 12;

    // GRAPHICS

    const planc HEAD_RADIUS = WIDTH / 2.f;
    const planc FACE_RADIUS = HEAD_RADIUS * 0.75f;
    const planc NECK_LENGTH = HEAD_RADIUS * 0.21f;
    const planc NECK_WIDTH = HEAD_RADIUS * 0.5f;
    const planc EYE_SPACING = FACE_RADIUS * 0.55f;

    const planc BODY_HEIGHT = HEIGHT / 2.f * 0.8f;
    const planc LEG_HEIGHT = HEIGHT / 3.f;

    const planc BODY_WIDTH = WIDTH;
    const planc PANTS_WIDTH = BODY_WIDTH - 6.f;
    const planc PANTS_HEIGHT = BODY_HEIGHT / 3.f;
    const planc ARM_WIDTH = WIDTH / 3.f;
    const planc UPPER_ARM_LENGTH = BODY_HEIGHT * 0.64f;
    const planc LOWER_ARM_LENGTH = UPPER_ARM_LENGTH * 0.8f;
    const planc LEG_WIDTH = WIDTH / 3.f;

    const planc BELT_THICKNESS = 3.f;
    const planc BELT_BUCKLE_WIDTH = 5.f;

    // COLORS

    const Color SKIN_COLOR = Color::rgb( 0xFFE1BE ); // Color( 0x9B5032 )
    const Color HAIR_COLOR = RED;
    const Color EYE_COLOR = BLACK;

    const Color HAT_COLOR = YELLOW;
    const Color JACKET_COLOR = Color::rgb( 0x8C4B2D );
    const Color JACKET_FRONT_COLOR = JACKET_COLOR;
    const Color JACKET_BACK_COLOR = JACKET_COLOR;
    const Color UNDERSHIRT_COLOR = WHITE;
    const Color PANTS_COLOR = Color::rgb( 0xCD8741 );
    const Color BELT_COLOR = BLACK;
    const Color BELT_BUCKLE_COLOR = YELLOW;
    const Color BOOT_COLOR = BLACK;
    const Color SOLE_COLOR = GRAY_MID;
    const Color LACE_COLOR = WHITE;

    // crossbow
    const ufloat HANDLE_LENGTH = 12.f;
    const Color CROSSBOW_COLOR = Color::rgb( 0xA54B23 );
};

}

#endif /* Climber_hpp */
