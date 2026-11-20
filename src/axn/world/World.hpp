#ifndef World_hpp
#define World_hpp

#include "axn/system/Engine.hpp"
#include "axn/physics/Physics.hpp"

namespace axn
{

namespace graphics
{

class Camera;

} // namespace graphics

namespace reality
{

class Room;
class Object;
class Player;
class Door;

} // namespace reality

class Entity : public Identifiable
{

};

class Event
{

private:

    uint m_processed_count = 0;

public:

    bool processed( ) const { return( is_pos( processed_count( ) ) ); }
    uint processed_count( ) const { return( m_processed_count ); }
    virtual void process( ptr<Object> in_object = nullptr ) { m_processed_count++; }

};

namespace reality
{

class World
{

public:

    virtual ~World( );

protected:

    World( );

public:

    virtual string name( ) const { return( "AXION" ); }

    virtual bool quit( ) const { return( false ); }

    virtual void init( );
    virtual void render( );
    virtual void update( );

    virtual void pause( bool in_pause );
    virtual bool paused( ) const;

    virtual void input( list<ptr<Input>> cref in_inputs = { } );

    bool loading( ) const { return( m_loading ); }
    void update_loading( );

    uint age( ) const { return( m_age ); }
    // C! moves every room (only the current one when in_current_only) on by in_time, and the world's age with them; the sounds they
    // C! queue meanwhile are dropped, they're for time that's skipped. Run behind the loading screen (it's slow, and nothing else may touch the rooms)
    void age( ticks in_time, bool in_current_only = false );

    ptr<Room> next_room( );
    ptr<Room> current_room( );
    virtual ptr<Room> current_room( ptr<Room> in_room );

    virtual ptr<Room> add_room( ptr<Room> in_room, bool in_current = false );

    // C! goes through in_door to the room on its other side, taking in_objects (a player included) with it: they're moved
    // C! when the rooms swap, after the camera has transitioned out, and come out of in_door's other side
    virtual void pass( varray<ptr<Object>> cref in_objects, ptr<Door> in_door );

    ptr<Camera> camera( ) const { return( m_camera ); }

    // C! sounds aren't played straight away: they're queued while the world updates, and played together by play_queued_sounds( ),
    // C! which Engine calls at the end of each pass of the main loop (paused or not)
    void play_sound( SoundFX cref in_sound, Point cref in_position ); // C! heard by the active Listeners in the current room it reaches, the best at its reach( ) times their volume( )
    void play_sound( SoundFX cref in_sound ); // C! a global( ) sound, which has no position
    void play_queued_sounds( );

    #if defined( AXN_DEBUG )
    // C! a sound played with a position, kept while it plays, for the DEBUG_AUDIO overlay
    struct HeardSound
    {

        Point position;
        Planc umbra;
        Planc penumbra;
        Countdown playing;

    };

    varray<HeardSound> cref heard_sounds( ) const { return( m_heard_sounds ); }
    #endif

    virtual Visible::LayerOrder layer_order( ptr<const Object> in_object ) const { return( Visible::LayerOrder( ) ); }

    virtual dec air_resistance( ) const { return( 0.0 ); }
    virtual Vector gravity( ) const { return( V0 ); }

    bool has_gravity( ) const { return( gravity( ).has_magnitude( ) ); }

protected:

    virtual void create( );
    virtual void destroy( );
    virtual void reset( );

    virtual bool loads_in_background( ) const { return( true ); } // C! false for a world shown straight away (the splash and loading screens)

    // C! shows the loading screen until in_load (run on its own thread) is done and it's been up at least LOADING_SCREEN_MIN_TICKS
    void start_loading( function<void( )> cref in_load = nullptr );

    ptr<Room> current_room_swap( ptr<Room> in_room );

    // C! a player made invisible and invincible while its room ages, and whether it was always invincible already, to put back after
    struct HiddenPlayer
    {

        ptr<Player> player;
        bool invincible_always = false;

    };

    varray<HiddenPlayer> hide_players( varray<ptr<Room>> cref in_rooms ); // C! every player in in_rooms
    void show_players( varray<HiddenPlayer> cref in_hidden );

    void catch_up( ptr<Room> in_room ); // C! ages in_room up to the world's age, dropping the sounds it queues meanwhile

private:

    uint m_age = 0;

    bool m_paused = false;

    uset<ptr<Room>> m_rooms;

    ptr<Room> m_next_room;
    ptr<Room> m_current_room;

    ptr<Door> m_passing_door; // C! the door they come out of, in the next room
    varray<ptr<Object>> m_passing_objects; // C! waiting to go through m_passing_door when the rooms swap

    ptr<Camera> m_camera;

    // C! a sound waiting for play_queued_sounds( ), and where it was played
    struct QueuedSound
    {

        SoundFX sound;
        Point position = ORIGIN;

    };

    varray<QueuedSound> m_queued_sounds;

    #if defined( AXN_DEBUG )
    varray<HeardSound> m_heard_sounds;
    #endif

    bool m_loading = false;
    bool m_switching_room = false; // C! the load is a room switch, so the camera transitions back in when it's done
    atomic<bool> m_loaded = false; // C! set by the load's thread when it's done, read by the main loop
    thread m_load_thread;
    ticks m_loading_ticks = 0.0; // C! how long the loading screen has been up

    void load( ); // C! what the world does on its loading thread when it starts
    void finish_loading( );

};

} // namespace reality
} // namespace axn

#endif /* World_hpp */
