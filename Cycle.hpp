#ifndef Cycle_hpp
#define Cycle_hpp

#include "Planc.hpp"
#include "Coordinate.hpp"
#include "Angle.hpp"

namespace axn
{
namespace geometry
{

axnclass( Cycle )
{

private:

    Planc m_wavelength = 1.0;
    Planc m_amplitude = 1.0;

    Angle m_offset = 0.0;

public:

    Cycle( ) { }

    Cycle( Planc cref wavelength, Planc cref amplitude, cAngle offset = A0 ) : m_wavelength( wavelength ), m_amplitude( amplitude ), m_offset( offset ) { }

    Planc at( Planc x ) const { return ( ( m_wavelength && m_amplitude ) ? ( sin( ( x / m_wavelength ) - m_offset.radians( ) ) * m_amplitude ) : P0 ); }

    Planc wavelength( ) const { return ( m_wavelength ); }
    void wavelength( Planc cref wavelength ) { m_wavelength = wavelength; }

    Planc amplitude( ) const { return ( m_amplitude ); }
    void amplitude( Planc cref amplitude ) { m_amplitude = amplitude; }

    Angle offset( ) const { return ( m_offset ); }
    void offset( Angle offset ) { m_offset = offset; }

};

} // namespace geometry
} // namespace axn

#endif /* Cycle_hpp */
