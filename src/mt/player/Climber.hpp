#ifndef Climber_hpp
#define Climber_hpp

#include "axn/world/Player.hpp"

#include "mt/MountMerciless.hpp"
#include "mt/player/Hook.hpp"

namespace mtmercy
{

class Climber : public Player
{

public:

    virtual ~Climber( );

    Climber( ) : Player( 0, nullptr, ORIGIN ), m_hook( nullptr, this ) { } // todo delete probably?

    Climber( uint in_player_number, ptr<Room> in_room, Point cref in_position );

    #if defined( AXN_DEBUG )
    Drawing debug_overlay( ) const override;
    #endif

    void render( ) override;

    void update( ) override;
    void input( ptr<Input> in_input ) override;
    void clear_input( ) override;

    Hook cref hook( ) const { return( m_hook ); }

    void die( ) override;

    Planc light_sight( ) const override;

    bool opening_door( ) const override;

    bool moving_right( ) const;
    bool moving_left( ) const;
    bool looking_up( ) const;
    bool looking_down( ) const;

    bool dashing( ) const;

    bool jumping( ) const;

    bool can_jump( ) const;

    uint extra_jumps( ) const;
    uint extra_jumps_max( ) const;

    Angle aim_target( ) const;
    Angle aim_angle( ) const;
    bool aiming_right( ) const;
    bool aiming_left( ) const;
    bool aiming_up( ) const;
    bool aiming_down( ) const;
    Angle aim_shake( ) const;
    Angle aim_shake_range( ) const;

    // todo move to AimHUD
    Countdown cref aim_display_timer( ) const { return( m_aim_display_timer ); }
    Countdown cref aim_display_fade_timer( ) const { return( m_aim_display_fade_timer ); }

protected:

    virtual dec air_resistance( ) const override;

    void trim_velocity( ) override;
    void update_velocity( ) override;

    virtual dec check_movement( Vector cref in_velocity ) override;

    void movement_stop( );

    void moving_right( bool in_moving );
    void moving_left( bool in_moving );
    void looking_up( bool in_looking );
    void looking_down( bool in_looking );

    void ground( ptr<Terrain::Node> in_node, Terrain::Bumper cref in_bumper ) override;

    void dash( bool in_dashing );

    void jump( bool in_jumping );

    void extra_jumps( uint in_jump_count );

    void aim( Angle cref in_aim_angle );
    void aim_shake( Angle cref in_aim_shake_angle );

    void launch_hook( );
    void launch_arrow( );

    void release_hook( );
    void release_arrow( );

    void release_bow( );

    // all of these are offsets from position

    Point head_center( ) const;
    Point face_center( ) const;

    Point shoulder( bool in_front ) const;
    Point elbow( bool in_front ) const;
    Point hand( bool in_front ) const;

    Point hand_to_crossbow_offset( bool in_front ) const;

    Planc arm_width( bool in_front ) const;
    Planc arm_length_upper( bool in_front ) const;
    Planc arm_length_lower( bool in_front ) const;

    Point hip( bool in_front ) const;
    Point foot( bool in_front ) const;

    void draw_head( );

    void draw_torso( );

    void draw_arm( bool in_front );
    void draw_hand( bool in_front );

    void draw_arm_front( ) { draw_arm( true ); }
    void draw_arm_back( ) { draw_arm( false ); }

    void draw_legs( );

    void draw_rope( );
    void draw_hook( );
    void draw_arrow( );
    void draw_crossbow( );

    enum ColorPiece
    {
        SKIN,
        HAIR,
        HAIR_SHAVED,
        EYE,
        UNDERSHIRT,
        JACKET,
        PANTS,
        BELT,
        BELT_BUCKLE,
        BOOT,
        BOOT_SOLE,
        BOOT_LACE,
    };

    Color color( ColorPiece in_color_piece ) const;
    Color eye_color( ) const override { return( color( EYE ) ); }

private:

    enum Outfit
    {
        NO_OUTFIT = 0,
        MOHAWK,
    }
    m_outfit;

    Outfit outfit( ) const;

    Hook m_hook;

    Planc m_movement_speed_dash;
    Planc m_movement_speed_ground;
    Planc m_movement_speed_air;

    bool m_moving_right = false;
    bool m_moving_left = false;
    bool m_looking_up = false;
    bool m_looking_down = false;

    Countdown m_dashing_timer;
    Countdown m_dash_reset_timer;

    bool m_jumping = false;
    dec m_jump_degradation = 0.0;
    Countdown m_jumping_timer;
    Countdown m_jump_reset_timer;
    Countdown m_coyote_jump_timer;
    uint m_extra_jumps_max;
    uint m_extra_jumps;

    RagDollLimb m_dead_face;
    RagDollLimb m_dead_arm_front;
    RagDollLimb m_dead_arm_back;

    bool m_aiming = true;
    Angle m_aim_target = A0;
    Angle m_aim_angle = A0;
    Angle m_dead_aim_angle = A0;
    Angle m_aim_shake_angle = A0;
    Countdown m_aim_display_timer;
    Countdown m_aim_display_fade_timer;

    bool m_launching_hook = false;
    bool m_launching_arrow = false;
    bool m_slashing = false;
    Countdown m_hook_reload_timer;
    Countdown m_arrow_reload_timer;
    Color m_arrow_feather_color;

    Countdown m_dust_timer;

public:

    class HealthHUD : public Camera::HUD
    {

    public:

        HealthHUD( ptr<Climber> in_climber = nullptr );

        void render( ptr<Camera> in_camera ) override;

    private:

        ptr<Climber> m_climber = nullptr;

    };

    HealthHUD cref healthbar( ) const { return( m_healthbar ); }
    void healthbar( HealthHUD cref in_healthbar ) { m_healthbar = in_healthbar; }

private:

    HealthHUD m_healthbar;

public:

    class JumpHUD : public Camera::HUD
    {

    public:

        JumpHUD( ptr<Climber> in_climber = nullptr );

        void render( ptr<Camera> in_camera ) override;

    private:

        ptr<Climber> m_climber = nullptr;

    };

    JumpHUD cref jumpbar( ) const { return( m_jumpbar ); }
    void jumpbar( JumpHUD cref in_jumpbar ) { m_jumpbar = in_jumpbar; }

private:

    JumpHUD m_jumpbar;

public:

    class AimHUD : public Camera::HUD
    {

    public:

        AimHUD( ptr<Climber> in_climber = nullptr );

        void render( ptr<Camera> camera ) override;

    private:

        ptr<Climber> m_climber = nullptr;

    };

private:

    AimHUD m_aim_hud;

public:

    class LowHealthAlertEffect : public Camera::ScreenVisual
    {

    public:

        LowHealthAlertEffect( ptr<Climber> in_climber = nullptr );

        void render( ptr<Camera> in_camera ) override;

    private:

        ptr<Climber> m_climber = nullptr;

    };

    LowHealthAlertEffect cref low_health_effect( ) const { return( m_low_health_effect ); }
    void low_health_effect( LowHealthAlertEffect cref in_low_health_effect ) { m_low_health_effect = in_low_health_effect; }

private:

    LowHealthAlertEffect m_low_health_effect;

};

} // namespace mtmercy

#endif /* Climber_hpp */
