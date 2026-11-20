# Letting Claude play Axion: input injection and seeing the game

Investigation only, no code has been changed. This covers how Claude could send keyboard, mouse, and controller input to the engine, how it could see what's happening, and the changes needed.

## Summary

- **Input:** add an `Automation` module that reads commands from a file and turns them into the same `Input` objects SDL produces (`KeyInput`, `MouseInput`, `ControllerButtonInput`, `ControllerJoystickInput`). They're appended to the frame's input list in `Engine::run`, so `World::input`, `Room::input`, and `Climber::input` handle them like real input. Nothing in game code changes.
- **Seeing:** a **text dump** of what the camera sees (decided: text is fine instead of an image). It lists the player, every object in view, nearby terrain as line segments, and an optional coarse character map for layout. This is simpler than screenshots (no OpenGL readback, no PNG library) and gives exact numbers. Screenshots stay a possible later addition, written up at the end.
- **Timing:** Claude takes seconds per turn and the game runs at 60 FPS, so the engine runs in **lockstep** (decided: holding the game for input is fine). It waits until Claude says "run N frames", runs exactly that many with the current held inputs, then waits again.
- **All of it is inside `#if defined( AXN_AUTOMATION )` checks** (decided). That includes the `Automation` module's contents, every call in `Engine.cpp`, the dump hooks in `World`, `Object`, and the game, and the argument parsing in `main.cpp`. Without the macro, the build is exactly what it is today. With it, the feature is still switched on at runtime with a `--automation <dir>` argument.

## How input works today

1. `Engine::run` (`src/axn/system/Engine.cpp:56`) runs one loop per frame: `clear_inputs( )`, `input_eng( inputs )`, `in_world->input( inputs )`, update (unless paused), `in_world->render( )`, `render_eng( )`, then waits out the rest of the 1/60 s.
2. `Engine::input_eng` (`src/axn/system/EngineSDL.cpp:223`) is the **only** place input enters. It drains `SDL_PollEvent` into a `list<ptr<Input>>`:
   - keys become `KeyInput( char( keysym.sym ), PRESSED / RELEASED )`. Uppercase is folded to lowercase. Modifiers end up as negative chars (shift is `-27`, `-30`, or `-31`, ctrl `-32`), because SDL's keycodes are truncated to `char`. SDL key repeat sends extra `PRESSED` events; nothing ever produces `HELD`.
   - the mouse becomes `MouseInput` with `MOVE` (screen pixels), button `PRESSED` / `RELEASED`, and scroll.
   - PS4 controller buttons and triggers become `ControllerButtonInput`; sticks become `ControllerJoystickInput` (only sent when the value changes, with a dead zone of 0.25).
3. `World::input` (`src/axn/world/World.cpp:126`) handles global keys (esc quits, `p` pauses, `'` steps, debug toggles `1`–`0`, and others), then passes the whole list to `Room::input`, which hands each input to every input subscriber, such as `Climber::input` (`src/mt/player/Climber.cpp:427`).
4. `Climber` reads `w` / `a` / `s` / `d`, space, `q` / `e`, and the mouse: it aims at `camera( )->cursor_world_position( )`, which comes from the **screen-pixel** mouse position.

The input layer is already abstracted away from SDL. Injecting `Input` objects next to the SDL ones is enough.

### Gotchas found

- **The existing pause/step can't be used for lockstep.** `Engine::run` calls `in_world->pause( paused( ) )` every frame, and `World::pause( true )` calls `clear_input( )` on every player. `Room::input` also ignores input while `Engine::paused( )`. So a key held through a paused step is dropped. Lockstep needs its own gate that holds the loop without pausing the world.
- `World::paused( )` is also true while the camera is transitioning between rooms, and input is dropped then. The dump reports it, so Claude knows why a key did nothing.
- `esc` quits, and `;` with shift reseeds `Random` from the clock. Otherwise the run is deterministic: `Random::seed( 0 )` at start.
- **Aiming is in screen pixels**, and the window fills the desktop. Claude thinks in world coordinates from the dump, so add a `mouse world x y` command that converts with `Camera::world_to_screen( )` (`src/axn/graphics/Camera.hpp:120`) before making the `MouseInput`. Then window size doesn't matter.
- Real input still arrives while Claude is driving. That's useful (you can take over), but an `ignore_real_input` option should exist.

## Proposed input injection

### New module `src/axn/system/Automation.hpp` / `.cpp` (namespace `axn`)

```cpp
class Automation
{

private:

    Automation( ) { }

public:

    static Error init( string cref in_directory );
    static bool active( );

    static void input( list<ptr<Input>> ref in_inputs ); // C! appends the scripted inputs for this frame
    static void wait_for_frame( );                       // C! blocks until a run command allows a frame

    static void frame_done( ptr<World> in_world );       // C! writes status and any requested dump, acks commands

};
```

### Command channel: files in a folder

A plain-file mailbox works the same on Windows and OSX, needs no networking library (SDL_net isn't linked), and Claude can drive it with the Write and Bash tools it already has.

- `<dir>/commands.txt`: Claude appends lines. The engine remembers how far it has read and handles new lines at the start of each frame.
- `<dir>/status.txt`: the engine rewrites it every frame (written to a temporary file, then renamed, so it's never read half written): the frame number, the last finished command id, whether it's waiting, and whether the world is paused or transitioning.
- `<dir>/state.txt`: the dump (see below).

Each command line is `<id> <verb> <args>`:

```
1 key d down              # C! held until released
2 run 30                  # C! run 30 frames, then wait
3 key space down
4 run 1
5 key space up
6 mouse world 900.0 -110.0 # C! aim at a world point, converted to screen pixels
7 mouse left down
8 run 20
9 mouse left up
10 pad R1 press           # C! press = down this frame, up next frame
11 stick left 1.0 0.0
12 state                  # C! write state.txt after the next frame
13 state every 10         # C! or keep it updated every 10 frames
```

Key names map to the same chars SDL produces (`space` is `' '`, `shift` is `-31`, `ctrl` is `-32`, `esc` is `27`, `tab` is `9`), so `World::input` and `Climber::input` need no changes. Controller button names map to the `ControllerButtonInput::Button` values.

### Engine loop changes (`Engine.cpp`)

```cpp
                cycle_count++;

                #if defined( AXN_AUTOMATION )
                Automation::wait_for_frame( ); // C! holds here, still pumping SDL events, until a run command allows a frame
                #endif

                clear_inputs( );
                input_eng( inputs );

                #if defined( AXN_AUTOMATION )
                Automation::input( inputs ); // C! scripted inputs go after the real ones, so world and room handle them the same way
                #endif
```

- `wait_for_frame( )` keeps calling `SDL_PumpEvents( )` and sleeps a few milliseconds between checks of `commands.txt`. Otherwise Windows marks the window "Not responding" after about 5 seconds.
- While a run is going, skip the end-of-frame `wait_eng( )`, so `run 600` finishes as fast as the machine allows.
- After `render_eng( )`, call `Automation::frame_done( in_world )`.
- Optional: save the existing Debug `logged_inputs` to a file at quit. With a loader, a session Claude played can be replayed exactly, which pairs well with `TestsMovement.hpp` for reproducing bugs.

## Proposed text dump (`state.txt`)

What's in view is decided the same way `Room` decides what to draw (`src/axn/world/Room.cpp:79`): `camera->in_view( object->bounding_box( ) + object->position( ), object->z( ) )`. The dump walks the current room's objects with that test and asks `room( )->terrain_in_range( )` for the terrain inside the camera's view.

Example:

```
frame 1234
room TrainingGrounds
paused 0 transitioning 0
camera center 800.0 -90.0 zoom 1.0 view 480.0 -360.0 to 1120.0 180.0

player Climber id 12 position 812.4 -103.0 velocity 3.2 0.0 grounded 1 health 0.8 alive 1 facing right aim 15.0
player hook none arrow ready

objects in view
  Gopher id 31 position 900.1 -110.0 velocity -1.0 0.0 grounded 1 health 1.0 state WALKING
  Spikes id 40 position 950.0 -120.0 bounds 940.0 -125.0 to 960.0 -115.0
  Door id 7 position 1100.0 -100.0

terrain in view
  edge 480.0 -120.0 to 700.0 -118.0
  edge 700.0 -118.0 to 760.0 -60.0
  ...

map 80x24 (one char = 8.0 x 22.5, @ player, # terrain, G Gopher, ^ Spikes, D door)
................................................................................
...........................................@....G.......^.................D....
################################################################################
```

### Hooks

- `World::automation_state( string ref )`: virtual, writes the frame, room, pause, and camera lines. `MtMerciless` doesn't need to override it unless there's game-wide state worth adding.
- `Object::automation_state( string ref ) const`: virtual. The base writes the type name, `id( )`, `position( )`, `velocity( )`, `grounded( )`, and bounds. Subclasses override it to add what matters: `Mob` adds health and state, and `Climber` adds aim, facing, hook, and arrow.
  - The type name needs a portable source. `typeid( ).name( )` gives `class mtmercy::Gopher` on MSVC and a mangled name on clang, so either add a short name per class or strip both forms in one helper.
  - It's a new `Object` virtual, so `CLAUDE.md`'s "overrides follow `Object.hpp`'s order" list gains an entry. Decide where it sits (after `debug_overlay` makes sense, since both are debug-ish).
- The character map is optional but cheap: rasterize terrain edges and object centers into a grid over the camera's view. It shows layout (gaps, ledges, walls) at a glance, which the coordinate lists don't.

## How Claude would drive it

1. Build Debug with `AXN_AUTOMATION` defined.
2. Launch the exe in the background with `--automation <dir>` (Bash `run_in_background`). `main.cpp` currently ignores `argv`, so it needs to pass this to `Engine::run` or `Settings`.
3. Each turn: append commands to `commands.txt`, then run a small helper (`etc/automation/axn-wait.sh <id>`) that polls `status.txt` until that command id has finished, then Read `state.txt`.
4. Later, if wanted: wrap the same file protocol in a local MCP server exposing tools like `press`, `run_frames`, and `state`, so each action is one tool call instead of a write plus a wait.

Other approaches I looked at and wouldn't use:

- **OS-level input (Windows `SendInput`, OSX `CGEvent`, or desktop computer use):** needs window focus, is real-time only, and can't fake a PS4 controller.
- **`SDL_PushEvent`:** goes through `input_eng` unchanged, but joystick events need an opened device and the char-casting is redone. Creating `Input` objects directly is simpler.

## Change list

| File | Change |
|---|---|
| `src/axn/system/Automation.hpp` / `.cpp` (new) | Command parsing, held-input state, the lockstep gate, the status and state files, the character map |
| `src/axn/system/Engine.cpp` | `wait_for_frame( )`, `input( )`, and `frame_done( )` calls; skip the frame wait during a run |
| `src/axn/system/Debug.hpp` | `AXN_AUTOMATION` define, next to `AXN_DEBUG_CONTROLS` (or in the project's preprocessor settings, see open questions); every other change in this table sits inside `#if defined( AXN_AUTOMATION )` |
| `src/main.cpp` | Parse `--automation <dir>` |
| `src/axn/world/World.hpp` / `.cpp` | Virtual `automation_state( )` |
| `src/axn/world/Object.hpp` / `.cpp`, `Mob`, `src/mt/player/Climber.*` | Virtual `automation_state( )` and overrides |
| `.claude/CLAUDE.md` | Add `automation_state` to the `Object.hpp` override order list |
| `Axion.vcxproj` / `.filters`, Xcode project | Add the new files |
| `etc/automation/axn-wait.sh` (new) | Wait for a command id in `status.txt` |
| `.gitignore` | The automation output folder |

All new code would follow `CLAUDE.md`'s style, with `C!` on every comment.

## Later, if wanted: screenshots

Not needed now, kept here since it was looked into.

- Rendering is OpenGL through SDL2, with an 8x multisampled default framebuffer and no framebuffer objects. `Engine::render_eng` (`src/axn/system/EngineSDL.cpp:148`) is `SDL_GL_SwapWindow( WINDOW )` then `ogl::clear( )`.
- After the swap the back buffer is undefined, so a capture would call a new `ogl::read_pixels( )` (`glReadPixels` on `GL_BACK`) just before the swap. Reading a multisampled default framebuffer returns resolved pixels, so no extra step is needed.
- The pixels would be flipped (GL is bottom-up), scaled down to about 960 px wide, and written as a PNG with `stb_image_write.h` added to `lib/`. Claude's Read tool shows PNG and JPG.
- It costs a GPU stall of a few milliseconds, so only on request.

## Open questions

- Where should `AXN_AUTOMATION` be defined: in `Debug.hpp` (commented out until wanted), or only in a separate build configuration's preprocessor settings in `Axion.vcxproj` and the Xcode project?
- Where should `automation_state` go in the `Object.hpp` override order?
- Should real input be ignored while automation is active, or mixed in so you can take over?
- Is an MCP server wanted eventually, or are the file protocol and a script enough?
