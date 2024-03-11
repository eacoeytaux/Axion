#ifndef Camera_hpp
#define Camera_hpp

#include "axn.hpp"
#include "Color.hpp"
#include "ColoredTriangle.hpp"
#include "Drawing.hpp"
#include "Lighting.hpp"
#include "Visible.hpp"
#include "Geometry.hpp"

namespace axn {
namespace graphics {

class Camera {
public:
    virtual ~Camera( ) {}
    Camera( const Coordinate & target = ORIGIN, const planc & width = 0.f,
            const planc & height = 0.f, ufloat zoom = 1.f );

    Coordinate screen_to_world( const Coordinate & screen_position ) const;
    Coordinate world_to_screen( const Coordinate & world_position ) const;
    bool in_view( const Coordinate & world_position ) const;

    uint age( ) const;

    Camera & capture( const Visible* subject );
    Camera & capture( const varray<const Visible*> & subjects ) {
        for_each( subject, subjects ) capture( subject );
        return *this;
    }

    Camera & clear( );
    Camera & render( );
    const varray<ColoredTriangle> & rendering( ) const;

    Camera & update( const Coordinate & target, bool hard_target_set = false );
    Camera & update( bool hard_target_set = false ) {
        update( target( ), hard_target_set );
        return *this;
    }

    planc width( ) const;
    Camera & width( const planc & );
    planc height( ) const;
    Camera & height( const planc & );

    Coordinate center( ) const;
    Camera & center( const Coordinate & );

    Coordinate target( ) const;
    Camera & target( const Coordinate &, bool hard_set = false );

    ufloat zoom( ) const;
    Camera & zoom( ufloat );

    Coordinate m_cursor_world_position; // TODO make private

private:
    Drawing cursor_drawing( ) const;
    Camera & render( const Visible* subject );

    uint m_age = 0;

    planc m_width, m_height;
    Coordinate m_center;
    Coordinate m_target;
    ufloat m_zoom = 1.f;
    ufloat m_movement_ratio = 0.5;

    varray<const Visible*> m_subjects;
    varray<ColoredTriangle> m_colored_triangles;
};

} // namespace graphics
} // namespace axn

#endif /* Camera_hpp */
