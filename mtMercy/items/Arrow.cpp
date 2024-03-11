#include "Arrow.hpp"

#include "World.hpp"

AXN_NAMESPACES
using axn::reality::Arrow;

const ufloat SHAFT_LENGTH = 32.f;
const Color TIP_COLOR = GRAY_LIGHT;
const Color SHAFT_COLOR = Color::rgb( 0x8B4513 );

Arrow::Arrow( World * world, const Coordinate & tip, const Vector & launch_speed, const Color & _feather_color )
: Object( world, tip )
{
    gravity_ratio( 0.5f );
    velocity( launch_speed );
    
    m_feather_color = _feather_color;
    angle( launch_speed.angle() );
}

const Arrow & Arrow::render( ) const {
    Object::render( );
    
    Angle motion_angle = angle();
    Coordinate tip = ORIGIN;
    Coordinate back = tip - Vector( (planc)SHAFT_LENGTH, (planc)0.f );
    
    Shape head( {
        tip + Vector( (planc)4.f, (planc)0.f ),
        tip + Vector( (planc)-4.f, (planc)4.f ),
        tip + Vector( (planc)-4.f, (planc)-4.f )
    } );
    Shape feather( {
        back + Vector( (planc)4.f, (planc)0.f ),
        back + Vector( (planc)0.f, (planc)4.f ),
        back + Vector( (planc)-8.f, (planc)4.f ),
        back + Vector( (planc)-4.f, (planc)0.f ),
        back + Vector( (planc)-8.f, (planc)-4.f ),
        back + Vector( (planc)0.f, (planc)-4.f )
    } );
    
    back.rotate( motion_angle );
    head.rotate( motion_angle );
    feather.rotate( motion_angle );
    
    draw( SHAFT_COLOR, Line( tip, back ), 3.f );
    draw( TIP_COLOR, head );
    draw( m_feather_color, feather );
    
    return *this;
}

Arrow & Arrow::update( ) {
    if ( velocity().magnitude() ) angle( velocity().angle() );
    Object::update( );
    return *this;
}

Arrow & Arrow::move( ) {
    //RectangleFixed delta = RectangleFixed( position(), position() + velocity() );
    //varray<shared_ptr<Object>> possible_hits = world()->objects_in_range( delta );
    //for_each ( possible_hit, possible_hits ) {
    //    if ( id() == possible_hit->id() ) continue;
    //    // TODO how to determine collision types ugh
    //    if ( dynamic_pointer_cast<Mob>( possible_hit ) ) dynamic_pointer_cast<Mob>( possible_hit )->hurt( 32 );
    //}
    Object::move( );
    return *this;
}

Angle Arrow::angle( ) const {
    return m_angle;
}

Arrow & Arrow::angle( const Angle & _angle ) {
    m_angle = _angle;
    drawing_dirty( true );
    return *this;
}

ufloat Arrow::length( ) const {
    return SHAFT_LENGTH;
}
