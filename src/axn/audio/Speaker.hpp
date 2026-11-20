#ifndef Speaker_hpp
#define Speaker_hpp

#include "axn/AXN.hpp"

namespace axn
{
namespace audio
{

class SoundFX;

class Speaker
{

private:

    declmp m_volume = 1.0;

public:

    Speaker( declmp in_volume = 1.0 ) : m_volume( in_volume ) { }

    declmp volume( ) const { return( m_volume ); }
    Speaker ref volume( declmp in_volume ) { m_volume = in_volume; rethis; }

    void play( SoundFX cref in_sound, declmp in_heard_volume = 1.0 ) const; // C! in_sound at its own volume times this speaker's, times how loudly it's heard

};

} // namespace audio
} // namespace axn

#endif /* Speaker_hpp */
