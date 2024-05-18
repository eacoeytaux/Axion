// #include "SoundClip.hpp"
//
// #include "SDL2.hpp"
// #include "SYSAudio.hpp"
//
// using axn::audio::SoundClip;
//
// SoundClip::SoundClip( const string _load_name ) {
//     //Assert(Mix_LoadWAV( ( base_location + _load_name ).c_str( ) ),
//     Mix_GetError() );
// }
//
// bool SoundClip::play( ) const {
//     if ( _volume <= 0 ) return true;
//     if ( SYSAudio::muted() ) return true;
//     bool success = true;
//     //dec volume = 1.0;
//     //int global_volume = MIX_MAX_VOLUME * SYSAudio::volume( ) * volume;
//     //success = !Mix_Volume( -1, global_volume ) && success;
//     //success = !Mix_PlayChannel( -1, Mix_LoadWAV( ( base_location + _load_name ).c_str( ) ), 0 ) && success;
//     //Assert( success, Mix_GetError( ) );
//     return success;
// }
