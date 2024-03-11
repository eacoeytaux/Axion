#ifndef MohawkMan_hpp
#define MohawkMan_hpp

#include "Climber.hpp"

namespace mtmercy {

class MohawkMan : public Climber {
public:
    virtual ~MohawkMan( ) { }
    MohawkMan( World * world, const Coordinate & position ) : Climber( world, position ) { /* god( true, 666 ); */ }
    
    virtual const MohawkMan & render( ) const override;
};

}

#endif /* MohawkMan_hpp */
