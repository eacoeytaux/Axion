#include "Bird.hpp"

#include "World.hpp"

AXN_NAMESPACES
using axn::reality::Feather;
using axn::reality::Bird;

const uint BIRD_HEALTH = 1;

const ufloat FEATHER_LENGTH = 15.f;
const ufloat FEATHER_BASE_LENGTH = 5.f;
const ufloat FEATHER_STEM_LENGTH = 3.f;
const ufloat FEATHER_STEM_THICKNESS = 2.f;

const ufloat SPEED = 3.f;
const ufloat ROTATIONAL_SPEED = 2.f;
const ufloat ROTATIONAL_RADIUS = 160.f;
const ufloat LIFT_SPEED = 1.f;
const ufloat LIFT_DISTANCE = 16.f;

Feather::Feather( World * world, const Coordinate & _position )
: Object( world, _position )
{
    m_color = Random::rng_Color();
    gravity_ratio( 0.05 );
}

const Feather & Feather::render( ) const {
    Object::render( );
    
    Coordinate center = position( );
    //Polygon feather = Polygon( { center + Vector( FEATHER_LENGTH, 0.f ), center + Vector( (planc)0.f, (planc)-((float)FEATHER_BASE_LENGTH) ), center + Vector( (planc)-((float)FEATHER_BASE_LENGTH), 0.f ), center + Vector( (planc)0.f, FEATHER_BASE_LENGTH ) } );
    //Line stem = Line( position(), position() - Vector( FEATHER_BASE_LENGTH + FEATHER_STEM_LENGTH ) );
    //draw( m_color, feather );
    //draw( m_color, stem, FEATHER_STEM_THICKNESS );
    
    return *this;
}

Bird::Bird( World * world, const Coordinate & _position )
: Mob( world, _position )
{
    space( Shape::rectangle( 32.f, 32.f ) );
    gravity_ratio( 0.f );
    heal( BIRD_HEALTH );
}

const Bird & Bird::render( ) const {
    Mob::render( );
    
    Color color = RED; // Random::rng_Color();
    draw( color, Line( position() + Vector( (planc)-16.f, (planc)0.f ), position() + Vector( (planc)16.f, (planc)0.f ) ), 4.f);

    Shape wing = Shape( { Coordinate( 0.f, 0.f ), Coordinate( -24.f, sin( world()->age() / 16.f ) * 16.f ), Coordinate( 16.f, 0.f ) } );
    if ( velocity().dx() < 0.f ) wing.mirror_y( );
    wing += position();
    draw( RED, wing );
    return *this;
}

Bird & Bird::update( ) {
    if ( alive() && world()->player() ) {
        //Coordinate target = world()->player()->position();
        //target += Vector( (planc)0.f, (planc)270.f );
        //target += Vector( sin( (float)(world()->age()) * ROTATIONAL_SPEED / ROTATIONAL_RADIUS ) * ROTATIONAL_RADIUS, 0.f );
        //target += Vector( (planc)0.f, -sin( ( world()->age() - (LIFT_DISTANCE.half()) ) * LIFT_SPEED / LIFT_DISTANCE ) * LIFT_DISTANCE );
        //Vector movement = Vector( position(), target );
        //if ( movement.dx() > 0.f ) movement.dx( min<float>( SPEED, movement.dx() ) );
        //else movement.dx(max<float>( -SPEED, movement.dx() ) );
        //velocity( movement );
    }
    
    Mob::update( );
    return *this;
}

Bird & Bird::die( ) {
    gravity_ratio( 1 );
    Mob::die( );
    return *this;
}
