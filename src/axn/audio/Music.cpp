#include "axn/audio/Music.hpp"

var_const( string MUSIC_DIR = ( "music" ) ); // C! the folder in res/ music is in

Music::Music( string cref in_filename ) : Sound( MUSIC_DIR, in_filename ) { loop( true ); }
