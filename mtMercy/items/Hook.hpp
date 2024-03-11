#ifndef Hook_hpp
#define Hook_hpp

#include "axn.hpp"
#include "Object.hpp"

namespace axn {
namespace reality {

class Hook : public Object {
public:
    virtual ~Hook( ) { }
    Hook( World * world, const Coordinate & hook_tip, const Coordinate & base, const Angle & angle = Angle() );
    
    virtual const Hook & render( ) const override;
    
    virtual Hook & update( ) override;
    
    Angle angle( ) const;
    Hook & angle( const Angle & );
    Coordinate hook_tip( ) const;
    Coordinate hook_base( ) const;
    Coordinate base( ) const;
    Hook & base( const Coordinate &, const Angle & );
    planc length( ) const;
    Hook & length( const planc & );
    planc max_length( ) const;
    Hook & max_length( const planc & );
    
    Hook & fire( const Vector & launch_speed );
    Hook & retract( );
    Hook & load( );
    
private:
    Coordinate m_base;
    Angle m_angle;
    planc m_length;
    planc m_max_length;
    planc m_growth_speed;
    planc m_retract_speed;
    
    enum HOOK_STATE {
        LOADED,
        HOOKED,
        FIRING,
        RETRACTING
    } m_state = LOADED;
};

}
}

#endif /* Hook_hpp */
