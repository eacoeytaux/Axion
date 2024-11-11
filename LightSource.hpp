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
    void position( const Coordinate & );

    Planc radius( ) const;
    void radius( dec );

    // dec flicker( ) const;
    // void flicker( dec );

    // const Color & tint( ) const;
    // void tint( const Color & );

    default_equal( LightSource );

private:
    Coordinate m_position;
    Planc m_radius;

    // dec m_flicker;
    // color m_tint;
};

} // namespace graphics
} // namespace axn

#endif /* LightSource_hpp */
