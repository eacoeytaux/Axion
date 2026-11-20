#ifndef Spring_hpp
#define Spring_hpp

#include "axn/geometry/Geometry.hpp"
#include "axn/physics/Mass.hpp"

namespace axn
{
namespace physics
{

// C! a spring with a Mass on each end: it pulls them together when stretched past its rest length and pushes them apart when
// C! squeezed (Hooke's law), and damping slows the ends moving towards or away from each other, so it settles rather than bouncing forever.
// C! Each update( ) moves the ends by one step; an end that's held doesn't move, so a spring can hang from a fixed point
class Spring
{

public:

    // C! the rest length starts as the distance between the ends
    Spring( Point cref in_end1, Point cref in_end2, dec in_stiffness, dec in_damping = 0.0, dec in_mass1 = 1.0, dec in_mass2 = 1.0 )
        : m_end1( in_end1 ), m_end2( in_end2 ), m_rest_length( in_end1.distance_to( in_end2 ) ), m_stiffness( in_stiffness ), m_damping( in_damping ) { m_end1.mass( in_mass1 ); m_end2.mass( in_mass2 ); }

    Mass cref end1( ) const { return( m_end1 ); }
    Mass ref end1( ) { return( m_end1 ); }
    Mass cref end2( ) const { return( m_end2 ); }
    Mass ref end2( ) { return( m_end2 ); }

    // C! a held end stays where it's put, whatever pulls on it
    bool end1_held( ) const { return( m_end1_held ); }
    Spring ref hold_end1( bool in_held ) { m_end1_held = in_held; if( in_held ) { m_end1.velocity( V0 ); } rethis; }
    bool end2_held( ) const { return( m_end2_held ); }
    Spring ref hold_end2( bool in_held ) { m_end2_held = in_held; if( in_held ) { m_end2.velocity( V0 ); } rethis; }

    Planc cref rest_length( ) const { return( m_rest_length ); }
    void rest_length( Planc cref in_rest_length ) { m_rest_length = in_rest_length; }

    dec stiffness( ) const { return( m_stiffness ); } // C! the force for each unit it's stretched or squeezed
    void stiffness( dec in_stiffness ) { m_stiffness = in_stiffness; }

    dec damping( ) const { return( m_damping ); } // C! the force for each unit of speed the ends move apart or together
    void damping( dec in_damping ) { m_damping = in_damping; }

    Planc length( ) const { return( m_end1.position( ).distance_to( m_end2.position( ) ) ); }
    Planc stretch( ) const { return( length( ) - rest_length( ) ); } // C! how much longer than its rest length it is, negative when squeezed

    // C! the force on end1 (end2 gets the opposite): towards end2 when stretched, away when squeezed, none when the ends are on top of each other
    Vector force( ) const
    {
        Vector along( m_end1.position( ), m_end2.position( ) );
        return_if( !along.has_magnitude( ), V0 );

        Vector direction = along.normalized( );
        Vector relative_velocity = m_end2.velocity( ) - m_end1.velocity( );
        dec closing_speed = ( dec( relative_velocity.dx( ) ) * dec( direction.dx( ) ) ) + ( dec( relative_velocity.dy( ) ) * dec( direction.dy( ) ) ); // C! positive while the ends move apart

        return( direction * ( ( m_stiffness * dec( stretch( ) ) ) + ( m_damping * closing_speed ) ) );
    }

    // C! moves both ends on by in_time (one tick by default): the force changes their velocities first, then the velocities move them
    void update( ticks in_time = 1.0 )
    {
        Vector f = force( );

        if( !end1_held( ) ) { m_end1.add_force( f * in_time ); m_end1.move( m_end1.velocity( ) * in_time ); }
        if( !end2_held( ) ) { m_end2.add_force( -f * in_time ); m_end2.move( m_end2.velocity( ) * in_time ); }
    }

private:

    Mass m_end1;
    Mass m_end2;

    bool m_end1_held = false;
    bool m_end2_held = false;

    Planc m_rest_length = 0.0;
    dec m_stiffness = 0.0;
    dec m_damping = 0.0;

};

} // namespace physics
} // namespace axn

#endif /* Spring_hpp */
