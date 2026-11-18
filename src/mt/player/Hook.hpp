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

    void render( ) override;

    void update( ) override;

    void trim_velocity( ) override;
    void update_velocity( ) override;

    void ground( ptr<Terrain::Node> in_node, Terrain::Bumper cref in_bumper ) override;

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

protected:

    virtual bool collide( ptr<Object> in_object ) override;

private:

    Climber * m_owner;

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
