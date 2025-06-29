#ifndef Mob_hpp
#define Mob_hpp

#include "Object.hpp"

namespace axn
{
namespace reality
{

class Mob : public Object
{

public:

    Mob( Room * room, Coordinate cref position, dec health = 1 );

    #if defined ( AXN_DEBUG )
    virtual Drawing debug_overlay( ) const override;
    #endif

protected:

    virtual void render( ) override;
    virtual void hurt_display_settings( );

    virtual void update( ) override;
    virtual void update_movement( ) override;
    virtual void update_velocity( ) override;

public:

    virtual bool alive( ) const;
    bool dead( ) const { return !alive( ); }

    dec health_percentage( ) const;
    virtual dec health( ) const;
    virtual dec max_health( ) const;

    bool invincible( ) const { return ( m_invincible_always || m_invincible_timer.remaining( ) ); }

    virtual bool invincible_always( ) const { return m_invincible_always; }
    virtual uint invincible_duration( ) const { return m_invincible_duration; }

    virtual bool facing_right( ) const;
    bool facing_left( ) const { return !facing_right( ); }

    virtual void die( );

    virtual void revive( );

    virtual void max_health( dec health ); // will decrease health if necessary

    virtual void heal( dec health ); // won't excede max health
    virtual void heal_full( ) { heal( max_health( ) - health( ) ); }

    virtual void hurt( dec health );
    virtual void hurt_full( ) { hurt( health( ) ); }

    virtual void damage( Damage cref );
    virtual bool damages( Damage cref ) const { return ( alive( ) && !invincible( ) ); }

    virtual void invincible_pause( uint invincible_duration ) { if( invincible_duration ) { m_invincible_timer.reset( max( invincible_duration, m_invincible_timer.remaining( ) ) ); } }

    virtual void invincible_always( bool invincible ) { m_invincible_always = invincible; }
    virtual void invincible_duration( uint duration ) { m_invincible_duration = duration; }

    Countdown cref invincible_timer( ) const { return m_invincible_timer; }

    Countdown cref hurt_display_timer( ) const { return m_hurt_display_timer; }
    void hurt_display_duration( uint ticks ) { m_hurt_display_timer.duration( ticks ); }

    virtual void facing_right( bool facing_right );
    void facing_left( bool facing_left ) { return facing_right( !facing_left ); }

    // eyes

    virtual void eye_info( Planc eye_radius, uint blink_duration, const Span<uint> & blink_wait_span, Color cref eye_color = BLACK );
    virtual void eye_info( Planc eye_radius, Color cref eye_color ) { return eye_info( eye_radius, 0.0, { 0, 0 }, eye_color ); }
    virtual void eye_info( Planc eye_radius ) { return eye_info( eye_radius, 0.0, { 0, 0 } ); }

    virtual bool eyes_closed( ) const { return m_eyes_closed || ( m_blink_duration && m_blink_timer.remaining( ) ); }
    virtual void eyes_closed( bool closed ) { m_eyes_closed = closed; }

    bool eyes_open( ) const { return !eyes_closed( ); }
    void eyes_open( bool open ) { eyes_closed( !open ); }

    void close_eyes( ) { eyes_closed( true ); }
    void open_eyes( ) { eyes_open( true ); }

    virtual bool squinting( ) const { return m_eyes_squinting; }
    virtual void squinting( bool squinting ) { m_eyes_squinting = squinting; }

    virtual void draw_eye( Coordinate cref eye_position, Angle cref angle = A0 );
    virtual void draw_eyes( Coordinate cref eye_position1, Coordinate cref eye_position2, Angle cref angle = A0 ) { draw_eye( eye_position1, angle ); draw_eye( eye_position2, angle ); }
    virtual void draw_eyes( varray<Coordinate> cref eye_positions, Angle cref angle = A0 ) { for_each( position, eye_positions ) { draw_eye( position, angle ); } }

    virtual Planc eye_radius( ) const { return m_eye_radius; }
    virtual Color eye_color( ) const { return m_eye_color; }

private:

    bool m_alive = true;
    Slider<dec> m_health;

    bool m_invincible_always = false;
    uint m_invincible_duration = 0;
    Countdown m_invincible_timer;
    Countdown m_hurt_display_timer;

    bool m_facing_right = true;

    bool m_eye_info_set = false;
    Planc m_eye_radius;
    bool m_eyes_closed;
    bool m_eyes_squinting;
    uint m_blink_duration;
    Countdown m_blink_timer;
    Span<uint> m_blink_pause_duration;
    Countdown m_blink_pause_timer;
    Color m_eye_color;

protected:

    class RagDollLimb
    {

    public:

        RagDollLimb( ) { }

        RagDollLimb( Mob * parent, Coordinate cref base = ORIGIN ) : m_parent( parent ), m_base( base ) { }

        Mob * parent( ) { return m_parent; }

        Coordinate base( ) const { return m_base; }

        Coordinate position( uint i ) const { Coordinate p = m_base; for_range( j, i + 1 ) { p += VectorA( m_limbs[ j ].angle, m_limbs[ j ].length ); } return p; }

        Angle angle( uint i ) const { return m_limbs[ i ].angle; }

        void add_limb( Angle angle, Planc length = P0 ) { m_limbs.insert_back( Limb( angle, length ) ); }

        void limb_length( uint i, Planc length ) { m_limbs[ i ].length = length; }
        void limb_rotation( uint i, Angle angle ) { m_limbs[ i ].angle = angle; }
        void limb_rotation_speed( uint i, Angle d_angle ) { m_limbs[ i ].d_angle = d_angle; }
        void limb_ccw_limit( uint i, Angle ccw_limit ) { m_limbs[ i ].has_ccw_limit = true; m_limbs[ i ].ccw_limit = ccw_limit; }
        void limb_cw_limit( uint i, Angle cw_limit ) { m_limbs[ i ].has_cw_limit = true; m_limbs[ i ].cw_limit = cw_limit; }

        void update( );

        default_equal( RagDollLimb );

    private:

        Mob * m_parent = nullptr;

        Coordinate m_base = ORIGIN;

        struct Limb
        {
            Limb( ) { }

            Limb( Angle a, Planc p ) : angle( a ), length( p ) { }

            Planc length = P0;

            Angle angle = A0;
            Angle d_angle = A0;

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
