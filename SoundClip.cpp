#include "SoundClip.hpp"

using axn::audio::SoundClip;

SoundClip::SoundClip( const string _load_name )
{
    // Assert( Mix_LoadWAV( ( base_location + _load_name ).c_str( ) ), Mix_GetError( ) );
}

bool SoundClip::play( ) const
{
    //if( m_volume <= 0 ) return true;

    bool success = true;

    // dec volume = 1.0;
    // int global_volume = SYSAudio::volume( ) * volume;
    // success = !Mix_Volume( -1, global_volume ) && success;
    // success = !Mix_PlayChannel( -1, Mix_LoadWAV( ( base_location + _load_name ).c_str( ) ), 0 ) && success;
    // Assert( success, Mix_GetError( ) );

    return success;
}
