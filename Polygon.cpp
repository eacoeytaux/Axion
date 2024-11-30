#include "Polygon.hpp"

namespace
{
const uint CIRCLE_PRECISION = 60;
}

uint Polygon::circle_precision( const dec _perimeter )
{
    return CIRCLE_PRECISION;
}

Polygon::Polygon( const varray<Coordinate> & _coordinates, const Transform & _transform )
{
    process( _coordinates );
    transform( _transform );
}

Triangle::Triangle( const Coordinate & c1, const Coordinate & c2, const Coordinate & c3 )
    : Polygon( { c1, c2, c3 } ) { }

Rectangle::Rectangle( const Planc & _width, const Planc & _height, const Coordinate & _center, const Angle _rotation )
    : Polygon( { _center + Vector( _width, _height ).half( ).rotate( _rotation ),
                 _center + Vector( -_width, _height ).half( ).rotate( _rotation ),
                 _center + Vector( -_width, -_height ).half( ).rotate( _rotation ),
                 _center + Vector( _width, -_height ).half( ).rotate( _rotation ) },
               true ) { }

Square::Square( const Planc & _width, const Coordinate & _center, const Angle _rotation )
    : Polygon( { _center + Vector( _width, _width ).half( ).rotate( _rotation ),
                 _center + Vector( -_width, _width ).half( ).rotate( _rotation ),
                 _center + Vector( -_width, -_width ).half( ).rotate( _rotation ),
                 _center + Vector( _width, -_width ).half( ).rotate( _rotation ) },
               true ) { }

Circle::Circle( const Planc & _radius, const Coordinate & _center )
    : Polygon( Polygon::equilateral( Polygon::circle_precision( _radius ) ).coordinates_raw( ), ScaleTransform( _radius ).chain( MoveTransform( _center ) ) ) { }

Polygon Polygon::equilateral( const uint _side_count, const Planc & _radius, const Coordinate & _center, const Angle _rotation )
{
    static umap<uint, Polygon> equilaterals;

    if( !equilaterals.contains( _side_count ) )
    {
        varray<Coordinate> coordinates( _side_count );

        Angle start_angle = Angle( RIGHT_ANGLE, true ); // start at the top + rotation
        Angle delta_angle( TAU / (dec)_side_count );
        for_each( offset, coordinates )
        {
            offset = VectorA( start_angle );
            start_angle += delta_angle;
        }

        equilaterals[ _side_count ] = Polygon( coordinates );
    }

    Polygon equilateral = equilaterals[ _side_count ];
    equilateral.scale( _radius );
    equilateral.rotate( _rotation );
    equilateral.move( _center );
    
    return equilateral;
}

Polygon Polygon::expand( const Polygon & _polygon, const Planc & _expansion )
{
    if( !_expansion )
    {
        return Polygon( _polygon );
    }

    const varray<Coordinate> _coordinates = _polygon.coordinates( );
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

        vector_prev.magnitude( _expansion );
        vector_next.magnitude( _expansion );

        line_prev += vector_prev;
        line_next += vector_next;

        new_coordinates[ i ] = line_prev.intersection( line_next );
    }

    return Polygon( new_coordinates );
}

const Polygon & Polygon::dirty( ) const
{
    Transformable::dirty( );
    
    m_coordinates_dirty = true;
    
    return *this;
}

void Polygon::process( const varray<Coordinate> & _coordinates )
{
    if( m_processed )
    {
        return;
    }
    
    m_coordinates = _coordinates;
    uint coordinate_count = m_coordinates.size( );
    
    m_convex = true;
    
    if( coordinate_count )
    {
        m_lower_bound_x = INFINITY_POSITIVE;
        m_lower_bound_y = INFINITY_POSITIVE;
        m_upper_bound_x = INFINITY_NEGATIVE;
        m_upper_bound_y = INFINITY_NEGATIVE;
        
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
                Coordinate & coordinate = m_coordinates[ i ];
                
                // TODO
                // Assert( !is_infinity( coordinate.x( ) ) && !is_infinity( coordinate.y( ) ), "no infinities allowed (for now)" );
                
                m_lower_bound_x = min( m_lower_bound_x, coordinate.x( ) );
                m_lower_bound_y = min( m_lower_bound_y, coordinate.y( ) );
                m_upper_bound_x = max( m_upper_bound_x, coordinate.x( ) );
                m_upper_bound_y = max( m_upper_bound_y, coordinate.y( ) );
                
                Coordinate & prev_coordinate = m_coordinates[ ( i + coordinate_count - 1 ) % coordinate_count ];
                Coordinate & prev_prev_coordinate = m_coordinates[ ( i + coordinate_count - 2 ) % coordinate_count ];
                
                edge_curve += (dec)( ( coordinate.x( ) - prev_coordinate.x( ) ) * ( coordinate.y( ) + prev_coordinate.y( ) ) );
                
                if( convex_cw || convex_ccw )
                {
                    if( i > 2 )
                    {
                        if( !Line( prev_prev_coordinate, prev_coordinate ).above( coordinate ) )
                        {
                            convex_cw = false;
                        }
                        if( Line( prev_prev_coordinate, prev_coordinate ).above( coordinate ) )
                        {
                            convex_ccw = false;
                        }
                    }
                }
            }
            
            bool clockwise = ( edge_curve > 0 );
            
            if( clockwise )
            {
                m_coordinates.reverse( );
            }
            
            m_convex = clockwise ? convex_cw : convex_ccw;
        }
    }

    m_coordinates_raw = m_coordinates;
    
    m_processed = true;
}

void Polygon::apply_transform( ) const
{
    const Transform & t = cumulative_transform( );
    if( !t.identity( ) )
    {
        if( m_coordinates_dirty )
        {
            m_lower_bound_x = INFINITY_POSITIVE;
            m_lower_bound_y = INFINITY_POSITIVE;
            m_upper_bound_x = INFINITY_NEGATIVE;
            m_upper_bound_y = INFINITY_NEGATIVE;
            
            for_range( i, m_coordinates.size( ) )
            {
                m_coordinates[ i ] = t.transform( m_coordinates_raw[ i ] );
                
                m_lower_bound_x = min( m_lower_bound_x, m_coordinates[ i ].x( ) );
                m_lower_bound_y = min( m_lower_bound_y, m_coordinates[ i ].y( ) );
                m_upper_bound_x = max( m_upper_bound_x, m_coordinates[ i ].x( ) );
                m_upper_bound_y = max( m_upper_bound_y, m_coordinates[ i ].y( ) );
            }
            
            m_coordinates_dirty = false;
        }
    }
}

Planc Polygon::area( ) const
{
    Planc area = ZERO;

    if( convex( ) )
    {
        const varray<Coordinate> & c = coordinates( );
        
        area += ( ( c.back( ).x( ) * c.front( ).y( ) ) - ( c.back( ).y( ) * c.front( ).x( ) ) );

        for_range( i, c.size( ) - 2 )
        {
            const Coordinate & c1 = c[ i ];
            const Coordinate & c2 = c[ i + 1 ];
            area += ( ( c1.x( ) * c2.y( ) ) - ( c1.y( ) * c2.x( ) ) );
        }

        area = abs( half( area ) );
    }
    else
    {
        for_each( convex, convex_partitions( ) ) { area += convex.area( ); }
    }

    return area;
}

uint Polygon::sides( ) const
{
    return m_coordinates.size( );
}

bool Polygon::convex( ) const
{
    return m_convex;
}

Planc Polygon::lower_bound_x( ) const
{
    apply_transform( );
    return m_lower_bound_x;
}

Planc Polygon::lower_bound_y( ) const
{
    apply_transform( );
    return m_lower_bound_y;
}

Planc Polygon::upper_bound_x( ) const
{
    apply_transform( );
    return m_upper_bound_x;
}

Planc Polygon::upper_bound_y( ) const
{
    apply_transform( );
    return m_upper_bound_y;
}

bool Polygon::contains( const Coordinate & _coordinate, const bool _inclusive ) const
{
    if( convex( ) )
    {
        const varray<Line> & lines = Polygon::perimeter( );
        for_each( line, lines )
        {
            if( !line.below( _coordinate, _inclusive ) )
            {
                return false;
            }
        }

        return true;
    }
    else
    {
        for_each( convex_polygon, convex_partitions( ) )
        {
            if( convex_polygon.contains( _coordinate, true ) )
            {
                if( !_inclusive )
                {
                    for_each( line, perimeter( ).lines( ) )
                    {
                        if( line.on( _coordinate ) )
                        {
                            return false;
                        }
                    }
                    
                    return true;
                }
                else
                {
                    return true;
                }
            }
        }
        
        return false;
    }
}

varray<Line> Polygon::intersection( const Line & _line ) const
{
    varray<Line> intersection = { };
    varray<Coordinate> intersection_coordinates = { };

    if( contains( _line.c1( ) ) )
    {
        intersection_coordinates.insert_back( _line.c1( ) );
    }

    if( contains( _line.c2( ) ) )
    {
        intersection_coordinates.insert_back( _line.c2( ) );
    }

    const varray<Line> & lines = Polygon::perimeter( );
    for_each( line, lines )
    {
        if( line.intersects( _line ) )
        {
            intersection_coordinates.insert_back( Coordinate( line.intersection( _line ) ) );
        }
    }

    // sort intersections by distance from start of line
    struct
    {
        Coordinate origin;
        bool operator( )( const Coordinate & c1, const Coordinate & c2 ) { return ( c1.distance_to( origin ) < c2.distance_to( origin ) ); }
    } line_intersection_distance;
    
    line_intersection_distance.origin = _line.c1( );
    intersection_coordinates.sort( line_intersection_distance );

    // Assert( ( intersection_coordinates.size( ) % 2 ) == 0 );
    for_range( i, intersection_coordinates.size( ) / 2 )
    {
        uint i1 = i * 2;
        uint i2 = i1 + 1;
        intersection.insert_back( Line( intersection_coordinates[ i1 ], intersection_coordinates[ i2 ] ) );
    }

    return intersection;
}

const varray<Coordinate> & Polygon::coordinates( ) const
{
    apply_transform( );
    return m_coordinates;
}

Path Polygon::perimeter( ) const
{
    const varray<Coordinate> & c = coordinates( );
    const uint count = c.size( );

    if( count >= 3 )
    {
        varray<Line> lines( c.size( ) );
        if( c.size( ) )
        {
            for_range( i, c.size( ) - 1 ) lines[ i ] = Line( c[ i ], c[ i + 1 ] );
            lines[ c.size( ) - 1 ] = Line( c[ c.size( ) - 1 ], c[ 0 ] );
        }
        return lines;
    }
    else if( count == 2 )
    {
        return varray<Line>( { Line( c[ 0 ], c[ 1 ] ) } );
    }

    return varray<Line>( );
}

varray<Polygon> Polygon::triangles( ) const
{
    if( sides( ) == 3 )
    {
        return { *this };
    }
    else
    {
        apply_transform( );
        
        varray<Polygon> t;
        
        if( convex( ) )
        {
            for_range( i, m_coordinates.size( ) - THREE )
            {
                t.insert_back( Polygon( { m_coordinates[ 0 ], m_coordinates[ i + ONE ], m_coordinates[ i + TWO ] } ) );
            }
        }
        else
        {
            uint i = ZERO;
            varray<Coordinate> temp_coordinates = m_coordinates;
            
            while( temp_coordinates.size( ) > THREE )
            {
                i %= temp_coordinates.size( );
                uint i_prev = ( i + ( temp_coordinates.size( ) - 1 ) ) % temp_coordinates.size( );
                uint i_next = ( i + 1 ) % temp_coordinates.size( );
                
                Coordinate c = temp_coordinates[ i ];
                
                Coordinate c_prev = temp_coordinates[ i_prev ];
                Coordinate c_next = temp_coordinates[ i_next ];
                
                bool is_ear = true;
                Triangle ear( c, c_next, c_prev );
                
                for_range( j, temp_coordinates.size( ) )
                {
                    if( ( j != i ) && ( j != i_prev ) && ( j != i_next ) )
                    {
                        if( ear.contains( temp_coordinates[ j ], false ) )
                        {
                            is_ear = false;
                            break;
                        }
                    }
                }
                
                if( is_ear )
                {
                    temp_coordinates.erase( i );
                    t.insert_back( ear );
                }
                else
                {
                    ++i;
                }
            }
            
            t.insert_back( temp_coordinates );
        }
        
        return t;
    }
}

varray<Polygon> Polygon::convex_partitions( ) const
{
    if( convex( ) )
    {
        return { *this };
    }
    else
    {
        return triangles( );
    }
}

Polygon Polygon::operator+( const Vector & _v ) const { return Polygon( *this ).move( _v ); }

Polygon & Polygon::operator+=( const Vector & _v ) { return move( _v ); }

Polygon Polygon::operator-( const Vector & _v ) const { return *this + -_v; }

Polygon & Polygon::operator-=( const Vector & _v ) { return *this += -_v; }

bool Polygon::operator==( const Polygon & _polygon ) const
{
    if( m_coordinates.size( ) != _polygon.m_coordinates.size( ) )
    {
        return false;
    }

    uint coordinate_count = m_coordinates.size( );

    const varray<Coordinate> & _c1 = coordinates( );
    const varray<Coordinate> & _c2 = _polygon.coordinates( );

    bool matched = false;
    int offset = 0;

    for_range( i, coordinate_count )
    {
        ++offset;
        if( _c1[ offset ] == _c2[ 0 ] )
        {
            matched = true;
            break;
        }
    }

    if( !matched )
    {
        return false;
    }

    for_range( i, coordinate_count )
    {
        if( _c1[ ( i + offset ) % coordinate_count ] != _c2[ i ] )
        {
            return false;
        }
    }

    return true;
}
