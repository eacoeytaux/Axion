#ifndef Fire_hpp
#define Fire_hpp

#include "MountMerciless.hpp"
#include "Object.hpp"

namespace mtmercy
{

class Fire : public Object
{

public:

    Fire( Room *, Coordinate cref position );

    virtual void render( ) override;
    virtual void update( ) override;

    virtual void move( Vector cref ) override;

    bool lit( ) const;

    virtual void extinguish( );

    void enable_base_flame( bool );
    void enable_inner_flames( bool );

    void flame_radius( Planc cref );
    void flame_speed( Planc cref );
    void flame_shrink( Planc cref );
    void flame_deviation( Angle cref );

    void flame_pause( Span<uint> cref );

    void flame_color( Color cref );

    void flame_radius_min( Planc cref );

    void flame_base_radius_ratio( Span<dec> cref );
    void flame_base_max_offset_ratio( dec );

    void flame_inner_radius_ratio( dec );
    void flame_inner_speed_ratio( dec );
    void flame_inner_shrink_ratio( dec );
    void flame_inner_outline_ratio( dec );

    void flame_inner_color( Color cref );

    void flame_alpha_shrink( dec );

    void wind_resistance_ratio( dec );
    void movement_resistance_ratio( dec );

    void light_distance( Planc cref );

    void tint_distance( Planc cref );
    void tint_flicker( Planc cref );
    void tint_intensity( dec );

private:

    struct Flame
    {
        dec alpha = 1.0;
        Coordinate offset;
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

    Span<uint> m_flame_pause = { 1 };

    Planc m_flame_radius_min = 0.0;

    Planc m_flame_radius = 0.0;
    Planc m_flame_speed = 0.0;
    Planc m_flame_shrink = 0.0;

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

    Planc m_tint_distance = 0.0;
    dec m_tint_flicker = 0.0;
    dec m_tint_intensity = 0.0;

    dec m_flame_alpha_shrink = 0.0;

    Color m_flame_color;
    Color m_flame_inner_color;

    void set_light_source( );

};

} // namespace mtmercy

#endif /* Fire_hpp */
