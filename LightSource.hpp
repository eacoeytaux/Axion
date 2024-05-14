#ifndef LightSource_hpp
#define LightSource_hpp

#include "Color.hpp"
#include "Geometry.hpp"

namespace axn
{
namespace graphics
{

class LightSource
{
public:
    virtual ~LightSource( ) { }

    LightSource( ) { }
    LightSource( const Coordinate & source, const Planc & distance );

    const Coordinate & position( ) const;
    LightSource & position( const Coordinate & );

    Planc radius( ) const;
    LightSource & radius( double );

    // double flicker( ) const;
    // LightSource & flicker( double );

    // const Color & tint( ) const;
    // LightSource & tint( const Color & );

    bool operator==( const LightSource & ) const;
    bool operator!=( const LightSource & ) const;

private:
    Coordinate m_position;
    Planc m_radius;

    // double m_flicker;
    // color m_tint;
};

} // namespace graphics
} // namespace axn

#endif /* LightSource_hpp */
