#include "Path.hpp"

#include "Polygon.hpp"

Path::Path( const varray<Coordinate> & _coordinates )
{
    if( _coordinates.size( ) == ONE )
    {
        m_lines = { Line( _coordinates.front( ), _coordinates.front( ) ) };
    }
    else if( _coordinates.size( ) > ONE )
    {
        for_range( i, _coordinates.size( ) - ONE )
        {
            m_lines.insert_back( Line( _coordinates[ i ], _coordinates[ i + 1 ] ) );
        }
    }
}

Path::Path( const varray<Line> & _lines )
{
    m_lines = _lines;
    
    for_range( i, m_lines.size( ) - ONE )
    {
        Assert( m_lines[ i ].c2( ) == m_lines[ i + ONE ].c1( ) );
    }
}

Path & Path::transform( const Transform & _transform )
{
    Transformable::transform( _transform );
    for_each( line, m_lines )
    {
        line.transform( _transform );
    }
    return *this;
}

varray<Coordinate> Path::points( ) const
{
    if( !line_count( ) )
    {
        return { };
    }
    
    varray<Coordinate> points;
    for_each( line, m_lines )
    {
        points.insert_back( line.c1( ) );
    }
    points.insert_back( m_lines.back( ).c2( ) );
    
    return points;
}

Planc Path::length( ) const
{
    Planc length = 0;
    for_each( line, m_lines )
    {
        length += line.length( );
    }
    return length;
}

bool Path::has_length( ) const
{
    for_each( line, m_lines )
    {
        if( line.length( ) )
        {
            return true;
        }
    }
    return false;
}

bool Path::on( const Coordinate & _coordinate ) const
{
    for_each( line, m_lines )
    {
        if( line.on( _coordinate ) )
        {
            return true;
        }
    }
    return false;
}

bool Path::above( const Coordinate & _coordinate, bool _inclusive ) const
{
    for_each( line, m_lines )
    {
        if( line.above( _coordinate, _inclusive ) )
        {
            return true;
        }
    }
    return false;
}

bool Path::below( const Coordinate & _coordinate, bool _inclusive ) const
{
    for_each( line, m_lines )
    {
        if( line.below( _coordinate, _inclusive ) )
        {
            return true;
        }
    }
    return false;
}

bool Path::in_box( const Coordinate & _coordinate, bool _inclusive ) const
{
    Planc min_x = INFINITY_POSITIVE;
    Planc max_x = INFINITY_NEGATIVE;
    Planc min_y = INFINITY_POSITIVE;
    Planc max_y = INFINITY_NEGATIVE;
    
    if( m_lines.size( ) )
    {
        min_x = min<Planc>( min_x, m_lines.front( ).c1( ).x( ) );
        max_x = max<Planc>( max_x, m_lines.front( ).c1( ).x( ) );
        min_y = min<Planc>( min_y, m_lines.front( ).c1( ).y( ) );
        max_y = max<Planc>( max_y, m_lines.front( ).c1( ).y( ) );
        
        for_each( line, m_lines )
        {
            min_x = min<Planc>( min_x, line.c2( ).x( ) );
            max_x = max<Planc>( max_x, line.c2( ).x( ) );
            min_y = min<Planc>( min_y, line.c2( ).y( ) );
            max_y = max<Planc>( max_y, line.c2( ).y( ) );
        }
        
        return ( in_range( _coordinate.x( ), min_x, max_x, _inclusive ) ) &&
               ( in_range( _coordinate.y( ), min_y, max_y, _inclusive ) );
    }
    
    return false;
}

bool Path::intersects( const Line & _line, bool _inclusive ) const
{
    for_each( line, m_lines )
    {
        if( line.intersects( _line, _inclusive ) )
        {
            return true;
        }
    }
    return false;
}

bool Path::intersects_self( ) const
{
    for_range( i, line_count( ) - 1 )
    {
        for_range( j, line_count( ) - i )
        {
            if( m_lines[ i ].intersects( m_lines[ i + j + 1 ] ) )
            {
                return true;
            }
        }
    }
    return false;
}

varray<Line> Arc::generate( const Coordinate & _center, const Planc & _radius, const Angle & _start_angle, const Angle & _end_angle, const bool _clockwise )
{
    if( _start_angle == _end_angle )
    {
        return { };
    }
    
    varray<Line> lines;
    
    Angle d_angle;
    
    Angle start_angle = _start_angle;
    Angle end_angle = _end_angle;
    
    start_angle.truncate( true );
    end_angle.truncate( true );
    
    if( _clockwise )
    {
        if( start_angle < end_angle )
        {
            d_angle = start_angle + ( TAU - end_angle );
        }
        else
        {
            d_angle = start_angle - end_angle;
        }
    }
    else
    {
        if( start_angle > end_angle )
        {
            d_angle = end_angle + ( TAU - start_angle );
        }
        else
        {
            d_angle = end_angle - start_angle;
        }
    }
    
    uint line_count = max( ONE, ceil( ( d_angle.radians( ) / TAU ) * (dec)Polygon::circle_precision( _radius ) ) );
    Angle dd_angle = d_angle / (dec)line_count;
    
    Coordinate start = _center + VectorA( start_angle, _radius );
    
    for_range( i, line_count - ONE )
    {
        Coordinate c = _center + VectorA( negative( start_angle + ( dd_angle * (dec)i ), _clockwise ), _radius );
        lines.insert_back( Line( start, c ) );
        start = c;
    }
    
    Coordinate end = _center + VectorA( end_angle, _radius );
    lines.insert_back( Line( start, end ) );
    
    return lines;
}

Arc::Arc( const Coordinate & _center, const Planc & _radius, const Angle & _start, const Angle & _end, const bool _clockwise )
: Path( generate( _center, _radius, _start, _end, _clockwise ) ) { }

varray<Line> Beziel::generate( const varray<Coordinate> & _control_points, const Planc & _min_distance )
{
    varray<Line> lines;
 
    return lines;
}

Beziel::Beziel( const varray<Coordinate> & _control_points, const Planc & _min_distance )
: Path( generate( _control_points, _min_distance ) ) { }
