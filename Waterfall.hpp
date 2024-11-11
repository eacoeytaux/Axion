#ifndef Waterfall_hpp
#define Waterfall_hpp

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

public:
    virtual ~Waterfall( ) { }
    Waterfall( World *, const Coordinate & bottom, Planc width, Planc height, dec z = 1.0 );

    void render( ) override;
    void update( ) override;

private:
    list<Foam> m_foam;
};

} // namespace mtmercy

#endif /* Waterfall_hpp */
