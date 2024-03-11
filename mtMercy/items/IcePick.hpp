#ifndef IcePick_hpp
#define IcePick_hpp

#include "axn.hpp"
#include "Object.hpp"

namespace axn {
namespace reality {

class IcePick : public Object {
public:
    virtual ~IcePick( ) { }
    IcePick( World*, const Coordinate & position );
    
    virtual const IcePick & render( ) const override;
    
private:
    
};

}
}

#endif /* IcePick_hpp */
