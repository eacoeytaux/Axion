#ifndef Fox_hpp
#define Fox_hpp

#include "axn.hpp"
#include "Mob.hpp"

namespace axn {
namespace reality {

class Fox : public Object {
public:
    virtual ~Fox( ) { }
    Fox( World*, const Coordinate & position );
    
    virtual const Fox & render( ) const override;
    virtual Fox & update( ) override;
    
private:
    
};

}
}

#endif /* Fox_hpp */
