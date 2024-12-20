#ifndef Polygon_hpp
#define Polygon_hpp

#include "Planc.hpp"
#include "Coordinate.hpp"
#include "Angle.hpp"
#include "Vector.hpp"
#include "Transform.hpp"
#include "Line.hpp"
#include "Path.hpp"

namespace axn
{
namespace geometry
{

class Polygon : public Transformable
{
private:
    varray<Coordinate> m_coordinates_raw;
    mutable varray<Coordinate> m_coordinates;

    // stores in 3s
    mutable varray<varray<uint>> m_triangle_indices;

    mutable Planc m_lower_bound_x = P0;
    mutable Planc m_lower_bound_y = P0;
    mutable Planc m_upper_bound_x = P0;
    mutable Planc m_upper_bound_y = P0;
    mutable bool m_bounds_dirty = false;

    bool m_convex = true;

public:
    Polygon( ) { }

    Polygon( const varray<Coordinate> & coordinates )
    {
        m_coordinates = m_coordinates_raw = coordinates;
        uint coordinate_count = m_coordinates.size( );

        m_convex = true;

        if( coordinate_count )
        {
            m_lower_bound_x = INFINITY_POS;
            m_lower_bound_y = INFINITY_POS;
            m_upper_bound_x = INFINITY_NEG;
            m_upper_bound_y = INFINITY_NEG;

            if( coordinate_count <= 3 )
            {
                for_range( i, coordinate_count )
                {
                    m_lower_bound_x = min( m_lower_bound_x, m_coordinates[ i ].x( ) );
                    m_lower_bound_y = min( m_lower_bound_y, m_coordinates[ i ].y( ) );
                    m_upper_bound_x = max( m_upper_bound_x, m_coordinates[ i ].x( ) );
                    m_upper_bound_y = max( m_upper_bound_y, m_coordinates[ i ].y( ) );
                }

                if( coordinate_count == 3 )
                {
                    if( Line( m_coordinates[ 0 ], m_coordinates[ 1 ] ).above( m_coordinates[ 2 ] ) )
                    {
                        // is clockwise
                        m_coordinates.reverse( );
                    }
                }
            }
            else
            {
                // iterate through coordinates and determine xy boundaries, convex, and clockwise see
                // https://stackoverflow.com/questions/1165647/how-to-determine-if-a-list-of-polygon-points-are-in-clockwise-order

                // default to true then check for violations
                bool convex_cw = true;
                bool convex_ccw = true;

                dec edge_curve = ZERO;

                for_range( i, coordinate_count )
                {
                    const Coordinate & coordinate = m_coordinates[ i ];

                    // TODO
                    // Assert( !is_inf( coordinate.x( ) ) && !is_inf( coordinate.y( ) ), "no infinities allowed!" );

                    m_lower_bound_x = min( m_lower_bound_x, coordinate.x( ) );
                    m_lower_bound_y = min( m_lower_bound_y, coordinate.y( ) );
                    m_upper_bound_x = max( m_upper_bound_x, coordinate.x( ) );
                    m_upper_bound_y = max( m_upper_bound_y, coordinate.y( ) );

                    const Coordinate & prev_coordinate = m_coordinates[ ( i + coordinate_count - 1 ) % coordinate_count ];
                    edge_curve += (dec)( ( coordinate.x( ) - prev_coordinate.x( ) ) * ( coordinate.y( ) + prev_coordinate.y( ) ) );

                    if( convex_cw || convex_ccw )
                    {
                        const Coordinate & prev_prev_coordinate = m_coordinates[ ( i + coordinate_count - 2 ) % coordinate_count ];
                        Line line = Line( prev_prev_coordinate, prev_coordinate );

                        if( convex_cw && line.above( coordinate ) )
                        {
                            convex_cw = false;
                        }

                        if( convex_ccw && line.below( coordinate ) )
                        {
                            convex_ccw = false;
                        }
                    }
                }

                bool clockwise = ( edge_curve > 0 );

                if( clockwise )
                {
                    m_coordinates_raw.reverse( );
                    m_coordinates.reverse( );
                }

                m_convex = clockwise ? convex_cw : convex_ccw;
            }
        }
    }

    static Polygon triangle( const Coordinate & c1, const Coordinate & c2, const Coordinate & c3 ) { return Polygon( { c1, c2, c3 } ); }

    static Polygon rectangle( const Planc & width, const Planc & height, const Angle & rotation ) { return rectangle( width, height ).rotate( rotation ); }
    static Polygon rectangle( const Planc & width, const Planc & height, const Coordinate & center ) { return rectangle( width, height ).move( center ); }
    static Polygon rectangle( const Planc & width, const Planc & height, const Coordinate & center, const Angle & rotation ) { return rectangle( width, height ).rotate( rotation ).move( center ); }
    static Polygon rectangle( const Planc & width, const Planc & height )
    {
        Planc half_width = half( width );
        Planc half_height = half( height );

        return Polygon( { Coordinate( half_width, half_height ),
                          Coordinate( -half_width, half_height ),
                          Coordinate( -half_width, -half_height ),
                          Coordinate( half_width, -half_height ) } );
    }

    static Polygon equilateral( uint side_count, const Planc & radius, const Angle & rotation ) { return equilateral( side_count, radius ).rotate( rotation ); }
    static Polygon equilateral( uint side_count, const Planc & radius, const Coordinate & center ) { return equilateral( side_count, radius ).move( center ); }
    static Polygon equilateral( uint side_count, const Planc & radius, const Coordinate & center, const Angle & rotation ) { return equilateral( side_count, radius ).rotate( rotation ).move( center ); }
    static Polygon equilateral( uint side_count, const Planc & radius )
    {
        static umap<uint, Polygon> equilaterals;

        if( !equilaterals.contains( side_count ) )
        {
            varray<Coordinate> coordinates( side_count );

            Angle start_angle = Angle( RIGHT_ANGLE, true ); // start at the top + rotation
            Angle delta_angle( TAU / (dec)side_count );
            for_each( offset, coordinates )
            {
                offset = Vector::A( start_angle );
                start_angle += delta_angle;
            }

            equilaterals[ side_count ] = Polygon( coordinates );
        }

        return Polygon( equilaterals[ side_count ] ).scale( radius );
    }

    static Polygon circle( const Planc & radius = 1.0, const Coordinate & center = ORIGIN ) { return Polygon::equilateral( 60, radius, center ); }

    static Polygon expand( const Polygon & polygon, const Planc & expansion )
    {
        return_if( polygon, !expansion );

        const varray<Coordinate> _coordinates = polygon.coordinates( );
        const uint _coordinate_count = _coordinates.size( );

        varray<Coordinate> new_coordinates( _coordinate_count );

        for_range( i, _coordinate_count )
        {
            uint coordinate_prev_index = i ? ( i - 1 ) : ( _coordinate_count - 1 );
            uint coordinate_next_index = ( i + 1 ) % _coordinate_count;

            Line line_prev = Line( _coordinates[ coordinate_prev_index ], _coordinates[ i ] );
            Line line_next = Line( _coordinates[ i ], _coordinates[ coordinate_next_index ] );

            Vector vector_prev( line_prev.c1( ), line_prev.c2( ) );
            Vector vector_next( line_next.c1( ), line_next.c2( ) );

            vector_prev.rotate( -RIGHT_ANGLE );
            vector_next.rotate( -RIGHT_ANGLE );

            vector_prev.magnitude( expansion );
            vector_next.magnitude( expansion );

            line_prev += vector_prev;
            line_next += vector_next;

            new_coordinates[ i ] = line_prev.intersection( line_next );
        }

        return Polygon( new_coordinates );
    }

    virtual const Polygon & dirty( ) const override { Transformable::dirty( ); m_bounds_dirty = true; rethis; }
    virtual const Polygon & clean( ) const override { Transformable::clean( ); m_bounds_dirty = false; rethis; }

    transform_functions( Polygon );

    const varray<Coordinate> & coordinates( bool raw = false ) const { if( raw ) { return m_coordinates_raw; } else { apply_transform( ); return m_coordinates; } }

    const varray<varray<uint>> & triangle_indices( ) const
    {
        if( !m_triangle_indices.size( ) && ( m_coordinates_raw.size( ) >= 3 ) )
        {
            if( convex( ) )
            {
                for_range( i, m_coordinates_raw.size( ) - TWO )
                {
                    m_triangle_indices.insert_back( { 0, i + 1, i + 2 } );
                }
            }
            else
            {
                varray<Coordinate> temp_coordinates = m_coordinates_raw;

                varray<uint> temp_coordinate_indices( temp_coordinates.size( ) );
                for_range( i, temp_coordinate_indices.size( ) )
                {
                    temp_coordinate_indices[ i ] = i;
                }

                uint i = ZERO;
                while( temp_coordinates.size( ) > THREE )
                {
                    i %= temp_coordinates.size( );
                    uint i_prev = ( i + ( temp_coordinates.size( ) - 1 ) ) % temp_coordinates.size( );
                    uint i_next = ( i + 1 ) % temp_coordinates.size( );

                    const Coordinate & c = temp_coordinates[ i ];
                    const Coordinate & c_prev = temp_coordinates[ i_prev ];
                    const Coordinate & c_next = temp_coordinates[ i_next ];

                    bool is_ear = false;

                    if( !is_neg( ::cross( c_prev, c, c_next ) ) )
                    {
                        bool containing = false;

                        for_range( j, temp_coordinates.size( ) )
                        {
                            if( ( j != i ) && ( j != i_prev ) && ( j != i_next ) )
                            {
                                const Coordinate & c_temp = temp_coordinates[ j ];

                                if( ::contains( c, c_next, c_prev, c_temp ) )
                                {
                                    containing = true;
                                    break;
                                }
                            }
                        }

                        is_ear = !containing;
                    }

                    if( is_ear )
                    {
                        m_triangle_indices.insert_back( { temp_coordinate_indices[ i ], temp_coordinate_indices[ i_next ], temp_coordinate_indices[ i_prev ] } );

                        temp_coordinates.remove_index( i );
                        temp_coordinate_indices.remove_index( i );
                    }
                    else
                    {
                        ++i;
                    }
                }

                m_triangle_indices.insert_back( temp_coordinate_indices );
            }
        }

        return m_triangle_indices;
    }

    bool convex( ) const { return m_convex; }

    uint sides( ) const { return m_coordinates.size( ); }

    Planc area( ) const
    {
        const varray<Coordinate> & cc = m_coordinates_raw;

        if( cc.size( ) >= 3 )
        {
            Planc area = ( ( cc.back( ).x( ) * cc.front( ).y( ) ) - ( cc.back( ).y( ) * cc.front( ).x( ) ) );

            for_range( i, cc.size( ) - 1 )
            {
                const Coordinate & c1 = cc[ i ];
                const Coordinate & c2 = cc[ i + 1 ];

                area += ( ( c1.x( ) * c2.y( ) ) - ( c1.y( ) * c2.x( ) ) );
            }

            return abs( half( area ) ) * cumulative_transform( ).determinant( );
        }
        else
        {
            return 0.0;
        }
    }

    Planc lower_bound_x( bool tight = false ) const { apply_bounds_transform( tight ); return m_lower_bound_x; }
    Planc lower_bound_y( bool tight = false ) const { apply_bounds_transform( tight ); return m_lower_bound_y; }
    Planc upper_bound_x( bool tight = false ) const { apply_bounds_transform( tight ); return m_upper_bound_x; }
    Planc upper_bound_y( bool tight = false ) const { apply_bounds_transform( tight ); return m_upper_bound_y; }

    Planc bound_width( bool tight = false ) const { return upper_bound_x( tight ) - lower_bound_x( tight ); }
    Planc bound_height( bool tight = false ) const { return upper_bound_y( tight ) - lower_bound_y( tight ); }

    // todo do ray test instead
    bool contains( const Coordinate & coordinate, const bool inclusive = true ) const
    {
        Coordinate c = cumulative_transform( ).inverse( ).apply( coordinate );

        if( convex( ) )
        {
            for_each( line, Path( m_coordinates_raw, true ).lines( ) )
            {
                if( !line.above( c, inclusive ) )
                {
                    return false;
                }
            }

            return true;
        }
        else
        {
            const varray<Coordinate> & cc = m_coordinates_raw;

            for_each( t, triangle_indices( ) )
            {
                if( ::contains( cc[ t[ 0 ] ], cc[ t[ 1 ] ], cc[ t[ 2 ] ], c, inclusive ) )
                {
                    return true;
                }
                else if( !inclusive )
                {
                    // check if points lies on lines that is not part of the perimeter

                    if( abs( (int)( t[ 0 ] ) - (int)( t[ 1 ] ) ) != 1 )
                    {
                        return is_zero( ::cross( cc[ t[ 0 ] ], cc[ t[ 1 ] ], c ) );
                    }

                    if( abs( (int)( t[ 1 ] ) - (int)( t[ 2 ] ) ) != 1 )
                    {
                        return is_zero( ::cross( cc[ t[ 1 ] ], cc[ t[ 2 ] ], c ) );
                    }

                    if( abs( (int)( t[ 2 ] ) - (int)( t[ 0 ] ) ) != 1 )
                    {
                        return is_zero( ::cross( cc[ t[ 2 ] ], cc[ t[ 0 ] ], c ) );
                    }
                }
            }

            return false;
        }
    }

    bool intersects( const Line & line ) const
    {
        if( contains( line.c1( ) ) || contains( line.c2( ) ) )
        {
            return true;
        }

        Line line_transformed = line;
        line_transformed.transform( cumulative_transform( ).inverse( ) );

        for_each( line, Path( m_coordinates_raw, true ).lines( ) )
        {
            if( line.intersects( line_transformed ) )
            {
                return true;
            }
        }

        return false;
    }

    varray<Line> intersection( const Line & line ) const
    {
        varray<Coordinate> intersection_coordinates = { };

        if( contains( line.c1( ) ) )
        {
            intersection_coordinates.insert_back( line.c1( ) );
        }

        if( contains( line.c2( ) ) )
        {
            intersection_coordinates.insert_back( line.c2( ) );
        }

        Line line_transformed = line;
        line_transformed.transform( cumulative_transform( ).inverse( ) );

        for_each( line, Path( m_coordinates_raw, true ).lines( ) )
        {
            if( line.intersects( line_transformed ) )
            {
                intersection_coordinates.insert_back( apply_cumulative_transform( Coordinate( line.intersection( line_transformed ) ) ) );
            }
        }

        Assert( is_even( intersection_coordinates.size( ) ) );

        // sort intersections by distance from start of line
        struct { Coordinate origin; bool operator( )( const Coordinate & c1, const Coordinate & c2 ) { return ( c1.distance_to( origin ) < c2.distance_to( origin ) ); } }
        line_intersection_distance;

        line_intersection_distance.origin = line.c1( );

        intersection_coordinates.sort( line_intersection_distance );

        varray<Line> intersections( intersection_coordinates.size( ) / 2 );

        for_range( i, intersections.size( ) )
        {
            intersections[ i ] = Line( intersection_coordinates[ ( i * 2 ) ], intersection_coordinates[ ( i * 2 ) + 1 ] );
        }

        return intersections;
    }

    Polygon operator+( const Vector & v ) const { return Polygon( *this ).move( v ); }
    Polygon operator-( const Vector & v ) const { return Polygon( *this ).move( -v ); }

    Polygon & operator+=( const Vector & v ) { return move( v ); }
    Polygon & operator-=( const Vector & v ) { return move( -v ); }

    default_equal( Polygon );

private:
    void apply_transform( ) const
    {
        if( is_dirty( ) )
        {
            m_lower_bound_x = INFINITY_POS;
            m_lower_bound_y = INFINITY_POS;
            m_upper_bound_x = INFINITY_NEG;
            m_upper_bound_y = INFINITY_NEG;

            const Transform & t = cumulative_transform( );

            for_range( i, m_coordinates.size( ) )
            {
                m_coordinates[ i ] = t.apply( m_coordinates_raw[ i ] );

                m_lower_bound_x = min( m_lower_bound_x, m_coordinates[ i ].x( ) );
                m_lower_bound_y = min( m_lower_bound_y, m_coordinates[ i ].y( ) );
                m_upper_bound_x = max( m_upper_bound_x, m_coordinates[ i ].x( ) );
                m_upper_bound_y = max( m_upper_bound_y, m_coordinates[ i ].y( ) );
            }

            clean( );
        }
    }

    void apply_bounds_transform( bool tight ) const
    {
        if( tight )
        {
            apply_transform( );
        }
        else if( m_bounds_dirty )
        {
            const Transform & t = transform( );

            Coordinate c1 = t.apply( Coordinate( m_lower_bound_x, m_lower_bound_y ) );
            Coordinate c2 = t.apply( Coordinate( m_upper_bound_x, m_lower_bound_y ) );
            Coordinate c3 = t.apply( Coordinate( m_lower_bound_x, m_upper_bound_y ) );
            Coordinate c4 = t.apply( Coordinate( m_upper_bound_x, m_upper_bound_y ) );

            m_lower_bound_x = min<Planc>( { c1.x( ), c2.x( ), c3.x( ), c4.x( ) } );
            m_lower_bound_y = min<Planc>( { c1.y( ), c2.y( ), c3.y( ), c4.y( ) } );
            m_upper_bound_x = max<Planc>( { c1.x( ), c2.x( ), c3.x( ), c4.x( ) } );
            m_upper_bound_y = max<Planc>( { c1.y( ), c2.y( ), c3.y( ), c4.y( ) } );

            m_bounds_dirty = false;
        }
    }
};

} // namespace geometry
} // namespace axn

#endif /* Polygon_hpp */
