#ifndef Transform_hpp
#define Transform_hpp

#include "Planc.hpp"
#include "Coordinate.hpp"
#include "Angle.hpp"
#include "Vector.hpp"

namespace axn
{
namespace geometry
{

class Transform
{
public:
    virtual ~Transform( ) { }
    Transform( bool identity = true );

    Planc get( uint, uint ) const;
    Transform & set( uint, uint, Planc );
    Transform & set( const Transform & t );

    Coordinate transform( const Coordinate & ) const;

    Transform & chain( const Transform & );

    bool identity( ) const;
    bool null( ) const;
    dec determinant( ) const;

    Transform & invert( );
    Transform inverse( ) const;

    Transform & operator=( const Transform & );

    Transform operator+( dec ) const;
    Transform operator-( dec ) const;
    Transform operator*( dec ) const;
    Transform operator/( dec ) const;

    Transform & operator+=( dec );
    Transform & operator-=( dec );
    Transform & operator*=( dec );
    Transform & operator/=( dec );

    Transform operator+( const Transform & ) const;
    Transform operator*( const Transform & ) const;

    Transform & operator+=( const Transform & );
    Transform & operator*=( const Transform & );

    bool operator==( const Transform & ) const;
    bool operator!=( const Transform & t ) const { return !( *this == t ); }

private:
    Planc m_matrix[ 3 ][ 3 ];
};

class IdentityTransform : public Transform
{
public:
    IdentityTransform( ) : Transform( true ) { }
};

class MoveTransform : public Transform
{
public:
    MoveTransform( const Vector & translation );
};

class RotateTransform : public Transform
{
public:
    RotateTransform( const Angle & angle, const Coordinate & origin = ORIGIN );
};

class ReflectionTransform : public Transform
{
public:
    ReflectionTransform( const Vector & axis );
};

class ScaleTransform : public Transform
{
public:
    ScaleTransform( const Planc & scale, const Coordinate & origin = ORIGIN );
};

class StretchTransform : public Transform
{
public:
    StretchTransform( const Vector & stretch );
};

const Transform IDENTITY_TRANSFORM = IdentityTransform( );

} // namespace geometry
} // namespace axn

#endif /* Transform_hpp */
