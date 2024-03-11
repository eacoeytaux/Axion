#include "Shape.hpp"

AXN_NAMESPACES
using axn::geometry::Shape;

Shape::Shape( const varray<Coordinate> & _coordinates ) {
    Assert( _coordinates.size( ) >= 3, "Polygon requires at least 3 coordinates" );
    m_coordinates = _coordinates;
    int coordinate_count = (int)m_coordinates.size( );

    m_convex = true; // default to true then check for violations

    Coordinate first_coordinate = m_coordinates[ 0 ];
    m_lower_bound_x = min<float>( m_lower_bound_x, first_coordinate.x( ) );
    m_lower_bound_y = min<float>( m_lower_bound_y, first_coordinate.y( ) );
    m_upper_bound_x = max<float>( m_upper_bound_x, first_coordinate.x( ) );
    m_upper_bound_y = max<float>( m_upper_bound_y, first_coordinate.y( ) );

    Coordinate second_coordinate = m_coordinates[ 1 ];
    m_lower_bound_x = min<float>( m_lower_bound_x, second_coordinate.x( ) );
    m_lower_bound_y = min<float>( m_lower_bound_y, second_coordinate.y( ) );
    m_upper_bound_x = max<float>( m_upper_bound_x, second_coordinate.x( ) );
    m_upper_bound_y = max<float>( m_upper_bound_y, second_coordinate.y( ) );

    // iterate through coordinates and determine xy boundaries, convex, and clockwise
    // see https://stackoverflow.com/questions/1165647/how-to-determine-if-a-list-of-polygon-points-are-in-clockwise-order
    float edge_curve = 0;
    for_range( coordinate_count ) {
        Coordinate coordinate = m_coordinates[ i ];

//        if( is_infinity( coordinate.x( ) ) || is_infinity( coordinate.y( ) ) )
//            Assert( false, "no infinities allowed (for now)" );

        m_lower_bound_x = min<float>( m_lower_bound_x, coordinate.x( ) );
        m_lower_bound_y = min<float>( m_lower_bound_y, coordinate.y( ) );
        m_upper_bound_x = max<float>( m_upper_bound_x, coordinate.x( ) );
        m_upper_bound_y = max<float>( m_upper_bound_y, coordinate.y( ) );

        {
            Coordinate prev_coordinate =
                m_coordinates[ ( i + coordinate_count - 1 ) % coordinate_count ];
            Coordinate prev_prev_coordinate =
                m_coordinates[ ( i + coordinate_count - 2 ) % coordinate_count ];
            Angle angle1( prev_prev_coordinate, prev_coordinate );
            Angle angle2( prev_prev_coordinate, coordinate );
            if( angle2 < angle1 )
                m_convex = false;
            edge_curve += (float)( ( coordinate.x( ) - prev_coordinate.x( ) ) *
                                   ( coordinate.y( ) + prev_coordinate.y( ) ) );
        }
    }
    dirty( false );

    m_clockwise = edge_curve > 0;
    if( m_clockwise )
        m_coordinates.reverse( );
}

Shape Shape::circle( const planc & radius, const Coordinate & center ) {
    return Shape::equilateral( 32, radius, center );
}

Shape Shape::triangle( const Coordinate & c1, const Coordinate & c2, const Coordinate & c3 ) {
    return Shape( { c1, c2, c3 });
}

Shape Shape::rectangle( const planc & width, const planc & height, const Coordinate & center, const Angle _rotation) { // TODO NOT ROTATING
    return Shape( { center + Vector( width.half( ), height.half( ) ), center + Vector( -width.half( ), height.half( ) ), center + Vector( -width.half( ), -height.half( ) ), center + Vector( width.half( ), -height.half( ) )} ).rotate( _rotation, center );
}

Shape Shape::equilateral( const int _side_count, const planc & _radius, const Coordinate & _center, const Angle _rotation ) {
    varray<Coordinate> coordinate_offsets( _side_count );
    Angle start_angle( PI_1D2 );
    Angle delta_angle( PI_2 / _side_count );
    start_angle += _rotation;
    for_each( offset, coordinate_offsets ) {
        offset = VectorA( start_angle, _radius, _center );
        start_angle += delta_angle;
    }
    return Shape( coordinate_offsets );
}

void Shape::evaluate( ) const {
    if( dirty( ) ) {
        varray<Line> new_lines( m_coordinates.size( ) );
        m_lines = new_lines;
        for_range( m_lines.size( ) - 1 ) m_lines[ i ] =
            Line( m_coordinates[ i ], m_coordinates[ i + 1 ] );
        m_lines[ m_lines.size( ) - 1 ] =
            Line( m_coordinates[ m_coordinates.size( ) - 1 ], m_coordinates[ 0 ] );
        
        m_triangles = triangularize( m_coordinates );
        
        dirty( false );
    }
}

varray<Shape> Shape::triangularize( const varray<Coordinate> & _coordinates ) const {
    if( _coordinates.size( ) == 3 ) {
        return varray<Shape>( { Shape( { _coordinates[ 0 ], _coordinates[ 1 ], _coordinates[ 2 ] } ) } );
    }
    
    varray<Shape> triangles;
    triangles.reserve( _coordinates.size( ) - 2 );
    if(true) { //( convex( ) ) {
        for_range( _coordinates.size( ) - 2 ) {
            triangles.push_back( Shape( { _coordinates[ 0 ], _coordinates[ i + 1 ], _coordinates[ i + 2 ] } ) );
        }
    } else {
        int index = 0;
        varray<Coordinate> coordinates = _coordinates;

        int size = coordinates.size( );

        int cycle = 0;
        int max_cycles = ( size * size ) / 2;
        bool success = true; // todo this shouldn't be necessary

        while( coordinates.size( ) > 3 ) {
            ++cycle;
            if( cycle >= max_cycles ) {
                success = false;
                break;
            }

            Shape triangle;
            bool is_ear = false;
            Coordinate coordinate = coordinates[ index % coordinates.size( ) ];
            Coordinate next_coordinate = coordinates[ ( index + 1 ) % coordinates.size( ) ];
            Angle next_angle( coordinate, next_coordinate );

            // check if coordinate is colinear and thus can be removed
            // Coordinate prev_coordinate = coordinates[(index +
            // coordinates.size() - 1) % coordinates.size()]; Angle
            // angle_prev(coordinate, prev_coordinate); if ((next_angle ==
            // angle_prev) || (next_angle - angle_prev == PI)) {
            //    coordinates.remove(index);
            //    continue;
            //}

            Coordinate next_next_coordinate =
                coordinates[ ( index + 2 ) % coordinates.size( ) ];
            Angle next_next_angle( coordinate, next_next_coordinate );

            // check if next two points are colinear and if so skip so it will
            // be removed in the next iteration
            if( !( ( next_angle == next_next_angle ) ||
                   ( next_angle - next_next_angle == PI ) ) ) {
                triangle = Shape( { coordinate, next_coordinate, next_next_coordinate } );
                if( ( next_next_angle - next_angle ) < PI ) {
                    is_ear = true;
                    for_range( coordinates.size( ) ) {
                        if( ( i == index ) || ( i == ( index + 1 ) % coordinates.size( ) ) ||
                            ( i == ( index + 2 ) % coordinates.size( ) ) )
                            continue;
                        if( triangle.contains( coordinates[ i ] ) ) {
                            is_ear = false;
                            break;
                        }
                    }
                }
            }

            if( is_ear ) {
                // check and add to triangle list
                // Assert(triangle.area(), "triangle must have some area or else 0po-9b =[[[[[[[something is wrong");
                triangles.push_back( triangle );
                coordinates.remove( ( index + 1 ) % coordinates.size( ) );
            } else {
                // move index over to continue search
                ++index;
                index %= coordinates.size( );
            }
        }

        if( success ) {
            Assert( coordinates.size( ) == 3,
                    "all triangularized Triangles should have 3 points, obviously" );
            Shape triangle = Shape( { coordinates[ 0 ], coordinates[ 1 ], coordinates[ 2 ] } );
            if( triangle.area( ) )
                triangles.push_back( triangle );
        }
    }
    return triangles;
}

Shape & Shape::move( const Vector & _vector ) {
    evaluate( );
    
    for_each( coordinate, m_coordinates )
        coordinate += _vector;
    
    dirty( true );
    return *this;
}

Shape & Shape::stretch( const Vector & _axis ) {
    evaluate( );
    
    move( -Vector( _axis.origin( ) ) );
    rotate( -_axis.angle( ) );

    planc stretch = _axis.magnitude( );
    for_each( coordinate, m_coordinates )
        coordinate.x( coordinate.x( ) * stretch );

    rotate( _axis.angle( ) );
    move( Vector( _axis.origin( ) ) );
    return *this;
}

Shape & Shape::scale( const float _scale, const Coordinate & _origin ) {
    evaluate( );
    
    for_each( coordinate, m_coordinates ) coordinate =
        ( Vector( coordinate ) * _scale ).destination( );
    dirty( true );
    
    return *this;
}

Shape & Shape::rotate( const Angle & _angle, const Coordinate & _origin ) {
    evaluate( );
    
    for_each( coordinate, m_coordinates )
        coordinate.rotate( _angle, _origin );
    
    dirty( true );
    return *this;
}

Shape & Shape::mirror( const Vector & _axis ) {
    evaluate( );
    
    for_each( coordinate, m_coordinates )
        coordinate.mirror( _axis );
    m_coordinates.reverse( );
    
    dirty( true );
    return *this;
}

Shape & Shape::expand( planc distance ) {
    dirty( true ); // TODO this shouldn't need to be called
    evaluate( );
    
    for_range( m_coordinates.size( ) )
    {
        Line line_prev = m_lines[ i ? i - 1 : m_lines.size( ) - 1 ];
        Line line_next = m_lines[ i ];
        
        Vector vector_prev( line_prev.c1( ), line_prev.c2( ) );
        Vector vector_next( line_next.c1( ), line_next.c2( ) );
        
        vector_prev.rotate( -PI_1D2 );
        vector_next.rotate( -PI_1D2 );
        
        vector_prev.magnitude( distance );
        vector_next.magnitude( distance );
        
        line_prev += vector_prev;
        line_next += vector_next;
        
        m_coordinates[ i ] = line_prev.intersection(line_next);
    }
    
    dirty( true );
    return *this;
}

planc Shape::area( ) const {
    planc area = 0.f;
    //for_each( triangle, m_triangles ) area += triangle.area( );
    return area;
}

uint Shape::sides( ) const { return coordinates( ).size( ); }

bool Shape::convex( ) const { return m_convex; }

Shape Shape::convex_hull( ) const {
    evaluate( );
    
    if( convex( ) )
        return *this;

    int bottom_index = 0;
    Coordinate bottom = coordinates( )[ 0 ];
    for_range( coordinates( ).size( ) ) {
        Coordinate coordinate = coordinates( )[ i ];
        if( ( coordinate.y( ) < bottom.y( ) ) ||
            ( coordinate.y( ) == bottom.y( ) && ( coordinate.x( ) < bottom.x( ) ) ) ) {
            bottom = coordinate;
            bottom_index = i;
        }
    }

    varray<Coordinate> convex_hull_coordinates;
    convex_hull_coordinates.reserve( coordinates( ).size( ) );
    convex_hull_coordinates.push_back( bottom );

    int offset = bottom_index;
    for_range( coordinates( ).size( ) - 1 ) {
        int index = (int)( i + offset + 1 ) % coordinates( ).size( );
        Coordinate coordinate = coordinates( )[ index ];

        while( convex_hull_coordinates.size( ) > 1 ) {
            Coordinate prev_coordinate =
                convex_hull_coordinates[ convex_hull_coordinates.size( ) - 1 ];
            Coordinate prev_prev_coordinate =
                convex_hull_coordinates[ convex_hull_coordinates.size( ) - 2 ];
            Angle angle( prev_coordinate, coordinate );
            Angle prev_angle( prev_prev_coordinate, prev_coordinate );

            if( ( angle - prev_angle ) < PI )
                break;
            else
                convex_hull_coordinates.remove_back( );
        }
        convex_hull_coordinates.push_back( coordinate );
    }

    Shape convex_hull( convex_hull_coordinates );
    Assert( convex_hull.convex( ), "convex hull must be convex...obviously" );
    return convex_hull;
}

float Shape::lower_bound_x( ) const { return m_lower_bound_x; }

float Shape::lower_bound_y( ) const { return m_lower_bound_y; }

float Shape::upper_bound_x( ) const { return m_upper_bound_x; }

float Shape::upper_bound_y( ) const { return m_upper_bound_y; }

bool Shape::contains( const Coordinate & _c ) const {
//    if( convex( ) ) {
//        for_each( line, lines( ) ) if( !line.below( _c, true ) ) return false;
//        return true;
//    } else {
//        for_each( triangle, m_triangles ) if( triangle.contains( _c ) ) return true;
//        return false;
//    }
    return false;
}

varray<Line> Shape::intersection( const Line & _line ) const {
    varray<Line> intersection = { };

    // find all intersection points
    varray<Coordinate> intersections = { };
    for_each( line, lines( ) ) if( line.intersects( _line ) )
        intersections.push_back( Coordinate( line.intersection( _line ) ) );

    // sort intersections by distance from start of line
    struct {
        Coordinate origin;
        inline bool operator( )( const Coordinate & c1, const Coordinate & c2 ) {
            return ( c1.distance( origin ) < c2.distance( origin ) );
        }
    } line_intersection_distance;
    line_intersection_distance.origin = _line.c1( );
    sort( intersections.begin( ), intersections.end( ), line_intersection_distance );

    // create lines based off points
    Assert( intersections.size( ) % 2 == 0,
            "confirm intersection lines have an entry and exit" );

    return intersection;
}

const varray<Coordinate> & Shape::coordinates( ) const {
    evaluate( );
    return m_coordinates;
}

const varray<Line> & Shape::lines( ) const {
    evaluate( );
    return m_lines;
}

const varray<Shape> & Shape::triangles( ) const {
    evaluate( );
    return m_triangles;
}

Shape Shape::operator+( const Vector & _v ) const {
    varray<Coordinate> moved_coordinates( coordinates( ) );
    for_each( coordinate, moved_coordinates ) coordinate += _v;
    return Shape( moved_coordinates );
}

Shape & Shape::operator+=( const Vector & _v ) {
    dirty( true );
    *this = *this + _v;
    return *this;
}

Shape Shape::operator-( const Vector & _v ) const { return *this + -_v; }

Shape & Shape::operator-=( const Vector & _v ) {
    dirty( true );
    *this += -_v;
    return *this;
}

bool Shape::operator==( const Shape & _shape ) const {
    if( coordinates( ).size( ) != _shape.coordinates( ).size( ) )
        return false;
    int coordinate_count = (int)coordinates( ).size( );
    bool matched = false;
    int offset = 0;
    for_range( coordinate_count ) {
        ++offset;
        if( coordinates( )[ offset ] == _shape.coordinates( )[ 0 ] ) {
            matched = true;
            break;
        }
    }
    if( !matched )
        return false;
    for_range( coordinate_count ) {
        if( coordinates( )[ ( i + offset ) % coordinate_count ] !=
           _shape.coordinates( )[ i ] ) return false;
    }
    return true;
}
