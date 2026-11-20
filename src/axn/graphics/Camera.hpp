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
    class Shutter;

    ~Camera( ) { clear_all( ); }

    Camera( ptr<World> in_world, Planc cref in_width = P0, Planc cref in_height = P0, dec in_zoom = 1.0 );

    #if defined( AXN_DEBUG )
    bool draw_debug = false;
    Drawing debug_overlay_drawing( ) const;
    #endif

    uint age( ) const { return( m_age ); }

    ptr<World> world( ) const { return( m_world ); }

    virtual void update( ); // C! every frame, paused or not (called by Engine::run)
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

    Planc cref width( ) const { return( m_width ); }
    void width( Planc cref in_width ) { m_width = in_width; }

    Planc cref height( ) const { return( m_height ); }
    void height( Planc cref in_height ) { m_height = in_height; }

    Point cref center( ) const { return( m_center ); }
    void center( Point cref in_center );

    FixedRectangle bounds( bool in_positioned = true ) const;
    FixedRectangle lens_bounds( ) const; // C! the part of bounds( false ) that's shown, shorter in cinematic mode

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

    bool cinematic( ) const;
    void cinematic( bool in_cinematic ); // C! black bars top and bottom, without changing the frame or moving the center

    dec hud_offset_percentage( ) const;
    void hud_offset_percentage( dec in_offset );

    bool in_view( Point cref in_world_position, dec in_z = 1.0 ) const;
    bool in_view( FixedRectangle cref in_world_bounding_box, dec in_z = 1.0 ) const;

    // C! how far outside the view an object's last drawing can be and still be redrawn: Room::render( ) skips redrawing the rest,
    // C! so this has to cover how much a drawing can change between frames (its position is always the current one)
    Planc cref render_buffer( ) const { return( m_render_buffer ); }
    void render_buffer( Planc cref in_render_buffer ) { m_render_buffer = in_render_buffer; }

    bool in_render_range( ptr<const Object> in_object ) const; // C! whether the object's max_bounding_box( ), where it is now and grown by render_buffer( ) on every side, is in view

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

    Shutter ref shutter( ); // C! always one of the screen effects, shown once it's started

private:

    ptr<World> m_world;

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

    Planc m_render_buffer = P0;
    bool m_show_hud = true;

    bool m_cinematic = false;

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
        virtual void update( ptr<Camera> in_camera ) { } // C! called by Camera::update( ) every frame, before the render, even while the engine is paused

    };

    class HUD : public ScreenVisual
    {

    public:

        HUD( dec in_center_x_percent, dec in_center_y_percent, dec in_width_percent, dec in_height_percent );

        virtual FixedRectangle bounds( ptr<Camera> in_camera ) const override;

        virtual void render( ptr<Camera> in_camera ) override { ScreenVisual::render( in_camera ); }

        void width( dec in_width_percent );
        void height( dec in_height_percent );
        void center( dec in_center_x_percent, dec in_center_y_percent );

    private:

        dec m_center_x_percent = 0.0;
        dec m_center_y_percent = 0.0;

        dec m_width_percent = 0.0;
        dec m_height_percent = 0.0;

    };

    // C! the screen closing and opening again like a camera's iris shutter
    class Shutter : public ScreenVisual
    {

    public:

        Shutter( uint in_sides, ticks in_duration ) : m_sides( in_sides ), m_duration( in_duration ) { }

        virtual void render( ptr<Camera> in_camera ) override;
        virtual void update( ptr<Camera> in_camera ) override;

        void start( );

        bool active( ) const { return( !m_countdown.completed( ) ); }
        bool paused_engine( ) const { return( m_paused_engine ); } // C! the pause on now is the shutter's own
        uint sides( ) const { return( m_sides ); }
        void sides( uint in_sides ) { m_sides = in_sides; }

        ticks duration( ) const { return( m_duration ); }
        void duration( ticks in_duration ) { m_duration = in_duration; }

    private:

        uint m_sides = 0;
        ticks m_duration = 0.0;

        Countdown m_countdown;

        bool m_paused_engine = false; // C! start( ) paused the engine, so it gets unpaused when the shutter is done
        bool m_starting = false; // C! start( ) was just called, so the next update( ) doesn't tick and the first render shows it fully open

    };

private:

    Shutter m_shutter;

};

} // namespace graphics
} // namespace axn

#endif /* Camera_hpp */
