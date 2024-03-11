#include "Cloud.hpp"

AXN_NAMESPACES
using axn::reality::Cloud;

const ufloat X_STRETCH = 2.f;
// const uint MIN_SMALL_PUFF_COUNT = 10;
// const uint MAX_SMALL_PUFF_COUNT = 16;
// const planc MIN_SMALL_PUFF_RADIUS = 8;
const planc MAX_SMALL_PUFF_RADIUS = 20.f;
const planc MAX_SMALL_PUFF_DISTANCE = 80.f;
// const uint MIN_LARGE_PUFF_COUNT = 16;
// const uint MAX_LARGE_PUFF_COUNT = 24;
// const planc MIN_LARGE_PUFF_RADIUS = 25.f;
const planc MAX_LARGE_PUFF_RADIUS = 64;
const planc MAX_LARGE_PUFF_DISTANCE = 60.f;
const ufloat PUFF_OUTLINE_RATIO = 0.64f;
const Color INSIDE_COLOR = Color::rgb( 0x87CDEB );

Cloud::Puff::Puff( const planc & _radius, const Vector & _center_offset ) {
    m_radius = _radius;
    m_center_offset = _center_offset;
}

Cloud::Cloud( World* world ) : Object( world ) {
    z( 0.1f );
    gravity_ratio( 0.f );

    // set location

    // ufloat x = max_dx();
    // if (world->player()) x = world->player()->position().x() +
    // (world->camera()->screen().width().half()) + max_dx(); Coordinate
    // location(x, Random::rng_float(max_dy())); position(location);
    // velocity(world()->wind());
    //
    // int small_puff_count = Random::rng_int(MIN_SMALL_PUFF_COUNT,
    // MAX_SMALL_PUFF_COUNT); for_range (small_puff_count) {
    //     Vector offset(Random::r_Angle(),
    //     Random::rng_float(MAX_LARGE_PUFF_DISTANCE, MAX_SMALL_PUFF_DISTANCE));
    //     offset.dx(offset.dx() * X_STRETCH);
    //     m_puffs.push_back(_puff(Random::rng_int(MIN_SMALL_PUFF_RADIUS,
    //     MAX_SMALL_PUFF_RADIUS), offset));
    // }
    //
    // int large_puff_count = Random::rng_int(MIN_LARGE_PUFF_COUNT,
    // MAX_LARGE_PUFF_COUNT); for_range (large_puff_count) {
    //     Vector offset(Random::r_Angle(),
    //     Random::rng_float(MAX_LARGE_PUFF_DISTANCE)); offset.dx(offset.dx() *
    //     X_STRETCH);
    //     m_puffs.push_back(_puff(Random::rng_int(MIN_LARGE_PUFF_RADIUS,
    //     MAX_LARGE_PUFF_RADIUS), offset));
    // }
}

const Cloud & Cloud::render( ) const {
    for_each( puff, m_puffs )
        draw( WHITE, Shape::circle( puff.m_radius, position( ) + puff.m_center_offset ), FILLED );
    for_each( puff, m_puffs )
        draw( INSIDE_COLOR,
              Shape::circle( puff.m_radius * PUFF_OUTLINE_RATIO, position( ) + puff.m_center_offset ),
              FILLED );
    return *this;
}

planc Cloud::max_dx( ) {
    return max<planc>( ( MAX_SMALL_PUFF_DISTANCE * X_STRETCH ) + MAX_SMALL_PUFF_RADIUS,
                       ( MAX_LARGE_PUFF_DISTANCE * X_STRETCH ) + MAX_LARGE_PUFF_RADIUS ) *
           2.f;
}

planc Cloud::max_dy( ) {
    return max<planc>( MAX_SMALL_PUFF_RADIUS + MAX_SMALL_PUFF_DISTANCE,
                       MAX_LARGE_PUFF_RADIUS + MAX_LARGE_PUFF_DISTANCE ) *
           2.f;
}
