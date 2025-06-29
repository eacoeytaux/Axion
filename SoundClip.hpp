#ifndef SoundClip_hpp
#define SoundClip_hpp

#include "AXN.hpp"

namespace axn
{
namespace audio
{

class SoundClip
{

public:

    SoundClip( string load_name );

    bool play( ) const;

};

} // namespace axn
} // namespace audio

#endif /* SoundClip_hpp */
