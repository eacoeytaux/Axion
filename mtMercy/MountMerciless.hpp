#ifndef MountMerciless_hpp
#define MountMerciless_hpp

#include "World.hpp"

namespace mtmercy {

class MountMerciless : public World {
    virtual Player* create_player( const Coordinate & position ) override;
};

}

#endif /* MountMerciless_hpp */
