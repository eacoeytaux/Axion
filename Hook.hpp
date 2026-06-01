#ifndef Hook_hpp
#define Hook_hpp

#include "MountMerciless.hpp"
#include "Object.hpp"

namespace mtmercy
{

class Climber;

class Hook : public Object
{

public:

    Hook( Room *, Climber * );

    void render( ) override;

    void update( ) override;
    void update_velocity( ) override;
    void ground( Terrain::Node * node, Terrain::Bumper cref bumper ) override;

    Planc max_rope_length( ) const;
    Planc rope_length( ) const;

    void extend_rope( Planc length );
    void shorten_rope( Planc length );

    Coordinate hook_base( ) const;
    Coordinate hook_tip( ) const;

    Vector rope( ) const;

    bool taut( ) const;
    bool hooked( ) const;
    bool loaded( ) const;
    bool launching( ) const;
    bool retracting( ) const;

    void launch( Vector cref launch_speed );
    void retract( );
    void reload( );

protected:

    virtual bool collide( Object * ) override;

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
