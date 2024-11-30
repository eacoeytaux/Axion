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
    virtual ~Mob( ) { }
    Mob( World * world, const Coordinate & position, dec health = 1 );

#ifdef AXN_DEBUG
    virtual Drawing debug_overlay( ) const override;
#endif

    virtual void die( );
    virtual bool alive( ) const;
    bool dead( ) const { return !alive( ); }

    bool invincible( ) const { return ( m_invincible_always || m_invincible_counter.remaining( ) ); }
    virtual void invincible_pause( uint invincible_duration ) { if( invincible_duration ) { m_invincible_counter.reset( max( invincible_duration, m_invincible_counter.remaining( ) ) ) ; } }
    
    virtual bool invincible_always( ) const { return m_invincible_always; }
    virtual void invincible_always( bool invincible ) { m_invincible_always = invincible; }
    
    virtual uint invincible_duration( ) const { return m_invincible_duration; }
    virtual void invincible_duration( uint duration ) { m_invincible_duration = duration; }

    virtual dec health( ) const;
    virtual void health( dec health ); // will increase max health if necessary

    dec health_percentage( ) const;
    void health_percentage( dec health_percentage );

    virtual void heal( dec health ); // won't excede max health
    virtual void heal_full( );
    virtual void hurt( dec damage );

    virtual dec max_health( ) const;
    virtual void max_health( dec health ); // will decrease health if necessary

    virtual bool facing_right( ) const;
    virtual void facing_right( bool facing_right );

    bool facing_left( ) const { return !facing_right( ); }
    void facing_left( bool facing_left ) { return facing_right( !facing_left ); }

protected:
    virtual void render( ) override;
    virtual void hurt_display_settings( );

    virtual void update( ) override;
    virtual void update_movement( ) override;
    virtual void update_velocity( ) override;
    
// eyes
protected:
    virtual void eye_info( Planc eye_radius, uint blink_duration, const Span<uint> & blink_wait_span, const Color & eye_color = BLACK );
    virtual void eye_info( Planc eye_radius, const Color & eye_color) { return eye_info( eye_radius, ZERO, { (uint)ZERO, (uint)ZERO }, eye_color); }
    virtual void eye_info( Planc eye_radius ) { return eye_info( eye_radius, ZERO, { (uint)ZERO, (uint)ZERO } ); }
    
    bool eyes_open( ) const { return !eyes_closed( ); }
    void eyes_open( bool open ) { eyes_closed( !open ); }
    virtual bool eyes_closed( ) const { return m_eyes_closed || ( m_blink_duration && !m_blink_wait_counter.remaining( ) ); }
    virtual void eyes_closed( bool closed ) { m_eyes_closed = closed; }
    void open_eyes( ) { eyes_open( true ); }
    void close_eyes( ) { eyes_closed( true ); }
    
    virtual bool squinting( ) const { return m_eyes_squinting; }
    virtual void squinting( bool squinting ) { m_eyes_squinting = squinting; }
    
    virtual void draw_eyes( const Coordinate & eye_position, const Angle & angle = ZERO );
    virtual void draw_eyes( const Coordinate & eye_position1, const Coordinate & eye_position2, const Angle & angle = ZERO )
    {
        draw_eyes( eye_position1, angle );
        draw_eyes( eye_position2, angle );
    }
    virtual void draw_eyes( const varray<Coordinate> & eye_positions, const Angle & angle = ZERO )
    {
        for_each( position, eye_positions )
        {
            draw_eyes( position, angle );
        }
    }
    
    virtual Color eye_color( ) const { return m_eye_color; }
    
    bool m_eye_info_set = false;
    Planc m_eye_radius;
    bool m_eyes_closed;
    bool m_eyes_squinting;
    uint m_blink_duration;
    Counter m_blink_duration_counter;
    Span<uint> m_blink_wait_span;
    Counter m_blink_wait_counter;
    Color m_eye_color;

protected:
    const Counter & invincible_counter( ) const { return m_invincible_counter; }
    
    const Counter & hurt_display( ) const { return m_hurt_display; }
    void hurt_display_length( uint ticks ) { m_hurt_display.set( ticks ); }

private:
    bool m_alive = true;
    Slider<dec> m_health;
    
    bool m_invincible_always = false;
    uint m_invincible_duration;
    Counter m_invincible_counter;
    Counter m_hurt_display;

    bool m_facing_right = true;
};

} // namespace reality
} // namespace axn

#endif /* Mob_hpp */
