# Threads in the game loop: what could run where

Investigation only, except for the loading screen test (`LoadingScreen`), which is built and runs before the game for `LOADING_SCREEN_TEST_SECONDS` (a dev param in `Engine.cpp`, 0 turns it off).

## How a frame runs now (`Engine::run`)

All on the main thread, 60 times a second:

1. `input_eng( )`: SDL events become `Input`s.
2. `World::input( )`, then `World::update( )` → `Room::update( )`: every object's `update( )`, including movement and collisions.
3. `Camera::update( )`: screen effects (the shutter).
4. `World::render( )`:
   - `Room::render( )` redraws each object's `Drawing` (CPU only, no OpenGL), skipping those out of range.
   - `Camera::render( )` sorts what's in view and sends every polygon to OpenGL.
5. `render_eng( )`: the screenshot read, if one was asked for, then the buffer swap.

The one thing already off the main thread is writing a screenshot's file.

## What has to stay on the main thread

- **OpenGL:** the context belongs to the thread that made it, so `Camera::render( )`'s drawing calls and `SDL_GL_SwapWindow( )` stay where they are.
- **SDL events and the window:** on OSX these have to be on the main thread.
- **Anything that changes a room's lists** (`add_object( )`, `remove_object( )`, the grid) while something else might read them.

## Options, from most to least worthwhile

### 1. Loading on its own thread (tested now)

`LoadingScreen` takes a load function, runs it on its own thread, and keeps the normal loop going with a spinner until the load is done. The test's load just waits.

To load something real, like a world's rooms, the load can't touch anything the loading screen is using at the same time:

| Problem | Where it is now | What it needs |
|---|---|---|
| Objects reach for the camera while they're built | `Climber`'s constructor adds its HUD to the camera; `Room::add_player( )` targets it | Do that when the world goes live, not when it's built (a `World::start( )` step on the main thread) |
| `Random` is one shared sequence | `Random::` everywhere in object constructors and room creation | Keep the loading screen from using `Random` (it doesn't now), or give the load its own generator |
| Static caches aren't thread-safe | `Polygon::equilateral( )`'s cache (used by every `Polygon::circle( )`), `circle_points( )` in `Camera.cpp` | Lock them, or make them per thread |
| `Log( )` isn't thread-safe | Logger writes to one file | A mutex in `Logger::log_message( )` |
| The world is made by `Engine::run` | `run_world( new MtMerciless( ) )` builds and runs it straight away | Build it in the load, hand it over when done |

The camera row needs the most care. The other rows are small, self-contained fixes.

### 2. Redrawing objects in parallel

Each object's `render( )` only builds its own `Drawing` (`CLAUDE.md` now requires that), so in principle the redraws in `Room::render( )` can be split across threads with a small thread pool. Each object's bounding box can be worked out on the same threads.

- **What makes it unsafe today:** the static caches above, used by `Polygon::circle( )` in almost every `render( )`. Those have to be thread-safe first.
- **What it gains:** Room's redraw step divided by the number of cores. Whether that's worth it depends on how much of a frame that step takes, which nobody has measured yet (see "Measure first").
- **Needs:** a `ThreadPool` in `axn/system` with a `parallel_for( count, function )`. Its number of threads would be a dev param (default: cores − 1, since the main thread works too).

### 3. Updating while the last frame is drawn

The main thread sends frame N to OpenGL while another thread updates frame N + 1. This needs a copy of everything the camera draws, taken at the end of each update, so the update can carry on changing the objects. It gains at most the shorter of update and render, but it touches most of the engine and adds a frame of delay to input.

### 4. Updating objects in parallel (not recommended)

Movement reads other objects' positions and the room's grid while other objects change them, and collisions change both objects. It would need updates split into a "work out" phase and an "apply" phase. That's a large redesign, and order-dependent behaviour (who collides first) would change.

### 5. Audio

SDL already plays audio on its own thread through a callback. The audio work just needs the queue of sounds to play to be thread-safe (a mutex, or a lock-free queue) between `Object` code and the callback.

## Measure first

Before options 2 and 3, add frame timing: how long input, update, redraw, and OpenGL each take, logged every few seconds in Debug (a dev param turns it on). That shows which step is worth splitting up. Option 1 doesn't need it: it's about the game staying responsive while loading, not speed.

## Recommended order

1. Frame timing.
2. A real load on the loading screen: the `World::start( )` step, a thread-safe `Logger`, and thread-safe static caches.
3. The thread pool and parallel redraws, if the timing says redrawing is a large part of a frame. Parallel redraws need the same thread-safe caches as step 2.
