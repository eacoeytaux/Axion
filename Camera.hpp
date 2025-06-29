#ifndef Camera_hpp
#define Camera_hpp

#include "Geometry.hpp"
#include "Color.hpp"
#include "Drawing.hpp"
#include "Lighting.hpp"
#include "Visible.hpp"

namespace axn
{

namespace reality
{

class World;
class Object;

} // namespace reality

namespace graphics
{

class Camera
{

public:

    class HeadUpDisplay;
    class ScreenEffect;

    ~Camera( ) { clear_all( ); }

    Camera( World * world, Planc cref width = P0, Planc cref height = P0, dec zoom = 1.0 );

    #if defined ( AXN_DEBUG )
    bool draw_debug = false;
    Drawing debug_overlay_drawing( ) const;
    #endif

    uint age( ) const { return m_age; }

    virtual void update( );
    virtual void render( );

    void clear_all( );
    void clear_subjects( );
    void clear_screen_effects( );
    void clear_hud_elements( );

    virtual void capture( Visible * subject, bool should_delete = false );
    void capture( varray<Visible *> & subjects, bool should_delete = false ) { for_each( subject, subjects ) { capture( subject, should_delete ); } }

    virtual void add_screen_effect( ScreenEffect * effect, bool should_delete = false );
    void add_screen_effects( varray<ScreenEffect *> & effects, bool should_delete = false ) { for_each( effect, effects ) { add_screen_effect( effect, should_delete ); } }

    virtual void remove_screen_effect( ScreenEffect * hud_element );
    void remove_screen_effects( varray<ScreenEffect *> & effects ) { for_each( effect, effects ) { remove_screen_effect( effect ); } }

    virtual void add_hud_element( HeadUpDisplay * hud_element, bool should_delete = false );
    void add_hud_elements( varray<HeadUpDisplay *> & hud_elements, bool should_delete = false ) { for_each( hud_element, hud_elements ) { add_hud_element( hud_element, should_delete ); } }

    virtual void remove_hud_element( HeadUpDisplay * hud_element );
    void remove_hud_elements( varray<HeadUpDisplay *> & hud_elements ) { for_each( hud_element, hud_elements ) { remove_hud_element( hud_element ); } }

    #if defined ( AXN_DEBUG )
    virtual void capture_debug( Visible * subject, bool should_delete = false );
    void capture_debug( varray<Visible *> & subjects, bool should_delete = false ) { for_each( subject, subjects ) { capture_debug( subject, should_delete ); } }
    #endif

    Planc width( ) const;
    void width( Planc cref );

    Planc height( ) const;
    void height( Planc cref );

    Coordinate center( ) const;
    void center( Coordinate cref );

    FixedRectangle bounds( ) const;

    Coordinate target( ) const;
    void target( Coordinate cref, bool hard_set = false );

    Vector target_offset( ) const;

    dec zoom( ) const;
    void zoom( dec );

    dec min_zoom( ) const;
    dec max_zoom( ) const;

    bool show_hud( ) const;
    void show_hud( bool );

    dec hud_offset_percentage( ) const;
    void hud_offset_percentage( dec );

    bool in_view( Coordinate cref world_position, dec z = 1.0 ) const;
    bool in_view( FixedRectangle cref world_bounding_box, dec z = 1.0 ) const;

    Coordinate screen_to_world( Coordinate cref screen_position ) const;
    Coordinate world_to_screen( Coordinate cref world_position ) const;

    Coordinate cursor_world_position( );
    void cursor_world_position( Coordinate cref );
    void cursor_world_position_reset( );

    Drawing cursor_drawing( ) const;

private:

    World * m_world = nullptr;

    uint m_age = 0;

    Planc m_width = P0;
    Planc m_height = P0;

    Coordinate m_center = ORIGIN;
    Coordinate m_target = ORIGIN;

    dec m_zoom = 1.0;
    dec m_movement_ratio = 0.5;

    Coordinate m_cursor_world_position = ORIGIN;

    varray<Visible *> m_subjects;
    varray<Visible *> m_owned_subjects; // subset of subjects that camera needs to delete

    varray<ScreenEffect *> m_screen_effects;
    varray<ScreenEffect *> m_owned_screen_effects; // subset of screen effects that camera needs to delete

    varray<HeadUpDisplay *> m_hud_elements;
    varray<HeadUpDisplay *> m_owned_hud_elements; // subset of hud elements that camera needs to delete

    dec m_hud_offset_percentage = 0.0;
    bool m_show_hud = true;

    #if defined ( AXN_DEBUG )
    varray<Visible *> m_debug_subjects;
    varray<Visible *> m_owned_debug_subjects; // subset of debug subjects that camera needs to delete
    #endif

public:

    class HeadUpDisplay : public Visible
    {

    public:

        HeadUpDisplay( dec center_x_percent, dec center_y_percent, dec width_percent, dec height_percent );

        virtual void render( Camera * ) { Visible::render( ); }

        FixedRectangle bounds( Camera * ) const;

    private:

        dec m_center_x_percent = 0.0;
        dec m_center_y_percent = 0.0;

        dec m_width_percent = 0.0;
        dec m_height_percent = 0.0;

    };

    class ScreenEffect : public Visible
    {

    public:

        ScreenEffect( );

        virtual void render( Camera * ) { Visible::render( ); }

    };

};

} // namespace graphics
} // namespace axn

#endif /* Camera_hpp */
