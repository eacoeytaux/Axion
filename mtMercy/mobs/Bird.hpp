#ifndef Bird_hpp
#define Bird_hpp

#include "axn.hpp"
#include "Mob.hpp"

namespace axn {
namespace reality {

class Feather : public Object {
public:
    virtual ~Feather( ) { }
    Feather( World*, const Coordinate & position );
    
    virtual const Feather & render( ) const override;
    
private:
    Color m_color;
};

class Bird : public Mob {
public:
    virtual ~Bird( ) { }
    Bird( World*, const Coordinate & position );
    
    virtual const Bird & render( ) const override;
    virtual Bird & update( ) override;
    
    virtual Bird & die( ) override;
    
private:
    
};

}
}

#endif /* Bird_hpp */
