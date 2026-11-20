#ifndef TestsSkeletonRig_hpp
#define TestsSkeletonRig_hpp

#include "axn/tests/Tests.hpp"

#include "axn/world/SkeletonRig.hpp"

namespace axn
{

// C! an arm and a head hanging off the origin: the shoulder above it, the elbow and hand out to the right of the shoulder, and the head higher up
enum TestRigPoint
{
    NO_RIG_PT = 0,
    RIG_SHOULDER,
    RIG_ELBOW,
    RIG_HAND,
    RIG_HEAD,
};

test( "SkeletonRig tests" )
{
    SkeletonRig<TestRigPoint> rig( nullptr, Point( 10.0, 20.0 ) );

    rig.add( RIG_SHOULDER, VectorY( 5.0 ) );
    rig.add( RIG_ELBOW, RIG_SHOULDER, VectorX( 4.0 ) );
    rig.add( RIG_HAND, RIG_ELBOW, VectorX( 3.0 ) );
    rig.add( RIG_HEAD, VectorY( 10.0 ) );

    section( "points hang off their parents, all the way back to the object's position" )
    {
        check( rig.points( ).size( ) == 4 );
        check( rig.has( NO_RIG_PT ) ); // C! the origin is always there
        check( rig.parent( RIG_HAND ) == RIG_ELBOW );
        check( rig.parent( RIG_SHOULDER ) == NO_RIG_PT );

        check( rig.point_position( NO_RIG_PT ) == Point( 10.0, 20.0 ) );
        check( rig.point_position( RIG_SHOULDER ) == Point( 10.0, 25.0 ) );
        check( rig.point_position( RIG_ELBOW ) == Point( 14.0, 25.0 ) );
        check( rig.point_position( RIG_HAND ) == Point( 17.0, 25.0 ) );
        check( rig.point_position( RIG_HEAD ) == Point( 10.0, 30.0 ) );
    }

    section( "moving the object moves every point" )
    {
        rig.position( Point( 0.0, 0.0 ) );

        check( rig.point_position( RIG_HAND ) == Point( 7.0, 5.0 ) );
        check( rig.point_position( RIG_HEAD ) == Point( 0.0, 10.0 ) );
    }

    section( "turning a point carries the points hanging off it, not the others" )
    {
        rig.local_rotation( RIG_SHOULDER, RIGHT );

        check( rig.point_rotation( RIG_SHOULDER ) == RIGHT );
        check( rig.point_rotation( RIG_HAND ) == RIGHT ); // C! the elbow and hand add nothing of their own
        check( rig.point_rotation( RIG_HEAD ) == A0 );

        check( rig.point_position( RIG_SHOULDER ) == Point( 10.0, 25.0 ) ); // C! a point's own rotation doesn't move it, only what hangs off it
        check( rig.point_position( RIG_ELBOW ) == Point( 10.0, 29.0 ) );
        check( rig.point_position( RIG_HAND ) == Point( 10.0, 32.0 ) );
        check( rig.point_position( RIG_HEAD ) == Point( 10.0, 30.0 ) );

        // C! the rotations add up down the chain
        rig.local_rotation( RIG_ELBOW, RIGHT );
        check( rig.point_rotation( RIG_HAND ) == PI );
        check( rig.point_position( RIG_HAND ) == Point( 7.0, 29.0 ) );
    }

    section( "turning the object turns the whole rig around its position" )
    {
        rig.rotate( RIGHT );

        check( rig.rotation( ) == RIGHT );
        check( rig.point_rotation( RIG_HEAD ) == RIGHT );
        check( rig.point_position( NO_RIG_PT ) == Point( 10.0, 20.0 ) );
        check( rig.point_position( RIG_SHOULDER ) == Point( 5.0, 20.0 ) );
        check( rig.point_position( RIG_HAND ) == Point( 5.0, 27.0 ) );
        check( rig.point_position( RIG_HEAD ) == Point( 0.0, 20.0 ) );

        // C! setting the origin's own rotation does the same
        rig.local_rotation( NO_RIG_PT, A0 );
        check( rig.point_position( RIG_HEAD ) == Point( 10.0, 30.0 ) );
    }

    section( "changing an offset moves the point and what hangs off it" )
    {
        rig.offset( RIG_ELBOW, VectorX( 6.0 ) );

        check( rig.offset( RIG_ELBOW ) == VectorX( 6.0 ) );
        check( rig.point_position( RIG_ELBOW ) == Point( 16.0, 25.0 ) );
        check( rig.point_position( RIG_HAND ) == Point( 19.0, 25.0 ) );
    }

    section( "adding again replaces a point, and can move it to another parent" )
    {
        rig.add( RIG_HAND, RIG_SHOULDER, VectorY( 1.0 ) );

        check( rig.points( ).size( ) == 4 );
        check( rig.parent( RIG_HAND ) == RIG_SHOULDER );
        check( rig.point_position( RIG_HAND ) == Point( 10.0, 26.0 ) );
    }

    section( "adds that would break the tree are refused" )
    {
        // C! the shoulder hanging off its own hand would loop forever
        rig.add( RIG_SHOULDER, RIG_HAND, VectorX( 1.0 ) );
        check( rig.parent( RIG_SHOULDER ) == NO_RIG_PT );

        // C! nor can a point hang off itself, or off a point that hasn't been added, and the origin can't be added
        rig.add( RIG_ELBOW, RIG_ELBOW, VectorX( 1.0 ) );
        check( rig.parent( RIG_ELBOW ) == RIG_SHOULDER );

        SkeletonRig<TestRigPoint> empty( nullptr );
        empty.add( RIG_HAND, RIG_ELBOW, VectorX( 1.0 ) );
        empty.add( NO_RIG_PT, VectorX( 1.0 ) );
        check( empty.points( ).empty( ) );
        check_not( empty.has( RIG_HAND ) );

        // C! asking about a point that isn't there gives the origin
        check( empty.point_position( RIG_HAND ) == ORIGIN );
        check( empty.point_rotation( RIG_HAND ) == A0 );
    }

    #if defined( AXN_DEBUG )
    section( "the debug overlay draws a line for every point, but not the origin, only while skeletons are shown" )
    {
        const bool c_showing = Settings::get( Settings::DEBUG_SKELETONS ); // C! put back afterwards, so the game's setting isn't changed by the tests

        // C! everything in the overlay, whatever its color: the rig's lines are what changes between skeletons shown and not, so the test
        // C! doesn't depend on SKELETON_RIG_COLOR (a dev param) being a color nothing else in the overlay uses
        lambdaf( overlay_polygons )( )
        {
            uint polys = 0;

            for_each_const( node, rig.debug_overlay( ).nodes( ) ) { polys += node.colored_polys.size( ); }

            return( polys );
        };

        Settings::set( Settings::DEBUG_SKELETONS, false );
        const uint c_without = overlay_polygons( );

        Settings::set( Settings::DEBUG_SKELETONS, true );
        check( overlay_polygons( ) == ( c_without + 4 ) ); // C! a line to the parent for each point

        Settings::set( Settings::DEBUG_SKELETONS, c_showing );
    }
    #endif
}

} // namespace axn

#endif /* TestsSkeletonRig_hpp */
