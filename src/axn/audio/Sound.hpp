#ifndef Sound_hpp
#define Sound_hpp

#include "axn/audio/Speaker.hpp"
#include "axn/geometry/Geometry.hpp"

namespace axn
{

class Engine;

namespace audio
{

// C! what every kind of sound has: a file (in res/, its folder given by the kind: SoundFX in sounds/, Music in music/), its samples, a
// C! volume, and whether it loops. Each file is loaded once, the first time any sound uses it, into a table every sound shares, so making
// C! one each time it plays is cheap. Only its kinds are made (SoundFX and Music), and they decide whether it can loop
class Sound
{

private:

    static umap<string, varray<dec>> soundbank; // C! each file's samples by name, loaded once (by Engine::load_sound( )), kept empty when it couldn't be, so it isn't tried every time

protected:

    Sound( string cref in_folder, string cref in_filename ); // C! in_filename is in res/in_folder/

    Sound ref loop( bool in_loop ) { m_loop = in_loop; rethis; }

public:

    string cref filename( ) const { return( m_filename ); }

    ptr<const varray<dec>> samples( ) const { return( m_samples ); }

    bool loaded( ) const { return( !is_null( m_samples ) && !m_samples->empty( ) ); } // C! false when its file couldn't be loaded, so there's nothing to play

    declmp volume( ) const { return( m_volume ); }
    Sound ref volume( declmp in_volume ) { m_volume = in_volume; rethis; }

    bool loop( ) const { return( m_loop ); } // C! starts again from the beginning when it ends

private:

    string m_filename;

    ptr<const varray<dec>> m_samples; // C! in soundbank, empty when the file couldn't be loaded

    declmp m_volume = 1.0;
    bool m_loop = false;

};

// C! one sound playing (a SoundFX or Music): the sound, how loudly it's heard, and how far through it is
class Voice final
{

public:

    Voice( Sound cref in_sound, declmp in_volume = 1.0 ) : m_sound( in_sound ), m_volume( in_volume ) { }

    Sound cref sound( ) const { return( m_sound ); }

    ptr<const varray<dec>> samples( ) const { return( sound( ).samples( ) ); } // C! left and right in turn

    ticks at( ) const { return( m_at ); } // C! how far through it is, in frames (a left and a right sample each), between frames when it's pitched
    Voice ref at( ticks in_at ) { m_at = in_at; rethis; }

    ticks step( ) const { return( m_step ); } // C! frames moved on for each frame played: 2 is an octave up, 0.5 an octave down
    Voice ref step( ticks in_step ) { m_step = in_step; rethis; }

    declmp heard_volume( ) const { return( m_volume ); } // C! how loudly it's heard, without the sound's own volume (the volume HUD counts this)
    declmp volume( ) const { return( m_volume * sound( ).volume( ) ); } // C! the sound's own volume, times how loudly it's heard

private:

    Sound m_sound;

    declmp m_volume = 1.0;

    ticks m_at = 0.0;
    ticks m_step = 1.0;

};

} // namespace audio
} // namespace axn

#endif /* Sound_hpp */
