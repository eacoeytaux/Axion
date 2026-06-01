#include "mt/environment/misc/Bone.hpp"

var_const( dec MOVEMENT_RATIO = ( 0.25 ) );

var_const( dec SKULL_SPACE_RATIO = ( 0.65 ) );
var_const( Planc SKULL_RADIUS = ( METER / 3.0 ) );

var_const( Angle SKULL_JAW_ANGLE = ( Angle::fraction( 6.0 ) ) );
var_const( dec SKULL_JAW_WIDTH_RATIO = ( 0.65 ) );

var_const( Span<uint> SKULL_TEETH_COUNT = { 3 } );
var_const( Span<Planc> SKULL_TEETH_LENGTH = { 2.75, 3.5 } );
var_const( dec SKULL_TEETH_GAP_WIDTH_RATIO = ( 0.05 ) );

var_const( Planc SKULL_NOSE_RADIUS = ( 0.3 ) );
var_const( Planc SKULL_NOSE_SPACING_Y = ( 0.15 ) );

var_const( Planc SKULL_EYE_RADIUS = ( 0.3 ) );
var_const( dec SKULL_EYE_OFFSET_X = ( 0.4 ) );
var_const( dec SKULL_EYE_OFFSET_Y = ( 0.05 ) );
var_const( dec SKULL_EYE_SPACING = ( 0.5 ) );

var_const( Planc FEMUR_WIDTH = ( 3.0 ) );
var_const( Planc FEMUR_LENGTH = ( 19.0 ) );
var_const( Planc FEMUR_KNUB_RADIUS = ( FEMUR_WIDTH * 0.65 ) );

Bone::Bone( ptr<Room> in_room, Planc cref in_thickness, Point cref in_position ) : Object( in_room, in_position )
{
    persist_render( true );

    interactive( true );

    thickness( in_thickness );

    movement_ratio( MOVEMENT_RATIO );

    dec c = one_minus( Random::rdec( 0.05, 0.15 ) );
    m_bone_color = ( Color::rgb( c, c, c ) );
}

void Bone::update( )
{
    Object::update( );

    if( grounded( ) && is_pos( rotation_distance( ) ) )
    {
        transform( Transform::rotate( TAU * Random::negated( ( velocity( ).magnitude( ) / rotation_distance( ) ), !is_neg( velocity( ).dx( ) ) ) ) );
    }
}

bool Bone::collide( ptr<Object> in_object )
{
    Object::collide( in_object );

    if( grounded( ) && in_object->interactive( ) )
    {
        if( is_pos( movement_ratio( ) ) )
        {
            // todo check directions line up
            if( ( velocity( ).magnitude( ) / movement_ratio( ) ) < ( in_object->velocity( ).magnitude( ) ) )
            {
                add_velocity( in_object->velocity( ) * movement_ratio( ) );

                return( true );
            }
        }
    }

    return( false );
}

void Bone::thickness( Planc cref in_thickness ) { m_thickness = in_thickness; }
void Bone::rotation_distance( Planc cref in_distance ) { m_rotation_distance = in_distance; }
void Bone::movement_ratio( dec in_ratio ) { m_movement_ratio = in_ratio; }

Skull::Skull( ptr<Room> in_room, Point cref in_position ) : Bone( in_room, SKULL_RADIUS, in_position )
{
    Assert( ( SKULL_TEETH_COUNT.max( ) * SKULL_TEETH_GAP_WIDTH_RATIO ) < 1.0 );

    space( Polygon::circle( thickness( ) * SKULL_SPACE_RATIO ) );

    rotation_distance( Circle::circumference( thickness( ) ) );

    varray<Point> skull_arc = ( Arc::ccw( thickness( ), ( -RIGHT + SKULL_JAW_ANGLE ), ( -RIGHT - SKULL_JAW_ANGLE ) ).points( ) );

    Planc skull_base_length = ( skull_arc.front( ).distance_to( skull_arc.back( ) ) );
    Point skull_base_mid = ( midpoint( skull_arc.front( ), skull_arc.back( ) ) );

    Planc jaw_length = ( skull_base_length * SKULL_JAW_WIDTH_RATIO );
    Line jaw_line = ( Line( skull_base_mid - VectorX( half( jaw_length ) ),
                            skull_base_mid + VectorX( half( jaw_length ) ) ) );

    uint teeth_count = ( Random::rint( SKULL_TEETH_COUNT ) );

    Planc tooth_gap = ( jaw_line.length( ) * SKULL_TEETH_GAP_WIDTH_RATIO );
    Planc tooth_width = ( ( jaw_length - ( tooth_gap * max<uint>( 0, teeth_count - 1 ) ) ) / teeth_count );

    Vector offset;
    for_range( i, teeth_count )
    {
        Planc t_width = ( tooth_width );
        Planc t_height = ( Random::rPlanc( SKULL_TEETH_LENGTH ) );

        Polygon tooth = ( Polygon::rectangle( t_width, t_height ) + Vector( half( t_width ), -half( t_height ) ) + jaw_line.pt1( ) + offset );

        draw( bone_color( ), tooth );

        offset += VectorX( t_width + tooth_gap );
    }

    draw( bone_color( ), Polygon( skull_arc ) );

    draw( BLACK, Polygon::equilateral( 3, ( SKULL_NOSE_RADIUS * thickness( ) ), PointY( -SKULL_NOSE_SPACING_Y * thickness( ) ) ) );

    draw( BLACK, Polygon::circle( ( SKULL_EYE_RADIUS * thickness( ) ), Point( ( +SKULL_EYE_SPACING * thickness( ) ), ( SKULL_EYE_OFFSET_Y * thickness( ) ) ) ) );
    draw( BLACK, Polygon::circle( ( SKULL_EYE_RADIUS * thickness( ) ), Point( ( -SKULL_EYE_SPACING * thickness( ) ), ( SKULL_EYE_OFFSET_Y * thickness( ) ) ) ) );

    transform( Transform::rotate( Random::rAngle( ) ) );
}

Femur::Femur( ptr<Room> in_room, Point cref in_position ) : Bone( in_room, FEMUR_WIDTH, in_position )
{
    Assert( x2( FEMUR_KNUB_RADIUS ) >= FEMUR_WIDTH );

    space( Polygon::square( FEMUR_WIDTH ) );

    rotation_distance( x2( FEMUR_LENGTH ) );

    draw( bone_color( ), Line( -VectorX( half( FEMUR_LENGTH ) ), VectorX( half( FEMUR_LENGTH ) ) ), FEMUR_WIDTH );

    draw( bone_color( ), Polygon::circle( FEMUR_KNUB_RADIUS, Vector( half( -FEMUR_LENGTH ), -FEMUR_KNUB_RADIUS ) ) );
    draw( bone_color( ), Polygon::circle( FEMUR_KNUB_RADIUS, Vector( half( -FEMUR_LENGTH ), +FEMUR_KNUB_RADIUS ) ) );
    draw( bone_color( ), Polygon::circle( FEMUR_KNUB_RADIUS, Vector( half( +FEMUR_LENGTH ), -FEMUR_KNUB_RADIUS ) ) );
    draw( bone_color( ), Polygon::circle( FEMUR_KNUB_RADIUS, Vector( half( +FEMUR_LENGTH ), +FEMUR_KNUB_RADIUS ) ) );

    transform( Transform::rotate( Random::rAngle( ) ) );
}