#ifndef Waterfall_hpp
#define Waterfall_hpp

#include "mt/MountMerciless.hpp"
#include "axn/world/Object.hpp"

namespace mtmercy
{

class Waterfall : public Object
{

private:

    struct Foam
    {

        Point position;
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

    Waterfall( ptr<Room> in_room, Point cref in_bottom, Planc cref in_width, Planc cref in_height, dec in_z = 1.0 );

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
