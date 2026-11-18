#ifndef Speaker_hpp
#define Speaker_hpp

#include "axn/AXN.hpp"

namespace axn
{
namespace audio
{

class Speaker
{

private:

    dec m_volume = 1.0;

public:

    Speaker( dec in_volume );

    dec volume( ) const { return( m_volume ); }
    void volume( dec in_volume ) { m_volume = in_volume; }

};

} // namespace audio
} // namespace axn

#endif /* Speaker_hpp */
