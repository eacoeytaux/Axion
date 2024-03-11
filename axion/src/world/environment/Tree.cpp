#include "Tree.hpp"

#include "World.hpp"

AXN_NAMESPACES
using axn::reality::Tree;

const Angle MAX_SWAY = PI / 32;
const ufloat SWAY_PERIOD = 40.f;
const ufloat TRUNK_HEIGHT = 64.f;
const ufloat TRUNK_THICKNESS = 16.f;
const ufloat SUB_TRUNK_HEIGHT = 100.f;
const uint LEAVES_COUNT = 3;
const ufloat LEAVES_HEIGHT_BASE_LENGTH = 160.f;
const ufloat LEAVES_SHRINK_RATIO = 0.64f;
const ufloat LEAVES_BORDER_WIDTH = 4.f;

const Color TRUNK_COLOR = Color::rgb( 0xA56405 );
const Color TRUNK_BORDER_COLOR = Color::rgb( 0x804800 );
const Color LEAVES_COLOR = Color::rgb( 0x00C000 );
const Color LEAVES_BORDER_COLOR = Color::rgb( 0x008000 );

Tree::Tree( World* world, const Coordinate & _root, const ufloat _z ) : Object( world, _root ) {
    drawing_always_dirty( true );
    z( _z );
}

const Tree & Tree::render( ) const {
    Object::render( );

    Coordinate base;
    Angle sway_angle = Angle( sin( world( )->age( ) / SWAY_PERIOD ) * MAX_SWAY.radians( ) );
    
    Drawing trunk;
    Vector trunk_vector( Vector( base, base + VectorA( sway_angle + PI_1D2, TRUNK_HEIGHT ) ) );
    trunk.add( TRUNK_COLOR, Line( trunk_vector.origin( ), trunk_vector.destination( ) ), TRUNK_THICKNESS );
    trunk.border( TRUNK_BORDER_COLOR, LEAVES_BORDER_WIDTH );
    draw( trunk );

    Coordinate top = trunk_vector.destination( );
    ufloat shink_ratio = LEAVES_SHRINK_RATIO;
    
    Drawing leaves;
    leaves.border( LEAVES_BORDER_COLOR, LEAVES_BORDER_WIDTH );
    
    for_range( LEAVES_COUNT ) {
        Vector sub_trunk(
            top, top + VectorA( ( sway_angle * ( i + 2.f ) ) + PI_1D2, SUB_TRUNK_HEIGHT * shink_ratio ) );
        // base leaves triangle
        Shape leaf_bunch = Shape( { Coordinate( 0.f, 0.f ),
                        Coordinate( -LEAVES_HEIGHT_BASE_LENGTH * shink_ratio / 2.f,
                                    -LEAVES_HEIGHT_BASE_LENGTH * shink_ratio ),
                        Coordinate( LEAVES_HEIGHT_BASE_LENGTH * shink_ratio / 2.f,
                                    -LEAVES_HEIGHT_BASE_LENGTH * shink_ratio ) } );
        leaf_bunch += top + sub_trunk;
        leaf_bunch.rotate( sway_angle * ( i + 2.f ), leaf_bunch.coordinates( )[ 0 ] );
        
        leaves.add( LEAVES_COLOR, leaf_bunch );
        
        top = ( sub_trunk * 0.75f ).destination( );
        shink_ratio *= LEAVES_SHRINK_RATIO;
    }
    
    draw( leaves );

    return *this;
}
