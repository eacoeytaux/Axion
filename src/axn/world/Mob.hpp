#ifndef Mob_hpp
#define Mob_hpp

#include "axn/world/Object.hpp"

namespace axn
{
namespace reality
{

class Mob : public Object
{

public:

    Mob( ptr<Room> in_room, Point cref in_position, dec in_health = 1.0 );

    #if defined( AXN_DEBUG )
    virtual Drawing debug_overlay( ) const override;
    #endif

protected:

    virtual void render( ) override;
    virtual void hurt_display_settings( );

    virtual void update( ) override;

    virtual void trim_velocity( );
    virtual void update_velocity( );
    virtual void update_movement( );

public:

    // some of these should definitely be protected

    virtual bool alive( ) const;
    bool dead( ) const { return( !alive( ) ); }

    dec health_percentage( ) const;
    virtual dec health( ) const;
    virtual dec max_health( ) const;

    bool invincible( ) const { return( m_invincible_always || !m_invincible_timer.completed( ) ); }

    virtual bool invincible_always( ) const { return( m_invincible_always ); }
    virtual uint invincible_duration( ) const { return( m_invincible_duration ); }

    virtual bool facing_right( ) const;
    bool facing_left( ) const { return( !facing_right( ) ); }

    virtual void die( );

    virtual void revive( );

    virtual void max_health( dec in_health, bool in_add ); // will decrease health if necessary

    virtual void heal( dec in_health ); // won't excede max health
    virtual void heal_full( ) { heal( max_health( ) - health( ) ); }

    virtual void hurt( dec in_health );
    virtual void hurt_full( ) { hurt( health( ) ); }

    virtual void damage( Damage cref in_damage );
    virtual bool damages( Damage cref in_damage ) const { return( !invincible( ) ); }

    Damage cref killing_blow( ) const { return( m_killing_blow ); }

    virtual void invincible_pause( ticks in_invincible_duration ) { if( in_invincible_duration ) { m_invincible_timer.reset( max( in_invincible_duration, m_invincible_timer.remaining( ) ) ); } }

    virtual void invincible_always( bool in_invincible ) { m_invincible_always = in_invincible; }
    virtual void invincible_duration( ticks in_duration ) { m_invincible_duration = in_duration; }

    Countdown cref invincible_timer( ) const { return( m_invincible_timer ); }

    Countdown cref hurt_display_timer( ) const { return( m_hurt_display_timer ); }
    void hurt_display_duration( ticks in_ticks ) { m_hurt_display_timer.duration( in_ticks ); }

    virtual void facing_right( bool in_facing_right );
    void facing_left( bool in_facing_left ) { return( facing_right( !in_facing_left ) ); }

    // eyes

    virtual void eye_info( Planc cref in_eye_radius, ticks in_blink_duration, Span<ticks> cref in_blink_wait_span, Color cref in_eye_color = BLACK );
    virtual void eye_info( Planc cref in_eye_radius, Color cref in_eye_color ) { return( eye_info( in_eye_radius, 0.0, { 0.0, 0.0 }, in_eye_color ) ); }
    virtual void eye_info( Planc cref in_eye_radius ) { return( eye_info( in_eye_radius, 0.0, { 0.0, 0.0 } ) ); }

    virtual bool eyes_closed( ) const { return( m_eyes_closed || ( m_blink_duration && !m_blink_timer.completed( ) ) ); }
    virtual void eyes_closed( bool in_closed ) { m_eyes_closed = in_closed; }

    bool eyes_open( ) const { return( !eyes_closed( ) ); }
    void eyes_open( bool in_open ) { eyes_closed( !in_open ); }

    void close_eyes( ) { eyes_closed( true ); }
    void open_eyes( ) { eyes_open( true ); }

    virtual bool squinting( ) const { return( m_eyes_squinting ); }
    virtual void squinting( bool in_squinting ) { m_eyes_squinting = in_squinting; }

    virtual void draw_eye( Point cref in_eye_position, Angle cref in_angle = A0 );
    virtual void draw_eyes( Point cref in_eye_position1, Point cref in_eye_position2, Angle cref in_angle = A0 ) { draw_eye( in_eye_position1, in_angle ); draw_eye( in_eye_position2, in_angle ); }
    virtual void draw_eyes( varray<Point> cref in_eye_positions, Angle cref in_angle = A0 ) { for_each( position, in_eye_positions ) { draw_eye( position, in_angle ); } }

    virtual Planc eye_radius( ) const { return( m_eye_radius ); }
    virtual Color eye_color( ) const { return( m_eye_color ); }

private:

    bool m_alive = true;
    Slider<dec> m_health;

    bool m_invincible_always = false;
    ticks m_invincible_duration = 0;
    Countdown m_invincible_timer;
    Countdown m_hurt_display_timer;

    Damage m_killing_blow;

    bool m_facing_right = true;

    bool m_eye_info_set = false;
    Planc m_eye_radius;
    bool m_eyes_closed;
    bool m_eyes_squinting;
    ticks m_blink_duration;
    Countdown m_blink_timer;
    Span<ticks> m_blink_pause_duration;
    Countdown m_blink_pause_timer;
    Color m_eye_color;

protected:

    class RagDollLimb
    {

    public:

        RagDollLimb( ) { }

        RagDollLimb( Mob * in_parent, Point cref in_base = ORIGIN ) : m_parent( in_parent ), m_base( in_base ) { }

        Mob * parent( ) { return( m_parent ); }

        Point base( ) const { return( m_base ); }

        Point position( uint in_i ) const { Point p = m_base; for_range( j, in_i + 1 ) { p += VectorA( m_limbs[ j ].angle, m_limbs[ j ].length ); } return( p ); }

        Angle angle( uint in_i ) const { return( m_limbs[ in_i ].angle ); }

        void add_limb( Angle in_angle, Planc in_length = P0 ) { m_limbs.insert_back( Limb( in_angle, in_length ) ); }

        void limb_length( uint in_i, Planc in_length ) { m_limbs[ in_i ].length = in_length; }
        void limb_rotation( uint in_i, Angle in_angle ) { m_limbs[ in_i ].angle = in_angle; }
        void limb_rotation_speed( uint in_i, Angle in_d_angle ) { m_limbs[ in_i ].d_angle = in_d_angle; }
        void limb_ccw_limit( uint in_i, Angle in_ccw_limit ) { m_limbs[ in_i ].has_ccw_limit = true; m_limbs[ in_i ].ccw_limit = in_ccw_limit; }
        void limb_cw_limit( uint in_i, Angle in_cw_limit ) { m_limbs[ in_i ].has_cw_limit = true; m_limbs[ in_i ].cw_limit = in_cw_limit; }
        void limb_velocity_ratio( uint in_i, dec in_ratio ) { m_limbs[ in_i ].force_ratio = in_ratio; }

        void update( );

        default_equal( RagDollLimb );

    private:

        Mob * m_parent = nullptr;

        Point m_base = ORIGIN;

        struct Limb
        {

            Limb( ) { }

            Limb( Angle in_a, Planc in_p ) : angle( in_a ), length( in_p ) { }

            Planc length = P0;

            Angle angle = A0;
            Angle d_angle = A0;

            dec force_ratio = 1.0;

            bool has_ccw_limit = false;
            bool has_cw_limit = false;

            Angle ccw_limit = A0;
            Angle cw_limit = A0;

            default_equal( Limb );

        };

        varray<Limb> m_limbs;

    };
};

} // namespace reality
} // namespace axn

#endif /* Mob_hpp */
