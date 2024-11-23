#ifndef Hook_hpp
#define Hook_hpp

#include "Object.hpp"

namespace mtmercy
{

class Climber;

class Hook : public Object
{
public:
    virtual ~Hook( ) { }
    Hook( World *, const Climber * );

    void render( ) override;

    void update( ) override;
    void update_velocity( ) override;
    void ground( TerrainEdge * ground ) override;

    Coordinate hook_tip( ) const;
    Coordinate hook_base( ) const;
    
    Planc rope_length( ) const { return m_rope_length; }
    Planc max_rope_length( ) const { return m_max_rope_length; }

    void fire( const Vector & launch_speed );
    void retract( );
    void load( );

private:
    const Climber * m_owner;

    Angle m_angle;
    Planc m_rope_length;
    Planc m_max_rope_length;
    Planc m_rope_growth_speed;
    Planc m_rope_retract_speed;
    
    STATE( LOADED,
           HOOKED,
           FIRING,
           RETRACTING );
};

} // namespace mtmercy

#endif /* Hook_hpp */
