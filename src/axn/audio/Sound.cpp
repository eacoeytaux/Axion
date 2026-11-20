#include "axn/audio/Sound.hpp"

#include "axn/system/Engine.hpp"

umap<string, varray<dec>> Sound::soundbank = umap<string, varray<dec>>( );

Sound::Sound( string cref in_folder, string cref in_filename ) : m_filename( in_folder + "/" + in_filename )
{
    if( !soundbank.contains( filename( ) ) ) { soundbank.insert( filename( ), Engine::load_sound( filename( ) ) ); }

    m_samples = &soundbank[ filename( ) ];
}
