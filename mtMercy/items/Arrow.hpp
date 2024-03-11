#ifndef Arrow_hpp
#define Arrow_hpp

#include "axn.hpp"
#include "Object.hpp"

namespace axn {
namespace reality {

class Arrow : public Object {
public:
    virtual ~Arrow( ) { }
    Arrow( World*, const Coordinate & tip, const Vector & launch_speed, const Color & feather_color );
    
    virtual const Arrow & render( ) const override;
    virtual Arrow & update( ) override;
    
    virtual Arrow & move( ) override;
    
    Angle angle( ) const;
    Arrow & angle( const Angle & );
    
    ufloat length( ) const;
    
private:
    Angle m_angle;
    Color m_feather_color;

};

}
}

#endif /* Arrow_hpp */
