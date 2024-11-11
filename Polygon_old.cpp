//#include "Polygon.hpp"
//
//const uint Polygon::CIRCLE_PRECISION = 60;
//
//Polygon::Polygon( ) { }
//
//Polygon::Polygon( const varray<Coordinate> & _coordinates, const bool _assume_is_convex )
//{
//    m_coordinates_raw = m_coordinates = _coordinates;
//}
//
//Polygon::Polygon( const varray<Coordinate> & _coordinates, const Transform & _transform, const bool _assume_is_convex )
//{
//    m_coordinates_raw = m_coordinates = _coordinates;
//    transform( _transform );
//}
//
//Triangle::Triangle( const Coordinate & c1, const Coordinate & c2, const Coordinate & c3 ) : Polygon( { c1, c2, c3 } ) { }
//
//Rectangle::Rectangle( const Planc & _width, const Planc & _height, const Coordinate & _center, const Angle _rotation )
//    : Polygon( { _center + Vector( _width, _height ).half( ).rotate( _rotation ),
//                 _center + Vector( -_width, _height ).half( ).rotate( _rotation ),
//                 _center + Vector( -_width, -_height ).half( ).rotate( _rotation ),
//                 _center + Vector( _width, -_height ).half( ).rotate( _rotation ) },
//               true ) { }
//
//Square::Square( const Planc & _width, const Coordinate & _center, const Angle _rotation )
//    : Polygon( { _center + Vector( _width, _width ).half( ).rotate( _rotation ),
//                 _center + Vector( -_width, _width ).half( ).rotate( _rotation ),
//                 _center + Vector( -_width, -_width ).half( ).rotate( _rotation ),
//                 _center + Vector( _width, -_width ).half( ).rotate( _rotation ) },
//               true ) { }
//
//Circle::Circle( const Planc & _radius, const Coordinate & _center ) : Polygon( Polygon::equilateral( Polygon::CIRCLE_PRECISION ).scale( _radius ).move( _center ) ) { }
//
//Polygon Polygon::equilateral( const uint _side_count, const Planc & _radius, const Coordinate & _center, const Angle _rotation )
//{
//    static umap<uint, Polygon> equilaterals;
//
//    if( !equilaterals.contains( _side_count ) )
//    {
//        varray<Coordinate> coordinates( _side_count );
//
//        Angle start_angle = Angle( RIGHT_ANGLE, true ); // start at the top + rotation
//        Angle delta_angle( TAU / (dec)_side_count );
//        for_each( offset, coordinates )
//        {
//            offset = VectorA( start_angle );
//            start_angle += delta_angle;
//        }
//
//        equilaterals[ _side_count ] = Polygon( coordinates, false );
//    }
//
//    return Polygon( equilaterals[ _side_count ] ).scale( _radius ).rotate( _rotation ).move( _center );
//}
//
//Polygon Polygon::expand( const Polygon & _polygon, const Planc & _expansion )
//{
//    if( !_expansion )
//    {
//        return Polygon( _polygon );
//    }
//
//    const varray<Coordinate> _coordinates = _polygon.coordinates( );
//    const uint _coordinate_count = _coordinates.size( );
//
//    varray<Coordinate> new_coordinates( _coordinate_count );
//
//    for_range( i, _coordinate_count )
//    {
//        uint coordinate_prev_index = i ? ( i - 1 ) : ( _coordinate_count - 1 );
//        uint coordinate_next_index = ( i + 1 ) % _coordinate_count;
//
//        Line line_prev = Line( _coordinates[ coordinate_prev_index ], _coordinates[ i ] );
//        Line line_next = Line( _coordinates[ i ], _coordinates[ coordinate_next_index ] );
//
//        Vector vector_prev( line_prev.c1( ), line_prev.c2( ) );
//        Vector vector_next( line_next.c1( ), line_next.c2( ) );
//
//        vector_prev.rotate( -RIGHT_ANGLE );
//        vector_next.rotate( -RIGHT_ANGLE );
//
//        vector_prev.magnitude( _expansion );
//        vector_next.magnitude( _expansion );
//
//        line_prev += vector_prev;
//        line_next += vector_next;
//
//        new_coordinates[ i ] = line_prev.intersection( line_next );
//    }
//
//    return Polygon( new_coordinates );
//}
//
//Transform Polygon::transform( const bool _cumulative ) const
//{
//    return _cumulative ? m_cumulative_transform : m_transform;
//}
//
//void Polygon::dirty( ) const
//{
//    m_coordinates_dirty = m_lines_dirty = m_triangles_dirty = m_convex_partitions_dirty = m_convex_hull_dirty = true;
//}
//
//void Polygon::process( bool transform, bool lines, bool triangles, bool convex_partitions, bool convex_hull ) const
//{
//    uint coordinate_count = m_coordinates.size( );
//
//    if( m_transform.identity( ) )
//    {
//        transform = false;
//    }
//
//    if( convex_partitions )
//    {
//        triangles = true;
//    }
//
//    if( !m_lines_dirty )
//    {
//        lines = false;
//    }
//
//    if( !m_triangles_dirty )
//    {
//        triangles = false;
//    }
//
//    if( !m_convex_partitions_dirty )
//    {
//        convex_partitions = false;
//    }
//
//    if( !m_convex_hull_dirty )
//    {
//        convex_hull = false;
//    }
//
//    if( !m_coordinates_dirty && !transform && !lines && !triangles && !convex_partitions && !convex_hull )
//    {
//        return;
//    }
//
//    if( coordinate_count == 1 )
//    {
//        if( transform )
//        {
//            m_coordinates[ 0 ] = m_transform.transform( m_coordinates[ 0 ] );
//            m_transform = IDENTITY_TRANSFORM;
//        }
//        
//        if( lines )
//        {
//            m_lines = { };
//            m_lines_dirty = false;
//        }
//        
//        if( triangles )
//        {
//            m_triangles_indices = { };
//            m_triangles_dirty = false;
//        }
//        
//        if( convex_partitions )
//        {
//            m_convex_partitions_indices = { };
//            m_convex_partitions_dirty = false;
//        }
//        
//        if( convex_hull )
//        {
//            m_convex_hull_indices = { 0 };
//            m_convex_hull_dirty = false;
//        }
//
//        m_convex = true;
//
//        m_lower_bound_x = m_coordinates[ 0 ].x( );
//        m_lower_bound_y = m_coordinates[ 0 ].y( );
//        m_upper_bound_x = m_coordinates[ 0 ].x( );
//        m_upper_bound_y = m_coordinates[ 0 ].y( );
//        
//        m_coordinates_dirty = false;
//    }
//    else if( coordinate_count == 2 )
//    {
//        if( transform )
//        {
//            m_coordinates[ 0 ] = m_transform.transform( m_coordinates[ 0 ] );
//            m_coordinates[ 1 ] = m_transform.transform( m_coordinates[ 1 ] );
//            m_transform = IDENTITY_TRANSFORM;
//        }
//        
//        if( lines )
//        {
//            m_lines = { Line( m_coordinates[ 0 ], m_coordinates[ 1 ] ) };
//            m_lines_dirty = false;
//        }
//        
//        if( triangles )
//        {
//            m_triangles_indices = { };
//            m_triangles_dirty = false;
//        }
//        
//        if( convex_partitions )
//        {
//            m_convex_partitions_indices = { };
//            m_convex_partitions_dirty = false;
//        }
//        
//        if( convex_hull )
//        {
//            m_convex_hull_indices = { 0, 1 };
//            m_convex_hull_dirty = false;
//        }
//
//        m_convex = true;
//
//        m_lower_bound_x = min( m_coordinates[ 0 ].x( ), m_coordinates[ 1 ].x( ) );
//        m_lower_bound_y = min( m_coordinates[ 0 ].y( ), m_coordinates[ 1 ].y( ) );
//        m_upper_bound_x = max( m_coordinates[ 0 ].x( ), m_coordinates[ 1 ].x( ) );
//        m_upper_bound_y = max( m_coordinates[ 0 ].y( ), m_coordinates[ 1 ].y( ) );
//        
//        m_coordinates_dirty = false;
//    }
//    else if( coordinate_count == 3 )
//    {
//        if( transform )
//        {
//            m_coordinates[ 0 ] = m_transform.transform( m_coordinates[ 0 ] );
//            m_coordinates[ 1 ] = m_transform.transform( m_coordinates[ 1 ] );
//            m_coordinates[ 2 ] = m_transform.transform( m_coordinates[ 2 ] );
//            m_transform = IDENTITY_TRANSFORM;
//        }
//
//        if( lines )
//        {
//             m_lines.resize( 3 );
//             m_lines[ 0 ] = Line( m_coordinates[ 0 ], m_coordinates[ 1 ] );
//             m_lines[ 1 ] = Line( m_coordinates[ 1 ], m_coordinates[ 2 ] );
//             m_lines[ 2 ] = Line( m_coordinates[ 2 ], m_coordinates[ 0 ] );
//            m_lines_dirty = false;
//        }
//
//        if( triangles )
//        {
//            m_triangles_indices = { { 0, 1, 2 } };
//            m_triangles_dirty = false;
//        }
//
//        if( convex_partitions )
//        {
//            m_convex_partitions_indices = { { 0, 1, 2 } };
//            m_convex_partitions_dirty = false;
//        }
//        
//        m_convex = true;
//        if( Line( m_coordinates[ 0 ], m_coordinates[ 1 ] ).above( m_coordinates[ 2 ] ) )
//        {
//            // is clockwise
//            m_coordinates.reverse( );
//        }
//
//        m_lower_bound_x = min( m_coordinates[ 0 ].x( ), min( m_coordinates[ 1 ].x( ), m_coordinates[ 2 ].x( ) ) );
//        m_lower_bound_y = min( m_coordinates[ 0 ].y( ), min( m_coordinates[ 1 ].y( ), m_coordinates[ 2 ].y( ) ) );
//        m_upper_bound_x = max( m_coordinates[ 0 ].x( ), max( m_coordinates[ 1 ].x( ), m_coordinates[ 2 ].x( ) ) );
//        m_upper_bound_y = max( m_coordinates[ 0 ].y( ), max( m_coordinates[ 1 ].y( ), m_coordinates[ 2 ].y( ) ) );
//
//        m_coordinates_dirty = false;
//    }
//    else if( coordinate_count )
//    {
//        // default to true then check for violations
//        bool convex_ccw = true;
//        bool convex_cw = true;
//
//        if( lines )
//        {
//            m_lines.resize( coordinate_count );
//        }
//
//        if( triangles )
//        {
//            // hope for convex, recalculate if not
//            m_triangles_indices.resize( m_coordinates.size( ) - 2 );
//        }
//
//        // iterate through coordinates and determine xy boundaries, convex, and clockwise see
//        // https://stackoverflow.com/questions/1165647/how-to-determine-if-a-list-of-polygon-points-are-in-clockwise-order
//
//        m_lower_bound_x = INFINITY_POSITIVE;
//        m_lower_bound_y = INFINITY_POSITIVE;
//        m_upper_bound_x = INFINITY_NEGATIVE;
//        m_upper_bound_y = INFINITY_NEGATIVE;
//
//        uint bottom_index = 0; // for convex hull
//        dec edge_curve = 0;
//        for_range( i, coordinate_count )
//        {
//            Coordinate & coordinate = m_coordinates[ i ];
//
//            if( is_infinity( coordinate.x( ) ) || is_infinity( coordinate.y( ) ) )
//            {
//                Assert( "no infinities allowed (for now)" );
//            }
//
//            if( transform )
//            {
//                coordinate = m_transform.transform( coordinate );
//            }
//
//            if( m_coordinates_dirty )
//            {
//                m_lower_bound_x = min( m_lower_bound_x, coordinate.x( ) );
//                m_lower_bound_y = min( m_lower_bound_y, coordinate.y( ) );
//                m_upper_bound_x = max( m_upper_bound_x, coordinate.x( ) );
//                m_upper_bound_y = max( m_upper_bound_y, coordinate.y( ) );
//
//                Coordinate prev_coordinate, prev_prev_coordinate;
//                if( i > 1 )
//                {
//                    prev_coordinate = m_coordinates[ i - 1 ];
//                    prev_prev_coordinate = m_coordinates[ i - 2 ];
//                }
//                else if( i == 1 )
//                {
//                    prev_coordinate = m_coordinates[ 0 ];
//                    prev_prev_coordinate = m_coordinates[ coordinate_count - 1 ];
//
//                    if( transform )
//                    {
//                        prev_prev_coordinate = m_transform.transform( prev_prev_coordinate );
//                    }
//                }
//                else
//                { // i == 0
//                    prev_coordinate = m_coordinates[ coordinate_count - 1 ];
//                    prev_prev_coordinate = m_coordinates[ coordinate_count - 2 ];
//
//                    if( transform )
//                    {
//                        prev_coordinate = m_transform.transform( prev_coordinate );
//                        prev_prev_coordinate = m_transform.transform( prev_prev_coordinate );
//                    }
//                }
//
//                edge_curve += (dec)( ( coordinate.x( ) - prev_coordinate.x( ) ) * ( coordinate.y( ) + prev_coordinate.y( ) ) );
//            }
//
//            if( lines )
//            {
//                if( i )
//                {
//                    // m_lines[ i - 1 ] = Line( m_coordinates[ i - 1 ], m_coordinates[ i ] );
//                }
//                else
//                { // i = 0
//                    Coordinate c = m_coordinates[ m_coordinates.size( ) - 1 ];
//                    if( transform )
//                    {
//                        c = m_transform.transform( c );
//                    }
//                    // m_lines[ m_lines.size( ) - 1 ] = Line( c, m_coordinates[ 0 ] );
//                }
//            }
//
//            if( triangles && ( i >= 2 ) )
//            {
//                m_triangles_indices[ i - 2 ] = { 0, i - 1, i };
//            }
//
//            if( convex_hull )
//            {
//                if( ( coordinate.y( ) < m_coordinates[ bottom_index ].y( ) ) || ( coordinate.y( ) == m_coordinates[ bottom_index ].y( ) && ( coordinate.x( ) < m_coordinates[ bottom_index ].x( ) ) ) )
//                {
//                    bottom_index = i;
//                }
//            }
//        }
//
//        if( m_coordinates_dirty )
//        {
//            if( edge_curve > 0 ) // clockwise
//            {
//                m_convex = convex_cw;
//                
//                m_coordinates.reverse( );
//
//                if( triangles && m_convex )
//                {
//                    for_range( i, m_triangles_indices.size( ) )
//                    {
//                        m_triangles_indices[ i ].reverse( );
//                    }
//                }
//            }
//            else
//            {
//                m_convex = convex_ccw;
//            }
//
//            m_coordinates_dirty = false;
//        }
//
//        // if( triangles )
//        // {
//        //     if( !m_convex )
//        //     { // else already set in for loop above
//        //         int index = 0;
//        //         varray<Coordinate> coordinates = m_coordinates;
//        //
//        //         m_triangles.clear( );
//        //         m_triangles.reserve( coordinates.size( ) - 2 );
//        //
//        //         int size = coordinates.size( );
//        //
//        //         int cycle = 0;
//        //         int max_cycles = ( size * size ) / 2;
//        //         bool success = true; // todo this shouldn't be necessary
//        //
//        //         while( coordinates.size( ) > 3 )
//        //         {
//        //             ++cycle;
//        //             if( cycle >= max_cycles )
//        //             {
//        //                 success = false;
//        //                 break;
//        //             }
//        //
//        //             Polygon triangle;
//        //             bool is_ear = false;
//        //             Coordinate coordinate = coordinates[ index % coordinates.size( ) ];
//        //             Coordinate next_coordinate = coordinates[ ( index + 1 ) % coordinates.size( ) ];
//        //             Angle next_angle( coordinate, next_coordinate );
//        //
//        // #if 0
//        //             // check if coordinate is colinear and thus can be removed
//        //             Coordinate prev_coordinate = coordinates[ ( index + coordinates.size( ) - 1 ) % coordinates.size( ) ];
//        //             Angle angle_prev( coordinate, prev_coordinate );
//        //             if ( ( next_angle == angle_prev ) || ( next_angle - angle_prev == PI ) ) {
//        //                 coordinates.erase( index );
//        //                 continue;
//        //             }
//        // #endif
//        //
//        //             Coordinate next_next_coordinate = coordinates[ ( index + 2 ) % coordinates.size( ) ];
//        //             Angle next_next_angle( coordinate, next_next_coordinate );
//        //
//        //             // check if next two points are colinear and if so skip so
//        //             // it will be removed in the next iteration
//        //             if( !( ( next_angle == next_next_angle ) || ( ( next_angle - next_next_angle ) == Angle( PI ) ) ) )
//        //             {
//        //                 triangle = Polygon( { coordinate, next_coordinate, next_next_coordinate } );
//        //                 if( ( next_next_angle - next_angle ) < Angle( PI ) )
//        //                 {
//        //                     is_ear = true;
//        //                     for_range( i, coordinates.size( ) )
//        //                     {
//        //                         if( ( i == index ) || ( i == ( index + 1 ) % coordinates.size( ) ) || ( i == ( index + 2 ) % coordinates.size( ) ) )
//        //                             continue;
//        //                         if( triangle.contains( coordinates[ i ] ) )
//        //                         {
//        //                             is_ear = false;
//        //                             break;
//        //                         }
//        //                     }
//        //                 }
//        //             }
//        //
//        //             if( is_ear )
//        //             {
//        //                 // check and add to triangle list
//        //                 // Assert( triangle.area( ), "triangle must have some
//        //                 // area or else something is wrong" );
//        //                 m_triangles.insert_back( triangle );
//        //                 coordinates.erase( ( index + 1 ) % coordinates.size( ) );
//        //             }
//        //             else
//        //             {
//        //                 // move index over to continue search
//        //                 ++index;
//        //                 index %= coordinates.size( );
//        //             }
//        //         }
//        //
//        //         if( success )
//        //         {
//        //             Assert( coordinates.size( ) == 3, "all triangularized Triangles should have 3 "
//        //                                               "points, obviously" );
//        //             Polygon triangle = Polygon( { coordinates[ 0 ], coordinates[ 1 ], coordinates[ 2 ] } );
//        //             if( triangle.area( ) )
//        //                 m_triangles.insert_back( triangle );
//        //         }
//        //     }
//        //
//        //     m_triangles_dirty = false;
//        // }
//
//        if( convex_partitions )
//        {
//            if( m_convex )
//            {
//                m_convex_partitions_indices = varray<varray<uint>>( 1 );
//                for_range( i, 1 )
//                {
//                    uint side_count = m_convex_partitions_indices[ i ].size( );
//                    m_convex_partitions_indices[ i ].resize( side_count );
//                    for_range( j, side_count )
//                    {
//                        m_convex_partitions_indices[ i ][ j ] = j;
//                    }
//                }
//            }
//            else
//            {
//                // TODO technically this is true but there's definitely a better way
//                m_convex_partitions_indices = m_triangles_indices;
//            }
//
//            m_convex_partitions_dirty = false;
//        }
//
//        if( convex_hull )
//        {
//            if( m_convex )
//            {
//                m_convex_hull_indices.resize( m_coordinates.size( ) );
//                for_range( i, m_convex_hull_indices.size( ) )
//                {
//                    m_convex_hull_indices[ i ] = i;
//                }
//            }
//            else
//            {
//                m_convex_hull_indices = { bottom_index };
//
//                int index_offset = bottom_index;
//                for_range( i, coordinate_count - 1 )
//                {
//                    uint index = (uint)( i + index_offset + 1 ) % coordinate_count;
//                    Coordinate coordinate = m_coordinates[ index ];
//
//                    while( m_convex_hull_indices.size( ) > 1 )
//                    {
//                        Coordinate prev_coordinate = m_coordinates[ m_convex_hull_indices[ m_convex_hull_indices.size( ) - 1 ] ];
//                        Coordinate prev_prev_coordinate = m_coordinates[  m_convex_hull_indices[ m_convex_hull_indices.size( ) - 2 ] ];
//                        Angle angle( prev_coordinate, coordinate );
//                        Angle prev_angle( prev_prev_coordinate, prev_coordinate );
//
//                        if( ( angle - prev_angle ) < Angle( PI ) )
//                        {
//                            break;
//                        }
//                        else
//                        {
//                            m_convex_hull_indices.erase_back( );
//                        }
//                    }
//                    m_convex_hull_indices.insert_back( index );
//                }
//            }
//
//            // Assert( Polygon( m_convex_hull ).convex( ), "convex hull must be convex...obviously" );
//            m_convex_hull_dirty = false;
//        }
//    }
//
//    if( m_coordinates_dirty )
//    {
//        m_coordinates_dirty = false;
//    }
//
//    if( transform )
//    {
//        m_transform = IDENTITY_TRANSFORM;
//    }
//}
//
//Polygon & Polygon::transform( const Transform & _transform )
//{
//    dirty( );
//    m_transform.chain( _transform );
//    m_cumulative_transform.chain( _transform );
//    return *this;
//}
//
//Polygon & Polygon::move( const Vector & _vector )
//{
//    return transform( MoveTransform( _vector ) );
//}
//
//Polygon & Polygon::scale( const dec _scale, const Coordinate & _origin )
//{
//    return transform( ScaleTransform( _scale, _origin ) );
//}
//
//Polygon & Polygon::stretch( const Vector & _axis )
//{
//    return transform( StretchTransform( _axis ) );
//}
//
//Polygon & Polygon::rotate( const Angle & _angle, const Coordinate & _origin )
//{
//    return transform( RotateTransform( _angle, _origin ) );
//}
//
//Polygon & Polygon::mirror( const Vector & _axis )
//{
//    return transform( ReflectionTransform( _axis ) );
//}
//
//Planc Polygon::area( ) const
//{
//    process( true, false, true, false, false );
//
//    Planc area = ZERO;
//
//    if( convex( ) )
//    {
//        area += ( ( m_coordinates.back( ).x( ) * m_coordinates.front( ).y( ) ) - ( m_coordinates.back( ).y( ) * m_coordinates.front( ).x( ) ) );
//
//        for_range( i, m_coordinates.size( ) - 2 )
//        {
//            const Coordinate & c1 = m_coordinates[ i ];
//            const Coordinate & c2 = m_coordinates[ i + 1 ];
//            area += ( ( c1.x( ) * c2.y( ) ) - ( c1.y( ) * c2.x( ) ) );
//        }
//
//        area = abs( half( area ) );
//    }
//    else
//    {
//        for_each( convex_indices, m_convex_partitions_indices )
//        {
//            varray<Coordinate> coordinates;
//            for_each( index, convex_indices )
//            {
//                coordinates.insert_back( m_coordinates[ index ] );
//            }
//            area += Polygon( coordinates ).area( );
//        }
//    }
//
//    return area;
//}
//
//uint Polygon::sides( ) const
//{
//    return m_coordinates.size( );
//}
//
//bool Polygon::convex( ) const
//{
//    process( false, false, false, false, false );
//    return m_convex;
//}
//
//Planc Polygon::lower_bound_x( ) const
//{
//    process( true, false, false, false, false );
//    return m_lower_bound_x;
//}
//
//Planc Polygon::lower_bound_y( ) const
//{
//    process( true, false, false, false, false );
//    return m_lower_bound_y;
//}
//
//Planc Polygon::upper_bound_x( ) const
//{
//    process( true, false, false, false, false );
//    return m_upper_bound_x;
//}
//
//Planc Polygon::upper_bound_y( ) const
//{
//    process( true, false, false, false, false );
//    return m_upper_bound_y;
//}
//
//bool Polygon::contains( const Coordinate & _coordinate, const bool _inclusive ) const
//{
//    process( true, true, false, false, false );
//
//    if( convex( ) )
//    {
//        const varray<Line> & lines = Polygon::lines( );
//        for_each( line, lines )
//        {
//            if( !line.below( _coordinate, _inclusive ) )
//            {
//                return false;
//            }
//        }
//
//        return true;
//    }
//    else
//    {
//        for_each( convex_polygon, convex_partitions( ) )
//        {
//            if( convex_polygon.contains( _coordinate, true ) )
//            {
//                // todo need to check if not inclusive and on border
//                return true;
//            }
//        }
//
//        return false;
//    }
//}
//
//varray<Line> Polygon::intersection( const Line & _line ) const
//{
//    varray<Line> intersection = { };
//    varray<Coordinate> intersection_coordinates = { };
//
//    if( contains( _line.c1( ) ) )
//    {
//        intersection_coordinates.insert_back( _line.c1( ) );
//    }
//
//    if( contains( _line.c2( ) ) )
//    {
//        intersection_coordinates.insert_back( _line.c2( ) );
//    }
//
//    const varray<Line> & lines = Polygon::lines( );
//    for_each( line, lines )
//    {
//        if( line.intersects( _line ) )
//        {
//            intersection_coordinates.insert_back( Coordinate( line.intersection( _line ) ) );
//        }
//    }
//
//    // sort intersections by distance from start of line
//    struct
//    {
//        Coordinate origin;
//        bool operator( )( const Coordinate & c1, const Coordinate & c2 ) { return ( c1.distance_to( origin ) < c2.distance_to( origin ) ); }
//    } line_intersection_distance;
//    line_intersection_distance.origin = _line.c1( );
//    intersection_coordinates.sort( line_intersection_distance );
//
//    // Assert( ( intersection_coordinates.size( ) % 2 ) == 0 );
//    for_range( i, intersection_coordinates.size( ) / 2 )
//    {
//        uint i1 = i * 2;
//        uint i2 = i1 + 1;
//        intersection.insert_back( Line( intersection_coordinates[ i1 ], intersection_coordinates[ i2 ] ) );
//    }
//
//    return intersection;
//}
//
//const varray<Coordinate> & Polygon::coordinates( const bool _raw ) const
//{
//    if( _raw )
//    {
//        return m_coordinates_raw;
//    }
//    else
//    {
//        process( true, false, false, false, false );
//        return m_coordinates;
//    }
//}
//
//varray<Line> Polygon::lines( const bool _raw ) const
//{
//    process( _raw, true, false, false, false );
//    
//    return m_lines;
//}
//
//varray<Polygon> Polygon::triangles( const bool _raw ) const
//{
//    process( _raw, false, true, false, false );
//    
//    varray<Polygon> triangles;
//    for_each( indices, m_triangles_indices )
//    {
//        triangles.insert_back( Polygon( { m_coordinates[ indices[ 0 ] ],
//                                          m_coordinates[ indices[ 1 ] ],
//                                          m_coordinates[ indices[ 2 ] ] } ) );
//    }
//    return triangles;
//}
//
//varray<Polygon> Polygon::convex_partitions( const bool _raw ) const
//{
//    process( _raw, false, false, true, false );
//    
//    varray<Polygon> partitions;
//    for_each( indices, m_convex_partitions_indices )
//    {
//        varray<Coordinate> coordinates;
//        for_each( index, indices )
//        {
//            coordinates.insert_back( m_coordinates[ index ] );
//        }
//    }
//    return partitions;
//}
//
//Polygon Polygon::convex_hull( const bool _raw ) const
//{
//    process( _raw, false, false, false, true );
//    
//    varray<Coordinate> coordinates;
//    for_each( index, m_convex_hull_indices )
//    {
//        coordinates.insert_back( m_coordinates[ index ] );
//    }
//    return Polygon( coordinates );
//}
//
//Polygon Polygon::operator+( const Vector & _v ) const { return Polygon( *this ).move( _v ); }
//
//Polygon & Polygon::operator+=( const Vector & _v ) { return move( _v ); }
//
//Polygon Polygon::operator-( const Vector & _v ) const { return *this + -_v; }
//
//Polygon & Polygon::operator-=( const Vector & _v ) { return *this += -_v; }
//
//// TODO optimize this (no raw?)
//bool Polygon::operator==( const Polygon & _polygon ) const
//{
//    if( m_coordinates.size( ) != _polygon.m_coordinates.size( ) )
//    {
//        return false;
//    }
//
//    uint coordinate_count = m_coordinates.size( );
//
//    const varray<Coordinate> & _c1 = coordinates( false );
//    const varray<Coordinate> & _c2 = _polygon.coordinates( false );
//
//    bool matched = false;
//    int offset = 0;
//
//    for_range( i, coordinate_count )
//    {
//        ++offset;
//        if( _c1[ offset ] == _c2[ 0 ] )
//        {
//            matched = true;
//            break;
//        }
//    }
//
//    if( !matched )
//    {
//        return false;
//    }
//
//    for_range( i, coordinate_count )
//    {
//        if( _c1[ ( i + offset ) % coordinate_count ] != _c2[ i ] )
//        {
//            return false;
//        }
//    }
//
//    return true;
//}
