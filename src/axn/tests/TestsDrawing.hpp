#ifndef TestsDrawing_hpp
#define TestsDrawing_hpp

#include "axn/tests/Tests.hpp"

#include "axn/graphics/Drawing.hpp"

namespace axn
{

// C! the stencil a polygon is drawn under, as points on screen: off, or the crops minus the holes (everything minus the holes when there are no crops)
struct DrawnStencil
{

    bool active = false;

    vvarray<Point> crops;
    vvarray<Point> holes;

};

// C! a polygon as the camera ends up drawing it: its points on screen (for an outline, the corners of the rectangle drawn along each side),
// C! its colors, and the stencil it's drawn under
struct DrawnPolygon
{

    varray<Point> pts;
    varray<Color> colors;

    bool outline = false;
    bool preserve_thickness = false;

    DrawnStencil stencil;

};

struct DrawnState
{

    varray<DrawnPolygon> polys;
    DrawnStencil stencil;

};

static vvarray<Point> screen_points( varray<Polygon> cref in_polys, Transform cref in_transform )
{
    vvarray<Point> polys;

    for_each_const( poly, in_polys )
    {
        varray<Point> pts;

        for_each_const( pt, poly.points( true ) ) { pts.insert_back( in_transform.apply( pt ) ); }

        polys.insert_back( pts );
    }

    return( polys );
}

// C! does what render_nodes in Camera.cpp does to in_nodes[ 0 .. in_count - 1 ], in the same order and under the same transforms:
// C!   - each node redraws every crop and hole so far under its own transform, or turns the stencil off when there are none
// C!   - nothing restores the stencil afterwards, so it carries on to whatever is drawn next
static DrawnState drawn_state( uint in_count,
                               Drawing::Nodes cref in_nodes,
                               varray<Polygon> cref in_crops,
                               varray<Polygon> cref in_holes,
                               Transform cref in_outer,
                               DrawnState in_state )
{
    return_if( !in_count, in_state );

    Drawing::Node cref node = in_nodes[ in_count - 1 ];
    Transform inner = in_outer * node.transform;

    varray<Polygon> crops = in_crops;
    varray<Polygon> holes = in_holes;
    crops.insert_back( node.crop_polys );
    holes.insert_back( node.hole_polys );

    in_state.stencil = DrawnStencil( );

    if( !crops.empty( ) || !holes.empty( ) )
    {
        in_state.stencil.active = true;
        in_state.stencil.crops = screen_points( crops, inner );
        in_state.stencil.holes = screen_points( holes, inner );
    }

    in_state = drawn_state( in_count - 1, in_nodes, crops, holes, inner, in_state );

    for_each_const( colored_poly, node.colored_polys )
    {
        DrawnPolygon poly;

        varray<Point> cref pts = colored_poly.poly.points( true );

        if( colored_poly.thickness == FILLED )
        {
            for_each_const( pt, pts ) { poly.pts.insert_back( inner.apply( pt ) ); }
        }
        else
        {
            // C! the same rectangles render_colored_polygon draws, at a zoom of 1
            for_range( i, pts.size( ) )
            {
                Line line = Line( pts[ i ? ( i - 1 ) : ( pts.size( ) - 1 ) ], pts[ i ] );
                Vector line_vector( line.pt1( ), line.pt2( ) );

                Polygon rectangle = Polygon::rectangle( ( line_vector.magnitude( ) + ( colored_poly.extend_lines ? colored_poly.thickness : 0.0 ) ), colored_poly.thickness, half( line_vector ), Angle( line.angle( ) ) );

                for_each_const( pt, rectangle.points( ) ) { poly.pts.insert_back( inner.apply( pt ) ); }
            }
        }

        poly.colors = colored_poly.colors;
        poly.outline = ( colored_poly.thickness != FILLED );
        poly.preserve_thickness = colored_poly.preserve_thickness;
        poly.stencil = in_state.stencil;

        in_state.polys.insert_back( poly );
    }

    for_each_const( drawing, node.drawings ) { in_state = drawn_state( drawing.size( ), drawing, crops, holes, inner, in_state ); }

    return( in_state );
}

static varray<DrawnPolygon> drawn_polygons( Drawing cref in_drawing )
{
    Drawing::Nodes nodes = in_drawing.nodes( );

    return( drawn_state( nodes.size( ), nodes, { }, { }, IDENTITY_TRANSFORM, DrawnState( ) ).polys );
}

static bool same_points( varray<Point> cref in_pts1, varray<Point> cref in_pts2 )
{
    return_false_if( in_pts1.size( ) != in_pts2.size( ) );

    for_range( i, in_pts1.size( ) ) { return_false_if( in_pts1[ i ] != in_pts2[ i ] ); }

    return( true );
}

static bool same_point_set( varray<Point> cref in_pts1, varray<Point> cref in_pts2 )
{
    return_false_if( in_pts1.size( ) != in_pts2.size( ) );

    for_each_const( pt1, in_pts1 )
    {
        bool found = false;

        for_each_const( pt2, in_pts2 ) { if( pt1 == pt2 ) { found = true; break; } }

        return_false_if( !found );
    }

    return( true );
}

// C! every polygon in one is also in the other (the stencil is a union, so order and repeats don't matter)
static bool same_polygon_set( vvarray<Point> cref in_polygons1, vvarray<Point> cref in_polygons2 )
{
    for_each_const( pts1, in_polygons1 )
    {
        bool found = false;

        for_each_const( pts2, in_polygons2 ) { if( same_points( pts1, pts2 ) ) { found = true; break; } }

        return_false_if( !found );
    }

    for_each_const( pts2, in_polygons2 )
    {
        bool found = false;

        for_each_const( pts1, in_polygons1 ) { if( same_points( pts1, pts2 ) ) { found = true; break; } }

        return_false_if( !found );
    }

    return( true );
}

static bool same_stencil( DrawnStencil cref in_stencil1, DrawnStencil cref in_stencil2 )
{
    return_false_if( in_stencil1.active != in_stencil2.active );
    return_true_if( !in_stencil1.active );

    return( same_polygon_set( in_stencil1.crops, in_stencil2.crops ) && same_polygon_set( in_stencil1.holes, in_stencil2.holes ) );
}

// C! in_compare_stencils is false where optimize( ) is expected to drop crops or holes that don't change anything (the stencil differs but nothing it covers does)
static bool same_polygons( varray<DrawnPolygon> cref in_polygons1, varray<DrawnPolygon> cref in_polygons2, bool in_compare_stencils = true )
{
    return_false_if( in_polygons1.size( ) != in_polygons2.size( ) );

    for_range( i, in_polygons1.size( ) )
    {
        DrawnPolygon cref s1 = in_polygons1[ i ];
        DrawnPolygon cref s2 = in_polygons2[ i ];

        return_false_if( ( s1.colors != s2.colors ) || ( s1.outline != s2.outline ) || ( s1.preserve_thickness != s2.preserve_thickness ) );
        return_false_if( in_compare_stencils && !same_stencil( s1.stencil, s2.stencil ) );

        // C! a mirror can change the order an outline's rectangle corners come in, but not where they are
        return_false_if( s1.outline ? !same_point_set( s1.pts, s2.pts ) : !same_points( s1.pts, s2.pts ) );
    }

    return( true );
}

static uint crop_and_hole_count( Drawing::Nodes cref in_nodes )
{
    uint count = 0;

    for_each_const( node, in_nodes )
    {
        count += node.crop_polys.size( ) + node.hole_polys.size( );

        for_each_const( drawing, node.drawings ) { count += crop_and_hole_count( drawing ); }
    }

    return( count );
}

static Point random_point( )
{
    return( Point( Random::rdec( -20.0, +20.0 ), Random::rdec( -20.0, +20.0 ) ) );
}

static Polygon random_polygon( )
{
    Polygon poly;

    switch( Random::rint( 2 ) )
    {
        default:
        {
            poly = Polygon::square( Random::rdec( 1.0, 10.0 ), random_point( ), Random::rAngle( ) );
        }

        b_case( 1 ) :
        {
            poly = Polygon::triangle( random_point( ), random_point( ), random_point( ) );
        }

        b_case( 2 ) :
        {
            poly = Polygon::circle( Random::rdec( 1.0, 10.0 ), random_point( ) );
        }
    }

    return( poly );
}

// C! a drawing built from random draws, transforms, nested drawings, and (when in_crops) crops and holes, the way game code builds them
static Drawing random_drawing( uint in_depth, bool in_crops )
{
    Drawing drawing;

    do_count( Random::rint( 1, 8 ) )
    {
        switch( Random::rint( 10 ) )
        {
            default:
            {
                drawing.draw( Random::rColor( ), random_polygon( ) );
            }

            b_case( 1 ) :
            {
                drawing.draw( Random::rColor( ), random_polygon( ), Random::rdec( 0.5, 3.0 ), Random::rbool( ) );
            }

            b_case( 2 ) :
            {
                drawing.draw( Random::rColor( ), Random::rColor( ), Line( random_point( ), random_point( ) ), Random::rdec( 0.5, 3.0 ), Random::rbool( ), Random::rbool( ) );
            }

            b_case( 3 ) :
            {
                drawing.move( Vector( ORIGIN, random_point( ) ) );
            }

            b_case( 4 ) :
            {
                drawing.rotate( Random::rAngle( ), random_point( ) );
            }

            b_case( 5 ) :
            {
                drawing.scale( Random::rdec( 0.5, 2.0 ), random_point( ) );
            }

            b_case( 6 ) :
            {
                drawing.mirror_x( );
            }

            b_case( 7 ) :
            {
                drawing.stretch( VectorX( Random::rdec( 0.5, 2.0 ) ) );
            }

            b_case( 8 ) :
            {
                if( in_depth ) { drawing.draw( random_drawing( in_depth - 1, in_crops ) ); }
            }

            b_case( 9 ) :
            {
                if( in_crops ) { drawing.add_bounds( random_polygon( ) ); }
            }

            b_case( 10 ) :
            {
                if( in_crops ) { drawing.add_hole( random_polygon( ) ); }
            }
        }
    }

    return( drawing );
}

// C! optimizes a copy of in_drawing and checks it draws the same, takes no more nodes, and that optimizing again changes nothing
// C! returns what went wrong, or nostring when nothing did
static string check_optimize( Drawing cref in_drawing )
{
    Drawing drawing = in_drawing;

    varray<DrawnPolygon> before = drawn_polygons( drawing );
    uint nodes_before = drawing.nodes( ).node_count( );
    bool compare_stencils = !drawing.nodes( ).all_identity( ); // C! crops and holes are only ever dropped when there are no transforms

    drawing.optimize( );

    varray<DrawnPolygon> after = drawn_polygons( drawing );
    uint nodes_after = drawing.nodes( ).node_count( );

    return_if( !same_polygons( before, after, false ), "polygons changed" );
    return_if( !same_polygons( before, after, compare_stencils ), "stencil changed" );
    return_if( nodes_after > nodes_before, "more nodes" );
    return_if( drawing.opaque( ) != in_drawing.opaque( ), "opaque changed" );
    return_if( !equal( drawing.bounding_box( ).width( ), in_drawing.bounding_box( ).width( ) ) || !equal( drawing.bounding_box( ).height( ), in_drawing.bounding_box( ).height( ) ), "bounding box changed" );
    return_if( drawing.bounding_box( ).center( ) != in_drawing.bounding_box( ).center( ), "bounding box moved" );

    drawing.optimize( );

    return_if( !same_polygons( after, drawn_polygons( drawing ) ), "polygons changed when optimized again" );
    return_if( drawing.nodes( ).node_count( ) != nodes_after, "node count changed when optimized again" );

    return( nostring );
}

test( "Drawing optimize tests" )
{
    section( "filled polygons under any transform become one node" )
    {
        Drawing inner;
        inner.draw( BLUE, Polygon::circle( 3.0, Point( +1.0, +2.0 ) ) );
        inner.mirror_x( );
        inner.draw( GREEN, Polygon::triangle( Point( 0.0, 0.0 ), Point( +4.0, 0.0 ), Point( 0.0, +3.0 ) ) );

        Drawing drawing;
        drawing.draw( RED, Polygon::square( 10.0 ) );
        drawing.move( Vector( ORIGIN, Point( +5.0, 0.0 ) ) );
        drawing.draw( inner );
        drawing.rotate( half( RIGHT ) );
        drawing.draw( CYAN, Polygon::rectangle( 4.0, 2.0, Point( -3.0, +1.0 ) ) );
        drawing.scale( 2.0 );
        drawing.draw( MAGENTA, Polygon::square( 1.0 ) );

        varray<DrawnPolygon> before = drawn_polygons( drawing );
        uint nodes_before = drawing.nodes( ).node_count( );

        drawing.optimize( );

        check( same_polygons( before, drawn_polygons( drawing ) ) );
        check( drawing.nodes( ).node_count( ) == 1 );
        check( nodes_before > 1 );
    }

    // C! a node's transform also moves every node before it, so the scale is kept to its own drawing here
    section( "outlines are baked under rigid transforms but keep a scale, which would change their thickness" )
    {
        Drawing inner;
        inner.draw( RED, Polygon::square( 2.0 ), 1.0 );
        inner.mirror_y( );
        inner.draw( GREEN, Line( Point( 0.0, 0.0 ), Point( +3.0, +4.0 ) ), 0.5 );

        Drawing scaled;
        scaled.draw( BLUE, Polygon::square( 3.0 ), 2.0, true );
        scaled.scale( 3.0 );

        Drawing drawing;
        drawing.draw( inner );
        drawing.rotate( RIGHT );
        drawing.draw( scaled );
        drawing.draw( CYAN, Polygon::square( 1.0 ) );
        drawing.draw( MAGENTA, Polygon::square( 5.0 ), 1.0 );

        varray<DrawnPolygon> before = drawn_polygons( drawing );
        uint nodes_before = drawing.nodes( ).node_count( );

        drawing.optimize( );

        check( same_polygons( before, drawn_polygons( drawing ) ) );
        check( drawing.nodes( ).node_count( ) == 3 ); // C! one node for the baked polygons, one nested node keeping the scale, and one for the squares drawn after it (so they're drawn over it)
        check( nodes_before > 2 );
    }

    section( "a stretched outline keeps its transform" )
    {
        Drawing drawing;
        drawing.draw( RED, Polygon::square( 2.0 ), 1.0 );
        drawing.stretch( VectorX( 2.0 ) );

        varray<DrawnPolygon> before = drawn_polygons( drawing );

        drawing.optimize( );

        check( same_polygons( before, drawn_polygons( drawing ) ) );
        check( !drawing.nodes( ).all_identity( ) );
    }

    section( "crops and holes that change nothing are removed" )
    {
        Drawing drawing;
        drawing.draw( RED, Polygon::square( 10.0 ) );
        drawing.add_bounds( Polygon::square( 100.0 ) );
        drawing.add_hole( Polygon::square( 5.0, Point( +500.0, +500.0 ) ) );
        drawing.draw( GREEN, Polygon::square( 4.0, Point( +20.0, 0.0 ) ) );

        varray<DrawnPolygon> before = drawn_polygons( drawing );

        drawing.optimize( );

        check( same_polygons( before, drawn_polygons( drawing ), false ) );
        check( is_zero( crop_and_hole_count( drawing.nodes( ) ) ) );
        check( drawing.nodes( ).node_count( ) == 1 );
    }

    section( "crops and holes that change something are kept" )
    {
        Drawing drawing;
        drawing.draw( RED, Polygon::square( 10.0 ) );
        drawing.add_bounds( Polygon::square( 6.0 ) );
        drawing.add_hole( Polygon::square( 2.0 ) );
        drawing.add_hole( Polygon::square( 2.0, Point( +500.0, +500.0 ) ) );

        varray<DrawnPolygon> before = drawn_polygons( drawing );

        drawing.optimize( );

        check( same_polygons( before, drawn_polygons( drawing ), false ) );
        check( crop_and_hole_count( drawing.nodes( ) ) == 2 );
    }

    section( "a crop that's partly outside the polygons is kept" )
    {
        Drawing drawing;
        drawing.draw( RED, Polygon::square( 10.0 ) );
        drawing.add_bounds( Polygon::square( 10.0, Point( +5.0, 0.0 ) ) );

        drawing.optimize( );

        check( crop_and_hole_count( drawing.nodes( ) ) == 1 );
    }

    section( "an outline that keeps its thickness on screen keeps the crops around it" )
    {
        // C! it gets wider when zoomed out, so whether it fits inside the crop can't be known
        Drawing drawing;
        drawing.draw( RED, Polygon::square( 10.0 ), 1.0, true );
        drawing.add_bounds( Polygon::square( 100.0 ) );

        drawing.optimize( );

        check( crop_and_hole_count( drawing.nodes( ) ) == 1 );
    }

    section( "a crop also clips the polygons drawn after it" )
    {
        // C! the stencil isn't restored after a node, so GREEN is clipped by the crop too (AspineTree's trunk marks rely on this)
        Drawing drawing;
        drawing.draw( RED, Polygon::square( 10.0 ) );
        drawing.add_bounds( Polygon::square( 6.0 ) );
        drawing.draw( GREEN, Polygon::square( 8.0 ) );
        drawing.move( Vector( ORIGIN, Point( +5.0, 0.0 ) ) );

        varray<DrawnPolygon> before = drawn_polygons( drawing );

        require( before.size( ) == 2 );
        check( before[ 1 ].stencil.active );

        drawing.optimize( );

        varray<DrawnPolygon> after = drawn_polygons( drawing );

        check( same_polygons( before, after ) );
        check( crop_and_hole_count( drawing.nodes( ) ) == 1 );
    }

    section( "a crop is redrawn under a nested drawing's own transform" )
    {
        // C! the camera redraws the crops so far under each inner node's transform, so the rotated drawing must keep its rotation
        Drawing inner;
        inner.draw( RED, Polygon::square( 4.0 ) );
        inner.rotate( half( RIGHT ) );

        Drawing drawing;
        drawing.add_bounds( Polygon::rectangle( 6.0, 2.0 ) );
        drawing.draw( inner );
        drawing.draw( GREEN, Polygon::square( 1.0 ) );

        varray<DrawnPolygon> before = drawn_polygons( drawing );

        drawing.optimize( );

        check( same_polygons( before, drawn_polygons( drawing ) ) );
        check( crop_and_hole_count( drawing.nodes( ) ) == 1 );
    }

    section( "nodes with nothing in them and repeated crops are removed" )
    {
        Drawing drawing;
        drawing.draw( Drawing( ) );
        drawing.draw( RED, Polygon::square( 10.0 ) );
        drawing.add_bounds( Polygon::square( 6.0 ) );
        drawing.add_bounds( Polygon::square( 6.0 ) );
        drawing.draw( Drawing( ) );
        drawing.rotate( half( RIGHT ) );

        varray<DrawnPolygon> before = drawn_polygons( drawing );
        uint nodes_before = drawing.nodes( ).node_count( );

        drawing.optimize( );

        check( same_polygons( before, drawn_polygons( drawing ) ) );
        check( crop_and_hole_count( drawing.nodes( ) ) == 1 );
        check( drawing.nodes( ).node_count( ) < nodes_before );
    }

    section( "a tree built like AspineTree keeps its look with fewer nodes" )
    {
        Drawing trunk;
        Polygon trunk_poly = Polygon::triangle( Point( +3.0, 0.0 ), Point( 0.0, +40.0 ), Point( -3.0, 0.0 ) );
        trunk.draw( RED, Polygon::circle( 3.0 ) );
        trunk.draw( RED, trunk_poly );

        Drawing tree;
        tree.draw( GREEN, Polygon::circle( 15.0, Point( 0.0, +40.0 ) ) );

        for_range( i, 4 )
        {
            Drawing branch;
            branch.draw( GREEN, Polygon::circle( 5.0, Point( 0.0, +15.0 ) ) );
            branch.draw( RED, Line( ORIGIN, Point( 0.0, +15.0 ) ), 1.0 );
            branch.rotate( ( i % 2 ) ? half( RIGHT ) : -half( RIGHT ) );
            branch.move( Vector( ORIGIN, Point( 0.0, +( 8.0 * ( i + 1 ) ) ) ) );
            tree.draw( branch );
        }

        trunk.add_bounds( trunk_poly );

        for_range( i, 6 ) { trunk.draw( BLACK, Polygon::square( 1.0, Point( 0.0, +( 6.0 * ( i + 1 ) ) ), half( half( RIGHT ) ) ) ); }

        tree.draw( trunk );

        Drawing object;
        object.draw( tree.rotate( Angle( 0.1 ) ) );

        varray<DrawnPolygon> before = drawn_polygons( object );
        uint nodes_before = object.nodes( ).node_count( );

        object.optimize( );

        check( same_polygons( before, drawn_polygons( object ) ) );
        check( object.nodes( ).node_count( ) < nodes_before );
        check( crop_and_hole_count( object.nodes( ) ) == 1 );
    }

    section( "empty drawings stay empty" )
    {
        Drawing drawing;
        drawing.optimize( );

        check( drawing.nodes( ).empty( ) );

        drawing.draw( Drawing( ) );
        drawing.rotate( RIGHT );
        drawing.optimize( );

        check( drawn_polygons( drawing ).empty( ) );
    }
}

test( "Color hsl saturation tests" )
{
    section( "dark colors" )
    {
        check( equal( Color::rgb( 0.5, 0.0, 0.0 ).sl( ), 1.0 ) );
        check( equal( Color::rgb( 0.3, 0.1, 0.1 ).sl( ), 0.5 ) );
        check( equal( Color::rgb( 0.75, 0.25, 0.25 ).sl( ), 0.5 ) ); // C! lightness exactly 0.5
    }

    section( "light colors" )
    {
        // C! these came out as 0 and -0.5 when only min_rgb( ) was divided
        check( equal( Color::rgb( 1.0, 0.5, 0.5 ).sl( ), 1.0 ) );
        check( equal( Color::rgb( 1.0, 0.6, 0.6 ).sl( ), 1.0 ) );
        check( equal( Color::rgb( 0.9, 0.7, 0.7 ).sl( ), 0.5 ) );
    }

    section( "greys have no saturation" )
    {
        check( is_zero( Color::rgb( 0.0, 0.0, 0.0 ).sl( ) ) );
        check( is_zero( Color::rgb( 0.8, 0.8, 0.8 ).sl( ) ) );
        check( is_zero( Color::rgb( 1.0, 1.0, 1.0 ).sl( ) ) );
    }

    section( "saturation and lightness come back out of hsl( )" )
    {
        for_range( i, 9 )
        {
            dec l = 0.1 * dec( i + 1 );

            Color color = Color::hsl( 0.6, 0.4, l );

            check( equal( color.sl( ), 0.4 ) );
            check( equal( color.l( ), l ) );
        }
    }

    section( "changing a light color's lightness keeps its hue and saturation" )
    {
        Color color = Color::rgb( 1.0, 0.5, 0.5 ).l( 0.6 );

        check( equal( color.sl( ), 1.0 ) );
        check( equal( color.l( ), 0.6 ) );
        check( equal( color.r( ), 1.0 ) );
        check( equal( color.g( ), color.b( ) ) );
    }
}

test( "Drawing bounding box and colors" )
{
    section( "the max box is every box so far, and never shrinks" )
    {
        Drawing drawing;
        check( drawing.max_bounding_box( ) == FixedRectangle( ) ); // C! nothing drawn yet

        drawing.draw( RED, Polygon::rectangle( 4.0, 2.0 ) );
        check( drawing.max_bounding_box( ) == FixedRectangle( 4.0, 2.0 ) );

        // C! a smaller drawing shrinks the box but not the max box, which also takes in a polygon further away
        drawing.clear( );
        drawing.draw( RED, Polygon::rectangle( 2.0, 6.0 ) );
        check( drawing.bounding_box( ) == FixedRectangle( 2.0, 6.0 ) );
        check( drawing.max_bounding_box( ) == FixedRectangle( 4.0, 6.0 ) );

        drawing.clear( );
        drawing.draw( RED, Polygon::rectangle( 2.0, 2.0, Point( +10.0, 0.0 ) ) );
        check( drawing.max_bounding_box( ) == FixedRectangle( Point( -2.0, -3.0 ), Point( +11.0, +3.0 ) ) );

        // C! clearing to nothing leaves it as it was
        drawing.clear( );
        check( drawing.max_bounding_box( ) == FixedRectangle( Point( -2.0, -3.0 ), Point( +11.0, +3.0 ) ) );
    }

    section( "the box follows the drawing's transforms" )
    {
        Drawing drawing;
        drawing.draw( RED, Polygon::rectangle( 4.0, 2.0 ) );

        check( drawing.bounding_box( ) == FixedRectangle( 4.0, 2.0 ) );

        drawing.move( Vector( ORIGIN, Point( +10.0, 0.0 ) ) );
        check( drawing.bounding_box( ) == FixedRectangle( 4.0, 2.0, Point( +10.0, 0.0 ) ) );

        // C! turning a quarter around the moved center swaps the sides, and turning it many times doesn't grow the box
        Drawing turned;
        turned.draw( RED, Polygon::rectangle( 4.0, 2.0 ) );
        turned.rotate( RIGHT );
        check( turned.bounding_box( ) == FixedRectangle( 2.0, 4.0 ) );

        for_range( i, 40 ) { turned.rotate( Angle( TAU / 40.0 ) ); }
        check( turned.bounding_box( ) == FixedRectangle( 2.0, 4.0 ) );
    }

    section( "a crop cuts the box down and holes don't change it" )
    {
        Drawing drawing;
        drawing.draw( RED, Polygon::rectangle( 10.0, 10.0 ) );
        drawing.add_bounds( Polygon::rectangle( 2.0, 4.0, Point( +4.0, 0.0 ) ) );

        check( drawing.bounding_box( ) == FixedRectangle( Point( +3.0, -2.0 ), Point( +5.0, +2.0 ) ) );

        Drawing holed;
        holed.draw( RED, Polygon::rectangle( 10.0, 10.0 ) );
        holed.add_hole( Polygon::rectangle( 2.0, 2.0 ) );
        check( holed.bounding_box( ) == FixedRectangle( 10.0, 10.0 ) );

        // C! a polygon entirely outside the crop adds nothing
        Drawing outside;
        outside.draw( RED, Polygon::rectangle( 2.0, 2.0 ) );
        outside.draw( RED, Polygon::rectangle( 2.0, 2.0, Point( +20.0, 0.0 ) ) );
        outside.add_bounds( Polygon::rectangle( 4.0, 4.0 ) );
        check( outside.bounding_box( ) == FixedRectangle( 2.0, 2.0 ) );
    }

    section( "outlines widen the box and nested drawings count" )
    {
        Drawing drawing;
        drawing.draw( RED, Line( Point( 0.0, 0.0 ), Point( +4.0, 0.0 ) ), 1.0 );
        check( drawing.bounding_box( ).contains( Point( +2.0, +0.5 ) ) );
        check( drawing.bounding_box( ).contains( Point( -0.5, 0.0 ) ) );

        Drawing outer;
        outer.draw( BLUE, Polygon::square( 1.0 ) );
        outer.draw( Drawing( ).draw( RED, Polygon::square( 1.0, Point( +5.0, +5.0 ) ) ) );
        check( outer.bounding_box( ) == FixedRectangle( Point( -0.5, -0.5 ), Point( +5.5, +5.5 ) ) );

        Drawing empty;
        check( empty.bounding_box( ) == FixedRectangle( ) );
    }

    section( "the color filter reaches into nested drawings" )
    {
        Drawing inner;
        inner.draw( RED, Polygon::square( 1.0 ) );

        Drawing drawing;
        drawing.color_filter( [ ] ( Color & color ) { color = color.a( 0.5 ); } );
        drawing.draw( inner );

        varray<DrawnPolygon> polys = drawn_polygons( drawing );
        check( polys.size( ) == 1 );
        check( polys.front( ).colors.front( ) == RED.a( 0.5 ) );
        check_not( drawing.opaque( ) );
    }

    section( "drawing a drawing in one color" )
    {
        Drawing inner;
        inner.draw( RED, Polygon::square( 1.0 ) );
        inner.draw( Drawing( ).draw( GREEN, Polygon::square( 2.0 ) ) );

        Drawing drawing;
        drawing.draw( inner, BLUE );

        varray<DrawnPolygon> polys = drawn_polygons( drawing );
        check( polys.size( ) == 2 );
        for_each_const( poly, polys ) { for_each_const( color, poly.colors ) { check( color == BLUE ); } }
        check( drawing.opaque( ) );

        Drawing see_through;
        see_through.draw( inner, BLUE.a( 0.5 ) );
        check_not( see_through.opaque( ) );
    }
}

test( "Drawing optimize random tests" )
{
    uint seed = Random::seed( );

    Random::seed( 1 );

    // C! a failure shows what went wrong and which drawing it was (the same seed always makes the same drawings)
    for_range( i, 300 ) { string problem = check_optimize( random_drawing( 3, false ) ); if( !problem.empty( ) ) { check( ( "without crops " + to_string( i ) + ": " + problem ) == nostring ); } }
    for_range( i, 300 ) { string problem = check_optimize( random_drawing( 3, true ) ); if( !problem.empty( ) ) { check( ( "with crops " + to_string( i ) + ": " + problem ) == nostring ); } }

    Random::seed( seed ); // C! so the game's random numbers don't depend on these tests
}

test( "Drawing keeps the order things were drawn in" )
{
    // C! a polygon drawn after a nested drawing is drawn over it: the renderer draws a node's polygons before its nested drawings, so the
    // C! polygon has to be in a later node (a wisp bullet used to be hidden behind its own fire)
    Polygon square = Polygon::rectangle( 10.0, 10.0 );

    Drawing fire;
    fire.draw( RED, square );

    Drawing bullet;
    bullet.draw( fire );
    bullet.draw( BLUE, square );

    Drawing::Nodes nodes = bullet.nodes( );
    check( nodes.size( ) == 2 );
    if( nodes.size( ) == 2 )
    {
        check( nodes[ 0 ].drawings.size( ) == 1 ); // C! the fire, drawn first
        check( nodes[ 0 ].colored_polys.empty( ) );
        check( nodes[ 1 ].colored_polys.size( ) == 1 ); // C! the bullet, in the node after, so drawn over it
    }

    // C! the other way round, a nested drawing after a polygon already went in a later node
    Drawing under;
    under.draw( BLUE, square );
    under.draw( fire );
    check( under.nodes( ).size( ) == 2 );
}

test( "Drawing bounds and holes apply to what's drawn after them" )
{
    Polygon square = Polygon::rectangle( 10.0, 10.0 );

    Drawing drawing;
    drawing.add_bounds( square );
    drawing.add_hole( square );
    drawing.draw( RED, square );

    Drawing::Nodes nodes = drawing.nodes( );
    check( nodes.size( ) == 1 ); // C! one node, so its bounds and hole crop the polygon drawn after them
    if( nodes.size( ) == 1 )
    {
        check( nodes[ 0 ].crop_polys.size( ) == 1 );
        check( nodes[ 0 ].hole_polys.size( ) == 1 );
        check( nodes[ 0 ].colored_polys.size( ) == 1 );
    }
}

} // namespace axn

#endif /* TestsDrawing_hpp */
