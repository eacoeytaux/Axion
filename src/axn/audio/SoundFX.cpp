#include "axn/audio/SoundFX.hpp"

var_const( string SFX_DIR = ( "sounds" ) ); // C! the folder in res/ sound effects are in

SoundFX::SoundFX( string cref in_filename, Planc cref in_range_umbra, Planc cref in_range_penumbra ) : Sound( SFX_DIR, in_filename ), m_range_umbra( in_range_umbra ), m_range_penumbra( in_range_penumbra )
{
    Assert( !is_neg( in_range_umbra ) && !is_neg( in_range_penumbra ) );

    global( is_zero( in_range_umbra ) );
}
