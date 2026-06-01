#ifndef Camera_hpp
#define Camera_hpp

#include "axn/geometry/Geometry.hpp"
#include "axn/graphics/Color.hpp"
#include "axn/graphics/Drawing.hpp"
#include "axn/graphics/Visible.hpp"
#include "axn/graphics/Lighting.hpp"

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

    class HUD;
    class ScreenVisual;

    ~Camera( ) { clear_all( ); }

    Camera( ptr<World> in_world, Planc cref in_width = P0, Planc cref in_height = P0, dec in_zoom = 1.0 );

    #if defined( AXN_DEBUG )
    bool draw_debug = false;
    Drawing debug_overlay_drawing( ) const;
    #endif

    uint age( ) const { return( m_age ); }

    ptr<World> world( ) const { return( m_world ); }

    virtual void update( );
    virtual void render( );

    void clear_all( );
    void clear_subjects( );
    void clear_screen_effects( );
    void clear_hud_elements( );

    virtual ptr<Visible> capture( ptr<Visible> in_subject, bool in_should_delete = false );
    void capture( varray<ptr<Visible>> cref in_subjects, bool in_should_delete = false ) { for_each( subject, in_subjects ) { capture( subject, in_should_delete ); } }

    virtual ptr<Visible> capture_background( ptr<Visible> in_subject, bool in_should_delete = false );
    void capture_background( varray<ptr<Visible>> cref in_subjects, bool in_should_delete = false ) { for_each( subject, in_subjects ) { capture_background( subject, in_should_delete ); } }

    virtual ptr<Visible> capture_foreground( ptr<Visible> in_subject, bool in_should_delete = false );
    void capture_foreground( varray<ptr<Visible>> cref in_subjects, bool in_should_delete = false ) { for_each( subject, in_subjects ) { capture_foreground( subject, in_should_delete ); } }

    virtual ptr<ScreenVisual> add_screen_effect( ptr<ScreenVisual> in_effect, bool in_should_delete = false );
    void add_screen_effects( varray<ptr<ScreenVisual>> cref in_effects, bool in_should_delete = false ) { for_each( effect, in_effects ) { add_screen_effect( effect, in_should_delete ); } }

    virtual void remove_screen_effect( ptr<ScreenVisual> in_hud_element );
    void remove_screen_effects( varray<ptr<ScreenVisual>> cref in_effects ) { for_each( effect, in_effects ) { remove_screen_effect( effect ); } }

    virtual void add_hud_element( ptr<HUD> in_hud_element, bool in_should_delete = false );
    void add_hud_elements( varray<ptr<HUD>> cref in_hud_elements, bool in_should_delete = false ) { for_each( hud_element, in_hud_elements ) { add_hud_element( hud_element, in_should_delete ); } }

    virtual void remove_hud_element( ptr<HUD> in_hud_element );
    void remove_hud_elements( varray<ptr<HUD>> cref in_hud_elements ) { for_each( hud_element, in_hud_elements ) { remove_hud_element( hud_element ); } }

    virtual void add_always_hud_element( ptr<HUD> in_always_hud_element, bool in_should_delete = false );
    void add_always_hud_elements( varray<ptr<HUD>> cref in_always_hud_elements, bool in_should_delete = false ) { for_each( always_hud_element, in_always_hud_elements ) { add_always_hud_element( always_hud_element, in_should_delete ); } }

    virtual void remove_always_hud_element( ptr<HUD> in_always_hud_element );
    void remove_always_hud_elements( varray<ptr<HUD>> cref in_always_hud_elements ) { for_each( always_hud_element, in_always_hud_elements ) { remove_always_hud_element( always_hud_element ); } }

    #if defined( AXN_DEBUG )
    virtual void capture_debug( ptr<Visible> in_subject, bool in_should_delete = false );
    void capture_debug( varray<ptr<Visible>> cref in_subjects, bool in_should_delete = false ) { for_each( subject, in_subjects ) { capture_debug( subject, in_should_delete ); } }
    #endif

    Planc width( ) const;
    void width( Planc cref in_width );

    Planc height( ) const;
    void height( Planc cref in_height );

    Point center( ) const;
    void center( Point cref in_center );

    FixedRectangle bounds( bool in_positioned = true ) const;

    Point target( bool in_include_offset = false ) const;
    void target( Point cref in_target, bool in_hard_set = false );

    Vector target_offset( ) const;

    dec target_offset_ratio_y( ) const;
    void target_offset_ratio_y( dec in_offset_y );

    dec zoom( ) const;
    void zoom( dec in_zoom );

    dec min_zoom( ) const;
    dec max_zoom( ) const;

    bool show_hud( ) const;
    void show_hud( bool in_show_hud );

    dec hud_offset_percentage( ) const;
    void hud_offset_percentage( dec in_offset );

    bool in_view( Point cref in_world_position, dec in_z = 1.0 ) const;
    bool in_view( FixedRectangle cref in_world_bounding_box, dec in_z = 1.0 ) const;

    Point screen_to_world( Point cref in_screen_position ) const;
    Point world_to_screen( Point cref in_world_position ) const;

    Point cursor_world_position( );
    void cursor_world_position( Point cref in_pt );
    void cursor_world_position_reset( );

    Drawing cursor_drawing( ) const;

    enum transition_type
    {
        NO_TRANSITION = 0,
        FADE,
        SWIPE,
        CIRCLE,
    };

    bool transitioned( ) const;
    bool transitioning( ) const;

    void transition( transition_type in_transition, Vector cref in_vec = V0 );

    void transition_duration( ticks in_duration, ticks in_completion_duration );

private:

    ptr<World> m_world = nullptr;

    uint m_age = 0;

    Planc m_width = P0;
    Planc m_height = P0;

    Point m_center = ORIGIN;
    Point m_target = ORIGIN;

    FixedRectangle m_bounds;

    dec m_zoom = 1.0;
    dec m_movement_ratio = 0.5;

    dec m_offset_ratio_y = 0.0;

    Point m_cursor_world_position = ORIGIN;

    varray<ptr<Visible>> m_subjects;
    varray<ptr<Visible>> m_owned_subjects; // subset of subjects that camera needs to delete

    varray<ptr<Visible>> m_background_subjects;
    varray<ptr<Visible>> m_background_owned_subjects; // subset of subjects that camera needs to delete

    varray<ptr<Visible>> m_foreground_subjects;
    varray<ptr<Visible>> m_foreground_owned_subjects; // subset of subjects that camera needs to delete

    varray<ptr<ScreenVisual>> m_screen_effects;
    varray<ptr<ScreenVisual>> m_owned_screen_effects; // subset of screen effects that camera needs to delete

    varray<ptr<HUD>> m_hud_elements;
    varray<ptr<HUD>> m_owned_hud_elements; // subset of hud elements that camera needs to delete

    varray<ptr<HUD>> m_always_hud_elements; // hud elements that are always shown no matter what
    varray<ptr<HUD>> m_owned_always_hud_elements; // subset of always hud elements that camera needs to delete

    dec m_hud_offset_percentage = 0.0;
    bool m_show_hud = true;

    #if defined( AXN_DEBUG )
    varray<ptr<Visible>> m_debug_subjects;
    varray<ptr<Visible>> m_owned_debug_subjects; // subset of debug subjects that camera needs to delete
    #endif

    bool m_transitioned = false;
    bool m_transitioning_out = false;
    transition_type m_transition_type;
    Countdown m_transition_timer;
    Countdown m_transition_complete_timer;
    Vector m_transition_vec;

public:

    class ScreenVisual : public Visible
    {

    public:

        ScreenVisual( ) { persist_render( false ); needs_render_always( true ); }

        virtual FixedRectangle bounds( ptr<Camera> in_camera ) const { return( in_camera->bounds( ) ); }

        virtual void render( ptr<Camera> in_camera ) { Visible::render( ); }

    };

    class HUD : public ScreenVisual
    {

    public:

        HUD( dec in_center_x_percent, dec in_center_y_percent, dec in_width_percent, dec in_height_percent );

        void width( dec in_width_percent );
        void height( dec in_height_percent );
        void center( dec in_center_x_percent, dec in_center_y_percent );

        virtual FixedRectangle bounds( ptr<Camera> in_camera ) const override;

        virtual void render( ptr<Camera> in_camera ) override { ScreenVisual::render( in_camera ); }

    private:

        dec m_center_x_percent = 0.0;
        dec m_center_y_percent = 0.0;

        dec m_width_percent = 0.0;
        dec m_height_percent = 0.0;

    };

};

} // namespace graphics
} // namespace axn

#endif /* Camera_hpp */
