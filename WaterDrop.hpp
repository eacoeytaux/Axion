#ifndef WaterDrop_hpp
#define WaterDrop_hpp

#include "MountMerciless.hpp"
#include "Object.hpp"

namespace mtmercy
{

class WaterDrop : public Object
{

public:

    WaterDrop( Room *, Coordinate cref position, dec z = 1.0 );
    WaterDrop( Room *, Coordinate cref position, Vector cref upward, dec z = 1.0 );

    virtual void render( ) override;
    
    virtual void ground( TerrainEdge * ) override;
    
private:
    
    bool m_splashed;
    Planc m_radius;
    Color m_color;
    
};

} // namespace mtmercy

#endif /* WaterDrop_hpp */
