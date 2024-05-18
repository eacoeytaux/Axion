#ifndef Camera_hpp
#define Camera_hpp

#include "Geometry.hpp"
#include "Color.hpp"
#include "Drawing.hpp"
#include "Lighting.hpp"
#include "Visible.hpp"

namespace axn
{
namespace graphics
{

class Camera
{
public:
    virtual ~Camera( ) { }
    Camera( const Coordinate & target = ORIGIN, const Planc & width = 0.0, const Planc & height = 0.0, dec zoom = 1.0 );

    Coordinate screen_to_world( const Coordinate & screen_position ) const;
    Coordinate world_to_screen( const Coordinate & world_position ) const;
    bool in_view( const Coordinate & world_position ) const;

    uint age( ) const;

    Camera & capture( const Visible * subject );
    Camera & capture( const varray<const Visible *> & subjects )
    {
        for_each( subject, subjects ) capture( subject );
        return *this;
    }

    Camera& capture(const Visible & subject);
    Camera& capture(const varray<Visible>& subjects)
    {
        for_each(subject, subjects) capture(subject);
        return *this;
    }

    Camera & lighting( const Lighting * lighting );
    Camera & clear_lighting( );

    Camera & clear( );
    Camera & render( );

    Camera & update( const Coordinate & target, bool hard_target_set = false );
    Camera & update( bool hard_target_set = false ) { return update( target( ), hard_target_set ); }

    Planc width( ) const;
    Camera & width( const Planc & );
    Planc height( ) const;
    Camera & height( const Planc & );

    Coordinate center( ) const;
    Camera & center( const Coordinate & );

    Coordinate target( ) const;
    Camera & target( const Coordinate &, bool hard_set = false );

    Vector target_offset( ) const;

    dec zoom( ) const;
    Camera & zoom( dec );

    Coordinate m_cursor_world_position; // TODO make private

private:
    Drawing cursor_drawing( ) const;

    uint m_age = 0;

    Planc m_width, m_height;
    Coordinate m_center;
    Coordinate m_target;
    dec m_zoom = 1.0;
    dec m_movement_ratio = 0.5;

    varray<const Visible *> m_subjects;
    varray<Visible *> m_owned_subjects; // subset of subjects that Camera needs to delete

    const Lighting * m_lighting = nullptr;
};

} // namespace graphics
} // namespace axn

#endif /* Camera_hpp */
