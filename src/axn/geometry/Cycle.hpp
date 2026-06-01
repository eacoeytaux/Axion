#ifndef Cycle_hpp
#define Cycle_hpp

#include "axn/geometry/Planc.hpp"
#include "axn/geometry/Point.hpp"
#include "axn/geometry/Angle.hpp"

namespace axn
{
namespace geometry
{

class Cycle
{

private:

    Planc m_wavelength = 1.0;
    Planc m_amplitude = 1.0;

    Angle m_offset = A0;

public:

    Cycle( ) { }

    Cycle( Planc cref in_wavelength, Planc cref in_amplitude, Angle cref in_offset ) : m_wavelength( in_wavelength ), m_amplitude( in_amplitude ), m_offset( in_offset ) { }
    Cycle( Planc cref in_wavelength, Planc cref in_amplitude ) : m_wavelength( in_wavelength ), m_amplitude( in_amplitude ), m_offset( A0 ) { }
    Cycle( Planc cref in_wavelength, Angle cref in_offset ) : m_wavelength( in_wavelength ), m_amplitude( 1.0 ), m_offset( in_offset ) { }
    Cycle( Planc cref in_wavelength ) : m_wavelength( in_wavelength ), m_amplitude( 1.0 ), m_offset( A0 ) { }

    Planc at( ticks in_t ) const { return( ( m_wavelength && m_amplitude ) ? ( sin( ( in_t / m_wavelength ) - m_offset.radians( ) ) * m_amplitude ) : P0 ); }

    Planc wavelength( ) const { return( m_wavelength ); }
    void wavelength( Planc cref in_wavelength ) { m_wavelength = in_wavelength; }

    Planc amplitude( ) const { return( m_amplitude ); }
    void amplitude( Planc cref in_amplitude ) { m_amplitude = in_amplitude; }

    Angle offset( ) const { return( m_offset ); }
    void offset( Angle cref in_offset ) { m_offset = in_offset; }

};

} // namespace geometry
} // namespace axn

#endif /* Cycle_hpp */
