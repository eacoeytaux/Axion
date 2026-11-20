#ifndef SoundFX_hpp
#define SoundFX_hpp

#include "axn/audio/Sound.hpp"

namespace axn
{
namespace audio
{

// C! a sound effect (in res/sounds/): queued by an object and played when a Listener hears it (see World::play_sound( )), or everywhere
// C! when it's global( ). It never loops. The setters chain: SoundFX( "boop.wav" ).volume( 0.5 ).pitch( -1.0 )
class SoundFX final : public Sound
{

public:

    SoundFX( string cref in_filename, Planc cref in_range_umbra = P0, Planc cref in_range_penumbra = P0 ); // C! no umbra: global

    declmp volume( ) const { return( Sound::volume( ) ); } // C! both here, so the setter can give back the SoundFX for chaining
    SoundFX ref volume( declmp in_volume ) { Sound::volume( in_volume ); rethis; }

    bool global( ) const { return( m_global ); }
    SoundFX ref global( bool in_global ) { m_global = in_global; rethis; }

    dec speed( ) const { return( m_speed ); }
    SoundFX ref speed( dec in_speed ) { m_speed = in_speed; rethis; }

    dec pitch( ) const { return( m_pitch ); }
    SoundFX ref pitch( dec in_pitch ) { m_pitch = in_pitch; rethis; }

    Planc cref hearing_umbra( ) const { return( m_range_umbra ); }
    SoundFX ref hearing_umbra( Planc cref in_hearing_umbra ) { m_range_umbra = in_hearing_umbra; rethis; }

    Planc cref hearing_penumbra( ) const { return( m_range_penumbra ); }
    SoundFX ref hearing_penumbra( Planc cref in_hearing_penumbra ) { m_range_penumbra = in_hearing_penumbra; rethis; }

    Planc hearing_range( ) const { return( hearing_umbra( ) + hearing_penumbra( ) ); }

    // C! how loudly it reaches something in_distance away: fully within its umbra, fading to nothing across its penumbra, everywhere when it's global( )
    declmp reach( Planc cref in_distance ) const
    {
        return_if( ( global( ) || ( in_distance <= hearing_umbra( ) ) ), 1.0 );
        return_if( ( in_distance >= hearing_range( ) ) || !is_pos( hearing_penumbra( ) ), 0.0 );
        return( declmp( one_minus( ( in_distance - hearing_umbra( ) ) / hearing_penumbra( ) ) ) );
    }

private:

    dec m_speed = 1.0; // C! a multiplier on how fast it plays: 1 as recorded, 2 twice as fast
    dec m_pitch = 0.0;

    Planc m_range_umbra;
    Planc m_range_penumbra;

    bool m_global = false;

};

} // namespace audio
} // namespace axn

#endif /* SoundFX_hpp */
