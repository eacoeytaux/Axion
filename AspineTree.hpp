#ifndef AspineTree_hpp
#define AspineTree_hpp

#include "MountMerciless.hpp"
#include "Object.hpp"

namespace mtmercy
{

class AspineTree : public Object
{

public:

    AspineTree( Room *, Coordinate cref root, dec z = 1.0 );
    
    void update( ) override;
    
    class Leaf : public Object
    {
      
    public:
        
        Leaf( Room *, Color cref color, Coordinate cref center, dec z = 1.0 );
        
        void render( ) override;
        
    protected:
        
        void ground( TerrainEdge * ) override;
        
    private:
        
        Angle m_offset;
        
        Color m_color;
    };
    
private:
    
    Coordinate m_leaf_base;
    Coordinate m_leaf_side1;
    Coordinate m_leaf_side2;
    
    Countdown m_leaf_drop_countdown1;
    Countdown m_leaf_drop_countdown2;
    
    Color m_leaf_color;
    
};

} // namespace mtmercy

#endif /* AspineTree_hpp */
