#ifndef TestsGeometry_hpp
#define TestsGeometry_hpp

#include "axn/tests/Tests.hpp"

#include "axn/geometry/Geometry.hpp"
#include "axn/geometry/Grid.hpp" // C! not in Geometry.hpp, nothing else includes it yet

namespace axn
{

test( "Circle tests" )
{
    const Circle unit_circle = Circle( 1.0, ORIGIN );

    check( unit_circle.area( ) == PI );
    check( unit_circle.circumference( ) == TAU );

    check( unit_circle.intersects( Line( Point( 10.0, 1.0 ), Point( -10.0, 1.0 ) ), true ) );
    check_not( unit_circle.intersects( Line( Point( 10.0, 1.0 ), Point( -10.0, 1.0 ) ), false ) );

    check( unit_circle.intersects( Line( Point( 10.0, 0.9 ), Point( -10.0, 0.9 ) ) ) );
    check( unit_circle.intersects( Line( Point( 10.0, 9.5 ), Point( -10.0, -9.5 ) ) ) );
    check_not( unit_circle.intersects( Line( Point( 10.0, 1.1 ), Point( -10.0, 1.1 ) ) ) );

    check( unit_circle.intersection(
        Line( Point( 10.0, 0.0 ), Point( -10.0, 0.0 ) ) ) ==
        Line( Point( 1.0, 0.0 ), Point( -1.0, 0.0 ) ) );

    check( unit_circle.intersection(
        Line( Point( 10.0, 10.0 ), Point( -10.0, -10.0 ) ) ) ==
        Line( Point( sqrt( 0.5 ), sqrt( 0.5 ) ), Point( -sqrt( 0.5 ), -sqrt( 0.5 ) ) ) );

    check( unit_circle.intersection(
        Line( Point( 10.0, 0.5 ), Point( -10.0, 0.5 ) ) ) ==
        Line( Point( sqrt( 0.75 ), 0.5 ), Point( -sqrt( 0.75 ), 0.5 ) ) );
}

test( "Angle comparison tests" )
{
    const dec ROUNDING = DEFAULT_EPSILON / 10.0; // C! smaller than DEFAULT_EPSILON (whatever it's set to), like floating point rounding
    const dec REAL = DEFAULT_EPSILON * 10.0; // C! bigger than DEFAULT_EPSILON, a real difference

    // C! a rounding sized difference counts as equal whichever side the Angle is on
    check( Angle( numbers::RIGHT + ROUNDING ) == RIGHT );
    check_not( Angle( numbers::RIGHT + ROUNDING ) > RIGHT );
    check_not( ( numbers::RIGHT + ROUNDING ) > RIGHT );
    check( ( numbers::RIGHT + ROUNDING ) == RIGHT );
    check( ( numbers::RIGHT + ROUNDING ) <= RIGHT );

    // C! a real difference still counts
    check( Angle( numbers::RIGHT + REAL ) > RIGHT );
    check( ( numbers::RIGHT + REAL ) > RIGHT );
    check( ( numbers::RIGHT - REAL ) < RIGHT );

    // C! abs( ) keeps it an Angle so comparing the result allows for rounding too
    check_not( abs( Angle( -( numbers::RIGHT + ROUNDING ) ) ) > RIGHT );
    check( abs( Angle( -( numbers::RIGHT + REAL ) ) ) > RIGHT );
    check( abs( Angle( -0.5 ) ) == Angle( 0.5 ) );
}

test( "Transform shear, stretch, and squeeze" )
{
    // C! each one is worked out by hand; build it the long way from simple transforms and compare where they send some points
    lambdaf( long_way )( Vector cref in_vec, dec in_t11, dec in_t12, dec in_t21, dec in_t22 )
    {
        Transform mt = Transform::move( in_vec.origin( ) );
        Transform rt = Transform::rotate( in_vec.angle( ) );
        Transform st = Transform( true ).set( 0, 0, in_t11 ).set( 0, 1, in_t12 ).set( 1, 0, in_t21 ).set( 1, 1, in_t22 );

        return( mt * rt * st * rt.inverse( ) * mt.inverse( ) );
    };

    lambdaf( same_effect )( Transform cref in_trans1, Transform cref in_trans2 )
    {
        const varray<Point> pts = { ORIGIN, Point( +1.0, 0.0 ), Point( 0.0, +1.0 ), Point( -3.0, +7.5 ), Point( +12.0, -4.0 ) };
        for_each_const( pt, pts ) { return_false_if( in_trans1.apply( pt ) != in_trans2.apply( pt ) ); }
        return( true );
    };

    const varray<Vector> vecs = { Vector( ORIGIN, Point( 2.0, 0.0 ) ), Vector( Point( 1.0, 2.0 ), Point( 1.5, 3.0 ) ), Vector( Point( -4.0, 3.0 ), Point( -6.0, -1.0 ) ), Vector( Point( 5.0, -2.0 ), Point( 5.0, 1.0 ) ) };

    for_each_const( vec, vecs )
    {
        dec m = dec( vec.magnitude( ) );

        check( same_effect( Transform::shear( vec ), long_way( vec, 1.0, m, 0.0, 1.0 ) ) );
        check( same_effect( Transform::stretch( vec ), long_way( vec, m, 0.0, 0.0, 1.0 ) ) );
        check( same_effect( Transform::squeeze( vec ), long_way( vec, m, 0.0, 0.0, ( 1.0 / m ) ) ) );
    }

    // C! a stretch only moves points along its direction
    check( Transform::stretch( Vector( Point( 1.0, 1.0 ), Point( 1.0, 3.0 ) ) ).apply( Point( 1.0, 2.0 ) ) == Point( 1.0, 3.0 ) );
    check( Transform::stretch( Vector( Point( 1.0, 1.0 ), Point( 1.0, 3.0 ) ) ).apply( Point( 4.0, 1.0 ) ) == Point( 4.0, 1.0 ) );
}

test( "FixedRectangle line intersection" )
{
    const FixedRectangle square = FixedRectangle( 2.0, 2.0 );

    // C! a line through the middle, half in, fully in, and fully out
    check( square.intersection( Line( Point( -5.0, 0.0 ), Point( +5.0, 0.0 ) ) ) == varray<Line>( { Line( Point( -1.0, 0.0 ), Point( +1.0, 0.0 ) ) } ) );
    check( square.intersection( Line( ORIGIN, Point( +5.0, 0.0 ) ) ) == varray<Line>( { Line( ORIGIN, Point( +1.0, 0.0 ) ) } ) );
    check( square.intersection( Line( Point( -0.5, -0.5 ), Point( +0.5, +0.5 ) ) ) == varray<Line>( { Line( Point( -0.5, -0.5 ), Point( +0.5, +0.5 ) ) } ) );
    check( square.intersection( Line( Point( -5.0, 3.0 ), Point( +5.0, 3.0 ) ) ).empty( ) );
    check_not( square.intersects( Line( Point( 2.5, 0.5 ), Point( 0.5, 2.5 ) ) ) );

    // C! diagonal through a corner region
    check( square.intersection( Line( Point( 0.0, -2.0 ), Point( 2.0, 0.0 ) ) ) == varray<Line>( { Line( Point( 1.0, -1.0 ), Point( 1.0, -1.0 ) ) } ) );

    // C! a rectangle with no height is a line, so crossing it gives a point
    check( FixedRectangle( 2.0, 0.0 ).intersection( Line( Point( 0.5, -1.0 ), Point( 0.5, +1.0 ) ) ) == varray<Line>( { Line( Point( 0.5, 0.0 ), Point( 0.5, 0.0 ) ) } ) );
    check( FixedRectangle( 2.0, 0.0 ).intersection( Line( Point( 3.0, -1.0 ), Point( 3.0, +1.0 ) ) ).empty( ) );
}

test( "Polygon expanded and joined" )
{
    // C! an L polygon (one concave corner): pushing every side out by e adds the perimeter times e, plus e squared for each convex corner beyond the concave ones
    const Polygon l_poly = Polygon( { Point( 0.0, 0.0 ), Point( 2.0, 0.0 ), Point( 2.0, 1.0 ), Point( 1.0, 1.0 ), Point( 1.0, 2.0 ), Point( 0.0, 2.0 ) } );
    const dec e = 0.1;

    Polygon pointed = l_poly.expanded( e, Polygon::POINTED );
    check( pointed.points( ).size( ) == 6 );
    check( pointed.area( ) == ( 3.0 + ( 8.0 * e ) + ( 4.0 * e * e ) ) );
    check( pointed.contains( Point( 1.0 + e, 1.0 + e ) ) );
    check_not( pointed.contains( Point( 1.0 + ( 2.0 * e ), 1.0 + ( 2.0 * e ) ) ) );

    // C! flattened cuts each convex corner, so it's a little smaller than pointed but bigger than the polygon
    Polygon flattened = l_poly.expanded( e, Polygon::FLATTENED );
    check( flattened.area( ) == ( 3.0 + ( 8.0 * e ) + ( 1.5 * e * e ) ) );

    // C! two overlapping squares join into one outline with 8 corners
    const Polygon square1 = Polygon( { Point( 0.0, 0.0 ), Point( 1.0, 0.0 ), Point( 1.0, 1.0 ), Point( 0.0, 1.0 ) } );
    const Polygon square2 = square1 + Vector( 0.5, 0.5 );

    Polygon joined = square1.joined( square2 );
    check( joined.points( ).size( ) == 8 );
    check( joined.area( ) == 1.75 );
    check( square2.joined( square1 ).area( ) == 1.75 );

    // C! one inside the other gives the outer one, apart gives the first
    check( square1.joined( Polygon( { Point( 0.25, 0.25 ), Point( 0.75, 0.25 ), Point( 0.75, 0.75 ) } ) ).points( ) == square1.points( ) );
    check( square1.joined( square1 + Vector( 5.0, 0.0 ) ).points( ) == square1.points( ) );
}

test( "Crossings that don't count their ends" )
{
    // C! a vertical and a horizontal line crossing in their middles, and meeting at an end
    check( Line( Point( 0.0, -1.0 ), Point( 0.0, +1.0 ) ).intersects( Line( Point( -1.0, 0.0 ), Point( +1.0, 0.0 ) ), false ) );
    check_not( Line( Point( 0.0, 0.0 ), Point( 0.0, +1.0 ) ).intersects( Line( Point( -1.0, 0.0 ), Point( +1.0, 0.0 ) ), false ) );
    check( Line( Point( 0.0, 0.0 ), Point( 0.0, +1.0 ) ).intersects( Line( Point( -1.0, 0.0 ), Point( +1.0, 0.0 ) ), true ) );

    // C! vertical lines are equal to themselves
    check( Line( Point( 2.0, 0.0 ), Point( 2.0, 5.0 ) ) == Line( Point( 2.0, 0.0 ), Point( 2.0, 5.0 ) ) );

    // C! a line through two opposite corners of a square goes through it, one along a side or past a corner only touches it
    const Polygon square = Polygon( { Point( 0.0, 0.0 ), Point( 1.0, 0.0 ), Point( 1.0, 1.0 ), Point( 0.0, 1.0 ) } );
    check( square.intersects( Line( Point( -1.0, -1.0 ), Point( 2.0, 2.0 ) ), false ) );
    check_not( square.intersects( Line( Point( -1.0, 0.0 ), Point( 2.0, 0.0 ) ), false ) );
    check_not( square.intersects( Line( Point( -1.0, 0.0 ), Point( 1.0, 2.0 ) ), false ) );
    check( square.intersects( Line( Point( -1.0, 0.0 ), Point( 1.0, 2.0 ) ), true ) );

    // C! paths: a zigzag doesn't cross itself, a bow tie does, a T touch does, and folding back does
    check_not( Path( { Point( 0.0, 0.0 ), Point( 1.0, 1.0 ), Point( 2.0, 0.0 ), Point( 3.0, 1.0 ) } ).intersects_self( ) );
    check_not( Path( { Point( 0.0, 0.0 ), Point( 1.0, 0.0 ), Point( 1.0, 1.0 ), Point( 0.0, 1.0 ) }, true ).intersects_self( ) );
    check( Path( { Point( 0.0, 0.0 ), Point( 1.0, 1.0 ), Point( 1.0, 0.0 ), Point( 0.0, 1.0 ) }, true ).intersects_self( ) );
    check( Path( { Point( 0.0, 0.0 ), Point( 2.0, 0.0 ), Point( 2.0, 1.0 ), Point( 1.0, 1.0 ), Point( 1.0, 0.0 ) } ).intersects_self( ) );
    check( Path( { Point( 0.0, 0.0 ), Point( 2.0, 0.0 ), Point( 1.0, 0.0 ) } ).intersects_self( ) );
    check_not( Path( { Point( 0.0, 0.0 ), Point( 1.0, 0.0 ), Point( 2.0, 0.0 ) } ).intersects_self( ) );
}

test( "Trapezoid" )
{
    // C! the number of points its polygon has
    lambdaf( corners )( Trapezoid cref in_trapezoid ) { return( Polygon( in_trapezoid ).points( ).size( ) ); };

    check( corners( Trapezoid( 10.0, 6.0, 4.0 ) ) == 4 );
    check( corners( Trapezoid( 10.0, 0.0, 4.0 ) ) == 3 ); // C! one zero width: a triangle
    check( corners( Trapezoid( 0.0, 6.0, 4.0 ) ) == 3 );
    check( corners( Trapezoid( 0.0, 0.0, 4.0 ) ) == 2 ); // C! both widths zero: a line up the middle
    check( corners( Trapezoid( 10.0, 6.0, 0.0 ) ) == 2 ); // C! zero height: a line
    check( corners( Trapezoid( 0.0, 0.0, 0.0 ) ) == 1 ); // C! all zero: a point

    const Polygon c_box = Trapezoid( 10.0, 6.0, 4.0 );
    check( c_box.points( )[ 0 ] == Point( -5.0, -2.0 ) ); // C! centered on the origin, the base below
    check( c_box.points( )[ 2 ] == Point( 3.0, 2.0 ) );

    const Polygon c_line = Trapezoid( 10.0, 6.0, 0.0 );
    check( c_line.points( )[ 1 ] == Point( 5.0, 0.0 ) ); // C! as wide as the wider width

    const Polygon c_turned = Trapezoid( 0.0, 0.0, 4.0, Angle::fraction( 4.0 ) ); // C! a quarter turn counterclockwise
    check( c_turned.points( )[ 1 ] == Point( -2.0, 0.0 ) );

    const Polygon c_moved = Trapezoid( 0.0, 0.0, 4.0, Point( 10.0, 20.0 ), Angle::fraction( 4.0 ) ); // C! centered on the point given, turned about it
    check( c_moved.points( )[ 1 ] == Point( 8.0, 20.0 ) );
}

test( "Polygon rounded corners" )
{
    const Polygon c_square = Polygon( varray<Point>( { Point( 0.0, 0.0 ), Point( 10.0, 0.0 ), Point( 10.0, 10.0 ), Point( 0.0, 10.0 ) } ) );

    const Polygon c_octagon = c_square.rounded_corners( 2.0, 1 ); // C! one edge: a straight cut
    check( c_octagon.points( ).size( ) == 8 );
    check( c_octagon.points( )[ 0 ] == Point( 0.0, 2.0 ) ); // C! the first corner's cut, from its side before to its side after
    check( c_octagon.points( )[ 1 ] == Point( 2.0, 0.0 ) );

    const Polygon c_round = c_square.rounded_corners( 2.0, 4 );
    check( c_round.points( ).size( ) == 20 ); // C! each corner an arc of 4 edges, so 5 points
    for_each_const( pt, c_round.points( ) ) // C! every arc point is the radius from its corner's center
    {
        const Point c_center = Point( ( pt.x( ) < 5.0 ) ? 2.0 : 8.0, ( pt.y( ) < 5.0 ) ? 2.0 : 8.0 );
        check( equal( dec( pt.distance_to( c_center ) ), 2.0 ) );
    }

    const Polygon c_one = c_square.rounded_corners( 2.0, 1, { 2 } ); // C! only the corner at index 2
    check( c_one.points( ).size( ) == 5 );
    check( c_one.points( )[ 2 ] == Point( 10.0, 8.0 ) );
    check( c_one.points( )[ 3 ] == Point( 8.0, 10.0 ) );

    Polygon in_place = c_square;
    in_place.round_corners( 2.0, 1 );
    check( in_place.points( ).size( ) == 8 ); // C! the same, changing the polygon itself
}

test( "Dot product and winding" )
{
    check( equal( Vector( 3.0, 4.0 ).dot( Vector( 2.0, -1.0 ) ), 2.0 ) );
    check( is_zero( XHAT.dot( YHAT ) ) ); // C! at right angles: nothing

    const varray<Point> c_square = { Point( 0.0, 0.0 ), Point( 1.0, 0.0 ), Point( 1.0, 1.0 ), Point( 0.0, 1.0 ) };
    check( !clockwise( c_square ) );

    varray<Point> backwards = c_square;
    backwards.reverse( );
    check( clockwise( backwards ) );
}

test( "Thick path" )
{
    const Path c_straight( varray<Point>( { Point( 0.0, 0.0 ), Point( 10.0, 0.0 ) } ) );

    const Polygon c_even = Polygon::thicken_path( c_straight, 2.0, 2.0 );
    check( c_even.points( ).size( ) == 4 );
    check( c_even.points( )[ 0 ] == Point( 0.0, -1.0 ) ); // C! counterclockwise from the start's right side
    check( c_even.points( )[ 2 ] == Point( 10.0, 1.0 ) );

    const Polygon c_widening = Polygon::thicken_path( c_straight, 0.0, 4.0 );
    check( c_widening.points( ).size( ) == 3 ); // C! a zero width is a single point
    check( c_widening.points( )[ 1 ] == Point( 10.0, -2.0 ) );

    const Polygon c_narrowing = Polygon::thicken_path( c_straight, 4.0, 0.0 );
    check( c_narrowing.points( ).size( ) == 3 ); // C! to a point at the end
    check( c_narrowing.points( )[ 1 ] == Point( 10.0, 0.0 ) );

    check( Polygon::thicken_path( c_straight, 0.0, 0.0 ).points( ).size( ) == 2 ); // C! both widths zero: the path as it is

    // C! a right angle: the corner's sides stay half the width from both lines
    const Path c_corner( varray<Point>( { Point( 0.0, 0.0 ), Point( 10.0, 0.0 ), Point( 10.0, 10.0 ) } ) );
    const Polygon c_bent = Polygon::thicken_path( c_corner, 2.0, 2.0 );
    check( c_bent.points( ).size( ) == 6 );
    check( c_bent.points( )[ 1 ] == Point( 11.0, -1.0 ) );
    check( c_bent.points( )[ 4 ] == Point( 9.0, 1.0 ) );

    check( Polygon::thicken_path( c_corner, 0.0, 4.0 ).points( )[ 1 ] == Point( 11.0, -1.0 ) ); // C! halfway along: halfway between the widths
}

test( "Segmenting a path" )
{
    const Line c_line( Point( 0.0, 0.0 ), Point( 100.0, 0.0 ) );

    const Path c_quarters = c_line.segmented( 4 );
    check( c_quarters.size( ) == 4 );
    for_range( i, 4 ) { check( c_quarters[ i ].pt1( ) == Point( 25.0 * dec( i ), 0.0 ) ); }
    check( c_quarters.back( ).pt2( ) == Point( 100.0, 0.0 ) );

    // C! a bent path is cut evenly along its whole length, not each of its lines
    const Path c_bent( varray<Point>( { Point( 0.0, 0.0 ), Point( 10.0, 0.0 ), Point( 10.0, 30.0 ) } ) );
    const Path c_halves = c_bent.segmented( 2 );
    check( c_halves.size( ) == 2 );
    check( c_halves[ 0 ].pt2( ) == Point( 10.0, 10.0 ) );

    Path changed = c_bent;
    changed.segment( 4 );
    check( changed.size( ) == 4 ); // C! segment( ) changes the path itself
}

test( "Thick path with bulbed ends" )
{
    const Path c_straight( varray<Point>( { Point( 0.0, 0.0 ), Point( 10.0, 0.0 ) } ) );

    const Polygon c_plain = Polygon::thicken_path( c_straight, 2.0, 2.0 );
    const Polygon c_bulbed = Polygon::thicken_path( c_straight, 2.0, 2.0, true );

    check( c_bulbed.points( ).size( ) > c_plain.points( ).size( ) ); // C! the bulbs add their arcs
    check( c_bulbed.contains( Point( 10.9, 0.0 ) ) ); // C! the end bulb reaches half the width past the end
    check( !c_bulbed.contains( Point( 11.1, 0.0 ) ) );
    check( c_bulbed.contains( Point( -0.9, 0.0 ) ) ); // C! and the start bulb half the width before the start
    check( !c_plain.contains( Point( -0.9, 0.0 ) ) );

    for_each_const( pt, c_bulbed.points( ) ) // C! every point is on a side or on one of the half circles
    {
        const bool c_on_side = equal( dec( abs( pt.y( ) ) ), 1.0 );
        const bool c_on_bulb = equal( dec( pt.distance_to( Point( 0.0, 0.0 ) ) ), 1.0 ) || equal( dec( pt.distance_to( Point( 10.0, 0.0 ) ) ), 1.0 );
        check( ( c_on_side || c_on_bulb ) );
    }

    const Polygon c_pointed = Polygon::thicken_path( c_straight, 0.0, 4.0, true );
    check( !c_pointed.contains( Point( -0.1, 0.0 ) ) ); // C! a zero width end has no bulb
    check( c_pointed.contains( Point( 11.9, 0.0 ) ) ); // C! the other end's bulb is half its width
}

test( "Grid tests" )
{
    const FixedRectangle bounds = FixedRectangle( 2.0, 2.0 );

    // C! cells that fit exactly, then moved half a cell (partly outside cells at the edges are kept), then stepped like bricks
    vvarray<Point> plain = Grid::square( bounds, 1.0 ).centers( );
    check( plain.size( ) == 2 );
    check( plain[ 0 ] == varray<Point>( { Point( -0.5, -0.5 ), Point( +0.5, -0.5 ) } ) );
    check( plain[ 1 ] == varray<Point>( { Point( -0.5, +0.5 ), Point( +0.5, +0.5 ) } ) );

    vvarray<Point> moved = Grid::square( bounds, 1.0, Vector( 0.5, 0.0 ) ).centers( );
    check( moved.size( ) == 2 );
    check( moved[ 0 ] == varray<Point>( { Point( -1.0, -0.5 ), Point( 0.0, -0.5 ), Point( +1.0, -0.5 ) } ) );

    vvarray<Point> stepped = Grid::square_stepped( bounds, 1.0 ).centers( );
    check( stepped.size( ) == 2 );
    check( stepped[ 0 ].size( ) == 2 );
    check( stepped[ 1 ] == varray<Point>( { Point( -1.0, +0.5 ), Point( 0.0, +0.5 ), Point( +1.0, +0.5 ) } ) );

    // C! hexagons cover every point of the bounds, corners included, and every hexagon reaches into the bounds
    const FixedRectangle wide = FixedRectangle( 2.0, 1.0, Point( 3.0, -1.0 ) );
    const Grid hexagons = Grid::hexagon( wide, 0.3, Vector( 0.1, 0.2 ) );
    const vvarray<Polygon> cells = hexagons.cells( );

    bool all_covered = true;

    for_range( i, 21 )
    {
        for_range( j, 11 )
        {
            Point pt = wide.bottom_left( ) + Vector( ( 0.1 * i ), ( 0.1 * j ) );

            bool covered = false;
            for_each_const( row, cells ) { for_each_const( cell, row ) { covered = ( covered || cell.contains( pt ) ); } }

            all_covered = ( all_covered && covered );
        }
    }

    check( all_covered );

    bool all_reach_in = true;
    for_each_const( row, cells ) { for_each_const( cell, row ) { all_reach_in = ( all_reach_in && cell.bounds( ).intersects( wide ) ); } }
    check( all_reach_in );
}

} // namespace axn

#endif /* TestsGeometry_hpp */
