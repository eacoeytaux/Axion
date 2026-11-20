#include "axn/audio/Speaker.hpp"

#include "axn/audio/SoundFX.hpp"
#include "axn/system/Engine.hpp"

void Speaker::play( SoundFX cref in_sound, declmp in_heard_volume ) const { Engine::play_sound( SoundFX( in_sound ).volume( in_sound.volume( ) * volume( ) ), in_heard_volume ); }
