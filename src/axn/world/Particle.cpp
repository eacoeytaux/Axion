#include "axn/world/Particle.hpp"

#include "axn/world/World.hpp"
#include "axn/world/Mob.hpp"

Particle::Particle( ptr<Room> in_room, Point cref in_position, Vector cref in_velocity ) : Object( in_room, in_position, in_velocity )
{
    needs_render_always( true );

    solid( false );
    interactive( false );

    no_gravity( );
    no_air_resistance( );
    terrain_bound( false );
}

void Particle::render( ) { draw( m_color.a( m_alpha ), Polygon::equilateral( m_sides, m_radius, m_rotation ) ); Object::render( ); }
void Particle::update( ) { if( !is_pos( m_radius ) || !is_pos( m_alpha ) ) { mark_deleted( ); } Object::update( ); }
void Particle::pop( ) { }

bool Particle::collide( ptr<Object> in_object ) { return( false ); }

void Particle::ground( ptr<Terrain::Node> in_node, Terrain::Bumper cref in_bumper ) { }