//#include "tests.hpp"
//
//TEST_CASE( "coordinate tests" ) {
//    Coordinate c;
//
//    TEST_COORDINATE( c = Coordinate( 10, 20 ), 10, 20, Q1, No_Axis );
//
//    c.x( 30 );
//    TEST_COORDINATE( c, 30, 20, Q1, No_Axis );
//    c.y( 40 );
//    TEST_COORDINATE( c, 30, 40, Q1, No_Axis );
//    c.xy( 10, 20 );
//    TEST_COORDINATE( c, 10, 20, Q1, No_Axis );
//
//    c.mirror_y( );
//    TEST_COORDINATE( c, -10, 20, Q2, No_Axis );
//    c.mirror_x( );
//    TEST_COORDINATE( c, -10, -20, Q3, No_Axis );
//    c.mirror_y( );
//    TEST_COORDINATE( c, 10, -20, Q4, No_Axis );
//    c.mirror_x( );
//    TEST_COORDINATE( c, 10, 20, Q1, No_Axis );
//
//    c.xy( 0, 0 );
//    TEST_COORDINATE( c, 0, 0, No_Quadrant, Origin );
//    c.xy( 10, 0 );
//    TEST_COORDINATE( c, 10, 0, No_Quadrant, X_Axis );
//    c.xy( -10, 0 );
//    TEST_COORDINATE( c, -10, 0, No_Quadrant, X_Axis );
//    c.xy( 0, 10 );
//    TEST_COORDINATE( c, 0, 10, No_Quadrant, Y_Axis );
//    c.xy( 0, -10 );
//    TEST_COORDINATE( c, 0, -10, No_Quadrant, Y_Axis );
//
//    c = Coordinate( 10, 20 );
//    TEST_COORDINATE( c, 10, 20, Q1, No_Axis );
//    REQUIRE_FLOAT_EQ( c.distance( Coordinate( 40, 60 ) ), 50 );
//}
//
//TEST_CASE( "line tests" ) {
//    Line l;
//    TEST_LINE( l, Coordinate( 0, 0 ), Coordinate( 0, 0 ), 0, 0 );
//
//    l = Line( Coordinate( 0, 0 ), Coordinate( 1, 0 ) );
//    TEST_LINE( l, Coordinate( 0, 0 ), Coordinate( 1, 0 ), 0, 1 );
//    REQUIRE( l.below( Coordinate( 1, 1 ) ) );
//    REQUIRE( l.below( Coordinate( -1, 1 ) ) );
//    REQUIRE( l.above( Coordinate( -1, -1 ) ) );
//    REQUIRE( l.above( Coordinate( 1, -1 ) ) );
//    REQUIRE( l.on( Coordinate( 10, 0 ) ) );
//}
//
//TEST_CASE( "circle tests" ) {
//    Circle c = Circle( 3, Coordinate( 2, 4 ) );
//    TEST_CIRCLE( c, Coordinate( 2, 4 ), 3 );
//    c.center( Coordinate( 5, 10 ) );
//    TEST_CIRCLE( c, Coordinate( 5, 10 ), 3 );
//    c.radius( 4 );
//    TEST_CIRCLE( c, Coordinate( 5, 10 ), 4 );
//    c.move( Vector( 8, 12 ) );
//    TEST_CIRCLE( c, Coordinate( 13, 22 ), 4 );
//    c.scale( 3, c.center( ) );
//    TEST_CIRCLE( c, Coordinate( 13, 22 ), 12 );
//}
//
//TEST_CASE( "polygon tests" ) {
//    Polygon p;
//    Polygon p_ch; // convex hull of p
//
//    /*
//     ----------
//     |        |
//     |        |
//     |        |
//     ----------
//     */
//    p = Polygon( varray<Coordinate>( { Coordinate( 10, 10 ), Coordinate( 0, 10 ),
//                                       Coordinate( 0, 0 ), Coordinate( 10, 0 ) } ) );
//    REQUIRE_EQ(
//        p, Polygon( varray<Coordinate>( { Coordinate( 0, 0 ), Coordinate( 10, 0 ),
//                                          Coordinate( 10, 10 ), Coordinate( 0, 10 ) } ) ) );
//    REQUIRE( p.convex( ) );
//    REQUIRE_FLOAT_EQ( p.area( ), 100 );
//
//    /*
//       ------
//       |    |
//     ---    ---
//     |        |
//     |        |
//     |        |
//     ----------
//     */
//    p = Polygon(
//        varray<Coordinate>( { Coordinate( 10, 10 ), Coordinate( 8, 10 ), Coordinate( 8, 16 ),
//                              Coordinate( 2, 16 ), Coordinate( 2, 10 ), Coordinate( 0, 10 ),
//                              Coordinate( 0, 0 ), Coordinate( 10, 0 ) } ) );
//    REQUIRE( !p.convex( ) );
//    REQUIRE_FLOAT_EQ( p.area( ), 136 );
//    p_ch = p.convex_hull( );
//    REQUIRE( p_ch.convex( ) );
//    REQUIRE_FLOAT_EQ( p_ch.area( ), 148 );
//    REQUIRE_EQ( p_ch,
//                Polygon( varray<Coordinate>( { Coordinate( 10, 10 ), Coordinate( 8, 16 ),
//                                               Coordinate( 2, 16 ), Coordinate( 0, 10 ),
//                                               Coordinate( 0, 0 ), Coordinate( 10, 0 ) } ) ) );
//
//    /*
//     ------
//     |    |
//   ---    ---
//   |        |
//   |        |
//   |        ------|
//   ---------------|
//     */
//    p = Polygon( varray<Coordinate>(
//        { Coordinate( 10, 10 ), Coordinate( 8, 10 ), Coordinate( 8, 16 ), Coordinate( 2, 16 ),
//          Coordinate( 2, 10 ), Coordinate( 0, 10 ), Coordinate( 0, 0 ), Coordinate( 16, 0 ),
//          Coordinate( 16, 2 ), Coordinate( 10, 2 ) } ) );
//    REQUIRE( !p.convex( ) );
//    REQUIRE_FLOAT_EQ( p.area( ), 148 );
//
//    // todo test clockwise polygon
//}
//
//TEST_CASE( "test intersections" ) {
//    // todo add more
//    REQUIRE( (bool)Intersection( Rectangle( 10, 10, Coordinate( 5, 5 ) ),
//                                 Circle( 8, Coordinate( 0, 16 ) ) ) );
//}
