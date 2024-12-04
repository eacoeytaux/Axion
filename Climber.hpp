#ifndef Climber_hpp
#define Climber_hpp

#include "MountMerciless.hpp"
#include "Player.hpp"
#include "Hook.hpp"

namespace mtmercy
{

class Climber : public Player
{
public:
    virtual ~Climber( );
    Climber( World * world, const Coordinate & position );

    virtual void render( ) override;

    #ifdef AXN_DEBUG
    virtual Drawing debug_overlay( ) const override;
    #endif

    void update( ) override;
    void input( Input * ) override;
    void clear_input( ) override;

    const Hook & hook( ) const { return m_hook; }

    void hurt( dec health ) override;

    Planc light_sight( ) const override;

    bool moving_right( ) const;
    bool moving_left( ) const;
    bool looking_up( ) const;
    bool looking_down( ) const;

    Angle aim_angle( ) const;
    bool aiming_right( ) const;
    bool aiming_left( ) const;
    bool aiming_up( ) const;
    bool aiming_down( ) const;
    Angle aim_shake( ) const;
    Angle aim_shake_range( ) const;

protected:
    void update_velocity( ) override;

    void movement_stop( );

    void moving_right( bool );
    void moving_left( bool );
    void looking_up( bool );
    void looking_down( bool );

    void jump( bool );
    void ground( TerrainEdge * ) override;

    void aim( const Angle & );
    void aim_shake( const Angle & );

    void launch_hook( );
    void launch_arrow( );
    void release_bow( );

    // all of these are offsets from position
    Coordinate head_center( ) const;
    Coordinate face_center( ) const;
    Coordinate front_shoulder( ) const;
    Coordinate back_shoulder( ) const;
    Coordinate front_hand( ) const;
    Coordinate back_hand( ) const;
    Coordinate front_hip( ) const;
    Coordinate back_hip( ) const;
    Coordinate front_foot( ) const;
    Coordinate back_foot( ) const;

    Coordinate left_shoulder( ) const;
    Coordinate right_shoulder( ) const;
    Coordinate left_hand( ) const;
    Coordinate right_hand( ) const;
    Coordinate left_hip( ) const;
    Coordinate right_hip( ) const;
    Coordinate left_foot( ) const;
    Coordinate right_foot( ) const;

    void draw_head( );
    void draw_torso( );
    void draw_legs( );
    void draw_front_arm( );
    void draw_back_arm( );
    void draw_front_hand( );
    void draw_back_hand( );
    void draw_crossbow( );
    void draw_arrow( );

    enum ColorPiece
    {
        SKIN,
        HAIR,
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

    Color color( ColorPiece ) const;
    Color eye_color( ) const override { return color( EYE ); }

private:
    enum Skin
    {
        NO_SKIN = 0,
        MOHAWK,
    } m_skin;
    Skin skin( ) const;

    Hook m_hook;

    Planc m_movement_speed_ground = ZERO;
    Planc m_movement_speed_air = ZERO;
    bool m_moving_right = false;
    bool m_moving_left = false;
    bool m_looking_up = false;
    bool m_looking_down = false;
    bool m_jumping = false;
    dec m_jump_degradation = ZERO;
    Counter m_jumping_timer;
    Counter m_jump_reset_timer;

    bool m_aiming = true;
    Angle m_aim_angle = ZERO;
    Angle m_aim_shake_angle = ZERO;
    bool m_launching_hook = false;
    bool m_launching_arrow = false;
    bool m_slashing = false;
    Counter m_reload_timer;
    Color m_arrow_feather_color;

    Counter m_dust_timer;

public:
    class HealthBar : public Camera::HeadUpDisplay
    {
    public:
        HealthBar( Climber * climber );

        void render( Camera * ) override;

    private:
        Climber * m_climber = nullptr;
    };

    const HealthBar & healthbar( ) const { return m_healthbar; }
    void healthbar( const HealthBar & healthbar ) { m_healthbar = healthbar; }

private:
    HealthBar m_healthbar;

public:
    class LowHealthAlertEffect : public Camera::ScreenEffect
    {
    public:
        LowHealthAlertEffect( Climber * climber );

        void render( Camera * ) override;

    private:
        Climber * m_climber = nullptr;
    };

    const LowHealthAlertEffect & low_health_effect( ) const { return m_low_health_effect; }
    void low_health_effect( const LowHealthAlertEffect & low_health_effect ) { m_low_health_effect = low_health_effect; }

private:
    LowHealthAlertEffect m_low_health_effect;
};

} // namespace mtmercy

#endif /* Climber_hpp */
