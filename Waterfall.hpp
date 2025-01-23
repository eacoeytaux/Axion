#ifndef Waterfall_hpp
#define Waterfall_hpp

#include "MountMerciless.hpp"
#include "Object.hpp"

namespace mtmercy
{

class Waterfall : public Object
{
private:
    struct Foam
    {
        Coordinate position;
        Vector movement;
        Planc radius;
        dec alpha;
    };
    
    struct Ripple
    {
        Polygon polygon;
        Planc height;
        dec alpha;
    };

public:
    Waterfall( Room *, Coordinate cref bottom, Planc width, Planc height, dec z = 1.0 );

    void render( ) override;
    void update( ) override;

private:
    bool m_create_foam = true;
    list<Foam> m_foam;
    list<Ripple> m_ripples;
    Countdown m_ripple_countdown;
};

} // namespace mtmercy

#endif /* Waterfall_hpp */
