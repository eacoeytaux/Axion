#include "Transform.hpp"

MoveTransform::MoveTransform( const Vector & _translation ) : Transform( true )
{
	set( 0, 2, _translation.dx( ) );
	set( 1, 2, _translation.dy( ) );
}

RotateTransform::RotateTransform( const Angle & _angle, const Coordinate & _origin ) : Transform( true )
{
	set( 0, 0, _angle.cos( ) );
	set( 0, 1, -_angle.sin( ) );
	set( 1, 0, _angle.sin( ) );
	set( 1, 1, _angle.cos( ) );

	if( _origin != ORIGIN )
	{
		Transform mt = MoveTransform( Vector( _origin ) );
		set( mt * ( *this ) * mt.inverse( ) );
	}
}

ReflectionTransform::ReflectionTransform( const Vector & _axis ) : Transform( true )
{
	set( 1, 1, -1.0 );

	Transform rt = RotateTransform( _axis.angle( ), Vector( _axis.origin( ) ) );
	set( rt * ( *this ) * rt.inverse( ) );
}

ScaleTransform::ScaleTransform( const Planc & _scale, const Coordinate & _origin ) : Transform( true )
{
	set( 0, 0, _scale );
	set( 1, 1, _scale );

	if( _origin != ORIGIN )
	{
		Transform mt = MoveTransform( Vector( _origin ) );
		set( mt * ( *this ) * mt.inverse( ) );
	}
}

StretchTransform::StretchTransform( const Vector & stretch ) : Transform( true )
{
	set( 0, 0, stretch.magnitude( ) );

	if( stretch.has_magnitude( ) && stretch.angle( ) )
	{
		Transform rt = RotateTransform( stretch.angle( ), stretch.origin( ) );
		set( rt * ( *this ) * rt.inverse( ) );
	}
}

Transform::Transform( bool identity )
{
	clear( identity );
}

Transform & Transform::clear( bool identity )
{
	m_matrix[ 0 ][ 0 ] = m_matrix[ 1 ][ 1 ] = m_matrix[ 2 ][ 2 ] = identity ? ONE : ZERO;
	m_matrix[ 0 ][ 1 ] = m_matrix[ 0 ][ 2 ] = m_matrix[ 1 ][ 0 ] = m_matrix[ 1 ][ 2 ] = m_matrix[ 2 ][ 0 ] = m_matrix[ 2 ][ 1 ] = ZERO;
	return *this;
}

Planc Transform::get( const uint _i, const uint _j ) const
{
	Assert( in_range<uint>( _i, 2, true ) && in_range<uint>( _j, 2, true ), "index out of range" );
	return m_matrix[ _i ][ _j ];
}

Transform & Transform::set( const uint _i, const uint _j, const Planc _v )
{
	Assert( in_range<uint>( _i, 2, true ) && in_range<uint>( _j, 2, true ), "index out of range" );
	m_matrix[ _i ][ _j ] = _v;
	return *this;
}

Transform & Transform::set( const Transform & t )
{
	m_matrix[ 0 ][ 0 ] = t.m_matrix[ 0 ][ 0 ];
	m_matrix[ 0 ][ 1 ] = t.m_matrix[ 0 ][ 1 ];
	m_matrix[ 0 ][ 2 ] = t.m_matrix[ 0 ][ 2 ];
	m_matrix[ 1 ][ 0 ] = t.m_matrix[ 1 ][ 0 ];
	m_matrix[ 1 ][ 1 ] = t.m_matrix[ 1 ][ 1 ];
	m_matrix[ 1 ][ 2 ] = t.m_matrix[ 1 ][ 2 ];
	m_matrix[ 2 ][ 0 ] = t.m_matrix[ 2 ][ 0 ];
	m_matrix[ 2 ][ 1 ] = t.m_matrix[ 2 ][ 1 ];
	m_matrix[ 2 ][ 2 ] = t.m_matrix[ 2 ][ 2 ];
	return *this;
}

Coordinate Transform::transform( const Coordinate & coordinate ) const
{
	if( identity( ) )
	{
		return coordinate;
	}

	Planc x = coordinate.x( );
	Planc y = coordinate.y( );

	Planc xt = ( x * m_matrix[ 0 ][ 0 ] ) + ( y * m_matrix[ 0 ][ 1 ] ) + m_matrix[ 0 ][ 2 ];
	Planc yt = ( x * m_matrix[ 1 ][ 0 ] ) + ( y * m_matrix[ 1 ][ 1 ] ) + m_matrix[ 1 ][ 2 ];

	return Coordinate( xt, yt );
}

Transform & Transform::chain( const Transform & t )
{
	return set( t * ( *this ) );
}

Transform & Transform::invert( )
{
	return *this = inverse( );
}

Transform Transform::inverse( ) const
{
	dec d = determinant( );

	if( !d )
	{
		return Transform( false );
	}

	auto determinant2x2 = [ ] ( const Planc & a, const Planc & b, const Planc & c, const Planc & d )
	{
		return ( a * d ) - ( b * c );
	};

	Transform t;
	t.m_matrix[ 0 ][ 0 ] = determinant2x2( m_matrix[ 1 ][ 1 ], m_matrix[ 1 ][ 2 ], m_matrix[ 2 ][ 1 ], m_matrix[ 2 ][ 2 ] ) / d;
	t.m_matrix[ 0 ][ 1 ] = determinant2x2( m_matrix[ 0 ][ 2 ], m_matrix[ 0 ][ 1 ], m_matrix[ 2 ][ 2 ], m_matrix[ 2 ][ 1 ] ) / d;
	t.m_matrix[ 0 ][ 2 ] = determinant2x2( m_matrix[ 0 ][ 1 ], m_matrix[ 0 ][ 2 ], m_matrix[ 1 ][ 1 ], m_matrix[ 1 ][ 2 ] ) / d;
	t.m_matrix[ 1 ][ 0 ] = determinant2x2( m_matrix[ 1 ][ 2 ], m_matrix[ 1 ][ 0 ], m_matrix[ 2 ][ 2 ], m_matrix[ 2 ][ 0 ] ) / d;
	t.m_matrix[ 1 ][ 1 ] = determinant2x2( m_matrix[ 0 ][ 0 ], m_matrix[ 0 ][ 2 ], m_matrix[ 2 ][ 0 ], m_matrix[ 2 ][ 2 ] ) / d;
	t.m_matrix[ 1 ][ 2 ] = determinant2x2( m_matrix[ 0 ][ 2 ], m_matrix[ 0 ][ 0 ], m_matrix[ 1 ][ 2 ], m_matrix[ 1 ][ 0 ] ) / d;
	t.m_matrix[ 2 ][ 0 ] = determinant2x2( m_matrix[ 1 ][ 0 ], m_matrix[ 1 ][ 1 ], m_matrix[ 2 ][ 0 ], m_matrix[ 2 ][ 1 ] ) / d;
	t.m_matrix[ 2 ][ 1 ] = determinant2x2( m_matrix[ 0 ][ 1 ], m_matrix[ 0 ][ 0 ], m_matrix[ 2 ][ 1 ], m_matrix[ 2 ][ 0 ] ) / d;
	t.m_matrix[ 2 ][ 2 ] = determinant2x2( m_matrix[ 0 ][ 0 ], m_matrix[ 0 ][ 1 ], m_matrix[ 1 ][ 0 ], m_matrix[ 1 ][ 1 ] ) / d;
	return t;
}

bool Transform::identity( ) const
{
	return ( ( m_matrix[ 0 ][ 0 ] == 1.0 ) &&
			 ( m_matrix[ 1 ][ 1 ] == 1.0 ) &&
			 ( m_matrix[ 2 ][ 2 ] == 1.0 ) &&
			 !m_matrix[ 0 ][ 1 ] &&
			 !m_matrix[ 0 ][ 2 ] &&
			 !m_matrix[ 1 ][ 0 ] &&
			 !m_matrix[ 1 ][ 2 ] &&
			 !m_matrix[ 2 ][ 0 ] &&
			 !m_matrix[ 2 ][ 1 ] );
}

bool Transform::null( ) const
{
	return ( !m_matrix[ 0 ][ 0 ] &&
			 !m_matrix[ 0 ][ 1 ] &&
			 !m_matrix[ 0 ][ 2 ] &&
			 !m_matrix[ 1 ][ 0 ] &&
			 !m_matrix[ 1 ][ 1 ] &&
			 !m_matrix[ 1 ][ 2 ] &&
			 !m_matrix[ 2 ][ 0 ] &&
			 !m_matrix[ 2 ][ 1 ] &&
			 !m_matrix[ 2 ][ 2 ] );
}

dec Transform::determinant( ) const
{
	return ( m_matrix[ 0 ][ 0 ] * ( ( m_matrix[ 1 ][ 1 ] * m_matrix[ 2 ][ 2 ] ) - ( m_matrix[ 1 ][ 2 ] * m_matrix[ 2 ][ 1 ] ) ) ) -
		( m_matrix[ 0 ][ 1 ] * ( ( m_matrix[ 1 ][ 0 ] * m_matrix[ 2 ][ 2 ] ) - ( m_matrix[ 1 ][ 2 ] * m_matrix[ 2 ][ 0 ] ) ) ) +
		( m_matrix[ 0 ][ 2 ] * ( ( m_matrix[ 1 ][ 0 ] * m_matrix[ 2 ][ 1 ] ) - ( m_matrix[ 1 ][ 1 ] * m_matrix[ 2 ][ 0 ] ) ) );
}

Transform & Transform::operator=( const Transform & t )
{
	return set( t );
}

Transform Transform::operator+( const dec _d ) const
{
	Transform t;
	t.m_matrix[ 0 ][ 0 ] = m_matrix[ 0 ][ 0 ] + _d;
	t.m_matrix[ 0 ][ 1 ] = m_matrix[ 0 ][ 1 ] + _d;
	t.m_matrix[ 0 ][ 2 ] = m_matrix[ 0 ][ 2 ] + _d;
	t.m_matrix[ 1 ][ 0 ] = m_matrix[ 1 ][ 0 ] + _d;
	t.m_matrix[ 1 ][ 1 ] = m_matrix[ 1 ][ 1 ] + _d;
	t.m_matrix[ 1 ][ 2 ] = m_matrix[ 1 ][ 2 ] + _d;
	t.m_matrix[ 2 ][ 0 ] = m_matrix[ 2 ][ 0 ] + _d;
	t.m_matrix[ 2 ][ 1 ] = m_matrix[ 2 ][ 1 ] + _d;
	t.m_matrix[ 2 ][ 2 ] = m_matrix[ 2 ][ 2 ] + _d;
	return t;
}

Transform Transform::operator-( const dec _d ) const
{
	Transform t;
	t.m_matrix[ 0 ][ 0 ] = m_matrix[ 0 ][ 0 ] - _d;
	t.m_matrix[ 0 ][ 1 ] = m_matrix[ 0 ][ 1 ] - _d;
	t.m_matrix[ 0 ][ 2 ] = m_matrix[ 0 ][ 2 ] - _d;
	t.m_matrix[ 1 ][ 0 ] = m_matrix[ 1 ][ 0 ] - _d;
	t.m_matrix[ 1 ][ 1 ] = m_matrix[ 1 ][ 1 ] - _d;
	t.m_matrix[ 1 ][ 2 ] = m_matrix[ 1 ][ 2 ] - _d;
	t.m_matrix[ 2 ][ 0 ] = m_matrix[ 2 ][ 0 ] - _d;
	t.m_matrix[ 2 ][ 1 ] = m_matrix[ 2 ][ 1 ] - _d;
	t.m_matrix[ 2 ][ 2 ] = m_matrix[ 2 ][ 2 ] - _d;
	return t;
}

Transform Transform::operator*( const dec _d ) const
{
	Transform t;
	t.m_matrix[ 0 ][ 0 ] = m_matrix[ 0 ][ 0 ] * _d;
	t.m_matrix[ 0 ][ 1 ] = m_matrix[ 0 ][ 1 ] * _d;
	t.m_matrix[ 0 ][ 2 ] = m_matrix[ 0 ][ 2 ] * _d;
	t.m_matrix[ 1 ][ 0 ] = m_matrix[ 1 ][ 0 ] * _d;
	t.m_matrix[ 1 ][ 1 ] = m_matrix[ 1 ][ 1 ] * _d;
	t.m_matrix[ 1 ][ 2 ] = m_matrix[ 1 ][ 2 ] * _d;
	t.m_matrix[ 2 ][ 0 ] = m_matrix[ 2 ][ 0 ] * _d;
	t.m_matrix[ 2 ][ 1 ] = m_matrix[ 2 ][ 1 ] * _d;
	t.m_matrix[ 2 ][ 2 ] = m_matrix[ 2 ][ 2 ] * _d;
	return t;
}

Transform Transform::operator/( const dec _d ) const
{
	Assert( !_d, "cannot divide by zero" );

	Transform t;
	t.m_matrix[ 0 ][ 0 ] = m_matrix[ 0 ][ 0 ] / _d;
	t.m_matrix[ 0 ][ 1 ] = m_matrix[ 0 ][ 1 ] / _d;
	t.m_matrix[ 0 ][ 2 ] = m_matrix[ 0 ][ 2 ] / _d;
	t.m_matrix[ 1 ][ 0 ] = m_matrix[ 1 ][ 0 ] / _d;
	t.m_matrix[ 1 ][ 1 ] = m_matrix[ 1 ][ 1 ] / _d;
	t.m_matrix[ 1 ][ 2 ] = m_matrix[ 1 ][ 2 ] / _d;
	t.m_matrix[ 2 ][ 0 ] = m_matrix[ 2 ][ 0 ] / _d;
	t.m_matrix[ 2 ][ 1 ] = m_matrix[ 2 ][ 1 ] / _d;
	t.m_matrix[ 2 ][ 2 ] = m_matrix[ 2 ][ 2 ] / _d;
	return t;
}

Transform & Transform::operator+=( const dec _d )
{
	m_matrix[ 0 ][ 0 ] += _d;
	m_matrix[ 0 ][ 1 ] += _d;
	m_matrix[ 0 ][ 2 ] += _d;
	m_matrix[ 1 ][ 0 ] += _d;
	m_matrix[ 1 ][ 1 ] += _d;
	m_matrix[ 1 ][ 2 ] += _d;
	m_matrix[ 2 ][ 0 ] += _d;
	m_matrix[ 2 ][ 1 ] += _d;
	m_matrix[ 2 ][ 2 ] += _d;
	return *this;
}

Transform & Transform::operator-=( const dec _d )
{
	m_matrix[ 0 ][ 0 ] -= _d;
	m_matrix[ 0 ][ 1 ] -= _d;
	m_matrix[ 0 ][ 2 ] -= _d;
	m_matrix[ 1 ][ 0 ] -= _d;
	m_matrix[ 1 ][ 1 ] -= _d;
	m_matrix[ 1 ][ 2 ] -= _d;
	m_matrix[ 2 ][ 0 ] -= _d;
	m_matrix[ 2 ][ 1 ] -= _d;
	m_matrix[ 2 ][ 2 ] -= _d;
	return *this;
}

Transform & Transform::operator*=( const dec _d )
{
	m_matrix[ 0 ][ 0 ] *= _d;
	m_matrix[ 0 ][ 1 ] *= _d;
	m_matrix[ 0 ][ 2 ] *= _d;
	m_matrix[ 1 ][ 0 ] *= _d;
	m_matrix[ 1 ][ 1 ] *= _d;
	m_matrix[ 1 ][ 2 ] *= _d;
	m_matrix[ 2 ][ 0 ] *= _d;
	m_matrix[ 2 ][ 1 ] *= _d;
	m_matrix[ 2 ][ 2 ] *= _d;
	return *this;
}

Transform & Transform::operator/=( const dec _d )
{
	Assert( !_d, "cannot divide by zero" );

	m_matrix[ 0 ][ 0 ] /= _d;
	m_matrix[ 0 ][ 1 ] /= _d;
	m_matrix[ 0 ][ 2 ] /= _d;
	m_matrix[ 1 ][ 0 ] /= _d;
	m_matrix[ 1 ][ 1 ] /= _d;
	m_matrix[ 1 ][ 2 ] /= _d;
	m_matrix[ 2 ][ 0 ] /= _d;
	m_matrix[ 2 ][ 1 ] /= _d;
	m_matrix[ 2 ][ 2 ] /= _d;
	return *this;
}

Transform & Transform::operator+=( const Transform & t )
{
	return *this = ( *this ) + t;
}

Transform Transform::operator+( const Transform & t ) const
{
	Transform t2;
	t2.m_matrix[ 0 ][ 0 ] = m_matrix[ 0 ][ 0 ] + t.m_matrix[ 0 ][ 0 ];
	t2.m_matrix[ 0 ][ 1 ] = m_matrix[ 0 ][ 1 ] + t.m_matrix[ 0 ][ 1 ];
	t2.m_matrix[ 0 ][ 2 ] = m_matrix[ 0 ][ 2 ] + t.m_matrix[ 0 ][ 2 ];
	t2.m_matrix[ 1 ][ 0 ] = m_matrix[ 1 ][ 0 ] + t.m_matrix[ 1 ][ 0 ];
	t2.m_matrix[ 1 ][ 1 ] = m_matrix[ 1 ][ 1 ] + t.m_matrix[ 1 ][ 1 ];
	t2.m_matrix[ 1 ][ 2 ] = m_matrix[ 1 ][ 2 ] + t.m_matrix[ 1 ][ 2 ];
	t2.m_matrix[ 2 ][ 0 ] = m_matrix[ 2 ][ 0 ] + t.m_matrix[ 2 ][ 0 ];
	t2.m_matrix[ 2 ][ 1 ] = m_matrix[ 2 ][ 1 ] + t.m_matrix[ 2 ][ 1 ];
	t2.m_matrix[ 2 ][ 2 ] = m_matrix[ 2 ][ 2 ] + t.m_matrix[ 2 ][ 2 ];
	return t2;
}

Transform & Transform::operator*=( const Transform & t )
{
	return *this = ( *this ) * t;
}

Transform Transform::operator*( const Transform & t ) const
{
	Transform t2;
	t2.m_matrix[ 0 ][ 0 ] = ( m_matrix[ 0 ][ 0 ] * t.m_matrix[ 0 ][ 0 ] ) + ( m_matrix[ 0 ][ 1 ] * t.m_matrix[ 1 ][ 0 ] ) + ( m_matrix[ 0 ][ 2 ] * t.m_matrix[ 2 ][ 0 ] );
	t2.m_matrix[ 0 ][ 1 ] = ( m_matrix[ 0 ][ 0 ] * t.m_matrix[ 0 ][ 1 ] ) + ( m_matrix[ 0 ][ 1 ] * t.m_matrix[ 1 ][ 1 ] ) + ( m_matrix[ 0 ][ 2 ] * t.m_matrix[ 2 ][ 1 ] );
	t2.m_matrix[ 0 ][ 2 ] = ( m_matrix[ 0 ][ 0 ] * t.m_matrix[ 0 ][ 2 ] ) + ( m_matrix[ 0 ][ 1 ] * t.m_matrix[ 1 ][ 2 ] ) + ( m_matrix[ 0 ][ 2 ] * t.m_matrix[ 2 ][ 2 ] );
	t2.m_matrix[ 1 ][ 0 ] = ( m_matrix[ 1 ][ 0 ] * t.m_matrix[ 0 ][ 0 ] ) + ( m_matrix[ 1 ][ 1 ] * t.m_matrix[ 1 ][ 0 ] ) + ( m_matrix[ 1 ][ 2 ] * t.m_matrix[ 2 ][ 0 ] );
	t2.m_matrix[ 1 ][ 1 ] = ( m_matrix[ 1 ][ 0 ] * t.m_matrix[ 0 ][ 1 ] ) + ( m_matrix[ 1 ][ 1 ] * t.m_matrix[ 1 ][ 1 ] ) + ( m_matrix[ 1 ][ 2 ] * t.m_matrix[ 2 ][ 1 ] );
	t2.m_matrix[ 1 ][ 2 ] = ( m_matrix[ 1 ][ 0 ] * t.m_matrix[ 0 ][ 2 ] ) + ( m_matrix[ 1 ][ 1 ] * t.m_matrix[ 1 ][ 2 ] ) + ( m_matrix[ 1 ][ 2 ] * t.m_matrix[ 2 ][ 2 ] );
	t2.m_matrix[ 2 ][ 0 ] = ( m_matrix[ 2 ][ 0 ] * t.m_matrix[ 0 ][ 0 ] ) + ( m_matrix[ 2 ][ 1 ] * t.m_matrix[ 1 ][ 0 ] ) + ( m_matrix[ 2 ][ 2 ] * t.m_matrix[ 2 ][ 0 ] );
	t2.m_matrix[ 2 ][ 1 ] = ( m_matrix[ 2 ][ 0 ] * t.m_matrix[ 0 ][ 1 ] ) + ( m_matrix[ 2 ][ 1 ] * t.m_matrix[ 1 ][ 1 ] ) + ( m_matrix[ 2 ][ 2 ] * t.m_matrix[ 2 ][ 1 ] );
	t2.m_matrix[ 2 ][ 2 ] = ( m_matrix[ 2 ][ 0 ] * t.m_matrix[ 0 ][ 2 ] ) + ( m_matrix[ 2 ][ 1 ] * t.m_matrix[ 1 ][ 2 ] ) + ( m_matrix[ 2 ][ 2 ] * t.m_matrix[ 2 ][ 2 ] );
	return t2;
}
