#ifndef TestsUtil_hpp
#define TestsUtil_hpp

#include "axn/tests/Tests.hpp"

#include "axn/system/Utility.hpp"
#include "axn/audio/Audio.hpp"

namespace axn
{

test( "for_range tests" )
{
    int count;

    count = 0;
    for_range( i, 32 ) { count += 1; }

    check( count == 32 );

    count = 0;
    for_range( i, 5 ) { count += i; }

    check( count == 10 );

    count = 0;
    for_range( x, 5 ) { count += x; }

    check( count == 10 );
}

test( "while_max tests" )
{
    int count = 0;

    while_max( true, 10 ) { check( ( count++ ) < 10 ); }
    while_max( true, 10 ) { check( ( count++ ) < 20 ); }
    while_max( true, 10 ) { check( ( count++ ) < 30 ); }

    check( count == 30 );
}

test( "and_bool tests" )
{
    and_bool b;
    check( b );
    b = true;
    check( b );
    b = false;
    check( !b );
    b = true;
    check( !b );
}

test( "or_bool tests" )
{
    or_bool b;
    check( !b );
    b = false;
    check( !b );
    b = true;
    check( b );
    b = false;
    check( b );
}

test( "swap tests" )
{
    section( "int" )
    {
        int x = 3;
        int y = 5;

        swap_values( x, y );

        check( x == 5 );
        check( y == 3 );
    }

    section( "dec" )
    {
        dec x = 3.3;
        dec y = 5.5;

        swap_values( x, y );

        check( x == 5.5 );
        check( y == 3.3 );
    }

    section( "struct" )
    {
        struct structest { structest( int in_x ) { i = in_x; } int i = 0; };

        structest x = structest( 3 );
        structest y = structest( 5 );

        swap_values( x, y );

        check( x.i == 5 );
        check( y.i == 3 );
    }

    section( "if" )
    {
        int x = 3;
        int y = 5;

        swap_values_if( true, x, y );

        check( x == 5 );
        check( y == 3 );

        swap_values_if( false, x, y );

        check( x == 5 );
        check( y == 3 );
    }
}

test( "math tests" )
{
    section( "divisible" )
    {
        check( is_divisible( 5.0, 1.0 ) );
        check( is_divisible( 4.0, 2.0 ) );
        check( is_divisible( 3.0, 1.5 ) );

        check_not( is_divisible( 3.0, 1.6 ) );

        check( is_divisible( 3.0, 3.0 ) );
        check( is_divisible( 4.2, 4.2 ) );

        check_not( is_divisible( 0.0, 0.0 ) );
        check_not( is_divisible( 0.0, 1.0 ) );
        check_not( is_divisible( 1.0, 0.0 ) );
    }

    section( "prime" )
    {
        check_not( is_prime( 0 ) );
        check_not( is_prime( 1 ) );
        check( is_prime( 2 ) );
        check( is_prime( 3 ) );
        check_not( is_prime( 4 ) );
        check( is_prime( 5 ) );
        check_not( is_prime( 6 ) );
        check( is_prime( 7 ) );
        check_not( is_prime( 8 ) );
        check_not( is_prime( 9 ) );

        check_not( is_prime( 10 ) );
        check( is_prime( 11 ) );
        check_not( is_prime( 12 ) );
        check( is_prime( 13 ) );
        check_not( is_prime( 14 ) );
        check_not( is_prime( 15 ) );
        check_not( is_prime( 16 ) );
        check( is_prime( 17 ) );
        check_not( is_prime( 18 ) );
        check( is_prime( 19 ) );
        check_not( is_prime( 20 ) );

        check( is_prime( 101 ) );
        check_not( is_prime( 102 ) );
        check( is_prime( 103 ) );

        check_not( is_prime( 1.1 ) );
        check_not( is_prime( 2.1 ) );
        check_not( is_prime( 3.1 ) );
    }

    section( "composite" )
    {
        check_not( is_composite( 0 ) );
        check_not( is_composite( 1 ) );
        check_not( is_composite( 2 ) );
        check_not( is_composite( 3 ) );
        check( is_composite( 4 ) );
        check_not( is_composite( 5 ) );
        check( is_composite( 6 ) );
        check_not( is_composite( 7 ) );
        check( is_composite( 8 ) );
        check( is_composite( 9 ) );

        check_not( is_composite( 1.1 ) );
        check_not( is_composite( 2.1 ) );
        check_not( is_composite( 3.1 ) );
    }

    section( "pythagorean" )
    {
        check( is_zero( pythagorean( 0.0, 0.0 ) ) );
        check( pythagorean( 3.0, 4.0 ) == 5.0 );
        check( pythagorean( 1.0, 1.0 ) == sqrt( 2.0 ) );
        check( pythagorean( 1.0, 2.0 ) == sqrt( 5.0 ) );
        check( pythagorean( 5.0, 12.0 ) == 13.0 );
        check( pythagorean( 7.0, 24.0 ) == 25.0 );
        check( pythagorean( 20.0, 21.0 ) == 29.0 );
    }

    section( "additorial" )
    {
        check( is_zero( additorial( 0 ) ) );
        check( additorial( 1 ) == 1 );
        check( additorial( 2 ) == 3 );
        check( additorial( 3 ) == 6 );
        check( additorial( 4 ) == 10 );
        check( additorial( 5 ) == 15 );
        check( additorial( 6 ) == 21 );
        check( additorial( 7 ) == 28 );
        check( additorial( 8 ) == 36 );
        check( additorial( 9 ) == 45 );
    }

    section( "factorial" )
    {
        check( factorial( 0 ) == 1 );
        check( factorial( 1 ) == 1 );
        check( factorial( 2 ) == 2 );
        check( factorial( 3 ) == 6 );
        check( factorial( 4 ) == 24 );
        check( factorial( 5 ) == 120 );
        check( factorial( 6 ) == 720 );
        check( factorial( 7 ) == 5040 );
        check( factorial( 8 ) == 40320 );
        check( factorial( 9 ) == 362880 );
    }

    section( "fibonacci" )
    {
        check( fibonacci( 0 ) == 1 );
        check( fibonacci( 1 ) == 1 );
        check( fibonacci( 2 ) == 2 );
        check( fibonacci( 3 ) == 3 );
        check( fibonacci( 4 ) == 5 );
        check( fibonacci( 5 ) == 8 );
        check( fibonacci( 6 ) == 13 );
        check( fibonacci( 7 ) == 21 );
        check( fibonacci( 8 ) == 34 );
        check( fibonacci( 9 ) == 55 );
    }

    section( "pascal" )
    {
        check( pascal_triangle( 0, 0 ) == 1 );

        check( pascal_triangle( 1, 0 ) == 1 );
        check( pascal_triangle( 1, 1 ) == 1 );

        check( pascal_triangle( 2, 0 ) == 1 );
        check( pascal_triangle( 2, 1 ) == 2 );
        check( pascal_triangle( 2, 2 ) == 1 );

        check( pascal_triangle( 3, 0 ) == 1 );
        check( pascal_triangle( 3, 1 ) == 3 );
        check( pascal_triangle( 3, 2 ) == 3 );
        check( pascal_triangle( 3, 3 ) == 1 );

        check( pascal_triangle( 4, 0 ) == 1 );
        check( pascal_triangle( 4, 1 ) == 4 );
        check( pascal_triangle( 4, 2 ) == 6 );
        check( pascal_triangle( 4, 3 ) == 4 );
        check( pascal_triangle( 4, 4 ) == 1 );

        check( pascal_triangle( 5, 0 ) == 1 );
        check( pascal_triangle( 5, 1 ) == 5 );
        check( pascal_triangle( 5, 2 ) == 10 );
        check( pascal_triangle( 5, 3 ) == 10 );
        check( pascal_triangle( 5, 4 ) == 5 );
        check( pascal_triangle( 5, 5 ) == 1 );

        check( pascal_triangle( 6, 0 ) == 1 );
        check( pascal_triangle( 6, 1 ) == 6 );
        check( pascal_triangle( 6, 2 ) == 15 );
        check( pascal_triangle( 6, 3 ) == 20 );
        check( pascal_triangle( 6, 4 ) == 15 );
        check( pascal_triangle( 6, 5 ) == 6 );
        check( pascal_triangle( 6, 6 ) == 1 );

        check( pascal_triangle( 7, 0 ) == 1 );
        check( pascal_triangle( 7, 1 ) == 7 );
        check( pascal_triangle( 7, 2 ) == 21 );
        check( pascal_triangle( 7, 3 ) == 35 );
        check( pascal_triangle( 7, 4 ) == 35 );
        check( pascal_triangle( 7, 5 ) == 21 );
        check( pascal_triangle( 7, 6 ) == 7 );
        check( pascal_triangle( 7, 7 ) == 1 );

        check( pascal_triangle( 8, 0 ) == 1 );
        check( pascal_triangle( 8, 1 ) == 8 );
        check( pascal_triangle( 8, 2 ) == 28 );
        check( pascal_triangle( 8, 3 ) == 56 );
        check( pascal_triangle( 8, 4 ) == 70 );
        check( pascal_triangle( 8, 5 ) == 56 );
        check( pascal_triangle( 8, 6 ) == 28 );
        check( pascal_triangle( 8, 7 ) == 8 );
        check( pascal_triangle( 8, 8 ) == 1 );
    }
}

test( "mod tests" )
{
    // C! truncated (default), takes the sign of x like fmod
    check( mod( 7.0, 4.0 ) == 3.0 );
    check( mod( -7.0, 4.0 ) == -3.0 );
    check( mod( 7.0, -4.0 ) == 3.0 );
    check( mod( -7.0, -4.0 ) == -3.0 );
    check( is_zero( mod( 8.0, 4.0 ) ) );

    // C! floored, takes the sign of y
    check( mod( 7.0, 4.0, true ) == 3.0 );
    check( mod( -7.0, 4.0, true ) == 1.0 );
    check( mod( 7.0, -4.0, true ) == -1.0 );
    check( mod( -7.0, -4.0, true ) == -3.0 );
    check( is_zero( mod( -8.0, 4.0, true ) ) );
    check( mod( -1.0, 4.0, true ) == 3.0 );

    // C! ints
    check( mod( -7, 4 ) == -3 );
    check( mod( -7, 4, true ) == 1 );
}

test( "Countdown randomize tests" )
{
    Countdown countdown( 10.0 );

    for_range( i, 50 )
    {
        countdown.randomize( );

        // C! somewhere within the duration, which stays the same
        check( !is_neg( countdown.remaining( ) ) );
        check( countdown.remaining( ) <= 10.0 );
        check( countdown.duration( ) == 10.0 );
    }

    Countdown empty;
    empty.randomize( );
    check( empty.completed( ) );
}

test( "map tests" )
{
    omap<int, int> ordered;
    umap<int, int> unordered;

    // C! operator[ ] adds a default value for a new key, then works like at( )
    ordered[ 3 ] = 7;
    unordered[ 3 ] = 7;

    check( ordered.contains( 3 ) );
    check( ordered.at( 3 ) == 7 );
    check( ordered[ 3 ] == 7 );
    check( ordered.size( ) == 1 );

    check( unordered.contains( 3 ) );
    check( unordered.at( 3 ) == 7 );
    check( unordered[ 3 ] == 7 );
    check( unordered.size( ) == 1 );

    // C! insert doesn't replace an existing value, same as the std containers
    check( ordered.insert( 4, 9 ) == 9 );
    check( ordered.insert( 4, 1 ) == 9 );
    check( unordered.insert( 4, 9 ) == 9 );
    check( unordered.insert( 4, 1 ) == 9 );

    check( ordered.size( ) == 2 );
    check( unordered.size( ) == 2 );

    ordered.clear( );
    unordered.clear( );

    check( ordered.empty( ) );
    check( unordered.empty( ) );
    check_not( ordered.contains( 3 ) );
    check_not( unordered.contains( 3 ) );
}

test( "queue tests" )
{
    queue<int> q;

    // C! push gives back the element it just added, not the front
    q.push( 1 );
    q.push( 2 ) = 5;

    check( q.front( ) == 1 );
    check( q.back( ) == 5 );
    check( q.pop( ) == 1 );
    check( q.pop( ) == 5 );
}

test( "remove_duplicates tests" )
{
    // C! the first of each stays, in order
    varray<int> v = { 3, 1, 3, 2, 1, 3 };
    v.remove_duplicates( );
    check( v == varray<int>( { 3, 1, 2 } ) );

    list<int> l = { 3, 1, 3, 2, 1, 3 };
    l.remove_duplicates( );
    check( l == list<int>( { 3, 1, 2 } ) );

    varray<int> none;
    check( none.remove_duplicates( ).empty( ) );

    // C! sets and maps can't hold a key twice, so nothing changes
    oset<int> os = { 1, 2 };
    uset<int> us = { 1, 2 };
    check( os.remove_duplicates( ).size( ) == 2 );
    check( us.remove_duplicates( ).size( ) == 2 );

    omap<int, int> om;
    umap<int, int> um;
    om.insert( 1, 5 );
    om.insert( 2, 5 );
    um.insert( 1, 5 );
    um.insert( 2, 5 );
    check( om.remove_duplicates( ).size( ) == 2 );
    check( um.remove_duplicates( ).size( ) == 2 );
}

test( "carray tests" )
{
    // C! carray had never been used, so it hadn't been compiled: these make sure it builds and behaves like varray
    carray<int, 4> a = { 3, 1, 3 };

    check( a.size( ) == 4 );
    check( a[ 0 ] == 3 );
    check( is_zero( a[ 3 ] ) ); // C! the rest of a short list is value-initialized
    check( a.contains( 1 ) );
    check( !a.contains( 7 ) );
    check( a.count( 3 ) == 2 );

    a[ 3 ] = 9;
    check( a.back( ) == 9 );
    check( a.front( ) == 3 );

    check( is_zero( a.find_index_first( 3 ) ) );
    check( a.find_index_last( 3 ) == 2 );
    check( a.find_index_last( 7 ) == 4 ); // C! not found gives the size, like find_index_first

    carray<int, 4> sorted = a.sorted( [ ] ( int in_x1, int in_x2 ) { return( in_x1 < in_x2 ); } );
    check( sorted.front( ) == 1 );
    check( sorted.back( ) == 9 );

    carray<int, 2> zeros;
    check( is_zero( zeros[ 0 ] ) );
    check( is_zero( zeros[ 1 ] ) );

    varray<int> v = { 3, 1, 3 };
    check( v.find_index_last( 3 ) == 2 );
}

test( "Stringable tests" )
{
    // C! what a Stringable turns into, with and without its label
    check( stringify( Point( 1.0, 2.0 ) ) == "[ 1.000000 : 2.000000 ]" );
    check( Point( 1.0, 2.0 ).stringified_with_space( ) == "[ 1.000000 : 2.000000 ] " );

    bool labeling = Stringable::labeling;

    Stringable::labeling = true;
    check( string( Point( 1.0, 2.0 ) ) == "Point[ 1.000000 : 2.000000 ]" );
    check( string( Planc( 3.0 ) ) == "Planc[ 3.000000 ]" );

    Stringable::labeling = false;
    check( string( Point( 1.0, 2.0 ) ) == "[ 1.000000 : 2.000000 ]" );

    Stringable::labeling = labeling;
}

test( "Assert message tests" )
{
    // C! asserts are disabled while tests run, so failing ones only record their message
    check( string( Assert( ( false ), "value %d is %s", 5, "bad" ) ) == "value 5 is bad" );
    check( string( Assert( ( false ), "plain message" ) ) == "plain message" );
    check( string( Assert( ( true ), "value %d", 5 ) ) == nostring );
}

// C! the tests' sound: its file isn't there and there's no audio device while they run, so playing it does nothing
var_const( string TEST_SOUND = ( "test-boop.wav" ) );

test( "Sound" )
{
    SoundFX boop( TEST_SOUND );

    check( boop.filename( ) == "sounds/test-boop.wav" ); // C! every sound is in res/sounds/
    check( equal( boop.volume( ), 1.0 ) ); // C! full, unless it's set

    boop.volume( 0.5 );
    check( equal( boop.volume( ), 0.5 ) );

    // C! each setter gives the clip back, so they chain
    check( equal( SoundFX( TEST_SOUND ).volume( 0.25 ).pitch( -1.0 ).pitch( ), -1.0 ) );
    check( equal( SoundFX( TEST_SOUND ).pitch( 2.0 ).volume( 0.75 ).volume( ), 0.75 ) );
    check( is_zero( boop.pitch( ) ) ); // C! as recorded, unless it's set
}

class TestListener : public Listener { public: virtual Point listening_position( ) const override { return( ORIGIN ); } };

test( "Music" )
{
    Music music( TEST_SOUND );

    check( music.filename( ) == "music/test-boop.wav" ); // C! music is in res/music/
    check( equal( music.volume( ), 1.0 ) ); // C! full, unless it's set
    check( music.loop( ) ); // C! it loops, unless it's set not to
    check( !music.loaded( ) ); // C! the file isn't there

    check( !music.volume( 0.5 ).loop( false ).loop( ) ); // C! the setters chain
    check( equal( music.volume( ), 0.5 ) );
}

test( "Hearing a sound" )
{
    SoundFX boop( TEST_SOUND, 100.0, 50.0 );
    check( !boop.global( ) ); // C! it has an umbra

    check( is_one( boop.reach( Planc( 100.0 ) ) ) ); // C! the edge of its umbra: full volume
    check( equal( boop.reach( Planc( 125.0 ) ), 0.5 ) ); // C! halfway through its penumbra: half volume
    check( is_zero( boop.reach( Planc( 150.0 ) ) ) ); // C! the far edge: nothing
    check( is_zero( boop.reach( Planc( 151.0 ) ) ) );

    boop.hearing_penumbra( 0.0 );
    check( is_zero( boop.reach( Planc( 101.0 ) ) ) ); // C! no penumbra: nothing past the umbra

    check( is_one( boop.global( true ).reach( Planc( 1000000.0 ) ) ) ); // C! a global sound reaches everywhere
    check( SoundFX( TEST_SOUND ).global( ) ); // C! no umbra: global

    TestListener listener;
    check( is_one( listener.volume( ) ) ); // C! full, unless it's set
    check( equal( listener.active( false ).volume( 0.25 ).volume( ), 0.25 ) ); // C! the setters chain
    check( !listener.active( ) );
}

test( "Segmented path" )
{
    const Line c_line( Point( 0.0, 0.0 ), Point( 100.0, 0.0 ) );

    const Path c_straight = Random::rSegment( c_line, 4, 0.0 ); // C! no offset: the points are exactly where the cuts are
    check( c_straight.size( ) == 4 );
    for_range( i, 4 ) { check( c_straight[ i ].pt1( ) == Point( 25.0 * dec( i ), 0.0 ) ); }
    check( c_straight.back( ).pt2( ) == Point( 100.0, 0.0 ) );

    const Path c_jagged = Random::rSegment( c_line, 4, 5.0 );
    check( c_jagged.front( ).pt1( ) == Point( 0.0, 0.0 ) ); // C! the ends stay put
    check( c_jagged.back( ).pt2( ) == Point( 100.0, 0.0 ) );
    for_range( i, 3 ) { check( !greater( dec( c_jagged[ i ].pt2( ).distance_to( Point( 25.0 * dec( i + 1 ), 0.0 ) ) ), 5.0 ) ); } // C! each point between them within the offset of its cut

    check( Random::rSegment( c_line, 1, 5.0 ).size( ) == 1 ); // C! one segment: the line itself
}

test( "Random point in a polygon" )
{
    // C! an L: a 2 by 1 bar along the bottom and a 1 by 1 square on its left end, so 2 thirds of the area is in the bar's right half
    // C! or left half, and a third in the square on top
    const Polygon c_l( varray<Point>( { Point( 0.0, 0.0 ), Point( 2.0, 0.0 ), Point( 2.0, 1.0 ), Point( 1.0, 1.0 ), Point( 1.0, 2.0 ), Point( 0.0, 2.0 ) } ) );

    uint on_top = 0;
    const uint c_tries = 3000;
    for_range( i, c_tries )
    {
        const Point c_pt = Random::rPointPolygon( c_l );
        check( c_l.contains( c_pt ) ); // C! always inside
        if( c_pt.y( ) > 1.0 ) { on_top++; }
    }

    check( equal( dec( on_top ) / dec( c_tries ), 1.0 / 3.0, 0.05 ) ); // C! spread by area: about a third land in the top square
}

test( "Container statistics" )
{
    const varray<dec> c_values = { 4.0, 1.0, 3.0, 1.0, 6.0 };

    check( equal( min( c_values ), 1.0 ) );
    check( equal( max( c_values ), 6.0 ) );
    check( equal( range( c_values ), 5.0 ) );
    check( equal( sum( c_values ), 15.0 ) );
    check( equal( product( c_values ), 72.0 ) );
    check( equal( mean( c_values ), 3.0 ) );
    check( equal( median( c_values ), 3.0 ) ); // C! the middle of 1, 1, 3, 4, 6
    check( equal( mode( c_values ), 1.0 ) );

    check( equal( median( varray<dec>( { 4.0, 1.0, 3.0, 6.0 } ) ), 3.5 ) ); // C! an even count: between the middle two

    // C! the same functions take any container
    const list<dec> c_list = { 2.0, 8.0, 5.0 };
    check( equal( max( c_list ), 8.0 ) );
    check( equal( median( c_list ), 5.0 ) );

    queue<dec> waiting = { 3.0, 9.0, 3.0 };
    check( equal( sum( waiting ), 15.0 ) );
    check( equal( mode( waiting ), 3.0 ) );

    stack<dec> pile;
    pile.push( 7.0 );
    pile.push( 2.0 );
    check( equal( min( pile ), 2.0 ) );

    const uset<int> c_set = { 5, 3, 9 };
    check( min( c_set ) == 3 );
    check( max( c_set ) == 9 );
}

test( "Rounding to epsilon" )
{
    // C! every check gives its epsilon itself, so none depends on DEFAULT_EPSILON, a dev param
    check( decimal_places( 0.0001 ) == 4 );
    check( decimal_places( 0.01 ) == 2 );
    check( decimal_places( 1.0 ) == 0 );
    check( decimal_places( 0.5 ) == 1 );

    check( equal( round_epsilon( 1.0 / 3.0, 0.0001 ), 0.3333, 0.0001 ) );
    check( equal( round_epsilon( 2.00006, 0.0001 ), 2.0001, 0.0001 ) );
    check( !equal( round_epsilon( 2.00006, 0.0001 ), 2.0, 0.0001 ) ); // C! rounded up a step, not down
    check( equal( round_epsilon( -0.00004, 0.0001 ), 0.0, 0.0001 ) );

    // C! written to epsilon's places at most, without zeros on the end
    check( serialized_dec( 12.0, 0.0001 ) == "12" );
    check( serialized_dec( -0.1, 0.0001 ) == "-0.1" );
    check( serialized_dec( 1.0 / 3.0, 0.0001 ) == "0.3333" );
    check( serialized_dec( 2.00006, 0.0001 ) == "2.0001" );
    check( serialized_dec( -0.00004, 0.0001 ) == "0" ); // C! not -0
    check( equal( stod( serialized_dec( 1.0 / 3.0, 0.0001 ) ), 1.0 / 3.0, 0.0001 ) ); // C! read back, it compares equal to what was written
}

test( "Input serialization" )
{
    // C! each input read back from its own text gives the same text, so a recording plays back as it was
    varray<string> texts = { KeyInput( 'a', KeyInput::PRESSED ).serialized( ),
                             MouseInput( MouseInput::LEFT_BUTTON, MouseInput::MOVE, Vector( 12.0, -0.1 ) ).serialized( ),
                             ControllerButtonInput( ControllerButtonInput::L2_BUTTON, ControllerButtonInput::RELEASED ).serialized( ),
                             ControllerJoystickInput( ControllerJoystickInput::LEFT_JOYSTICK, Vector( 0.3, 1.0 / 3.0 ) ).serialized( ),
                             ControllerJoystickInput( ControllerJoystickInput::RIGHT_JOYSTICK ).serialized( ) };

    for_each_const( text, texts )
    {
        ptr<Input> input = Input::deserialized( text );
        check( !is_null( input ) );
        if( !is_null( input ) ) { check( input->serialized( ) == text ); }
        safe_delete( input );
    }

    check( texts[ 0 ] == "KEY 97 PRESSED" );
    check( texts[ 4 ] == "JOYSTICK RIGHT_JOYSTICK DEAD" );

    check( is_null( Input::deserialized( "NOTHING 1 2" ) ) );
    check( is_null( Input::deserialized( "KEY" ) ) );
}

} // namespace axn

#endif /* TestsUtil_hpp */
