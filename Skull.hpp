#ifndef Skull_hpp
#define Skull_hpp

#include "Object.hpp"

namespace mtmercy
{

class Skull : public Object
{
public:
    virtual ~Skull( ) { }
    Skull( World *, const Coordinate & position );

    virtual void render( ) override;

    virtual void update( ) override;

private:
    Drawing m_skull_drawing;
};

} // namespace mtmercy

#endif /* Skull_hpp */
