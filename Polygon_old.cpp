//#include "Polygon.hpp"
//
//namespace
//{
//const uint CIRCLE_PRECISION = 60;
//}
//
//uint Polygon::circle_precision( const dec _perimeter )
//{
//    return CIRCLE_PRECISION;
//}
//
//Polygon::Polygon( ) { }
//
//Polygon::Polygon( const varray<Coordinate> & _coordinates, const bool _assume_is_convex )
//{
//    m_coordinates_dirty = true;
//    m_coordinates_raw = m_coordinates = _coordinates;
//}
//
//Polygon::Polygon( const varray<Coordinate> & _coordinates, const Transform & _transform, const bool _assume_is_convex )
//{
//    m_coordinates_dirty = true;
//    m_coordinates_raw = m_coordinates = _coordinates;
//    transform( _transform );
//}
//
//Triangle::Triangle( const Coordinate & c1, const Coordinate & c2, const Coordinate & c3 )
//    : Polygon( { c1, c2, c3 } ) { }
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
//Circle::Circle( const Planc & _radius, const Coordinate & _center )
//    : Polygon( Polygon::equilateral( Polygon::circle_precision( _radius ) ).coordinates( ), ScaleTransform( _radius ).chain( MoveTransform( _center ) ), true ) { }
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
//    Polygon equilateral = equilaterals[ _side_count ];
//    equilateral.scale( _radius );
//    equilateral.rotate( _rotation );
//    equilateral.move( _center );
//    
//    return equilateral;
//}
//
//Polygon Polygon::expand( const Polygon & _polygon, const Planc & _expansion )
//{
//    //if( !_expansion )
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
//const Polygon & Polygon::dirty( ) const
//{
//    Transformable::dirty( );
//    m_coordinates_dirty = m_lines_dirty = m_triangles_dirty = m_convex_partitions_dirty = m_convex_hull_dirty = true;
//    return *this;
//}
//
//void Polygon::process( bool do_transform, bool do_lines, bool do_triangles, bool do_convex_partitions, bool do_convex_hull ) const
//{
//    do_transform = true;
//    //do_lines = true;
//    do_triangles = true;
//    do_convex_partitions = true;
//    do_convex_hull = true;
//    
//    uint coordinate_count = m_coordinates.size( );
//
//    if( Transformable::transform( ).identity( ) )
//    {
//        do_transform = false;
//    }
//
//    if( do_convex_partitions )
//    {
//        do_triangles = true;
//    }
//
//    if( !m_lines_dirty )
//    {
//        do_lines = false;
//    }
//
//    if( !m_triangles_dirty )
//    {
//        do_triangles = false;
//    }
//
//    if( !m_convex_partitions_dirty )
//    {
//        do_convex_partitions = false;
//    }
//
//    if( !m_convex_hull_dirty )
//    {
//        do_convex_hull = false;
//    }
//
//    if( !m_coordinates_dirty && !do_transform && !do_lines && !do_triangles && !do_convex_partitions && !do_convex_hull )
//    {
//        return;
//    }
//
//    if( coordinate_count == 1 )
//    {
//        if( do_transform )
//        {
//            m_coordinates[ 0 ] = Transformable::transform( ).transform( m_coordinates[ 0 ] );
//        }
//
//        m_convex = true;
//
//        m_lower_bound_x = m_coordinates[ 0 ].x( );
//        m_lower_bound_y = m_coordinates[ 0 ].y( );
//        m_upper_bound_x = m_coordinates[ 0 ].x( );
//        m_upper_bound_y = m_coordinates[ 0 ].y( );
//
//        m_lines_dirty = false;
//        m_triangles_dirty = false;
//        m_convex_partitions_dirty = false;
//        
//        m_coordinates_dirty = false;
//    }
//    else if( coordinate_count == 2 )
//    {
//        if( do_transform )
//        {
//            m_coordinates[ 0 ] = transform( m_coordinates[ 0 ] );
//            m_coordinates[ 1 ] = transform( m_coordinates[ 1 ] );
//        }
//
//        m_convex = true;
//
//        m_lower_bound_x = min( m_coordinates[ 0 ].x( ), m_coordinates[ 1 ].x( ) );
//        m_lower_bound_y = min( m_coordinates[ 0 ].y( ), m_coordinates[ 1 ].y( ) );
//        m_upper_bound_x = max( m_coordinates[ 0 ].x( ), m_coordinates[ 1 ].x( ) );
//        m_upper_bound_y = max( m_coordinates[ 0 ].y( ), m_coordinates[ 1 ].y( ) );
//
//        m_lines_dirty = false;
//        m_triangles_dirty = false;
//        m_convex_partitions_dirty = false;
//        
//        m_coordinates_dirty = false;
//    }
//    else if( coordinate_count == 3 )
//    {
//        if( do_transform )
//        {
//            m_coordinates[ 0 ] = transform( m_coordinates[ 0 ] );
//            m_coordinates[ 1 ] = transform( m_coordinates[ 1 ] );
//            m_coordinates[ 2 ] = transform( m_coordinates[ 2 ] );
//        }
//
//        if( do_lines )
//        {
//            m_lines.resize( 3 );
//            m_lines[ 0 ] = Line( m_coordinates[ 0 ], m_coordinates[ 1 ] );
//            m_lines[ 1 ] = Line( m_coordinates[ 1 ], m_coordinates[ 2 ] );
//            m_lines[ 2 ] = Line( m_coordinates[ 2 ], m_coordinates[ 0 ] );
//            m_lines_dirty = false;
//        }
//
//        if( do_triangles )
//        {
//            m_triangles = { Polygon( m_coordinates, true ) };
//            m_triangles_dirty = false;
//        }
//
//        if( do_convex_partitions )
//        {
//            m_convex_partitions = { Polygon( m_coordinates, true ) };
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
//        bool convex_cw = true;
//        bool convex_ccw = true;
//
//        if( do_lines )
//        {
//            m_lines.resize( coordinate_count );
//        }
//
//        if( do_triangles )
//        {
//            // hope for convex, recalculate if not
//            m_triangles.resize( m_coordinates.size( ) - 2 );
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
//        int bottom_index = 0; // for convex hull
//        dec edge_curve = 0;
//        for_range( i, coordinate_count )
//        {
//            Coordinate & coordinate = m_coordinates[ i ];
//
//            if( is_infinity( coordinate.x( ) ) || is_infinity( coordinate.y( ) ) )
//            {
//                // TODO
//                coordinate.x( ZERO );
//                coordinate.y( ZERO );
//                // Assert( "no infinities allowed (for now)" );
//            }
//
//            if( do_transform )
//            {
//                coordinate = transform( coordinate );
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
//                    
//                    if( i > 2 )
//                    {
//                        if( !Line( prev_prev_coordinate, prev_coordinate ).above( m_coordinates[ i ] ) )
//                        {
//                            convex_cw = false;
//                        }
//                        if( Line( prev_prev_coordinate, prev_coordinate ).above( m_coordinates[ i ] ) )
//                        {
//                            convex_ccw = false;
//                        }
//                    }
//                }
//                else if( i == 1 )
//                {
//                    prev_coordinate = m_coordinates[ 0 ];
//                    prev_prev_coordinate = m_coordinates[ coordinate_count - 1 ];
//
//                    if( do_transform )
//                    {
//                        prev_prev_coordinate = transform( prev_prev_coordinate );
//                    }
//                }
//                else
//                { // i == 0
//                    prev_coordinate = m_coordinates[ coordinate_count - 1 ];
//                    prev_prev_coordinate = m_coordinates[ coordinate_count - 2 ];
//
//                    if( do_transform )
//                    {
//                        prev_coordinate = transform( prev_coordinate );
//                        prev_prev_coordinate = transform( prev_prev_coordinate );
//                    }
//                }
//
//                edge_curve += (dec)( ( coordinate.x( ) - prev_coordinate.x( ) ) * ( coordinate.y( ) + prev_coordinate.y( ) ) );
//            }
//
//            if( do_lines )
//            {
//                if( i )
//                {
//                    // m_lines[ i - 1 ] = Line( m_coordinates[ i - 1 ], m_coordinates[ i ] );
//                }
//                else
//                { // i = 0
//                    Coordinate c = m_coordinates[ m_coordinates.size( ) - 1 ];
//                    if( do_transform )
//                    {
//                        c = transform( c );
//                    }
//                    // m_lines[ m_lines.size( ) - 1 ] = Line( c, m_coordinates[ 0 ] );
//                }
//            }
//
//            if( do_triangles && ( i >= 2 ) )
//            {
//                // this only works for convex, we clear it later if discovered to be concave
//                m_triangles[ i - 2 ] = Polygon( { m_coordinates[ 0 ], m_coordinates[ i - 1 ], m_coordinates[ i ] } );
//            }
//
//            if( do_convex_hull )
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
//            bool clockwise = ( edge_curve > 0 );
//            m_convex = clockwise ? convex_cw : convex_ccw;
//            
//            if( clockwise )
//            {
//                m_coordinates.reverse( );
//            }
//
//            m_coordinates_dirty = false;
//        }
//        
//        if( do_triangles )
//        {
//            if( !m_convex )
//            {
//                m_triangles.clear( );
//                
//                uint i = ZERO;
//                varray<Coordinate> temp_coordinates = m_coordinates;
//                
//                while( temp_coordinates.size( ) > THREE )
//                {
//                    i %= temp_coordinates.size( );
//                    uint i_prev = ( i + ( temp_coordinates.size( ) - 1 ) ) % temp_coordinates.size( );
//                    uint i_next = ( i + 1 ) % temp_coordinates.size( );
//                    
//                    Coordinate c = temp_coordinates[ i ];
//                    
//                    Coordinate c_prev = temp_coordinates[ i_prev ];
//                    Coordinate c_next = temp_coordinates[ i_next ];
//                    
//                    bool is_ear = true;
//                    Triangle ear( c, c_next, c_prev );
//                    
//                    for_range( j, temp_coordinates.size( ) )
//                    {
//                        if( ( j != i ) && ( j != i_prev ) && ( j != i_next ) )
//                        {
//                            if( ear.contains( temp_coordinates[ j ], false ) )
//                            {
//                                is_ear = false;
//                                break;
//                            }
//                        }
//                    }
//                    
//                    if( is_ear )
//                    {
//                        temp_coordinates.erase( i );
//                        m_triangles.insert_back( ear );
//                    }
//                    else
//                    {
//                        ++i;
//                    }
//                }
//                
//                m_triangles.insert_back( temp_coordinates );
//            }
//            
//            m_triangles_dirty = false;
//        }
//
//        if( do_convex_partitions )
//        {
//            if( m_convex )
//            {
//                m_convex_partitions = { Polygon( m_coordinates, true ) };
//            }
//            else
//            {
//                // todo this could be more accurate but does it matter?
//                m_convex_partitions = m_triangles;
//            }
//
//            m_convex_partitions_dirty = false;
//        }
//
//        if( do_convex_hull )
//        {
//            if( m_convex )
//            {
//                m_convex_hull = m_coordinates;
//            }
//            else
//            {
//                varray<Coordinate> convex_hull_coordinates;
//                convex_hull_coordinates.reserve( sides( ) );
//                convex_hull_coordinates.insert_back( m_coordinates[ bottom_index ] );
//
//                int index_offset = bottom_index;
//                for_range( i, coordinate_count - 1 )
//                {
//                    int index = (int)( i + index_offset + 1 ) % coordinate_count;
//                    Coordinate coordinate = m_coordinates[ index ];
//
//                    while( convex_hull_coordinates.size( ) > 1 )
//                    {
//                        Coordinate prev_coordinate = convex_hull_coordinates[ convex_hull_coordinates.size( ) - 1 ];
//                        Coordinate prev_prev_coordinate = convex_hull_coordinates[ convex_hull_coordinates.size( ) - 2 ];
//                        Angle angle( prev_coordinate, coordinate );
//                        Angle prev_angle( prev_prev_coordinate, prev_coordinate );
//
//                        if( ( angle - prev_angle ) < Angle( PI ) )
//                        {
//                            break;
//                        }
//                        else
//                        {
//                            convex_hull_coordinates.erase_back( );
//                        }
//                    }
//                    
//                    convex_hull_coordinates.insert_back( coordinate );
//                }
//                
//                m_convex_hull = convex_hull_coordinates;
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
//    if( do_transform )
//    {
//        const_clear_transform( );
//    }
//}
//
//void Polygon::process( ) const
//{
//    uint coordinate_count = m_coordinates.size( );
//    
//    if( coordinate_count == 1 )
//    {
//        m_coordinates[ 0 ] = transform( m_coordinates[ 0 ] );
//
//        m_convex = true;
//
//        m_lower_bound_x = m_coordinates[ 0 ].x( );
//        m_lower_bound_y = m_coordinates[ 0 ].y( );
//        m_upper_bound_x = m_coordinates[ 0 ].x( );
//        m_upper_bound_y = m_coordinates[ 0 ].y( );
//    }
//    else if( coordinate_count == 2 )
//    {
//        m_coordinates[ 0 ] = transform( m_coordinates[ 0 ] );
//        m_coordinates[ 1 ] = transform( m_coordinates[ 1 ] );
//
//        m_convex = true;
//
//        m_lower_bound_x = min( m_coordinates[ 0 ].x( ), m_coordinates[ 1 ].x( ) );
//        m_lower_bound_y = min( m_coordinates[ 0 ].y( ), m_coordinates[ 1 ].y( ) );
//        m_upper_bound_x = max( m_coordinates[ 0 ].x( ), m_coordinates[ 1 ].x( ) );
//        m_upper_bound_y = max( m_coordinates[ 0 ].y( ), m_coordinates[ 1 ].y( ) );
//
//    }
//    else if( coordinate_count == 3 )
//    {
//        m_coordinates[ 0 ] = transform( m_coordinates[ 0 ] );
//        m_coordinates[ 1 ] = transform( m_coordinates[ 1 ] );
//        m_coordinates[ 2 ] = transform( m_coordinates[ 2 ] );
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
//    }
//    else if( coordinate_count )
//    {
//        // iterate through coordinates and determine xy boundaries, convex, and clockwise see
//        // https://stackoverflow.com/questions/1165647/how-to-determine-if-a-list-of-polygon-points-are-in-clockwise-order
//        
//        // default to true then check for violations
//        bool convex_cw = true;
//        bool convex_ccw = true;
//        
//        // hope for convex, recalculate if not
//        m_triangles.resize( m_coordinates.size( ) - TWO );
//        
//        m_lower_bound_x = INFINITY_POSITIVE;
//        m_lower_bound_y = INFINITY_POSITIVE;
//        m_upper_bound_x = INFINITY_NEGATIVE;
//        m_upper_bound_y = INFINITY_NEGATIVE;
//        
//        dec edge_curve = ZERO;
//        
//        int bottom_index = ZERO; // for convex hull
//        
//        for_range( i, coordinate_count )
//        {
//            Coordinate & coordinate = m_coordinates[ i ];
//
//            if( is_infinity( coordinate.x( ) ) || is_infinity( coordinate.y( ) ) )
//            {
//                // TODO
//                coordinate.x( ZERO );
//                coordinate.y( ZERO );
//                // Assert( "no infinities allowed (for now)" );
//            }
//
//            coordinate = transform( coordinate );
//
//            m_lower_bound_x = min( m_lower_bound_x, coordinate.x( ) );
//            m_lower_bound_y = min( m_lower_bound_y, coordinate.y( ) );
//            m_upper_bound_x = max( m_upper_bound_x, coordinate.x( ) );
//            m_upper_bound_y = max( m_upper_bound_y, coordinate.y( ) );
//
//            Coordinate prev_coordinate, prev_prev_coordinate;
//            if( i > 1 )
//            {
//                prev_coordinate = m_coordinates[ i - 1 ];
//                prev_prev_coordinate = m_coordinates[ i - 2 ];
//                
//                if( i > 2 )
//                {
//                    if( !Line( prev_prev_coordinate, prev_coordinate ).above( m_coordinates[ i ] ) )
//                    {
//                        convex_cw = false;
//                    }
//                    if( Line( prev_prev_coordinate, prev_coordinate ).above( m_coordinates[ i ] ) )
//                    {
//                        convex_ccw = false;
//                    }
//                }
//            }
//            else if( i == 1 )
//            {
//                prev_coordinate = m_coordinates[ 0 ];
//                prev_prev_coordinate = transform( m_coordinates[ coordinate_count - 1 ] );
//            }
//            else
//            { // i == 0
//                prev_coordinate = transform( m_coordinates[ coordinate_count - 1 ] );
//                prev_prev_coordinate = transform( m_coordinates[ coordinate_count - 2 ] );
//            }
//
//            edge_curve += (dec)( ( coordinate.x( ) - prev_coordinate.x( ) ) * ( coordinate.y( ) + prev_coordinate.y( ) ) );
//
//            // this only works for convex, we clear it later if discovered to be concave
//            m_triangles[ i - 2 ] = Polygon( { m_coordinates[ 0 ], m_coordinates[ i - 1 ], m_coordinates[ i ] } );
//
//            // for convex hull
//            if( ( coordinate.y( ) < m_coordinates[ bottom_index ].y( ) ) || ( coordinate.y( ) == m_coordinates[ bottom_index ].y( ) && ( coordinate.x( ) < m_coordinates[ bottom_index ].x( ) ) ) )
//            {
//                bottom_index = i;
//            }
//        }
//
//        bool clockwise = ( edge_curve > 0 );
//        
//        if( clockwise )
//        {
//            m_coordinates.reverse( );
//        }
//        
//        m_convex = clockwise ? convex_cw : convex_ccw;
//    
//        if( !m_convex )
//        {
//            m_triangles.clear( );
//            
//            uint i = ZERO;
//            varray<Coordinate> temp_coordinates = m_coordinates;
//            
//            while( temp_coordinates.size( ) > THREE )
//            {
//                i %= temp_coordinates.size( );
//                uint i_prev = ( i + ( temp_coordinates.size( ) - 1 ) ) % temp_coordinates.size( );
//                uint i_next = ( i + 1 ) % temp_coordinates.size( );
//                
//                Coordinate c = temp_coordinates[ i ];
//                
//                Coordinate c_prev = temp_coordinates[ i_prev ];
//                Coordinate c_next = temp_coordinates[ i_next ];
//                
//                bool is_ear = true;
//                Triangle ear( c, c_next, c_prev );
//                
//                for_range( j, temp_coordinates.size( ) )
//                {
//                    if( ( j != i ) && ( j != i_prev ) && ( j != i_next ) )
//                    {
//                        if( ear.contains( temp_coordinates[ j ], false ) )
//                        {
//                            is_ear = false;
//                            break;
//                        }
//                    }
//                }
//                
//                if( is_ear )
//                {
//                    temp_coordinates.erase( i );
//                    m_triangles.insert_back( ear );
//                }
//                else
//                {
//                    ++i;
//                }
//            }
//            
//            m_triangles.insert_back( temp_coordinates );
//        }
//
//        // if( do_convex_partitions )
//        {
//            if( m_convex )
//            {
//                m_convex_partitions = { Polygon( m_coordinates, true ) };
//            }
//            else
//            {
//                // todo this could be more accurate but does it matter?
//                m_convex_partitions = m_triangles;
//            }
//        }
//
//        // if( do_convex_hull )
//        {
//            if( m_convex )
//            {
//                m_convex_hull = m_coordinates;
//            }
//            else
//            {
//                varray<Coordinate> convex_hull_coordinates;
//                convex_hull_coordinates.reserve( sides( ) );
//                convex_hull_coordinates.insert_back( m_coordinates[ bottom_index ] );
//
//                int index_offset = bottom_index;
//                for_range( i, coordinate_count - 1 )
//                {
//                    int index = (int)( i + index_offset + 1 ) % coordinate_count;
//                    Coordinate coordinate = m_coordinates[ index ];
//
//                    while( convex_hull_coordinates.size( ) > 1 )
//                    {
//                        Coordinate prev_coordinate = convex_hull_coordinates[ convex_hull_coordinates.size( ) - 1 ];
//                        Coordinate prev_prev_coordinate = convex_hull_coordinates[ convex_hull_coordinates.size( ) - 2 ];
//                        Angle angle( prev_coordinate, coordinate );
//                        Angle prev_angle( prev_prev_coordinate, prev_coordinate );
//
//                        if( ( angle - prev_angle ) < Angle( PI ) )
//                        {
//                            break;
//                        }
//                        else
//                        {
//                            convex_hull_coordinates.erase_back( );
//                        }
//                    }
//                    
//                    convex_hull_coordinates.insert_back( coordinate );
//                }
//                
//                m_convex_hull = convex_hull_coordinates;
//            }
//
//            // Assert( Polygon( m_convex_hull ).convex( ), "convex hull must be convex...obviously" );
//            m_convex_hull_dirty = false;
//        }
//    }
//
//    const_clear_transform( );
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
//        for_each( convex, m_convex_partitions ) { area += convex.area( ); }
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
//        const varray<Line> & lines = Polygon::perimeter( );
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
//                if( !_inclusive )
//                {
//                    for_each( line, perimeter( ).lines( ) )
//                    {
//                        if( line.on( _coordinate ) )
//                        {
//                            return false;
//                        }
//                    }
//                    
//                    return true;
//                }
//                else
//                {
//                    return true;
//                }
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
//    const varray<Line> & lines = Polygon::perimeter( );
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
//Path Polygon::perimeter( const bool _raw ) const
//{
//    const varray<Coordinate> & c = coordinates( _raw );
//    const uint count = c.size( );
//
//    if( count >= 3 )
//    {
//        varray<Line> lines( c.size( ) );
//        if( c.size( ) )
//        {
//            for_range( i, c.size( ) - 1 ) lines[ i ] = Line( c[ i ], c[ i + 1 ] );
//            lines[ c.size( ) - 1 ] = Line( c[ c.size( ) - 1 ], c[ 0 ] );
//        }
//        return lines;
//    }
//    else if( count == 2 )
//    {
//        return varray<Line>( { Line( c[ 0 ], c[ 1 ] ) } );
//    }
//
//    return varray<Line>( );
//}
//
//varray<Polygon> Polygon::triangles( const bool _raw ) const
//{
//    process( true, false, true, false, false );
//    return m_triangles;
//}
//
//varray<Polygon> Polygon::convex_partitions( const bool _raw ) const
//{
//    process( true, false, false, true, false );
//    return m_convex_partitions;
//}
//
//Polygon Polygon::convex_hull( const bool _raw ) const
//{
//    process( true, false, false, false, true );
//    return Polygon( m_convex_hull );
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
