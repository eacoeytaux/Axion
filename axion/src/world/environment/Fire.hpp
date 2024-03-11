#ifndef Fire_hpp
#define Fire_hpp

#include "axn.hpp"
#include "Object.hpp"

namespace axn {
namespace reality {

class Fire : public Object {
private:
    struct Flame {
    public:
        Flame( const planc & radius, const Coordinate & offset = ORIGIN );
        Coordinate offset;
        planc radius;
    };

public:
    virtual ~Fire( ) {}
    Fire( World*, const Coordinate & position );

    virtual const Fire & render( ) const override;
    virtual Fire & update( ) override;

private:
    list<Flame> m_flames_yellow;
    list<Flame> m_flames_orange;
    Counter flame_timer;
};

} // namespace reality
} // namespace axn

#endif /* Fire_hpp */
