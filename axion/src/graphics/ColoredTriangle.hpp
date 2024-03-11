#ifndef ColoredTriangle_hpp
#define ColoredTriangle_hpp

#include "axn.hpp"

#include "Geometry.hpp"
#include "Color.hpp"

namespace axn {
namespace graphics {

class ColoredTriangle {
public:
    Shape triangle;
    Color color1;
    Color color2;
    Color color3;

    virtual ~ColoredTriangle( ) {}
    ColoredTriangle( ) : triangle( Shape( ) ) {}
    ColoredTriangle( const Shape & t, const Color & c1, const Color & c2, const Color & c3 )
        : triangle( t ), color1( c1 ), color2( c2 ), color3( c3 ) {}
    ColoredTriangle( const Shape & t, const Color & c )
        : triangle( t ), color1( c ), color2( c ), color3( c ) {}
};

} // namespace graphics
} // namespace axn

#endif /* ColoredTriangle_hpp */
