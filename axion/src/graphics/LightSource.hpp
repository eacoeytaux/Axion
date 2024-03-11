#ifndef LightSource_hpp
#define LightSource_hpp

#include "axn.hpp"
#include "Geometry.hpp"
#include "Color.hpp"

namespace axn {
namespace graphics {

class LightSource {
public:
    LightSource( const Coordinate & source, const planc & distance,
                 const Color & tint = Color( WHITE ).a( 0.f ), ufloat flicker = 0.f );

    const Coordinate & position( ) const;
    LightSource & position( const Coordinate & );
    planc distance( ) const;
    LightSource & distance( ufloat );
    ufloat flicker( ) const;
    LightSource & flicker( ufloat );
    const Color & tint( ) const;
    LightSource & tint( const Color & );

    bool operator==( const LightSource & ) const;
    bool operator!=( const LightSource & ) const;

private:
    Coordinate m_position;
    planc m_distance;
    ufloat m_flicker; // todo: add frame delay
    Color m_tint;
};

} // namespace graphics
} // namespace axn

#endif /* LightSource_hpp */
