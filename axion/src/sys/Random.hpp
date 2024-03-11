#ifndef Random_hpp
#define Random_hpp

#include "Primitives.hpp"
#include "Error.hpp"

namespace axn {

namespace geometry {
class Angle;
}
namespace graphics {
class Color;
}

namespace system {

class Random {
private:
    Random( ) {}

public:
    static err seed( );
    static err seed( uint seed );
    static err seed( string seed );
    static bool rng_bool( ufloat true_odds = 0.5f );
    static int rng_int( int high_bound, int low_bound = 0 );
    static float rng_float( float high_bound, float low_bound = 0 );
    static geometry::Angle r_Angle( );
    static graphics::Color rng_Color( bool include_alpha = false );
};

} // namespace system
} // namespace axn

#endif /* Random_hpp */
