#ifndef Bedrock_hpp
#define Bedrock_hpp

#include "axn.hpp"
#include "Terrain.hpp"

namespace axn {
namespace reality {

class Bedrock : public Terrain {
public:
    virtual ~Bedrock( ) {}
    Bedrock( World* ) {}
};

} // namespace reality
} // namespace axn

#endif /* Bedrock_hpp */
