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

    LightSource( ) { }
    LightSource( Coordinate cref source, Planc cref distance );

    Coordinate cref position( ) const;
    void position( Coordinate cref );

    Planc radius( ) const;
    void radius( dec );

    // dec flicker( ) const;
    // void flicker( dec );

    // Color cref tint( ) const;
    // void tint( Color cref );

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
