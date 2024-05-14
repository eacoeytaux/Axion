#ifndef Climber_hpp
#define Climber_hpp

#include "MountMerciless.hpp"

#include "Player.hpp"

namespace mtmercy
{

class Climber : public Player
{
public:
    virtual ~Climber( ) { }
    Climber( World * world, const Coordinate & position );

    virtual const Climber & render( ) const override;

    Climber & update( ) override;
    Climber & input( Input * ) override;
    Climber & clear_input( ) override;

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

protected:
    Climber & update_velocity( ) override;

    Climber & moving_right( bool );
    Climber & moving_left( bool );
    Climber & looking_up( bool );
    Climber & looking_down( bool );

    Climber & jump( bool );
    Climber & ground( TerrainEdge * ) override;

    Climber & aim( const Angle & );

    Climber & fire_hook( );
    Climber & fire_arrow( );
    Climber & release_bow( );

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

    void render_head( ) const;
    void render_torso( ) const;
    void render_legs( ) const;
    void render_front_arm( ) const;
    void render_back_arm( ) const;
    void render_front_hand( ) const;
    void render_back_hand( ) const;
    void render_crossbow( ) const;
    void render_arrow( ) const;

    Color skin_color( ) const;
    Color hair_color( ) const;
    Color eye_color( ) const;

    Color undershirt_color( ) const;
    Color jacket_color( ) const;

    Color pants_color( ) const;
    Color belt_color( ) const;
    Color belt_buckle_color( ) const;

    Color boot_color( ) const;
    Color boot_sole_color( ) const;
    Color boot_lace_color( ) const;

private:
    enum Skin
    {
        NO_SKIN = 0,
        MOHAWK,
    } m_skin;
    Skin skin( ) const;

    Planc m_movement_speed = 0.0;
    bool m_moving_right = false;
    bool m_moving_left = false;
    bool m_looking_up = false;
    bool m_looking_down = false;
    bool m_jumping = false;
    double m_jump_degradation = 0.0;
    Counter m_jumping_timer;
    Counter m_jump_reset_timer;

    bool m_aiming = true;
    Angle m_aim_angle = 0.0;
    bool m_firing_hook = false;
    bool m_firing_arrow = false;
    bool m_slashing = false;
    Counter m_reload_timer;
    Color m_arrow_feather_color;

    Counter m_dust_timer;

public:
    class Hook : public Object
    {
    public:
        virtual ~Hook( ) { }
        Hook( World *, const Climber * );

        const Hook & render( ) const override;

        Hook & update( ) override;
        Hook & update_velocity( ) override;
        Hook & ground( TerrainEdge * ground ) override;

        Coordinate hook_tip( ) const;
        Coordinate hook_base( ) const;

        Hook & fire( const Vector & launch_speed );
        Hook & retract( );
        Hook & load( );

        enum HookState
        {
            LOADED,
            HOOKED,
            FIRING,
            RETRACTING
        };

        HookState state( ) const;

    private:
        const Climber * m_owner;

        Angle m_angle;
        Planc m_rope_length;
        Planc m_max_rope_length;
        Planc m_rope_growth_speed;
        Planc m_rope_retract_speed;

        HookState m_state = LOADED;
    } m_hook;
};

} // namespace mtmercy

#endif /* Climber_hpp */
