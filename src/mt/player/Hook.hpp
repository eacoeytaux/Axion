#ifndef Hook_hpp
#define Hook_hpp

#include "mt/MountMerciless.hpp"
#include "axn/world/Object.hpp"

namespace mtmercy
{

class Climber;

class Hook : public Object
{

public:

    Hook( ptr<Room> in_room, Climber * in_owner );

    virtual void render( ) override;

    virtual void update( ) override;

    virtual void trim_velocity( ) override;
    virtual void update_velocity( ) override;

protected:

    virtual bool collide( ptr<Object> in_object ) override;

public:

    virtual void ground( Terrain::Bumper cref in_ground ) override;

    Planc max_rope_length( ) const;
    Planc rope_length( ) const;

    void extend_rope( Planc cref in_length );
    void shorten_rope( Planc cref in_length );

    Point hook_base( ) const;
    Point hook_tip( ) const;

    Vector rope( ) const;

    bool taut( ) const;
    bool hooked( ) const;
    bool loaded( ) const;
    bool launching( ) const;
    bool retracting( ) const;

    void launch( Vector cref in_launch_speed );
    void retract( );
    void reload( );
    void stow( ); // C! back in the climber's hand straight away, as on the frame a retraction finishes: loaded, still, and pointing where it aims

private:

    // C! the hook's own members, through these everywhere, inside the class too
    ptr<Climber> owner( ) const { return( m_owner ); }
    void owner( ptr<Climber> in_owner ) { m_owner = in_owner; }

    Angle cref angle( ) const { return( m_angle ); } // C! the way the hook points, from its base to its tip
    void angle( Angle cref in_angle ) { m_angle = in_angle; }

    void rope_length( Planc cref in_rope_length ) { m_rope_length = in_rope_length; }
    void max_rope_length( Planc cref in_max_rope_length ) { m_max_rope_length = in_max_rope_length; }

    Planc cref rope_growth_speed( ) const { return( m_rope_growth_speed ); }
    void rope_growth_speed( Planc cref in_rope_growth_speed ) { m_rope_growth_speed = in_rope_growth_speed; }

    Planc cref rope_retract_speed( ) const { return( m_rope_retract_speed ); }
    void rope_retract_speed( Planc cref in_rope_retract_speed ) { m_rope_retract_speed = in_rope_retract_speed; }

private:

    ptr<Climber> m_owner = nullptr;

    Angle m_angle;
    Planc m_rope_length;
    Planc m_max_rope_length;
    Planc m_rope_growth_speed;
    Planc m_rope_retract_speed;

    STATE( LOADED,
           HOOKED,
           LAUNCHING,
           RETRACTING );

};

} // namespace mtmercy

#endif /* Hook_hpp */
