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
    
    Transform & clear( bool identity = true );

    Coordinate transform( const Coordinate & ) const;

    Transform & chain( const Transform & );

    Transform & invert( );
    Transform inverse( ) const;

    bool identity( ) const;
    bool null( ) const;
    dec determinant( ) const;

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

    default_equal( Transform );

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

const Transform ZERO_TRANSFORM = Transform( false );
const Transform IDENTITY_TRANSFORM = IdentityTransform( );

class Transformable
{
public:
    virtual ~Transformable( ) { }
    Transformable( ) { }
    
    const Transform & transform( ) const
    {
        return m_transform;
    }
    
    const Transform & cumulative_transform( ) const
    {
        return m_cumulative_transform;
    }

    virtual Transformable & clear_transform( )
    {
        m_transform = IDENTITY_TRANSFORM;
        return *this;
    }

    virtual const Transformable & const_clear_transform( ) const
    {
        m_transform = IDENTITY_TRANSFORM;
        return *this;
    }

    virtual Transformable & transform( const Transform & t )
    {
        dirty( );
        m_transform.chain( t );
        m_cumulative_transform.chain( t );
        return *this;
    }

    virtual const Transformable & const_transform( const Transform & t ) const
    {
        dirty( );
        m_transform.chain( t );
        m_cumulative_transform.chain( t );
        return *this;
    }

    Transformable & move( const Vector & v ) { return transform( MoveTransform( v ) ); }
    Transformable & stretch( const Vector & v ) { return transform( StretchTransform( v ) ); }
    Transformable & scale( dec scale, const Coordinate & origin = ORIGIN ) { return transform( ScaleTransform( scale, origin ) ); }
    Transformable & rotate( const Angle & angle, const Coordinate & origin = ORIGIN ) { return transform( RotateTransform( angle, origin ) ); }
    Transformable & mirror( const Vector & axis ) { return transform( ReflectionTransform( axis ) ); }
    Transformable & mirror_x( ) { return mirror( X_HAT ); }
    Transformable & mirror_y( ) { return mirror( Y_HAT ); }
    
    Coordinate transform( const Coordinate & coordinate ) const
    {
        return transform( ).transform( coordinate );
    }
    
    Coordinate cumulative_transform( const Coordinate & coordinate ) const
    {
        return cumulative_transform( ).transform( coordinate );
    }
    
    bool is_dirty( ) const
    {
        return m_dirty;
    }
    
    bool is_clean( ) const
    {
        return !is_dirty( );
    }

    default_equal( Transformable );
    
protected:
    virtual const Transformable & dirty( ) const
    {
        m_dirty = true;
        return *this;
    }
    
    virtual const Transformable & clean( ) const
    {
        m_dirty = false;
        return *this;
    }
    
private:
    mutable bool m_dirty = false;
    mutable Transform m_transform;
    mutable Transform m_cumulative_transform;
};

} // namespace geometry
} // namespace axn

#define transform_functions( Class ) \
Class & move( const Vector & v ) { Transformable::move( v ); return *this; } \
Class & stretch( const Vector & v ) { Transformable::stretch( v ); return *this; } \
Class & scale( dec scale, const Coordinate & origin = ORIGIN ) { Transformable::scale( scale, origin ); return *this; } \
Class & rotate(const Angle & angle, const Coordinate & origin = ORIGIN ) { Transformable::rotate( angle, origin ); return *this; } \
Class & mirror( const Vector & axis ) { Transformable::mirror( axis ); return *this; } \
Class & mirror_x( ) { Transformable::mirror_x( ); return *this; } \
Class & mirror_y( ) { Transformable::mirror_y( ); return *this; }

#endif /* Transform_hpp */
