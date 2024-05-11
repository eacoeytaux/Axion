#include "Shape.hpp"

using axn::geometry::Shape;

const uint Shape::CIRCLE_PRECISION = 60;

Shape::Shape( const varray<Coordinate> & _coordinates )
{
    Assert( ( _coordinates.size( ) >= 3 ), "Polygon requires at least 3 coordinates" );
    m_coordinates = _coordinates;
    dirty( );
}

Shape Shape::triangle( const Coordinate & c1, const Coordinate & c2, const Coordinate & c3 )
{
    return Shape( { c1, c2, c3 } );
}

Shape Shape::rectangle( const Planc & width, const Planc & height, const Coordinate & center, const Angle _rotation )
{
    return Shape( { center + Vector( width, height ).half( ), center + Vector( -width, height ).half( ), center + Vector( -width, -height ).half( ), center + Vector( width, -height ).half( ) } ).rotate( _rotation, center );
}

Shape Shape::square( const Planc & width, const Coordinate & center, const Angle _rotation )
{
    return Shape( { center + Vector( width, width ).half( ), center + Vector( -width, width ).half( ), center + Vector( -width, -width ).half( ), center + Vector( width, -width ).half( ) } ).rotate( _rotation, center );
}

Shape Shape::equilateral( const uint _side_count, const Planc & _radius, const Coordinate & _center, const Angle _rotation )
{
    Assert( ( _side_count >= 3 ), "Polygon requires at least 3 sides" );

    static umap<uint, Shape> equilaterals;

    if( !equilaterals.contains( _side_count ) )
    {
        varray<Coordinate> coordinates( _side_count );
        Angle start_angle = Angle( RIGHT_ANGLE, false ) + _rotation; // start at the top + rotation bv
        Angle delta_angle( TAU / (double)_side_count );
        for_each( offset, coordinates )
        {
            offset = VectorA( start_angle );
            start_angle += delta_angle;
        }

        equilaterals[ _side_count ] = Shape( coordinates );
    }

    return Shape( equilaterals[ _side_count ] ).scale( _radius ).move( _center );
}

Shape Shape::circle( const Planc & _radius, const Coordinate & _center )
{
    static const Shape unit_circle = Shape::equilateral( CIRCLE_PRECISION );
    return Shape( unit_circle ).scale( _radius ).move( _center );
}

Shape Shape::expand( const Shape & _shape, const Planc _expansion )
{
    varray<Coordinate> coordinates = _shape.coordinates( );
    uint coordinate_count = coordinates.size( );

    varray<Coordinate> new_coordinates( coordinate_count );

    for_range( coordinate_count )
    {
        uint coordinate_prev_index = i ? ( i - 1 ) : coordinate_count - 1;
        uint coordinate_next_index = ( i + 1 ) % coordinate_count;

        Line line_prev = Line( coordinates[ coordinate_prev_index ], coordinates[ i ] );
        Line line_next = Line( coordinates[ i ], coordinates[ coordinate_next_index ] );

        Vector vector_prev( line_prev.c1( ), line_prev.c2( ) );
        Vector vector_next( line_next.c1( ), line_next.c2( ) );

        vector_prev.rotate( -RIGHT_ANGLE );
        vector_next.rotate( -RIGHT_ANGLE );

        vector_prev.magnitude( _expansion );
        vector_next.magnitude( _expansion );

        line_prev += vector_prev;
        line_next += vector_next;

        new_coordinates[ i ] = line_prev.intersection( line_next );
    }

    return Shape( new_coordinates );
}

Transform Shape::transform( ) const
{
    return m_transform;
}

void Shape::dirty( ) const
{
    m_coordinates_dirty = m_lines_dirty = m_triangles_dirty = m_convex_partitions_dirty = m_convex_hull_dirty = true;
}

void Shape::process( bool transform, bool lines, bool triangles, bool convex_partitions, bool convex_hull ) const
{
    uint coordinate_count = m_coordinates.size( );

    if( m_transform.identity( ) )
        transform = false;

    if( convex_partitions )
        triangles = true;

    if( !m_lines_dirty )
        lines = false;
    if( !m_triangles_dirty )
        triangles = false;
    if( !m_convex_partitions_dirty )
        convex_partitions = false;
    if( !m_convex_hull_dirty )
        convex_hull = false;

    if( !m_coordinates_dirty && !lines && !triangles && !convex_partitions && !convex_hull )
        return;

    if( coordinate_count == 3 )
    {
        if( transform )
        {
            m_coordinates[ 0 ] = m_transform.transform( m_coordinates[ 0 ] );
            m_coordinates[ 1 ] = m_transform.transform( m_coordinates[ 1 ] );
            m_coordinates[ 2 ] = m_transform.transform( m_coordinates[ 2 ] );
        }

        {
            m_convex = true;
            if( Line( m_coordinates[ 0 ], m_coordinates[ 1 ] ).above( m_coordinates[ 2 ] ) )
            {
                m_clockwise = true;
                m_coordinates.reverse( );
            }
            else
            {
                m_clockwise = false;
            }

            m_lower_bound_x = min( m_lower_bound_x, m_coordinates[ 0 ].x( ) );
            m_lower_bound_y = min( m_lower_bound_y, m_coordinates[ 0 ].y( ) );
            m_upper_bound_x = max( m_upper_bound_x, m_coordinates[ 0 ].x( ) );
            m_upper_bound_y = max( m_upper_bound_y, m_coordinates[ 0 ].y( ) );

            m_lower_bound_x = min( m_lower_bound_x, m_coordinates[ 1 ].x( ) );
            m_lower_bound_y = min( m_lower_bound_y, m_coordinates[ 1 ].y( ) );
            m_upper_bound_x = max( m_upper_bound_x, m_coordinates[ 1 ].x( ) );
            m_upper_bound_y = max( m_upper_bound_y, m_coordinates[ 1 ].y( ) );

            m_lower_bound_x = min( m_lower_bound_x, m_coordinates[ 2 ].x( ) );
            m_lower_bound_y = min( m_lower_bound_y, m_coordinates[ 2 ].y( ) );
            m_upper_bound_x = max( m_upper_bound_x, m_coordinates[ 2 ].x( ) );
            m_upper_bound_y = max( m_upper_bound_y, m_coordinates[ 2 ].y( ) );

            m_coordinates_dirty = false;
        }

        if( lines )
        {
            m_lines.resize( 3 );
            m_lines[ 0 ] = Line( m_coordinates[ 0 ], m_coordinates[ 1 ] );
            m_lines[ 1 ] = Line( m_coordinates[ 1 ], m_coordinates[ 2 ] );
            m_lines[ 2 ] = Line( m_coordinates[ 2 ], m_coordinates[ 0 ] );
            m_lines_dirty = false;
        }

        if( triangles )
        {
            m_triangles = { *this };
            m_triangles_indices = { { 0, 1, 2 } };
            m_triangles_dirty = false;
        }

        if( convex_partitions )
        {
            m_convex_partitions = { *this };
            m_convex_partitions_indices = { { 0, 1, 2 } };
            m_convex_partitions_dirty = false;
        }
    }
    else
    {
        // default to true then check for violations
        bool convex_ccw = true;
        bool convex_cw = true;

        if( lines )
        {
            m_lines.resize( coordinate_count );
        }

        if( triangles )
        {
            // hope for convex, recalculate if not
            m_triangles.resize( m_coordinates.size( ) - 2 );
            m_triangles_indices.resize( m_coordinates.size( ) - 2 );
        }

        // iterate through coordinates and determine xy boundaries, convex, and clockwise see
        // https://stackoverflow.com/questions/1165647/how-to-determine-if-a-list-of-polygon-points-are-in-clockwise-order

        int bottom_index = 0; // for convex hull
        double edge_curve = 0;
        for_range( coordinate_count )
        {
            Coordinate & coordinate = m_coordinates[ i ];

            if( is_infinity( coordinate.x( ) ) || is_infinity( coordinate.y( ) ) )
                Assert( "no infinities allowed (for now)" );

            if( transform )
            {
                coordinate = m_transform.transform( coordinate );
            }

            if( m_coordinates_dirty )
            {
                m_lower_bound_x = min( m_lower_bound_x, coordinate.x( ) );
                m_lower_bound_y = min( m_lower_bound_y, coordinate.y( ) );
                m_upper_bound_x = max( m_upper_bound_x, coordinate.x( ) );
                m_upper_bound_y = max( m_upper_bound_y, coordinate.y( ) );

                Coordinate prev_coordinate, prev_prev_coordinate;
                if( i > 1 )
                {
                    prev_coordinate = m_coordinates[ i - 1 ];
                    prev_prev_coordinate = m_coordinates[ i - 2 ];
                }
                else if( i == 1 )
                {
                    prev_coordinate = m_coordinates[ 0 ];
                    prev_prev_coordinate = m_coordinates[ coordinate_count - 1 ];

                    if( transform )
                    {
                        prev_prev_coordinate = m_transform.transform( prev_prev_coordinate );
                    }
                }
                else
                { // i == 0
                    prev_coordinate = m_coordinates[ coordinate_count - 1 ];
                    prev_prev_coordinate = m_coordinates[ coordinate_count - 2 ];

                    if( transform )
                    {
                        prev_coordinate = m_transform.transform( prev_coordinate );
                        prev_prev_coordinate = m_transform.transform( prev_prev_coordinate );
                    }
                }

                edge_curve += (double)( ( coordinate.x( ) - prev_coordinate.x( ) ) * ( coordinate.y( ) + prev_coordinate.y( ) ) );
            }

            if( lines )
            {
                if( i )
                {
                    m_lines[ i - 1 ] = Line( m_coordinates[ i - 1 ], m_coordinates[ i ] );
                }
                else
                { // i = 0
                    m_lines[ m_lines.size( ) - 1 ] = Line( m_transform.transform( m_coordinates[ m_coordinates.size( ) - 1 ] ), m_coordinates[ 0 ] );
                }
            }

            if( triangles && ( i >= 2 ) )
            {
                m_triangles[ i - 2 ] = Shape( { m_coordinates[ 0 ], m_coordinates[ i - 1 ], m_coordinates[ i ] } );
                m_triangles_indices[ i - 2 ] = { 0, i - 1, i };
            }

            if( convex_hull )
            {
                if( ( coordinate.y( ) < m_coordinates[ bottom_index ].y( ) ) || ( coordinate.y( ) == m_coordinates[ bottom_index ].y( ) && ( coordinate.x( ) < m_coordinates[ bottom_index ].x( ) ) ) )
                {
                    bottom_index = i;
                }
            }
        }

        if( m_coordinates_dirty )
        {
            m_clockwise = edge_curve > 0;

            if( m_clockwise )
            {
                m_convex = convex_cw;

                m_coordinates.reverse( );

                if( lines )
                    m_lines.reverse( );

                if( triangles && m_convex )
                {
                    for_range( m_triangles.size( ) )
                    {
                        varray<Coordinate> coordinates = m_triangles[ i ].coordinates( );
                        m_triangles[ i ] = Shape( { coordinates[ 2 ], coordinates[ 1 ], coordinates[ 0 ] } );
                        m_triangles_indices[ i ].reverse( );
                    }
                }
            }
            else
            {
                m_convex = convex_ccw;
            }

            m_coordinates_dirty = false;
        }

        // if( triangles )
        //{
        //     if( !m_convex )
        //     { // else already set in for loop above
        //         int index = 0;
        //         varray<Coordinate> coordinates = m_coordinates;
        //
        //         m_triangles.clear( );
        //         m_triangles.reserve( coordinates.size( ) - 2 );
        //
        //         int size = coordinates.size( );
        //
        //         int cycle = 0;
        //         int max_cycles = ( size * size ) / 2;
        //         bool success = true; // todo this shouldn't be necessary
        //
        //         while( coordinates.size( ) > 3 )
        //         {
        //             ++cycle;
        //             if( cycle >= max_cycles )
        //             {
        //                 success = false;
        //                 break;
        //             }
        //
        //             Shape triangle;
        //             bool is_ear = false;
        //             Coordinate coordinate = coordinates[ index % coordinates.size( ) ];
        //             Coordinate next_coordinate = coordinates[ ( index + 1 ) % coordinates.size( ) ];
        //             Angle next_angle( coordinate, next_coordinate );
        //
        // #if 0
        //             // check if coordinate is colinear and thus can be removed
        //             Coordinate prev_coordinate = coordinates[ ( index + coordinates.size( ) - 1 ) % coordinates.size( ) ];
        //             Angle angle_prev( coordinate, prev_coordinate );
        //             if ( ( next_angle == angle_prev ) || ( next_angle - angle_prev == PI ) ) {
        //                 coordinates.remove( index );
        //                 continue;
        //             }
        // #endif
        //
        //             Coordinate next_next_coordinate = coordinates[ ( index + 2 ) % coordinates.size( ) ];
        //             Angle next_next_angle( coordinate, next_next_coordinate );
        //
        //             // check if next two points are colinear and if so skip so
        //             // it will be removed in the next iteration
        //             if( !( ( next_angle == next_next_angle ) || ( ( next_angle - next_next_angle ) == Angle( PI ) ) ) )
        //             {
        //                 triangle = Shape( { coordinate, next_coordinate, next_next_coordinate } );
        //                 if( ( next_next_angle - next_angle ) < Angle( PI ) )
        //                 {
        //                     is_ear = true;
        //                     for_range( coordinates.size( ) )
        //                     {
        //                         if( ( i == index ) || ( i == ( index + 1 ) % coordinates.size( ) ) || ( i == ( index + 2 ) % coordinates.size( ) ) )
        //                             continue;
        //                         if( triangle.contains( coordinates[ i ] ) )
        //                         {
        //                             is_ear = false;
        //                             break;
        //                         }
        //                     }
        //                 }
        //             }
        //
        //             if( is_ear )
        //             {
        //                 // check and add to triangle list
        //                 // Assert( triangle.area( ), "triangle must have some
        //                 // area or else something is wrong" );
        //                 m_triangles.insert_back( triangle );
        //                 coordinates.remove( ( index + 1 ) % coordinates.size( ) );
        //             }
        //             else
        //             {
        //                 // move index over to continue search
        //                 ++index;
        //                 index %= coordinates.size( );
        //             }
        //         }
        //
        //         if( success )
        //         {
        //             Assert( coordinates.size( ) == 3, "all triangularized Triangles should have 3 "
        //                                               "points, obviously" );
        //             Shape triangle = Shape( { coordinates[ 0 ], coordinates[ 1 ], coordinates[ 2 ] } );
        //             if( triangle.area( ) )
        //                 m_triangles.insert_back( triangle );
        //         }
        //     }
        //
        //     m_triangles_dirty = false;
        // }

        if( convex_partitions )
        {
            if( m_convex )
            {
                m_convex_partitions = { *this };
                m_convex_partitions_indices = varray<varray<uint>>( 1 );
                for_range( 1 )
                {
                    uint side_count = m_convex_partitions[ i ].sides( );
                    m_convex_partitions_indices[ i ].resize( side_count );
                    for_range_x( j, side_count ) { m_convex_partitions_indices[ i ][ j ] = j; }
                }
            }
            else
            {
                // todo technically this is true but there's definitely a better way
                m_convex_partitions = m_triangles;
                m_convex_partitions_indices = m_triangles_indices;
            }

            m_convex_partitions_dirty = false;
        }

        if( convex_hull )
        {
            if( m_convex )
                m_convex_hull = m_coordinates;
            else
            {
                varray<Coordinate> convex_hull_coordinates;
                convex_hull_coordinates.reserve( sides( ) );
                convex_hull_coordinates.insert_back( m_coordinates[ bottom_index ] );

                int index_offset = bottom_index;
                for_range( coordinate_count - 1 )
                {
                    int index = (int)( i + index_offset + 1 ) % coordinate_count;
                    Coordinate coordinate = m_coordinates[ index ];

                    while( convex_hull_coordinates.size( ) > 1 )
                    {
                        Coordinate prev_coordinate = convex_hull_coordinates[ convex_hull_coordinates.size( ) - 1 ];
                        Coordinate prev_prev_coordinate = convex_hull_coordinates[ convex_hull_coordinates.size( ) - 2 ];
                        Angle angle( prev_coordinate, coordinate );
                        Angle prev_angle( prev_prev_coordinate, prev_coordinate );

                        if( ( angle - prev_angle ) < Angle( PI ) )
                            break;
                        else
                            convex_hull_coordinates.remove_back( );
                    }
                    convex_hull_coordinates.insert_back( coordinate );
                }
                m_convex_hull = convex_hull_coordinates;
            }

            // Assert( Shape( m_convex_hull ).convex( ), "convex hull must be convex...obviously" );
            m_convex_hull_dirty = false;
        }
    }

    if( m_coordinates_dirty )
        m_coordinates_dirty = false;

    if( transform )
        m_transform = IdentityTransform( );
}

Shape & Shape::transform( const Transform & _transform )
{
    dirty( );
    m_transform.chain( _transform );
    return *this;
}

Shape & Shape::move( const Vector & _vector )
{
    dirty( );
    m_transform.chain( MoveTransform( _vector ) );
    return *this;
}

Shape & Shape::stretch( const Vector & _axis )
{
    dirty( );
    m_transform.chain( StretchTransform( _axis ) );
    return *this;
}

Shape & Shape::scale( const double _scale, const Coordinate & _origin )
{
    dirty( );
    m_transform.chain( ScaleTransform( _scale, _origin ) );
    return *this;
}

Shape & Shape::rotate( const Angle & _angle, const Coordinate & _origin )
{
    dirty( );
    m_transform.chain( RotateTransform( _angle, _origin ) );
    return *this;
}

Shape & Shape::mirror( const Vector & _axis )
{
    dirty( );
    m_transform.chain( ReflectionTransform( _axis ) );
    return *this;
}

Planc Shape::area( ) const
{
    process( true, false, true, false, false );

    Planc area = 0.0;
    if( convex( ) )
    {
        area += ( ( m_coordinates.back( ).x( ) * m_coordinates.front( ).y( ) ) - ( m_coordinates.back( ).y( ) * m_coordinates.front( ).x( ) ) );

        for_range( m_coordinates.size( ) - 2 )
        {
            const Coordinate & c1 = m_coordinates[ i ];
            const Coordinate & c2 = m_coordinates[ i + 1 ];
            area += ( ( c1.x( ) * c2.y( ) ) - ( c1.y( ) * c2.x( ) ) );
        }

        area = abs( half( area ) );
    }
    else
    {
        for_each( convex, m_convex_partitions ) { area += convex.area( ); }
    }
    return area;
}

uint Shape::sides( ) const
{
    return m_coordinates.size( );
}

bool Shape::convex( ) const
{
    process( false, false, false, false, false );
    return m_convex;
}

Planc Shape::lower_bound_x( ) const
{
    process( true, false, false, false, false );
    return m_lower_bound_x;
}

Planc Shape::lower_bound_y( ) const
{
    process( true, false, false, false, false );
    return m_lower_bound_y;
}

Planc Shape::upper_bound_x( ) const
{
    process( true, false, false, false, false );
    return m_upper_bound_x;
}

Planc Shape::upper_bound_y( ) const
{
    process( true, false, false, false, false );
    return m_upper_bound_y;
}

bool Shape::contains( const Coordinate & _c ) const
{
    process( true, true, false, false, false );

    if( convex( ) )
    {
        for_each( line, lines( ) )
        {
            if( !line.below( _c, true ) )
            {
                return false;
            }
        }

        return true;
    }
    else
    {
        for_each( convex_shape, convex_partitions( ) )
        {
            if( convex_shape.contains( _c ) )
            {
                return true;
            }
        }

        return false;
    }
}

varray<Line> Shape::intersection( const Line & _line ) const
{
    varray<Line> intersection = { };

    if( contains( _line.c1( ) ) )
    {
        intersection.insert_back( _line.c1( ) );
    }

    if( contains( _line.c2( ) ) )
    {
        intersection.insert_back( _line.c2( ) );
    }

    // find all intersection points
    varray<Coordinate> intersections = { };
    for_each( line, lines( ) )
    {
        if( line.intersects( _line ) )
        {
            intersections.insert_back( Coordinate( line.intersection( _line ) ) );
        }
    }

    // sort intersections by distance from start of line
    struct
    {
        Coordinate origin;
        inline bool operator( )( const Coordinate & c1, const Coordinate & c2 ) { return ( c1.distance( origin ) < c2.distance( origin ) ); }
    } line_intersection_distance;
    line_intersection_distance.origin = _line.c1( );
    intersections.sort( line_intersection_distance );

    return intersection;
}

const varray<Coordinate> & Shape::coordinates( ) const
{
    process( true, false, false, false, false );
    return m_coordinates;
}

const varray<Coordinate> & Shape::coordinates_raw( ) const
{
    return m_coordinates;
}

const varray<Line> & Shape::lines( ) const
{
    process( true, true, false, false, false );
    return m_lines;
}

const varray<Shape> & Shape::triangles( ) const
{
    process( true, false, true, false, false );
    return m_triangles;
}

const varray<Shape> & Shape::convex_partitions( ) const
{
    process( true, false, false, true, false );
    return m_convex_partitions;
}

Shape Shape::convex_hull( ) const
{
    process( true, false, false, false, true );
    return Shape( m_convex_hull );
}

Shape Shape::operator+( const Vector & _v ) const { return Shape( *this ).move( _v ); }

Shape & Shape::operator+=( const Vector & _v ) { return move( _v ); }

Shape Shape::operator-( const Vector & _v ) const { return *this + -_v; }

Shape & Shape::operator-=( const Vector & _v ) { return *this += -_v; }

bool Shape::operator==( const Shape & _shape ) const
{
    if( m_coordinates.size( ) != _shape.m_coordinates.size( ) )
        return false;

    uint coordinate_count = m_coordinates.size( );

    const varray<Coordinate> & c1 = coordinates( );
    const varray<Coordinate> & c2 = _shape.coordinates( );

    bool matched = false;
    int offset = 0;

    for_range( coordinate_count )
    {
        ++offset;
        if( c1[ offset ] == c2[ 0 ] )
        {
            matched = true;
            break;
        }
    }

    if( !matched )
        return false;

    for_range( coordinate_count )
    {
        if( c1[ ( i + offset ) % coordinate_count ] != c2[ i ] )
            return false;
    }

    return true;
}

FixedRectangle::FixedRectangle( ) : m_width( 0.0 ), m_height( 0.0 ), m_center( ORIGIN ) { }

FixedRectangle::FixedRectangle( const Planc & _width, const Planc & _height, const Coordinate & _center )
    : m_center( _center ), m_width( _width ), m_height( _height )
{
}

FixedRectangle::FixedRectangle( const Coordinate & _bottom, const Coordinate & _top )
    : m_center( Vector( _bottom, _top ).half( ).destination( ) ), m_width( _top.x( ) - _bottom.x( ) ), m_height( _top.y( ) - _bottom.y( ) )
{
}

const Coordinate & FixedRectangle::center( ) const { return m_center; }
FixedRectangle & FixedRectangle::center( const Coordinate & _center )
{
    m_center = _center;
    return *this;
}

const Planc & FixedRectangle::width( ) const { return m_width; }
FixedRectangle & FixedRectangle::width( const Planc & _width )
{
    m_width = _width;
    return *this;
}

const Planc & FixedRectangle::height( ) const { return m_height; }
FixedRectangle & FixedRectangle::height( const Planc & _height )
{
    m_height = _height;
    return *this;
}

Coordinate FixedRectangle::top_left( ) const
{
    return center( ) + half( Vector( -width( ), height( ) ) );
}

Coordinate FixedRectangle::top_right( ) const
{
    return center( ) + half( Vector( width( ), height( ) ) );
}

Coordinate FixedRectangle::bottom_left( ) const
{
    return center( ) + half( Vector( -width( ), -height( ) ) );
}

Coordinate FixedRectangle::bottom_right( ) const
{
    return center( ) + half( Vector( width( ), -height( ) ) );
}

Planc FixedRectangle::upper_bound_x( ) const
{
    return center( ).x( ) + width( ).half( );
}

Planc FixedRectangle::upper_bound_y( ) const
{
    return center( ).y( ) + height( ).half( );
}

Planc FixedRectangle::lower_bound_x( ) const
{
    return center( ).x( ) - width( ).half( );
}

Planc FixedRectangle::lower_bound_y( ) const
{
    return center( ).y( ) + height( ).half( );
}

bool FixedRectangle::contains( const Coordinate & _c ) const
{
    return ( in_range( _c.x( ), lower_bound_x( ), upper_bound_x( ), true ) && in_range( _c.y( ), lower_bound_y( ), upper_bound_y( ), true ) );
}

varray<Line> FixedRectangle::intersection( const Line & _line ) const
{
    return ( ( Shape ) * this ).intersection( _line );
}

Planc FixedRectangle::area( ) const
{
    return width( ) * height( );
}

FixedRectangle FixedRectangle::operator+( const Vector & _v ) const
{
    return FixedRectangle( width( ), height( ), center( ) + _v );
}

FixedRectangle & FixedRectangle::operator+=( const Vector & _v )
{
    return center( center( ) + _v );
}

FixedRectangle FixedRectangle::operator-( const Vector & _v ) const
{
    return FixedRectangle( width( ), height( ), center( ) - _v );
}

FixedRectangle & FixedRectangle::operator-=( const Vector & _v )
{
    return center( center( ) - _v );
}

bool FixedRectangle::operator==( const FixedRectangle & _rect ) const
{
    return ( ( center( ) == _rect.center( ) ) &&
             ( width( ) == _rect.width( ) ) &&
             ( height( ) == _rect.height( ) ) );
}

FixedRectangle::operator Shape( ) const
{
    return Shape( { top_right( ), top_left( ), bottom_left( ), bottom_right( ) } );
}
