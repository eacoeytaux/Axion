#ifndef Transform_hpp
#define Transform_hpp

#include "axn.hpp"
#include "Planc.hpp"
#include "Coordinate.hpp"
#include "Angle.hpp"
#include "Vector.hpp"

namespace axn {
namespace geometry {

class Transform {
public:
    virtual ~Transform( ) {}
    Transform( bool identity = true );

    Transform & set( uint, uint, planc );
    Transform & set( const Transform & t );

    Coordinate transform( const Coordinate & );

    Transform & chain( const Transform & );

    bool null( ) const;
    float determinant( ) const;

    Transform & invert( );
    Transform inverse( );

    Transform & operator=( const Transform & t );

    Transform operator+( float f ) const;
    Transform operator-( float f ) const;
    Transform operator*( float f ) const;
    Transform operator/( float f ) const;

    Transform & operator+=( float f );
    Transform & operator-=( float f );
    Transform & operator*=( float f );
    Transform & operator/=( float f );

    Transform operator+( const Transform & t ) const;
    Transform operator*( const Transform & t ) const;

    Transform & operator+=( const Transform & t );
    Transform & operator*=( const Transform & t );

protected:
    planc m_matrix[ 3 ][ 3 ];
};

class IdentityTransform : public Transform {
public:
    IdentityTransform( ) : Transform( true ) {}
};

class MoveTransform : public Transform {
public:
    MoveTransform( const Vector & translation ) : Transform( true ) {
        set( 0, 2, translation.dx( ) );
        set( 1, 2, translation.dy( ) );
    }
};

class RotateTransform : public Transform {
public:
    RotateTransform( const Angle & angle ) : Transform( true ) {
        set( 0, 0, angle.cos( ) );
        set( 0, 1, -angle.sin( ) );
        set( 1, 0, angle.sin( ) );
        set( 1, 1, angle.cos( ) );
    }
};

class ReflectionTransform : public Transform {
public:
    ReflectionTransform( const Vector & stretch ) : Transform( true ) {
        MoveTransform mt1( -Vector( stretch.origin( ) ) );
        MoveTransform mt2 = mt1;
        mt2.invert( );

        RotateTransform rt1( -stretch.angle( ) );
        RotateTransform rt2 = rt1;
        rt2.invert( );

        Transform t;
        t.set( 1, 1, -1.f );

        set( mt2 * rt2 * t * rt1 * mt1 );
    }
};

class ScaleTransform : public Transform {
public:
    ScaleTransform( planc scale, const Coordinate & origin = ORIGIN ) : Transform( true ) {
        MoveTransform mt1( -Vector( origin ) );
        MoveTransform mt2 = mt1;
        mt2.invert( );

        Transform t;
        t.set( 0, 0, scale );
        t.set( 1, 1, scale );

        set( mt2 * t * mt1 );
    }
};

class StretchTransform : public Transform {
public:
    StretchTransform( const Vector & stretch ) : Transform( true ) {
        MoveTransform mt1( -Vector( stretch.origin( ) ) );
        MoveTransform mt2 = mt1;
        mt2.invert( );

        RotateTransform rt1( -stretch.angle( ) );
        RotateTransform rt2 = rt1;
        rt2.invert( );

        Transform t;
        t.set( 0, 0, stretch.magnitude( ) );

        set( mt2 * rt2 * t * rt1 * mt1 );
    }
};

} // namespace geometry
} // namespace axn

#endif /* Transform_hpp */
