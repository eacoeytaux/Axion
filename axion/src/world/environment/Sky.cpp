#include "Sky.hpp"

#include "World.hpp"

AXN_NAMESPACES
using axn::reality::Sky;

Sky::Sky( World* world, const int _cloud_regen_rate ) : Background( world ) {
    m_clouds = varray<Cloud*>( );
    m_cloud_regen_rate = _cloud_regen_rate;

    if( _cloud_regen_rate ) {
        m_cloud_timer.reset( Random::rng_int( m_cloud_regen_rate ) );
    }

    m_top_color = BLUE;
    m_bottom_color = Color::rgb( 0xFF7800 );
}

const Sky & Sky::render( ) const {
    Object::render( );

    draw( { m_top_color, m_top_color, m_bottom_color, m_bottom_color },
         Shape::rectangle( world( )->active_camera( )->width( ) + 2.f,
                           world( )->active_camera( )->height( ) + 2.f, ORIGIN ), 0 ); // added + 2 just in case

    return *this;
}

Sky & Sky::update( ) {
    // drawing_dirty(true);
    // if (m_cloud_timer.tick()) {
    //     m_cloud_timer.reset(m_cloud_regen_rate);
    //     m_clouds.push_back(shared_ptr<Cloud>(new Cloud(world())));
    // }
    // for_each (cloud, m_clouds) cloud->update();
    return *this;
}
