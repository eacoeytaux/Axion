#include "mt/environment/plants/Tree.hpp"

var_const( dec LEAF_GRAVITY_DEFAULT = ( 0.025 ) );
var_const( Angle LEAF_ROTATION_SPEED_DEFAULT = ( 0.05 ) );
var_const( ticks LEAF_SPIN_RATE_DEFAULT = ( 50.0 ) );
var_const( ticks LEAF_SWAY_RATE_DEFAULT = ( 45.0 ) );
var_const( Planc LEAF_SWAY_DISTANCE_DEFAULT = ( 9.0 ) );

Tree::Tree( ptr<Room> in_room, Point cref in_root, dec in_z ) : Object( in_room, in_root, in_z )
{
    m_leaf_gravity = LEAF_GRAVITY_DEFAULT;
}

void Tree::update( )
{
    Object::update( );

    for_each( leaf_drop, m_leaf_drops )
    {
        if( leaf_drop.countdown.tick_auto_reset( ) )
        {
            room( )->add_object( new Leaf( room( ), leaf_drawing( ), leaf_drop.path.point( Random::rdec( ) ) + position( ), leaf_gravity( ), z( ) ) );
        }
    }
}

Tree::Leaf::Leaf( ptr<Room> in_room, Drawing cref in_drawing, Point cref in_center, dec in_gravity_scale, dec in_z ) : Object( in_room, in_center, in_z ), m_drawing( in_drawing )
{
    background( true );
    needs_render_always( true );

    gravity_scale( LEAF_GRAVITY_DEFAULT );

    m_rotation_speed = LEAF_ROTATION_SPEED_DEFAULT;

    m_spin.wavelength( LEAF_SPIN_RATE_DEFAULT );
    m_spin.offset( Random::rAngle( ) );

    m_sway.amplitude( LEAF_SWAY_DISTANCE_DEFAULT );
    m_sway.wavelength( LEAF_SWAY_RATE_DEFAULT );
    m_sway.offset( Random::rAngle( ) );
}

void Tree::Leaf::render( )
{
    Object::render( );

    draw( copy( m_drawing ).stretch( VectorX( m_spin.at( age( ) ) ) ).rotate( m_rotation_speed * age( ) ).move( VectorX( m_sway.at( age( ) ) ) ) );
}

void Tree::Leaf::ground( ptr<Terrain::Node> ground, Terrain::Bumper cref in_bumper )
{
    Object::ground( ground, in_bumper );

    if( ground )
    {
        mark_to_delete( ); // todo add time variable to leaf
    }
}
