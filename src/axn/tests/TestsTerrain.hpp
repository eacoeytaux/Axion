#ifndef TestsTerrain_hpp
#define TestsTerrain_hpp

#include "axn/tests/Tests.hpp"

#include "axn/world/Terrain.hpp"

namespace axn
{

// C! Polygon is written as geometry::Polygon because windows.h (pulled in by Catch) declares a Polygon function

// C! builds prev -> corner -> next and checks the bumpers of both edges and the corner vertex
// C! bumpers must meet end to end, and a hitbox centered anywhere on them must never be inside the terrain
static void check_corner_bumpers( Hitbox cref in_hitbox, Point cref in_prev, Point cref in_corner, Point cref in_next )
{
    const Planc EPSILON = 0.001;

    ptr<Terrain::Vertex> prev = new Terrain::Vertex( nullptr, in_prev );
    ptr<Terrain::Vertex> corner = new Terrain::Vertex( nullptr, in_corner );
    ptr<Terrain::Vertex> next = new Terrain::Vertex( nullptr, in_next );

    ptr<Terrain::Edge> edge1 = new Terrain::Edge( nullptr, prev, corner );
    ptr<Terrain::Edge> edge2 = new Terrain::Edge( nullptr, corner, next );

    Path bumper1 = edge1->bumpers( in_hitbox ).path( );
    Path bumper2 = edge2->bumpers( in_hitbox ).path( );
    varray<Point> arc = corner->bumpers( in_hitbox ).path( ).points( );

    varray<Point> hitbox_pts = in_hitbox.polygon( false ).points( );

    Vector n1 = VectorA( edge1->normal( ), 1.0 );
    Vector n2 = VectorA( edge2->normal( ), 1.0 );

    lambdaf( dot_product )( Vector cref in_v1, Vector cref in_v2 ) { return( ( in_v1.dx( ) * in_v2.dx( ) ) + ( in_v1.dy( ) * in_v2.dy( ) ) ); };
    lambdaf( close_to )( Point cref in_pt1, Point cref in_pt2 ) { return( in_pt1.distance_to( in_pt2 ) < EPSILON ); };
    lambdaf( is_endpoint )( Path cref in_path, Point cref in_pt ) { return( close_to( in_path.front( ).pt1( ), in_pt ) || close_to( in_path.back( ).pt2( ), in_pt ) ); };

    // C! the terrain on the inside of the corner, convex corners are the intersection of the two edges' half planes
    bool convex = ( dec( edge1->normal( ).min_distance( edge2->normal( ) ) ) < 0.0 );

    lambdaf( in_terrain )( Point cref in_pt )
    {
        bool behind1 = ( dot_product( Vector( in_corner, in_pt ), n1 ) < -EPSILON );
        bool behind2 = ( dot_product( Vector( in_corner, in_pt ), n2 ) < -EPSILON );

        return( convex ? ( behind1 && behind2 ) : ( behind1 || behind2 ) );
    };

    lambdaf( hitbox_in_terrain )( Point cref in_center )
    {
        for_each_const( pt, hitbox_pts ) { return_true_if( in_terrain( in_center + pt ) ); }
        return( false );
    };

    // C! edges, a hitbox on an edge's bumper touches the edge's line without crossing it
    lambdaf( check_edge_bumper )( Path cref in_bumper, Vector cref in_normal )
    {
        require( in_bumper.line_count( ) == 1 );

        for_each_const( center, varray<Point>( { in_bumper.front( ).pt1( ), in_bumper.back( ).pt2( ) } ) )
        {
            dec deepest = INFINITY;

            for_each_const( pt, hitbox_pts )
            {
                dec depth = dot_product( Vector( in_corner, center + pt ), in_normal );
                deepest = min( deepest, depth );
            }

            check( abs( deepest ) < EPSILON );
        }
    };

    check_edge_bumper( bumper1, n1 );
    check_edge_bumper( bumper2, n2 );

    bool concave = ( dec( edge1->normal( ).min_distance( edge2->normal( ) ) ) > 0.0 );

    // C! concave corners have no arc of their own, the edges' bumpers cross (or touch) and that is the corner
    if( concave )
    {
        check( is_empty( arc ) );
        check( bumper1.front( ).intersects( bumper2.front( ) ) );
    }
    // C! edges meet the vertex end to end
    else if( is_empty( arc ) )
    {
        Point end = ( is_endpoint( bumper2, bumper1.front( ).pt1( ) ) ? bumper1.front( ).pt1( ) : bumper1.back( ).pt2( ) );
        check( is_endpoint( bumper1, end ) );
        check( is_endpoint( bumper2, end ) );
    }
    else
    {
        check( is_endpoint( bumper1, arc.front( ) ) );
        check( is_endpoint( bumper2, arc.back( ) ) );

        // C! the vertex arc wraps around the outside of convex corners
        if( convex )
        {
            for_each_const( pt, arc )
            {
                check_not( hitbox_in_terrain( pt ) );
            }
        }
    }

    delete edge1;
    delete edge2;
    delete prev;
    delete corner;
    delete next;
}

test( "Terrain bumper tests" )
{
    varray<geometry::Polygon> hitboxes = {
        geometry::Polygon::rectangle( 10.0, 20.0 ),
        geometry::Polygon::rectangle( 10.0, 20.0, Angle::fraction( 16.0 ) ),
        geometry::Polygon::equilateral( 3, 12.0 ),
        geometry::Polygon::equilateral( 6, 8.0, Angle::fraction( 24.0 ) ),
        geometry::Polygon( { Point( -5.0, -10.0 ), Point( -5.0, +10.0 ), Point( +5.0, +10.0 ), Point( +5.0, -10.0 ) } ) // C! clockwise
    };

    // C! turns of the terrain at the corner, negative is a right turn (convex), zero is a straight line
    varray<dec> turns = { 0.0, -0.25, -0.785, -1.571, -2.356, -3.0, 0.785, 1.571 };

    for_each_const( hitbox_polygon, hitboxes )
    {
        Hitbox hitbox( hitbox_polygon );

        for_range( a, 12 )
        {
            Angle direction1( dec( a ) * ( TAU / 12.0 ) );

            for_each_const( turn, turns )
            {
                Angle direction2 = direction1 + Angle( turn );

                Point corner = Point( 50.0, 50.0 );
                Point prev = corner - VectorA( direction1, 100.0 );
                Point next = corner + VectorA( direction2, 100.0 );

                check_corner_bumpers( hitbox, prev, corner, next );
            }
        }
    }

    // C! a box on flat ground stands half its height above the ground
    {
        ptr<Terrain::Vertex> a = new Terrain::Vertex( nullptr, Point( 0.0, 0.0 ) );
        ptr<Terrain::Vertex> b = new Terrain::Vertex( nullptr, Point( 100.0, 0.0 ) );
        ptr<Terrain::Edge> ab = new Terrain::Edge( nullptr, a, b );

        Line line = ab->bumpers( Hitbox( geometry::Polygon::rectangle( 10.0, 20.0 ) ) ).path( ).front( );

        check( equal( line.pt1( ).y( ), 10.0 ) );
        check( equal( line.pt2( ).y( ), 10.0 ) );

        delete ab;
        delete a;
        delete b;
    }
}

} // namespace axn

#endif /* TestsTerrain_hpp */
