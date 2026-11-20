#ifndef Music_hpp
#define Music_hpp

#include "axn/audio/Sound.hpp"

namespace axn
{
namespace audio
{

// C! music (in res/music/): played by Engine::play_music( ) straight away, always global, on a channel of its own (sounds never cut it off,
// C! and it isn't counted by the volume HUD), one at a time. It loops unless it's set not to
class Music final : public Sound
{

public:

    Music( string cref in_filename ); // C! in_filename is in res/music/

    declmp volume( ) const { return( Sound::volume( ) ); } // C! both here, so the setter can give back the Music for chaining
    Music ref volume( declmp in_volume ) { Sound::volume( in_volume ); rethis; }

    bool loop( ) const { return( Sound::loop( ) ); } // C! it loops unless it's set not to
    Music ref loop( bool in_loop ) { Sound::loop( in_loop ); rethis; }

};

} // namespace audio
} // namespace axn

#endif /* Music_hpp */
