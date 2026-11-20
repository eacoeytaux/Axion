#ifndef SkeletonRig_hpp
#define SkeletonRig_hpp

#include "axn/world/Object.hpp"

namespace axn
{
namespace reality
{

#if defined( AXN_DEBUG )
section( SKELETON_RIG_DEBUG )
{
    var_const( Planc SKELETON_RIG_LINE_THICKNESS = ( 1.0 ) ); // C!
    var_const( Color SKELETON_RIG_COLOR = ( WHITE ) ); // C!
}
#endif

// C! a tree of points hanging off the object's position (the origin): each point is an offset from the point it hangs off (its parent),
// C! and has a rotation of its own
// C!   - a point's rotation is its parent's rotation plus its own, all the way back to the object's rotation( )
// C!   - its offset is turned by its parent's rotation, so moving or turning the object, or turning any point, carries every point after it along
// C! T1 is an enum naming the rig's points; its first value (the NO_x = 0 every enum starts with) stands for the origin
classT SkeletonRig : public Object
{

public:

    SkeletonRig( ptr<Room> in_room, Point cref in_position = ORIGIN, Angle cref in_rotation = A0 ) : Object( in_room, in_position )
    {
        #if defined( AXN_DEBUG )
        draw_debug = true;
        #endif

        rotate( in_rotation );
    }

    #if defined( AXN_DEBUG )
    // C! a line from each point back to the point it hangs off, while the DEBUG_SKELETONS setting is on
    virtual Drawing debug_overlay( ) const override
    {
        Drawing overlay = Object::debug_overlay( );

        return_if( !Settings::get( Settings::DEBUG_SKELETONS ), overlay ); // C! the lines only while skeletons are shown

        for_each_const( rig_pt, points( ) )
        {
            Point at = point_position( rig_pt ) - Vector( position( ) ); // C! relative to the object, like every debug overlay
            Point parent_at = point_position( parent( rig_pt ) ) - Vector( position( ) );

            overlay.draw( SKELETON_RIG_COLOR, Line( parent_at, at ), SKELETON_RIG_LINE_THICKNESS, true );
        }

        return( overlay );
    }
    #endif

    // C! adds in_point, or replaces it, hanging in_offset from in_parent (the origin when it's the enum's first value)
    // C! the parent has to be added first, and a point can't hang off one of the points hanging off it
    void add( T1 in_pt, T1 in_parent, Vector cref in_offset, Angle cref in_rotation = A0 )
    {
        Assert( ( !is_origin( in_pt ) ), "the origin is always there, it can't be added" );
        Assert( ( has( in_parent ) ), "the parent has to be added before the points hanging off it" );
        return_if( is_origin( in_pt ) || !has( in_parent ) );

        T1 ancestor = in_parent;
        while( !is_origin( ancestor ) )
        {
            Assert( ( ancestor != in_pt ), "a point can't hang off one of its own children" );
            return_if( ancestor == in_pt );

            ancestor = m_pts[ uint( ancestor ) ].parent;
        }

        if( m_pts.size( ) <= uint( in_pt ) ) { m_pts.resize( uint( in_pt ) + 1 ); }

        RigPoint ref pt = m_pts[ uint( in_pt ) ];
        pt.parent = in_parent;
        pt.offset = in_offset;
        pt.rotation = in_rotation;
        pt.added = true;
    }

    void add( T1 in_pt, Vector cref in_offset, Angle cref in_rotation = A0 ) { add( in_pt, T1( 0 ), in_offset, in_rotation ); } // C! hanging off the origin

    bool has( T1 in_pt ) const { return( is_origin( in_pt ) || ( m_pts.valid_index( uint( in_pt ) ) && m_pts[ uint( in_pt ) ].added ) ); }

    varray<T1> points( ) const { varray<T1> added; for_range( i, m_pts.size( ) ) { if( m_pts[ i ].added ) { added.insert_back( T1( i ) ); } } return( added ); } // C! every point added, not the origin

    T1 parent( T1 in_pt ) const { return( added_point( in_pt ).parent ); }

    // C! the point's own offset and rotation, as added (the origin's own rotation is the object's rotation( ))
    Vector cref offset( T1 in_pt ) const { return( added_point( in_pt ).offset ); }
    void offset( T1 in_pt, Vector cref in_offset ) { if( has_added( in_pt ) ) { m_pts[ uint( in_pt ) ].offset = in_offset; } }

    Angle cref local_rotation( T1 in_pt ) const { return_if( is_origin( in_pt ), rotation( ) ); return( added_point( in_pt ).rotation ); }
    void local_rotation( T1 in_pt, Angle cref in_rotation ) { if( is_origin( in_pt ) ) { rotate( in_rotation - rotation( ) ); } else if( has_added( in_pt ) ) { m_pts[ uint( in_pt ) ].rotation = in_rotation; } }

    // C! where the point is, and which way it faces, once the object and every point before it are taken into account
    Angle point_rotation( T1 in_pt ) const
    {
        Assert( ( has( in_pt ) ), "no such point in the rig" );
        return_if( is_origin( in_pt ) || !has( in_pt ), rotation( ) );

        RigPoint cref pt = m_pts[ uint( in_pt ) ];

        return( point_rotation( pt.parent ) + pt.rotation );
    }

    Point point_position( T1 in_pt ) const
    {
        Assert( ( has( in_pt ) ), "no such point in the rig" );
        return_if( is_origin( in_pt ) || !has( in_pt ), position( ) );

        RigPoint cref pt = m_pts[ uint( in_pt ) ];

        Vector offset = pt.offset;
        offset.rotate( point_rotation( pt.parent ) );

        return( point_position( pt.parent ) + offset );
    }

private:

    struct RigPoint
    {

        T1 parent = T1( 0 );
        Vector offset = V0;
        Angle rotation = A0;
        bool added = false;

    };

    varray<RigPoint> m_pts; // C! indexed by the enum, so it has a gap for the origin and for any point not added

    static bool is_origin( T1 in_pt ) { return( is_zero( uint( in_pt ) ) ); }

    bool has_added( T1 in_pt ) const { return( !is_origin( in_pt ) && has( in_pt ) ); }

    RigPoint cref added_point( T1 in_pt ) const
    {
        static const RigPoint c_none; // C! what's given back for the origin or a point that isn't there

        Assert( ( has_added( in_pt ) ), "no such point in the rig" );
        return_if( !has_added( in_pt ), c_none );

        return( m_pts[ uint( in_pt ) ] );
    }

};

} // namespace reality
} // namespace axn

#endif /* SkeletonRig_hpp */
