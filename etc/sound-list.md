# Sounds still to make

Places in the game that could use a sound, for whoever makes them. Nothing here exists yet. The six sounds already made, in `res/sounds/`, are `click.wav` (a screenshot), `arrow.wav`, `hook.wav`, `jump.wav`, `hurt.wav`, and `crackle.wav`.

## What a sound has to be

- **Format:** a `.wav` file. Any sample rate and mono or stereo work, since the engine converts them; mono, 44.1 kHz, 16-bit is plenty. MP3 and OGG can't be read.
- **Where:** `res/sounds/<name>.wav`. `res/` isn't in git, so commit with `etc/ult-commit.sh`, which zips it into `res-zipped.zip`.
- **Wiring one in:** name its file once at the top of the `.cpp` that plays it, `var_const( string LAND_SOUND = ( "land.wav" ) );`, then queue it from the object with `play_sound( Sound( LAND_SOUND ) );` (heard by a `Listener` in range, which the player is), or `play_sound( Sound( LAND_SOUND ).global( true ) );` to be heard everywhere (for menus and the like). The world plays everything queued at the end of each pass of the main loop. Volume, pitch, and speaker chain: `play_sound( Sound( LAND_SOUND ).volume( 0.5 ).pitch( Random::rdec( -0.1, 0.1 ) ) );`. Pitch is in octaves (0 as recorded, 1.0 an octave up).
- **Limits of the engine as it stands:**
  - **No looping:** a constant sound (a waterfall, wind) has to be short bursts played over and over, the way `CampFire` queues `crackle.wav` at random.
  - **No panning:** a positioned sound fades with distance from the nearest `Listener` (full volume within its range, fading to nothing across its penumbra), but it comes from both speakers equally, whatever side it's on.
  - **16 at once:** set by `SOUND_CHANNELS`; past that, the sound that's played longest is cut off. Sounds that can come in bursts (many arrows hitting at once) should be short.
- **Variety:** a sound that repeats often (footsteps, ratchets, hits) gets tiring. A small random pitch at play time helps, or two or three versions to pick from at random.

## The player (`src/mt/player/`)

| Sound | When | Where to play it | Character |
|---|---|---|---|
| land | landing on the ground after a fall | `Climber::ground( )`, when it goes from not grounded to grounded; louder for a faster fall | a soft thud with a scuff |
| extra jump | a jump in the air (an extra jump, with its puff of clouds) | `Climber::update( )`, where `add_clouds = true` is set | lighter and airier than `jump.wav` |
| footsteps | walking and running on the ground | `Climber::update( )`, every so many ticks while grounded and moving, faster when dashing | short, soft steps on dirt or stone (a couple of versions) |
| dash | starting a dash | `Climber::dash( )` | a quick burst of movement |
| hook catches | the hook biting into terrain or an object | `Hook::update_velocity( )`, where the state becomes `HOOKED` | a metal clank or thunk |
| hook retracting | the rope being reeled back in | `Hook::retract( )`, when the state becomes `RETRACTING` | a ratcheting reel, distinct from the throw |
| hook stowed | the hook back in the climber's hand | `Hook::update_velocity( )`, where a retracting hook becomes `LOADED` | a small click |
| rope shortening and lengthening | climbing up or down the rope | `Climber::update( )`, where `shorten_rope( )` and `extend_rope( )` are called | a creak or slide, played now and then, not every tick |
| arrow lands | an arrow sticking in the ground | `Projectile::ground( )` (engine side), or an override in `Arrow` | a dull thock into earth |
| arrow hits | an arrow hitting a creature | `Projectile::collide( )`, or an override in `Arrow` | a sharper, fleshier thwack |
| bow reloaded | the crossbow ready to fire again | `Climber::update( )`, when `m_arrow_reload_timer` completes | a small mechanical click |
| death | the climber dying | `Climber::die( )` | a heavier version of `hurt.wav`, or a fall |
| opening a door | going through a door | `Player::collide( )`, just before `door->open( )` | a heavy wooden door or a passage |

## Creatures (`src/mt/mobs/`, `src/mt/environment/critters/`)

| Sound | When | Where to play it | Character |
|---|---|---|---|
| wisp shoots | a wisp firing a bullet | `Wisp::update( )`, where it adds a `Bullet` | a soft whoosh of fire |
| wisp bullet fades | a bullet burning out | `Wisp::Bullet::update( )`, when it's marked to delete | a small fizzle |
| wisp dies | a wisp dying | `Wisp::die( )` | a fire snuffed out |
| bird | a bird flapping or calling | `Bird::update( )`, now and then while on screen | wingbeats or a short call |
| bird dies | a bird dying | `Bird::die( )` | a squawk |
| gopher, porcupine, unalope | each creature's own sounds (moving, attacking, getting hit) | their `update( )`s in `src/mt/mobs/enemies/` | one or two each |
| creature hurt | any creature losing health | `Mob::hurt( )` (engine side); `Climber` already plays `hurt.wav` for itself | a generic hit for anything that isn't the player |
| creature dies | any creature dying | `Mob::die( )` (engine side) | a generic death, which creatures with their own replace |
| snail, butterfly | ambience | `Snail::update( )`, `Butterfly::update( )`, rarely, when on screen | very quiet, or none |

## Hazards and things in the world (`src/mt/hazards/`, `src/mt/environment/`)

| Sound | When | Where to play it | Character |
|---|---|---|---|
| spikes | touching spikes | `Spike::collide( )` | a sharp stab |
| thorns | brushing through thorns | `Thorns::collide( )` | a scratchy rustle |
| sign | reading a sign | `Sign::collide( )`, when `m_reading` turns on | a paper or wood creak |
| water drip | a drop falling from a dripper | `WaterDropper::update( )`, when it drips | a tiny high plip |
| water splash | a drop landing | `WaterDrop::ground( )` | a small splash, varied by pitch |
| waterfall | a waterfall on screen | `Waterfall::update( )`, played over and over while in view | a steady rush (a burst of about a second, played back to back) |
| fire put out | a fire going out | `Fire::extinguish( )` | a hiss |
| leaves | a leaf landing, or a tree's leaves rustling | `Tree::Leaf::ground( )`, `Tree::update( )` | very soft |
| bones | knocking a bone | `Bone::collide( )` | a hollow knock |
| wind | outdoors (the training grounds) | `TrainingGrounds` or `Sky::update( )`, over and over | a low, soft wind |
| cave ambience | in the caves | `Caves`, over and over | low drips and echoes |

## The engine and menus (`src/axn/`)

| Sound | When | Where to play it | Character |
|---|---|---|---|
| pause and unpause | pausing the game | `World::input( )`, where the pause key calls `Engine::pause( )` (not in `Camera.cpp`, where the screenshot shutter pauses the game for a moment) | a soft two-note blip up and down |
| volume changed | pressing `=` or `-` | `World::input( )`, after `Engine::volume_up( )` and `volume_down( )` | a short tick at the new volume, so it can be heard |
| room transition | the camera's circle transition out and in | `World::update( )`, where `camera( )->transition( )` starts, and in `World::finish_loading( )` | a soft swoosh |
| loading done | the loading screen finishing | `World::finish_loading( )` | very quiet, or none |
| splash screen | the logo at startup (Release only) | `SplashScreen` | a short sting |
| screenshot saved | the screenshot written to disk | `Engine::screenshot_eng( )`'s saving thread (careful: it runs off the main thread, so it should only set a flag the main loop plays from) | none needed: `click.wav` already plays when it's taken |
