#include "Arrow.hpp"

#include "World.hpp"
#include "Mob.hpp"

using mtmercy::Arrow;

namespace
{
const double DAMAGE = 10.0;

const uint LIFESPAN = 300;
const uint FADESPAN = 50;

const Planc HEAD_LENGTH_INNER = 4.0;
const Planc HEAD_LENGTH_OUTER = 6.0;
const Planc HEAD_WIDTH = 3.0;
const Planc HEAD_BUFFER = 1.0;
const Planc SHAFT_LENGTH = 30.0;
const Planc SHAFT_THICKNESS = 3.0;

const Polygon HEAD( { Coordinate( HEAD_BUFFER, 0.0 ),
                      Coordinate( -HEAD_LENGTH_OUTER, -HEAD_WIDTH ),
                      Coordinate( -HEAD_LENGTH_INNER, 0.0 ),
                      Coordinate( -HEAD_LENGTH_OUTER, HEAD_WIDTH ) } );

const Polygon HEAD_BORDER = Polygon::expand( HEAD, 1.0 );

const Polygon FEATHER( { Coordinate( 4.0, 0.0 ),
                         Coordinate( 0.0, 4.0 ),
                         Coordinate( -8.0, 4.0 ),
                         Coordinate( -4.0, 0.0 ),
                         Coordinate( -8.0, -4.0 ),
                         Coordinate( 0.0, -4.0 ) } );

const Color TIP_COLOR = GRAY_LIGHT;
const Color SHAFT_COLOR = Color::rgb( 0x8B4513 );
} // namespace

Arrow::Arrow( World * world, const Coordinate & tip, const Vector & launch_speed, const Color & _feather_color ) : Object( world, tip )
{
#ifdef AXN_DEBUG
    draw_debug = true;
#endif

    solid( true );

    gravity_ratio( 0.5 );
    velocity( launch_speed );

    m_feather_color = _feather_color;
    angle( launch_speed.angle( ) );

    track_position( 1 );
}

double Arrow::fade_alpha( ) const
{
    if( age( ) < LIFESPAN )
        return 1.0;
    else
        return ( 1.0 - ( ( age( ) - (double)LIFESPAN ) / (double)FADESPAN ) );
}

const Arrow & Arrow::render( ) const
{
    Object::render( );

    render_shaft( );
    render_head( );
    render_feather( );

    return *this;
}

void Arrow::render_head( ) const
{
    Polygon head = HEAD;
    head.rotate( angle( ) );

    draw( TIP_COLOR.a( fade_alpha( ) ), head );
}

void Arrow::render_shaft( ) const
{
    Coordinate tip( -HEAD_LENGTH_INNER, 0.0 );
    Coordinate back( -( SHAFT_LENGTH + HEAD_LENGTH_INNER ), 0.0 );
    tip.rotate( angle( ) );
    back.rotate( angle( ) );

    draw( SHAFT_COLOR.a( fade_alpha( ) ), Line( tip, back ), 3.0 );
}

void Arrow::render_feather( ) const
{
    Polygon feather = FEATHER;
    feather.move( Vector( -( SHAFT_LENGTH + HEAD_LENGTH_INNER ), 0.0 ) );
    feather.rotate( angle( ) );

    draw( m_feather_color.a( fade_alpha( ) ), feather );
}

Arrow & Arrow::update( )
{
    bool airborn = !Object::ground( );

    Object::update( );

    if( !Object::ground( ) && velocity( ).has_magnitude( ) )
        angle( velocity( ).angle( ) );
    else if( airborn )
        drawing_dirty( true );

    if( age( ) >= LIFESPAN )
    {
        drawing_dirty( true );
        if( age( ) >= LIFESPAN + FADESPAN )
            mark_deleted( );
    }

    return *this;
}

Angle Arrow::angle( ) const { return m_angle; }

Arrow & Arrow::angle( const Angle & _angle )
{
    m_angle = _angle;
    drawing_dirty( true );
    return *this;
}

double Arrow::length( ) const { return SHAFT_LENGTH + HEAD_LENGTH_INNER; }

Arrow & Arrow::ground( TerrainEdge * ground )
{
    Object::ground( ground );

    if( ground )
    {
        stationary( true );
        solid( false );
    }

    return *this;
}

bool Arrow::collide( Object * object )
{
    Object::collide( object );

    if( solid( ) && !dynamic_cast<Arrow *>( object ) )
    {
        if( Mob * mob = dynamic_cast<Mob *>( object ) )
        {
            mob->hurt( DAMAGE );
        }

        subscribe_to_movement( object );
        stationary( true );
        solid( false );

        return true;
    }

    return false;
}

Arrow & Arrow::react_to_movement( Object * object, const Vector & _v )
{
    Object::react_to_movement( object, _v );

    position( position( ) + _v );

    return *this;
}
