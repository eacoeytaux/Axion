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
    Fire( World *, const Coordinate & position );

    virtual const Fire & render( ) const override;
    virtual Fire & update( ) override;

private:
    list<Flame> m_flames_yellow;
    list<Flame> m_flames_orange;
    Counter flame_timer;
};

} // namespace mtmercy

#endif /* Fire_hpp */
