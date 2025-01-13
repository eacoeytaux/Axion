#ifndef Fire_hpp
#define Fire_hpp

#include "Object.hpp"

namespace mtmercy
{

class Fire : public Object
{
private:
    struct Flame
    {
        Coordinate offset;
        Planc radius;
    };

public:
    virtual ~Fire( ) { }
    Fire( World *, Coordinate cref position );

    virtual void render( ) override;

    virtual void update( ) override;

private:
    list<Flame> m_flames_yellow;
    list<Flame> m_flames_orange;
    Counter flame_timer;
};

} // namespace mtmercy

#endif /* Fire_hpp */
