#ifndef Angle_hpp
#define Angle_hpp

#include "axn.hpp"
#include "Planc.hpp"
#include "Coordinate.hpp"

namespace axn {
namespace geometry {

class Angle {
public:
    virtual ~Angle( ) {}
    Angle( float radians = 0, bool truncate = true );
    Angle( const planc & dx, const planc & dy, bool truncate = true );
    Angle( const Coordinate & c, bool truncate = true );
    Angle( const Coordinate & from, const Coordinate & to, bool truncate = true );

    bool truncating( ) const;
    Angle & truncate( bool truncate );

    float radians( ) const;
    float degrees( ) const;
    Angle & radians( float radians );
    Angle & radians( const planc & dx, const planc & dy );
    float sin( float multiplier = 1.f ) const;
    float cos( float multiplier = 1.f ) const;

    Quadrant quadrant( ) const;
    Axis axis( ) const;
    bool in_quadrant( Quadrant quadrant ) const;
    bool on_axis( Axis axis ) const;

    Angle & operator=( float radians );

    Angle operator-( ) const;

    Angle operator+( const Angle & angle ) const;
    Angle operator-( const Angle & angle ) const;

    Angle & operator+=( const Angle & angle );
    Angle & operator-=( const Angle & angle );

    Angle operator+( float radians ) const;
    Angle operator-( float radians ) const;
    Angle operator*( float scale ) const;
    Angle operator/( float scale ) const;

    Angle & operator+=( float radians );
    Angle & operator-=( float radians );
    Angle & operator*=( float scale );
    Angle & operator/=( float scale );

    bool operator==( const Angle & angle ) const;
    bool operator!=( const Angle & angle ) const;
    bool operator<( const Angle & angle ) const;
    bool operator<=( const Angle & angle ) const;
    bool operator>( const Angle & angle ) const;
    bool operator>=( const Angle & angle ) const;

    static float radians_to_degrees( float radians );
    static float degrees_to_radians( float degrees );

private:
    bool m_truncate = true; // keeps radians within [0, 2*pi)
    float m_radians = 0.f;
#ifdef AXN_DEBUG
    float m_degrees = 0.f;
#endif
};

const float PI = 3.141592653589793238463f;
const float PI_2 = ( PI * 2.f );
const float PI_1D2 = ( PI / 2.f );
const float PI_3D2 = ( PI / 2.f ) * 3.f;
const float PI_1D3 = ( PI / 3.f );
const float PI_2D3 = ( PI / 3.f ) * 2.f;
const float PI_4D3 = ( PI / 3.f ) * 4.f;
const float PI_5D3 = ( PI / 3.f ) * 5.f;
const float PI_1D4 = ( PI / 4.f );
const float PI_3D4 = ( PI / 4.f ) * 3.f;
const float PI_5D4 = ( PI / 4.f ) * 5.f;
const float PI_7D4 = ( PI / 4.f ) * 7.f;
const float PI_1D8 = ( PI / 8.f );
const float PI_3D8 = ( PI / 8.f ) * 3.f;
const float PI_5D8 = ( PI / 8.f ) * 5.f;
const float PI_7D8 = ( PI / 8.f ) * 7.f;
const float PI_9D8 = ( PI / 8.f ) * 9.f;
const float PI_11D8 = ( PI / 8.f ) * 11.f;
const float PI_13D8 = ( PI / 8.f ) * 13.f;
const float PI_15D8 = ( PI / 8.f ) * 15.f;
const float PI_1D16 = ( PI / 16.f );
const float PI_3D16 = ( PI / 16.f ) * 3.f;
const float PI_5D16 = ( PI / 16.f ) * 5.f;
const float PI_7D16 = ( PI / 16.f ) * 7.f;
const float PI_9D16 = ( PI / 16.f ) * 9.f;
const float PI_11D16 = ( PI / 16.f ) * 11.f;
const float PI_13D16 = ( PI / 16.f ) * 13.f;
const float PI_15D16 = ( PI / 16.f ) * 15.f;
const float PI_17D16 = ( PI / 16.f ) * 17.f;
const float PI_19D16 = ( PI / 16.f ) * 19.f;
const float PI_21D16 = ( PI / 16.f ) * 21.f;
const float PI_23D16 = ( PI / 16.f ) * 23.f;
const float PI_25D16 = ( PI / 16.f ) * 25.f;
const float PI_27D16 = ( PI / 16.f ) * 27.f;
const float PI_29D16 = ( PI / 16.f ) * 29.f;
const float PI_31D16 = ( PI / 16.f ) * 31.f;
const float PI_33D16 = ( PI / 16.f ) * 33.f;
const float PI_35D16 = ( PI / 16.f ) * 35.f;

} // namespace geometry
} // namespace axn

#endif /* Angle_hpp */
