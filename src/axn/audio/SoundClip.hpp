#ifndef SoundClip_hpp
#define SoundClip_hpp

#include "axn/AXN.hpp"

namespace axn
{
namespace audio
{

class SoundClip
{

private:

    string m_load_name;

    dec m_volume = 1.0;

public:

    SoundClip( string cref in_load_name, dec in_volume = 1.0 );

    string load_name( ) const { return( m_load_name ); }

    dec volume( ) const { return( m_volume ); }

    bool play( dec in_volume = 1.0 ) const;

};

} // namespace axn
} // namespace audio

#endif /* SoundClip_hpp */
