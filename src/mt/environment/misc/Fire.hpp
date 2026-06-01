#ifndef Fire_hpp
#define Fire_hpp

#include "mt/MountMerciless.hpp"
#include "axn/world/Object.hpp"

namespace mtmercy
{

class Fire : public Object
{

public:

    Fire( ptr<Room> in_room, Point cref in_position );

    virtual void render( ) override;
    virtual void update( ) override;

    virtual void move( Vector cref in_movement ) override;

    bool lit( ) const;

    virtual void extinguish( );

    void enable_base_flame( bool in_enable );
    void enable_inner_flames( bool in_enable );

    void flame_radius( Planc cref in_radius );
    void flame_speed( Planc cref in_speed );
    void flame_shrink( Planc cref in_shrink );
    void flame_extinguished_shrink( Planc cref in_extinguished_shrink );

    void flame_deviation( Angle cref in_deviation );

    void flame_pause( Span<ticks> cref in_pause );

    void flame_color( Color cref in_color );

    void flame_radius_min( Planc cref in_radius );

    void flame_base_radius_ratio( Span<dec> cref in_ratio );
    void flame_base_max_offset_ratio( dec in_ratio );

    void flame_inner_radius_ratio( dec in_ratio );
    void flame_inner_speed_ratio( dec in_ratio );
    void flame_inner_shrink_ratio( dec in_ratio );
    void flame_inner_outline_ratio( dec in_ratio );

    void flame_inner_color( Color cref in_color );

    void flame_alpha_shrink( dec in_shrink );

    void wind_resistance_ratio( dec in_ratio );
    void movement_resistance_ratio( dec in_ratio );

    void light_distance( Planc cref in_distance );

    void tint_distance( Planc cref in_distance );
    void tint_flicker( Planc cref in_distance );
    void tint_intensity( dec in_intensity );

private:

    struct Flame
    {

        dec alpha = 1.0;
        Point offset;
        Planc radius;

    };

    Flame m_flame_base;
    Flame m_flame_base_inner;
    list<Flame> m_flames;
    list<Flame> m_flames_inner;
    Countdown m_flame_timer;

    bool m_lit = true;

    bool m_flame_base_enabled = true;
    bool m_flames_inner_enabled = false;

    Span<ticks> m_flame_pause = { 1.0 };

    Planc m_flame_radius_min = P0;

    Planc m_flame_radius = P0;
    Planc m_flame_speed = P0;
    Planc m_flame_shrink = P0;
    Planc m_flame_extinguished_shrink = P0;

    Span<dec> m_flame_base_radius_ratio = { 1.0 };
    dec m_flame_base_max_offset_ratio = 0.0;

    dec m_flame_inner_radius_ratio = 0.0;
    dec m_flame_inner_speed_ratio = 0.0;
    dec m_flame_inner_shrink_ratio = 0.0;
    dec m_flame_inner_outline_ratio = 0.0;

    Angle m_flame_deviation = 0.0;

    dec m_wind_resistance_ratio = 0.0;
    dec m_movement_resistance_ratio = 0.0;

    Planc m_light_distance = 0.0;

    Planc m_tint_distance = P0;
    Planc m_tint_flicker = P0;
    dec m_tint_intensity = 0.0;

    dec m_flame_alpha_shrink = 0.0;

    Color m_flame_color;
    Color m_flame_inner_color;

    void set_light_source( );

};

} // namespace mtmercy

#endif /* Fire_hpp */
