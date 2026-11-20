#ifndef TestsPhysics_hpp
#define TestsPhysics_hpp

#include "axn/tests/Tests.hpp"

#include "axn/physics/Spring.hpp"

namespace axn
{

test( "Spring tests" )
{
    section( "at its rest length it pushes and pulls nothing" )
    {
        Spring spring( Point( 0.0, 0.0 ), Point( 10.0, 0.0 ), 0.5 );

        check( spring.rest_length( ) == Planc( 10.0 ) );
        check( !spring.force( ).has_magnitude( ) );

        spring.update( );
        check( spring.end1( ).position( ) == Point( 0.0, 0.0 ) );
        check( spring.end2( ).position( ) == Point( 10.0, 0.0 ) );
    }

    section( "stretched, it pulls the ends together; squeezed, it pushes them apart" )
    {
        Spring spring( Point( 0.0, 0.0 ), Point( 10.0, 0.0 ), 0.5 );

        spring.end2( ).position( Point( 12.0, 0.0 ) );
        check( is_pos( spring.stretch( ) ) );
        check( is_pos( spring.force( ).dx( ) ) ); // C! end1 is pulled towards end2

        spring.update( );
        check( spring.length( ) < Planc( 12.0 ) );

        spring.end1( ).velocity( V0 );
        spring.end2( ).velocity( V0 );
        spring.end1( ).position( Point( 0.0, 0.0 ) );
        spring.end2( ).position( Point( 8.0, 0.0 ) );
        check( is_neg( spring.force( ).dx( ) ) ); // C! end1 is pushed away from end2
    }

    section( "a held end stays put, and the other swings on it" )
    {
        Spring spring( Point( 0.0, 0.0 ), Point( 0.0, -10.0 ), 0.5 );
        spring.hold_end1( true );
        spring.end2( ).position( Point( 0.0, -15.0 ) );

        for_range( i, 10 ) { spring.update( ); }

        check( spring.end1( ).position( ) == Point( 0.0, 0.0 ) );
        check( spring.end2( ).position( ) != Point( 0.0, -15.0 ) );
    }

    section( "with damping it settles at its rest length" )
    {
        Spring spring( Point( 0.0, 0.0 ), Point( 10.0, 0.0 ), 0.2, 0.3 );
        spring.end2( ).position( Point( 20.0, 0.0 ) );

        for_range( i, 1000 ) { spring.update( ); }

        check( equal( dec( spring.length( ) ), 10.0, 0.01 ) );
    }

    section( "the ends' pulls cancel out, so equal masses keep their middle where it was" )
    {
        Spring spring( Point( 0.0, 0.0 ), Point( 10.0, 0.0 ), 0.5 );
        spring.end2( ).position( Point( 14.0, 4.0 ) );

        for_range( i, 50 ) { spring.update( ); }

        check( midpoint( spring.end1( ).position( ), spring.end2( ).position( ) ) == Point( 7.0, 2.0 ) );
    }
}

} // namespace axn

#endif /* TestsPhysics_hpp */
