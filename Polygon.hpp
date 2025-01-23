#ifndef Polygon_hpp
#define Polygon_hpp

#include "Planc.hpp"
#include "Coordinate.hpp"
#include "Angle.hpp"
#include "Vector.hpp"
#include "Transform.hpp"
#include "Line.hpp"
#include "Path.hpp"
#include "Triangle.hpp"

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

    Polygon( varray<Coordinate> cref coordinates )
    {
        m_coordinates = coordinates;
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

                dec edge_curve = 0.0;
                
                for_range( i, coordinate_count )
                {
                    Coordinate cref coordinate = m_coordinates[ i ];
                    
                    if( i && ( coordinate == m_coordinates[ i - 1 ] ) )
                    {
                        m_coordinates.remove_index( i-- );
                        --coordinate_count;
                    }
                    else
                    {
                        m_lower_bound_x = min( m_lower_bound_x, coordinate.x( ) );
                        m_lower_bound_y = min( m_lower_bound_y, coordinate.y( ) );
                        m_upper_bound_x = max( m_upper_bound_x, coordinate.x( ) );
                        m_upper_bound_y = max( m_upper_bound_y, coordinate.y( ) );
                        
                        Coordinate cref prev_coordinate = m_coordinates[ ( i + coordinate_count - 1 ) % coordinate_count ];
                        edge_curve += (dec)( ( coordinate.x( ) - prev_coordinate.x( ) ) * ( coordinate.y( ) + prev_coordinate.y( ) ) );
                        
                        if( convex_cw || convex_ccw )
                        {
                            Coordinate cref prev_prev_coordinate = m_coordinates[ ( i + coordinate_count - 2 ) % coordinate_count ];
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
        
        m_coordinates_raw = m_coordinates;
    }

    static Polygon triangle( Coordinate cref c1, Coordinate cref c2, Coordinate cref c3 ) { return Polygon( { c1, c2, c3 } ); }

    static Polygon rectangle( Planc cref width, Planc cref height, Angle cref rotation ) { return rectangle( width, height ).rotate( rotation ); }
    static Polygon rectangle( Planc cref width, Planc cref height, Coordinate cref center ) { return rectangle( width, height ).move( center ); }
    static Polygon rectangle( Planc cref width, Planc cref height, Coordinate cref center, Angle cref rotation ) { return rectangle( width, height ).rotate( rotation ).move( center ); }
    static Polygon rectangle( Planc cref width, Planc cref height )
    {
        Planc half_width = half( width );
        Planc half_height = half( height );

        return Polygon( { Coordinate( half_width, half_height ),
                          Coordinate( -half_width, half_height ),
                          Coordinate( -half_width, -half_height ),
                          Coordinate( half_width, -half_height ) } );
    }
    
    static Polygon square( Planc cref width ) { return rectangle( width, width ); }
    static Polygon square( Planc cref width, Angle cref rotation ) { return rectangle( width, width, rotation ); }
    static Polygon square( Planc cref width, Coordinate cref center ) { return rectangle( width, width, center ); }
    static Polygon square( Planc cref width, Coordinate cref center, Angle cref rotation ) { return rectangle( width, width, center, rotation ); }

    static Polygon equilateral( uint side_count, Planc cref radius, Angle cref rotation ) { return equilateral( side_count, radius ).rotate( rotation ); }
    static Polygon equilateral( uint side_count, Planc cref radius, Coordinate cref center ) { return equilateral( side_count, radius ).move( center ); }
    static Polygon equilateral( uint side_count, Planc cref radius, Coordinate cref center, Angle cref rotation ) { return equilateral( side_count, radius ).rotate( rotation ).move( center ); }
    static Polygon equilateral( uint side_count, Planc cref radius )
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

    static Polygon circle( Planc cref radius = 1.0, Coordinate cref center = ORIGIN ) { return Polygon::equilateral( ceil( radius ) + 3, radius, center ); }

    static Polygon convex_hull( varray<Coordinate> cref coordinates )
    {
        cuint coordinate_count = coordinates.size( );

        if( coordinate_count )
        {
            if( coordinate_count <= 3 )
            {
                return Polygon( coordinates );
            }
            else
            {
                // using Graham Scan

                // find coordinate with minimum y value (if tied, also minimum x value)
                uint min_i = 0;

                for_range( i, coordinates.size( ) - 1 )
                {
                    if( ( coordinates[ i + 1 ].y( ) < coordinates[ min_i ].y( ) ) || ( ( coordinates[ i + 1 ].y( ) == coordinates[ min_i ].y( ) ) && ( coordinates[ i + 1 ].x( ) < coordinates[ min_i ].x( ) ) ) )
                    {
                        min_i = i + 1;
                    }
                }

                // sort coordinates by angle to min
                struct AngleToMin { AngleToMin( Coordinate cref c ) : min( c ) { } Coordinate min; bool operator( )( Coordinate cref c1, Coordinate cref c2 ) { Planc c = cross( min, c1, c2 ); return ( ( c > 0.0 ) || ( !c && min.closer_than( c1, c2 ) ) ); } };

                varray<Coordinate> sorted_coordinates = coordinates;
                sorted_coordinates.remove_index( min_i );
                sorted_coordinates.sort( AngleToMin( coordinates[ min_i ] ) );

                varray<Coordinate> convex_hull = { coordinates[ min_i ], sorted_coordinates[ 0 ], sorted_coordinates[ 1 ] };

                for_range( i, sorted_coordinates.size( ) - 2 )
                {
                    // remove hull coordinates until current coordinate is part of hull
                    while( ( convex_hull.size( ) > 1 ) && ( cross( convex_hull[ convex_hull.size( ) - 2 ], convex_hull[ convex_hull.size( ) - 1 ], sorted_coordinates[ i + 2 ] ) <= 0.0 ) )
                    {
                        convex_hull.remove_back( );
                    }

                    // add to hull
                    convex_hull.insert_back( sorted_coordinates[ i + 2 ] );
                }

                return Polygon( convex_hull );
            }
        }

        return Polygon( );
    }

    static Polygon expand( Polygon cref polygon, Planc cref expansion )
    {
        return_if( !expansion, polygon );

        const varray<Coordinate> _coordinates = polygon.coordinates( );
        cuint _coordinate_count = _coordinates.size( );

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

    virtual Polygon cref dirty( ) const override { Transformable::dirty( ); m_bounds_dirty = true; rethis; }
    virtual Polygon cref clean( ) const override { Transformable::clean( ); m_bounds_dirty = false; rethis; }

    transform_functions( Polygon );

    varray<Coordinate> cref coordinates( bool raw = false ) const { if( raw ) { return m_coordinates_raw; } else { apply_transform( ); return m_coordinates; } }

    const varray<varray<uint>> & triangle_indices( ) const
    {
        if( !m_triangle_indices.size( ) && ( m_coordinates_raw.size( ) >= 3 ) )
        {
            if( convex( ) )
            {
                for_range( i, m_coordinates_raw.size( ) - 2 )
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

                uint i = 0;
                while( temp_coordinates.size( ) > 3 )
                {
                    i %= temp_coordinates.size( );
                    uint i_prev = ( i + ( temp_coordinates.size( ) - 1 ) ) % temp_coordinates.size( );
                    uint i_next = ( i + 1 ) % temp_coordinates.size( );

                    Coordinate cref c = temp_coordinates[ i ];
                    Coordinate cref c_prev = temp_coordinates[ i_prev ];
                    Coordinate cref c_next = temp_coordinates[ i_next ];

                    bool is_ear = false;

                    if( !is_neg( ::cross( c_prev, c, c_next ) ) )
                    {
                        bool containing = false;

                        for_range( j, temp_coordinates.size( ) )
                        {
                            if( ( j != i ) && ( j != i_prev ) && ( j != i_next ) )
                            {
                                Coordinate cref c_temp = temp_coordinates[ j ];

                                if( Triangle( c, c_next, c_prev ).contains( c_temp ) )
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
        varray<Coordinate> cref cs = m_coordinates_raw;

        if( cs.size( ) >= 3 )
        {
            Planc area = ( ( cs.back( ).x( ) * cs.front( ).y( ) ) - ( cs.back( ).y( ) * cs.front( ).x( ) ) );

            for_range( i, cs.size( ) - 1 )
            {
                Coordinate cref c1 = cs[ i ];
                Coordinate cref c2 = cs[ i + 1 ];

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

    Coordinate centroid( ) const
    {
        Coordinate centroid;
        Planc area = 0.0;
        
        varray<Coordinate> cref cs = m_coordinates_raw;
        
        for_each( ti, triangle_indices( ) )
        {
            Triangle t( cs[ ti[ 0 ] ], cs[ ti[ 1 ] ], cs[ ti[ 2 ] ] );
            
            centroid += t.centroid( ) * t.area( );
            area += t.area( );
        }
        
        if( area )
        {
            return cumulative_transform( ).apply( centroid / area );
        }
        else
        {
            return INVALID_COORDINATE;
        }
    }

    bool contains( Coordinate cref coordinate, cbool inclusive = true ) const
    {
        Coordinate c = apply_cumulative_transform_inverse( coordinate );

        uint intersection_count = 0;

        for_each( line, Path( m_coordinates_raw, true ).lines( ) )
        {
            if( Line( c, Coordinate( c.x( ), line.lower_bound_y( ) - 1.0 ) ).intersects( line ) )
            {
                if( intersection_count && convex( ) )
                {
                    return false;
                }

                ++intersection_count;
            }
        }

        return is_odd( intersection_count ) && !is_zero( intersection_count );
    }

    bool intersects( Line cref line ) const
    {
        Line line_transformed = line;
        line_transformed.transform( cumulative_transform_inverse( ) );

        Path perimeter = Path( m_coordinates_raw, true );
        for_each( line, perimeter.lines( ) )
        {
            if( line.intersects( line_transformed ) )
            {
                return true;
            }
        }
        
        return ( contains( line.c1( ) ) || contains( line.c2( ) ) );
    }

    varray<Line> intersection( Line cref line ) const
    {
        varray<Coordinate> intersection_coordinates = { };

        bool c1 = contains( line.c1( ) );
        bool c2 = contains( line.c2( ) );
        
        if( c1 && c2 )
        {
            if( convex( ) )
            {
                return { line };
            }
        }
        
        if( c1 )
        {
            intersection_coordinates.insert_back( line.c1( ) );
        }

        if( c2 )
        {
            intersection_coordinates.insert_back( line.c2( ) );
        }

        Line line_transformed = line;
        line_transformed.transform( cumulative_transform_inverse( ) );

        Path perimeter = Path( m_coordinates_raw, true );
        for_each( line, perimeter.lines( ) )
        {
            if( line.intersects( line_transformed ) )
            {
                intersection_coordinates.insert_back( apply_cumulative_transform( line.intersection( line_transformed ) ) );
                
                if( convex( ) )
                {
                    if( intersection_coordinates.size( ) )
                    {
                        break;
                    }
                }
            }
        }

        if( is_even( intersection_coordinates.size( ) ) )
        {
            // sort intersections by distance from start of line
            struct LineIntersectionDistance { LineIntersectionDistance( Coordinate cref c ) : origin( c ) { } Coordinate origin; bool operator( )( Coordinate cref c1, Coordinate cref c2 ) { return origin.closer_than( c1, c2 ); } };
            
            intersection_coordinates.sort( LineIntersectionDistance( line.c1( ) ) );
            
            varray<Line> intersections( intersection_coordinates.size( ) / 2 );
            
            for_range( i, intersections.size( ) )
            {
                intersections[ i ] = Line( intersection_coordinates[ ( i * 2 ) ], intersection_coordinates[ ( i * 2 ) + 1 ] );
            }
            
            return intersections;
        }
        else
        {
            return { Line( intersection_coordinates.front( ), intersection_coordinates.front( ) ) };
        }
        
    }

    Polygon operator+( Vector cref v ) const { return Polygon( *this ).move( v ); }
    Polygon operator-( Vector cref v ) const { return Polygon( *this ).move( -v ); }

    Polygon & operator+=( Vector cref v ) { return move( v ); }
    Polygon & operator-=( Vector cref v ) { return move( -v ); }

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

            Transform cref t = cumulative_transform( );

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
            Transform cref t = transform( );

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
