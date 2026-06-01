#ifndef Crescent_hpp
#define Crescent_hpp

#include "axn/geometry/Planc.hpp"
#include "axn/geometry/Point.hpp"
#include "axn/geometry/Angle.hpp"
#include "axn/geometry/Vector.hpp"
#include "axn/geometry/Line.hpp"
#include "axn/geometry/Path.hpp"
#include "axn/geometry/Arc.hpp"
#include "axn/geometry/Circle.hpp"
#include "axn/geometry/Polygon.hpp"
#include "axn/geometry/Transform.hpp"

namespace axn
{
namespace geometry
{

class Crescent
{

private:

    Circle m_outer;
    Circle m_inner;

public:

    Crescent( Circle cref in_outer, Circle cref in_inner ) : m_outer( in_outer ), m_inner( in_inner ) { }

    Circle cref outer( ) const { return( m_outer ); }
    Circle cref inner( ) const { return( m_inner ); }

    void outer( Circle cref in_outer ) { m_outer = in_outer; }
    void inner( Circle cref in_inner ) { m_inner = in_inner; }

    bool full( ) const { return( !outer( ).intersects( inner( ) ) ); }
    bool empty( ) const { return( inner( ).overlaps( outer( ) ) ); }

    Polygon polygon( ) const
    {
        if( full( ) )
        {
            return( m_outer );
        }
        else if( !empty( ) )
        {
            varray<Point> intersections = m_outer.intersection( m_inner );

            if( intersections.size( ) == 2 )
            {
                Point cref pt1 = intersections[ 0 ];
                Point cref pt2 = intersections[ 1 ];

                Arc arc1 = Arc::ccw( outer( ).radius( ), outer( ).center( ), Angle( outer( ).center( ), pt1 ), Angle( outer( ).center( ), pt2 ) );
                Arc arc2 = Arc::cw( inner( ).radius( ), inner( ).center( ), Angle( inner( ).center( ), pt2 ), Angle( inner( ).center( ), pt1 ) );

                return( Polygon( arc1.points( ) + arc2.points( ) ) );
            }
        }

        return( Polygon( ) );
    }

    operator Polygon( ) const { return( polygon( ) ); }

    Crescent ref operator+=( Vector cref in_vec ) { outer( outer( ) + in_vec ); inner( inner( ) + in_vec ); rethis; }
    Crescent ref operator-=( Vector cref in_vec ) { outer( outer( ) - in_vec ); inner( inner( ) - in_vec ); rethis; }

    Crescent operator+( Vector cref in_vec ) const { return( Crescent( outer( ) + in_vec, inner( ) + in_vec ) ); }
    Crescent operator-( Vector cref in_vec ) const { return( Crescent( outer( ) - in_vec, inner( ) - in_vec ) ); }

    default_equal( Crescent );

};

class DynamicCresent
{

private:

    Crescent m_crescent;

    Planc m_d_radius;

    Planc m_d_offset;
    Angle m_d_offset_rotation;

public:

    DynamicCresent( Crescent cref in_crescent ) : m_crescent( in_crescent ) { }

    Crescent crescent( ) const { return( m_crescent ); }

    Planc d_radius( ) const { return( m_d_radius ); }

    Planc d_offset( ) const { return( m_d_radius ); }
    Angle d_offset_rotation( ) const { return( m_d_offset_rotation ); }

    void d_radius( Planc cref in_d_radius ) { m_d_radius = in_d_radius; }

    void d_offset( Planc cref in_d_offset ) { m_d_offset_rotation = in_d_offset; }
    void d_offset_rotation( Angle cref in_d_offset_rotation ) { m_d_offset_rotation = in_d_offset_rotation; }

    void update( )
    {
        Circle outer = crescent( ).outer( );
        Circle inner = crescent( ).inner( );

        inner.radius( inner.radius( ) + d_radius( ) );

        // TODO add offset!

        m_crescent = Crescent( outer, inner );
    }

    operator Crescent( ) const { return( crescent( ) ); }
};

} // namespace geometry
} // namespace axn

#endif /* Crescent_hpp */