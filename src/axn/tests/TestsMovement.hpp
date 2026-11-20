#ifndef TestsMovement_hpp
#define TestsMovement_hpp

#include "axn/tests/Tests.hpp"

#include "axn/world/Room.hpp"
#include "axn/world/Object.hpp"
#include "axn/world/Terrain.hpp"
#include "axn/world/Door.hpp"

// C! the full landing sweep drops the walker about 1,700 times and takes around 2 minutes in a Debug build, too slow for every launch,
// C! so normally a coarse sweep runs instead, plus the drops that have failed before. uncomment this when working on movement
//#define AXN_FULL_MOVEMENT_TESTS

namespace axn
{

#if defined( AXN_FULL_MOVEMENT_TESTS )
static const dec LANDING_X_STEP = 13.0;
static const varray<dec> LANDING_SPEEDS = { -9.0, -3.0, 0.0, 3.0, 9.0 };
#else
static const dec LANDING_X_STEP = 61.0;
static const varray<dec> LANDING_SPEEDS = { -9.0, 3.0 };
#endif

// C! simulates an object walking over real terrain using the same update_movement the game uses

class TestTerrain : public Terrain
{

public:

    TestTerrain( ptr<Room> in_room, vvarray<Point> cref in_vertices ) : Terrain( in_room, in_vertices ) { }

};

class TestRoom : public Room
{

private:

    varray<Point> m_ground;

public:

    TestRoom( varray<Point> cref in_ground ) : Room( nullptr ), m_ground( in_ground ) { }

    virtual Vector gravity( ) const override { return( VectorY( -0.625 ) ); } // C! same as MtMerciless
    virtual Vector gravity( Point cref in_position ) const override { return( gravity( ) ); }

    virtual dec air_resistance( ) const override { return( 0.025 ); }
    virtual dec air_resistance( Point cref in_position ) const override { return( air_resistance( ) ); }

    virtual void create( ) override { Room::create( FixedRectangle( Point( -2000.0, -2000.0 ), Point( 4000.0, 4000.0 ) ) ); }

    // C! height of the ground surface at in_x
    Planc ground_y( Planc cref in_x ) const
    {
        for_range( i, m_ground.size( ) - 1 )
        {
            Point cref pt1 = m_ground[ i ];
            Point cref pt2 = m_ground[ i + 1 ];

            if( ( in_x >= pt1.x( ) ) && ( in_x <= pt2.x( ) ) && ( pt1.x( ) != pt2.x( ) ) )
            {
                return( pt1.y( ) + ( ( pt2.y( ) - pt1.y( ) ) * ( ( in_x - pt1.x( ) ) / ( pt2.x( ) - pt1.x( ) ) ) ) );
            }
        }

        return( m_ground.back( ).y( ) );
    }

protected:

    virtual ptr<Terrain> generate_terrain( ) override
    {
        skip_override;

        vvarray<Point> vertices;
        vertices.insert_back( m_ground );

        return( new TestTerrain( this, vertices ) );
    }

};

class TestWalker : public Object
{

public:

    // C! same hitbox polygon and walking speed as Climber
    static constexpr dec HEIGHT = ( 32.0 * 1.85 );
    static constexpr dec WIDTH = ( HEIGHT / 2.5 );
    static constexpr dec WIDTH_GROUND = ( WIDTH * 0.25 );
    static constexpr dec SPEED = ( 1.0 );

    dec walking = 0.0; // C! -1 left, 0 still, +1 right

    TestWalker( ptr<Room> in_room, Point cref in_position ) : Object( in_room, in_position )
    {
        space( Polygon( { Point( +( WIDTH / 2.0 ), +( HEIGHT / 2.0 ) ),
                                    Point( -( WIDTH / 2.0 ), +( HEIGHT / 2.0 ) ),
                                    Point( -( WIDTH_GROUND / 2.0 ), -( HEIGHT / 2.0 ) ),
                                    Point( +( WIDTH_GROUND / 2.0 ), -( HEIGHT / 2.0 ) ) } ) );
    }

    void step( ) { update( ); }
    void track( uint in_count ) { track_position( in_count ); }
    void make_interactive( bool in_interactive ) { interactive( in_interactive ); }
    void add_light( Planc cref in_radius ) { add_light_source( position( ), in_radius ); }

protected:

    // C! walking the way Climber::update_velocity does it
    virtual void update_velocity( ) override
    {
        Object::update_velocity( );

        if( !is_zero( walking ) )
        {
            Vector v = VectorX( walking * SPEED );

            if( grounded( ) )
            {
                Line line = ground_bumper_line( );

                if( !line.vertical( ) )
                {
                    v.rotate_to( v.angle( ).closest( { line.angle( ), line.angle( ).flipped( ) } ) );
                }
            }

            add_velocity( v );
        }
    }

};

struct MovementFrame
{

    uint frame;
    Point position;
    Vector velocity;
    bool grounded;
    string ground;

};

static string describe_ground( Terrain::Bumper cref in_ground )
{
    if( is_null( in_ground.node( ) ) ) { return( "air" ); }
    if( !is_null( cast_const( in_ground.node( ), Terrain::Edge ) ) ) { return( "edge" ); }
    return( "vertex" );
}

static void print_frames( string cref in_title, varray<MovementFrame> cref in_frames, uint in_from, uint in_to )
{
    cout << "---- " << in_title << endl;

    for_range( i, min( in_to, in_frames.size( ) ) )
    {
        continue_if( i < in_from );

        MovementFrame cref f = in_frames[ i ];
        cout << "  frame " << f.frame << "  pos ( " << dec( f.position.x( ) ) << ", " << dec( f.position.y( ) ) << " )  vel ( " << dec( f.velocity.dx( ) ) << ", " << dec( f.velocity.dy( ) ) << " )  " << f.ground << endl;
    }
}

// C! drops a walker onto the terrain, lets it settle, then walks it in_direction for in_frames
// C! returns false (and prints the frames around the problem) if it gets stuck or ends up inside the terrain
static bool check_walk( string cref in_name, varray<Point> cref in_ground, Planc cref in_start_x, dec in_direction, uint in_frames, Planc cref in_stop_x )
{
    TestRoom room( in_ground );
    room.init( );

    TestWalker walker( &room, Point( in_start_x, room.ground_y( in_start_x ) + TestWalker::HEIGHT ) );

    varray<MovementFrame> frames;

    lambdaf( record )( uint in_frame )
    {
        frames.insert_back( { in_frame, walker.position( ), walker.velocity( ), walker.grounded( ), describe_ground( walker.ground( ) ) } );
    };

    uint settle_frames = 60;

    for_range( frame, settle_frames + in_frames )
    {
        walker.walking = ( ( frame < settle_frames ) ? 0.0 : in_direction );
        walker.step( );
        record( frame );

        // C! inside the terrain: the center went below the ground surface
        if( walker.position( ).y( ) < room.ground_y( walker.position( ).x( ) ) )
        {
            print_frames( in_name + ": fell into the terrain", frames, ( frames.size( ) > 10 ) ? ( frames.size( ) - 10 ) : 0, frames.size( ) );
            return( false );
        }

        // C! reached the end of the walk
        if( is_pos( in_direction ) ? ( walker.position( ).x( ) >= in_stop_x ) : ( walker.position( ).x( ) <= in_stop_x ) )
        {
            return( true );
        }

        // C! stuck: hardly moved over the last 20 walking frames
        if( frame >= ( settle_frames + 40 ) )
        {
            Planc moved = abs( walker.position( ).x( ) - frames[ frames.size( ) - 20 ].position.x( ) );

            if( moved < 2.0 )
            {
                print_frames( in_name + ": stuck", frames, frames.size( ) - 25, frames.size( ) );
                return( false );
            }
        }
    }

    print_frames( in_name + ": never reached the end", frames, frames.size( ) - 10, frames.size( ) );
    return( false );
}

// C! drops a walker onto the terrain and checks it stays put without walking
static bool check_stand( string cref in_name, varray<Point> cref in_ground, Planc cref in_x )
{
    TestRoom room( in_ground );
    room.init( );

    TestWalker walker( &room, Point( in_x, room.ground_y( in_x ) + TestWalker::HEIGHT ) );

    varray<MovementFrame> frames;
    uint settle_frames = 60;
    uint watch_frames = 240;

    for_range( frame, settle_frames + watch_frames )
    {
        walker.step( );
        frames.insert_back( { frame, walker.position( ), walker.velocity( ), walker.grounded( ), describe_ground( walker.ground( ) ) } );
    }

    Point settled = frames[ settle_frames ].position;
    Point last = frames.back( ).position;

    uint airborne = 0;
    for_range( i, frames.size( ) ) { continue_if( i < settle_frames ); if( !frames[ i ].grounded ) { airborne++; } }

    if( ( settled.distance_to( last ) > 1.0 ) || airborne )
    {
        cout << "---- " << in_name << ": drifted " << dec( settled.distance_to( last ) ) << " while standing, airborne " << airborne << " of " << watch_frames << " frames" << endl;
        print_frames( in_name, frames, settle_frames, settle_frames + 12 );
        print_frames( in_name + " (end)", frames, frames.size( ) - 6, frames.size( ) );
        return( false );
    }

    return( true );
}

// C! is one of the hitbox's bottom corners below the ground (ignoring corners right at a vertex, where a wall makes the height ambiguous)
static bool feet_in_ground( TestRoom cref in_room, varray<Point> cref in_ground, Point cref in_center )
{
    const Planc TOLERANCE = 0.5;

    for_each_const( side, varray<dec>( { -1.0, +1.0 } ) )
    {
        Point foot = in_center + Vector( side * ( TestWalker::WIDTH_GROUND / 2.0 ), -( TestWalker::HEIGHT / 2.0 ) );

        bool at_vertex = false;
        for_each_const( pt, in_ground ) { if( abs( dec( pt.x( ) - foot.x( ) ) ) < TOLERANCE ) { at_vertex = true; } }

        if( !at_vertex && ( foot.y( ) < ( in_room.ground_y( foot.x( ) ) - TOLERANCE ) ) )
        {
            return( true );
        }
    }

    return( false );
}

// C! drops the walker once at in_x with a sideways in_speed (walking that way too when in_walking), returns false if its feet went into the ground
static bool check_landing( string cref in_name, varray<Point> cref in_ground, dec in_x, dec in_speed, dec in_walking, bool in_print = true )
{
    TestRoom room( in_ground );
    room.init( );

    TestWalker walker( &room, Point( in_x, room.ground_y( in_x ) + 150.0 ) );
    walker.velocity( Vector( in_speed, 0.0 ) );
    walker.walking = ( is_pos( in_speed ) ? in_walking : ( is_neg( in_speed ) ? -in_walking : 0.0 ) );

    varray<MovementFrame> frames;

    for_range( frame, 150 )
    {
        walker.step( );
        frames.insert_back( { frame, walker.position( ), walker.velocity( ), walker.grounded( ), describe_ground( walker.ground( ) ) } );

        // C! walked off either end of the test terrain, the ground height is made up past there
        break_if( ( walker.position( ).x( ) < ( in_ground.front( ).x( ) + 50.0 ) ) || ( walker.position( ).x( ) > ( in_ground.back( ).x( ) - 50.0 ) ) );

        if( feet_in_ground( room, in_ground, walker.position( ) ) )
        {
            if( in_print )
            {
                cout << "---- " << in_name << ": dropped at x " << in_x << " speed " << in_speed << " walking " << in_walking << ", feet went into the ground" << endl;
                print_frames( "    frames", frames, ( frames.size( ) > 6 ) ? ( frames.size( ) - 6 ) : 0, frames.size( ) );
            }

            return( false );
        }
    }

    return( true );
}

// C! drops the walker at many places and speeds onto the terrain (see LANDING_X_STEP / LANDING_SPEEDS), returns how many drops ended up in the ground
static uint check_landings( string cref in_name, varray<Point> cref in_ground, Planc cref in_from_x, Planc cref in_to_x )
{
    uint failures = 0;

    // C! x runs from in_from_x up to in_to_x in LANDING_X_STEP steps, the epsilon keeps rounding from dropping the last step
    for_range( step, uint( ( dec( in_to_x - in_from_x ) / LANDING_X_STEP ) + DEFAULT_EPSILON ) + 1 )
    {
        dec x = dec( in_from_x ) + ( step * LANDING_X_STEP );

        for_each_const( speed, LANDING_SPEEDS )
        {
            for_each_const( walking, varray<dec>( { 0.0, 1.0 } ) )
            {
                // C! only print the first few, the rest are usually the same problem
                if( !check_landing( in_name, in_ground, x, speed, walking, ( failures < 3 ) ) )
                {
                    failures++;
                }
            }
        }
    }

    if( failures )
    {
        cout << "---- " << in_name << ": " << failures << " landings went into the ground" << endl;
    }

    return( failures );
}

test( "Landing on terrain" )
{
    varray<Point> flat = { Point( -400.0, 0.0 ), Point( 0.0, 0.0 ), Point( 200.0, 0.0 ), Point( 400.0, 0.0 ), Point( 600.0, 0.0 ), Point( 1400.0, 0.0 ) };
    varray<Point> hill = { Point( -400.0, 0.0 ), Point( 200.0, 0.0 ), Point( 300.0, 58.0 ), Point( 400.0, 58.0 ), Point( 500.0, 0.0 ), Point( 1400.0, 0.0 ) };
    varray<Point> valley = { Point( -400.0, 100.0 ), Point( 200.0, 100.0 ), Point( 300.0, 42.0 ), Point( 400.0, 42.0 ), Point( 500.0, 100.0 ), Point( 1400.0, 100.0 ) };
    varray<Point> bumpy = { Point( -400.0, 0.0 ), Point( 100.0, 0.0 ), Point( 160.0, 8.0 ), Point( 220.0, 12.0 ), Point( 280.0, 10.0 ), Point( 340.0, 0.0 ), Point( 400.0, -6.0 ), Point( 460.0, 0.0 ), Point( 1400.0, 0.0 ) };
    varray<Point> steps = { Point( -400.0, 0.0 ), Point( 200.0, 0.0 ), Point( 200.0, 40.0 ), Point( 300.0, 40.0 ), Point( 300.0, 80.0 ), Point( 450.0, 80.0 ), Point( 450.0, 20.0 ), Point( 1400.0, 20.0 ) };
    varray<Point> spikes = { Point( -400.0, 0.0 ), Point( 150.0, 0.0 ), Point( 170.0, 30.0 ), Point( 190.0, 0.0 ), Point( 210.0, 30.0 ), Point( 230.0, 0.0 ), Point( 1400.0, 0.0 ) };

    check( is_zero( check_landings( "flat", flat, 150.0, 650.0 ) ) );
    check( is_zero( check_landings( "hill", hill, 150.0, 550.0 ) ) );
    check( is_zero( check_landings( "valley", valley, 150.0, 550.0 ) ) );
    check( is_zero( check_landings( "bumpy", bumpy, 50.0, 500.0 ) ) );
    check( is_zero( check_landings( "steps", steps, 150.0, 500.0 ) ) );
    check( is_zero( check_landings( "spikes", spikes, 120.0, 260.0 ) ) );

    // C! drops that went into the ground before the fixes, always checked even when the sweep is coarse
    check( check_landing( "steps", steps, 150.0, 3.0, 1.0 ) ); // C! walked into a step, fell into it at the top corner
    check( check_landing( "steps", steps, 176.0, 3.0, 1.0 ) );
    check( check_landing( "spikes", spikes, 133.0, 3.0, 0.0 ) ); // C! slid into the valley between two spikes
    check( check_landing( "spikes", spikes, 146.0, 3.0, 0.0 ) );
    check( check_landing( "spikes", spikes, 159.0, 3.0, 0.0 ) );
}

test( "Movement over terrain" )
{
    varray<Point> flat = { Point( -400.0, 0.0 ), Point( 0.0, 0.0 ), Point( 200.0, 0.0 ), Point( 400.0, 0.0 ), Point( 600.0, 0.0 ), Point( 1400.0, 0.0 ) };
    varray<Point> hill = { Point( -400.0, 0.0 ), Point( 200.0, 0.0 ), Point( 300.0, 58.0 ), Point( 400.0, 58.0 ), Point( 500.0, 0.0 ), Point( 1400.0, 0.0 ) };
    varray<Point> valley = { Point( -400.0, 100.0 ), Point( 200.0, 100.0 ), Point( 300.0, 42.0 ), Point( 400.0, 42.0 ), Point( 500.0, 100.0 ), Point( 1400.0, 100.0 ) };
    varray<Point> bumpy = { Point( -400.0, 0.0 ), Point( 100.0, 0.0 ), Point( 160.0, 8.0 ), Point( 220.0, 12.0 ), Point( 280.0, 10.0 ), Point( 340.0, 0.0 ), Point( 400.0, -6.0 ), Point( 460.0, 0.0 ), Point( 1400.0, 0.0 ) };
    varray<Point> gentle_slant = { Point( -400.0, 0.0 ), Point( 1400.0, 400.0 ) };
    varray<Point> steep_slant = { Point( -400.0, 0.0 ), Point( 1400.0, 1000.0 ) };

    section( "walking" )
    {
        check( check_walk( "flat right", flat, 50.0, +1.0, 600, 900.0 ) );
        check( check_walk( "flat left", flat, 900.0, -1.0, 600, 50.0 ) );
        check( check_walk( "hill right", hill, 50.0, +1.0, 600, 900.0 ) );
        check( check_walk( "hill left", hill, 900.0, -1.0, 600, 50.0 ) );
        check( check_walk( "valley right", valley, 50.0, +1.0, 600, 900.0 ) );
        check( check_walk( "valley left", valley, 900.0, -1.0, 600, 50.0 ) );
        check( check_walk( "bumpy right", bumpy, 50.0, +1.0, 600, 900.0 ) );
        check( check_walk( "bumpy left", bumpy, 900.0, -1.0, 600, 50.0 ) );
    }

    section( "standing" )
    {
        check( check_stand( "flat", flat, 300.0 ) );
        check( check_stand( "gentle slant", gentle_slant, 500.0 ) );
        check( check_stand( "steep slant", steep_slant, 500.0 ) );
    }
}

test( "Object position history" )
{
    varray<Point> flat = { Point( -400.0, 0.0 ), Point( 1400.0, 0.0 ) };
    TestRoom room( flat );
    room.init( );

    TestWalker walker( &room, Point( 100.0, TestWalker::HEIGHT ) );
    walker.track( 3 );

    for_range( frame, 30 ) { walker.step( ); } // C! land and settle

    walker.walking = +1.0;

    varray<Point> frame_starts;
    for_range( frame, 5 )
    {
        frame_starts.insert_back( walker.position( ) );
        walker.step( );
    }

    // C! 1 = the current frame, which started where the object was 1 update ago
    check( walker.last_position( 0 ) == walker.position( ) );
    check( walker.last_position( 1 ) == frame_starts[ 4 ] );
    check( walker.last_position( 2 ) == frame_starts[ 3 ] );
    check( walker.last_position( 3 ) == frame_starts[ 2 ] );

    // C! only 3 frames are tracked, further back gives the oldest
    check( walker.last_position( 10 ) == frame_starts[ 2 ] );
    check( walker.last_positions( ).size( ) == 3 );
}

test( "Object updates and room grid" )
{
    varray<Point> flat = { Point( -400.0, 0.0 ), Point( 1400.0, 0.0 ) };
    TestRoom room( flat );
    room.init( );

    TestWalker walker( &room, Point( 300.0, 100.0 ) );

    lambdaf( in_grid )( )
    {
        for_each_const( object, room.objects_in_range( FixedRectangle( Point( 290.0, 90.0 ), Point( 310.0, 110.0 ) ) ) )
        {
            return_true_if( object.operator->( ) == &walker );
        }

        return( false );
    };

    // C! a new object updates even though the room is still at age 0
    walker.update_object( );
    check( walker.age( ) == 1 );

    // C! switching interactive on puts an object that's already in the room into the grid straight away, not the next time it moves
    check_not( in_grid( ) );
    walker.make_interactive( true );
    check( in_grid( ) );

    walker.make_interactive( false );
    check_not( in_grid( ) );
}

test( "Object movement subscriptions" )
{
    varray<Point> flat = { Point( -400.0, 0.0 ), Point( 1400.0, 0.0 ) };
    TestRoom room( flat );
    room.init( );

    ptr<TestWalker> leader = new TestWalker( &room, Point( 100.0, 100.0 ) );
    ptr<TestWalker> follower1 = new TestWalker( &room, Point( 200.0, 100.0 ) );
    ptr<TestWalker> follower2 = new TestWalker( &room, Point( 300.0, 100.0 ) );

    follower1->subscribe_to_movement( leader );
    follower2->subscribe_to_movement( leader );

    check( leader->movement_subscribers( ).size( ) == 2 );

    // C! deleting an object with several subscribers used to modify the set it was looping over
    delete leader;

    check( is_zero( follower1->movement_subscriptions( ).size( ) ) );
    check( is_zero( follower2->movement_subscriptions( ).size( ) ) );

    delete follower1;
    delete follower2;
}

test( "Objects passed between rooms" )
{
    varray<Point> flat = { Point( -400.0, 0.0 ), Point( 1400.0, 0.0 ) };
    TestRoom room1( flat );
    TestRoom room2( flat );
    room1.init( );
    room2.init( );

    ptr<TestWalker> walker = new TestWalker( &room1, Point( 300.0, 40.0 ) );
    walker->make_interactive( true );
    room1.add_object( walker );

    do_count( 60 ) { room1.update( ); } // C! long enough to land

    lambdaf( in_list )( ptr<Room> in_room )
    {
        for_each_const( object, in_room->objects( ) ) { return_true_if( object.operator->( ) == walker.operator->( ) ); }
        return( false );
    };

    lambdaf( in_grid )( ptr<Room> in_room )
    {
        for_each_const( object, in_room->objects_in_range( FixedRectangle( Point( -400.0, -100.0 ), Point( 1400.0, 200.0 ) ) ) ) { return_true_if( object.operator->( ) == walker.operator->( ) ); }
        return( false );
    };

    check( in_list( &room1 ) );
    check( in_grid( &room1 ) );
    check( walker->grounded( ) );

    // C! released: out of every list in the old room, but not deleted
    room1.release( walker );
    check_not( in_list( &room1 ) );
    check_not( in_grid( &room1 ) );

    // C! passed: in the new room's lists and grid straight away, with no ground from the old room's terrain
    room2.pass( { walker } );
    check( walker->room( ).operator->( ) == &room2 );
    check( in_list( &room2 ) );
    check( in_grid( &room2 ) );
    check_not( walker->grounded( ) );
    check( walker->position( ) == Point( 0.0, 50.0 ) ); // C! no door to come out of, so the default entrance

    // C! it lands on the new room's terrain
    do_count( 60 ) { room2.update( ); }
    check( walker->grounded( ) );
}

test( "Objects come out of a door standing in it" )
{
    varray<Point> flat = { Point( -400.0, 0.0 ), Point( 1400.0, 0.0 ) };
    TestRoom room1( flat );
    TestRoom room2( flat );
    room1.init( );
    room2.init( );

    // C! a door 60 wide and 100 tall, standing on the ground at x 200: it spans x 170 to 230 and y 0 to 100
    ptr<Door> door = room2.add_door( new Door( &room2, Point( 200.0, 0.0 ), 60.0, 100.0 ) );
    check( door->exit_gravity( ) == VectorY( -1.0 ).angle( ) ); // C! the room's gravity, down

    ptr<TestWalker> walker = new TestWalker( &room1, Point( 300.0, 40.0 ) );
    walker->add_light( 10.0 ); // C! a light where it is in the other room

    // C! gravity down: centered across the door, with the bottom of its hitbox on the bottom of the door
    room2.pass( { walker }, door );
    check( walker->position( ) == Point( 200.0, TestWalker::HEIGHT / 2.0 ) );
    check( walker->grounded( ) ); // C! standing on the floor as it comes out, without having to update first
    check( walker->light_sources( ).front( ).position( ) == walker->position( ) ); // C! its light came with it

    // C! gravity to the right: against the door's right side, centered top to bottom
    room2.release( walker );
    door->exit_gravity( Angle( 0.0 ) );
    room2.pass( { walker }, door );
    check( walker->position( ) == Point( 230.0 - ( TestWalker::WIDTH / 2.0 ), 50.0 ) );
    check( !walker->grounded( ) ); // C! the room's gravity is still down, and there's no floor under it here

    // C! gravity down and to the right (45 degrees): pushed into the door's bottom right corner, the furthest it goes that way, and centered
    // C! across that direction. Measured along "down" ( s, -s ) and "across" ( s, s ), with s = sin( 45 ) = 1 / sqrt( 2 ):
    // C!   the door goes furthest down at its corner ( 230, 0 ), 230 s, and across it spans 170 s to 330 s, so its middle is 250 s
    // C!   the walker goes furthest down at its bottom right corner, ( WIDTH_GROUND / 2 + HEIGHT / 2 ) s, and across it spans from
    // C!   ( -WIDTH_GROUND / 2 - HEIGHT / 2 ) s to ( WIDTH / 2 + HEIGHT / 2 ) s, so its middle is ( WIDTH - WIDTH_GROUND ) / 4 s
    room2.release( walker );
    door->exit_gravity( -Angle::fraction( 8.0 ) );
    room2.pass( { walker }, door );

    const dec c_down = 230.0 - ( TestWalker::WIDTH_GROUND / 2.0 ) - ( TestWalker::HEIGHT / 2.0 ); // C! how far it moves down, in units of s
    const dec c_across = 250.0 - ( ( TestWalker::WIDTH - TestWalker::WIDTH_GROUND ) / 4.0 ); // C! and across
    check( walker->position( ) == Point( ( c_down + c_across ) / 2.0, ( c_across - c_down ) / 2.0 ) ); // C! down ( s, -s ) and across ( s, s ), with s * s = 1 / 2
}

} // namespace axn

#endif /* TestsMovement_hpp */
