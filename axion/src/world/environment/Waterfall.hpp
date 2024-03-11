#ifndef Waterfall_hpp
#define Waterfall_hpp

#include "axn.hpp"
#include "Object.hpp"

namespace axn {
namespace reality {

class Waterfall : public StationaryObject {
private:
    struct Foam {
        Coordinate position;
        Vector movement;
        planc radius;
    };

public:
    virtual ~Waterfall( ) {}
    Waterfall( World*, const Coordinate & bottom, planc width, planc height, ufloat z = 1.f );

    virtual const Waterfall & render( ) const override;
    virtual Waterfall & update( ) override;

private:
    list<Foam> m_foam;
};

} // namespace reality
} // namespace axn

#endif /* Waterfall_hpp */
