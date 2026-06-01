#include "axn/audio/SoundClip.hpp"

var_const( string base_location = ( "/" ) );

SoundClip::SoundClip( string cref in_load_name, dec in_volume ) : m_load_name( in_load_name ), m_volume( in_volume )
{
    // Assert( Mix_LoadWAV( ( base_location + in_load_name ).c_str( ) ), Mix_GetError( ) );
}

bool SoundClip::play( dec in_volume ) const
{
    return_true_if( !is_pos( m_volume ) || !is_pos( in_volume ) );

    bool success = true;

    // dec volume = 1.0;
    // int global_volume = SYSAudio::volume( ) * volume;
    // success = !Mix_Volume( -1, global_volume ) && success;
    // success = !Mix_PlayChannel( -1, Mix_LoadWAV( ( base_location + m_load_name ).c_str( ) ), 0 ) && success;
    // Assert( success, Mix_GetError( ) );

    return( success );
}
