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
    Camera( World * world, const Planc & width = P0, const Planc & height = P0, dec zoom = ONE );

    uint age( ) const { return m_age; }

    void update( );
    void render( );

    void clear_all( );
    void clear_subjects( );
    void clear_screen_effects( );
    void clear_hud_elements( );

    void capture( Visible * subject, bool should_delete = false );
    void capture( varray<Visible *> & subjects, bool should_delete = false )
    {
        for_each( subject, subjects )
        {
            capture( subject, should_delete );
        }
    }

    void add_screen_effect( ScreenEffect * effect, bool should_delete = true );
    void add_screen_effects( varray<ScreenEffect *> & effects, bool should_delete = true )
    {
        for_each( effect, effects )
        {
            add_screen_effect( effect, should_delete );
        }
    }

    void remove_screen_effect( ScreenEffect * hud_element );
    void remove_screen_effects( varray<ScreenEffect *> & effects )
    {
        for_each( effect, effects )
        {
            remove_screen_effect( effect );
        }
    }

    void add_hud_element( HeadUpDisplay * hud_element, bool should_delete = true );
    void add_hud_elements( varray<HeadUpDisplay *> & hud_elements, bool should_delete = true )
    {
        for_each( hud_element, hud_elements )
        {
            add_hud_element( hud_element, should_delete );
        }
    }

    void remove_hud_element( HeadUpDisplay * hud_element );
    void remove_hud_elements( varray<HeadUpDisplay *> & hud_elements )
    {
        for_each( hud_element, hud_elements )
        {
            remove_hud_element( hud_element );
        }
    }

    #ifdef AXN_DEBUG
    void capture_debug( Visible * subject, bool should_delete = false );
    void capture_debug( varray<Visible *> & subjects, bool should_delete = false )
    {
        for_each( subject, subjects )
        {
            capture_debug( subject, should_delete );
        }
    }
    #endif

    Planc width( ) const;
    void width( const Planc & );
    Planc height( ) const;
    void height( const Planc & );

    Coordinate center( ) const;
    void center( const Coordinate & );

    FixedRectangle bounds( ) const;

    Coordinate target( ) const;
    void target( const Coordinate &, bool hard_set = false );

    Vector target_offset( ) const;

    dec zoom( ) const;
    void zoom( dec );

    dec min_zoom( ) const;
    dec max_zoom( ) const;

    bool show_hud( ) const;
    void show_hud( bool );

    dec hud_offset_percentage( ) const;
    void hud_offset_percentage( dec );

    bool in_view( const Coordinate & world_position, dec z = ONE ) const;
    bool in_view( const FixedRectangle & world_bounding_box, dec z = ONE ) const;

    Coordinate screen_to_world( const Coordinate & screen_position ) const;
    Coordinate world_to_screen( const Coordinate & world_position ) const;

    Coordinate cursor_world_position( );
    void cursor_world_position( const Coordinate & );
    void cursor_world_position_reset( );

private:
    Drawing cursor_drawing( ) const;
    #ifdef AXN_DEBUG
public:
    bool m_draw_debug = false;
private:
    Drawing debug_overlay_drawing( ) const;
    #endif

    World * m_world = nullptr;
    uint m_age = 0;

    Planc m_width, m_height;
    Coordinate m_center;
    Coordinate m_target;
    dec m_zoom = 1.0;
    dec m_movement_ratio = 0.5;

    Coordinate m_cursor_world_position;

    varray<Visible *> m_subjects;
    varray<Visible *> m_owned_subjects; // subset of subjects that camera needs to delete

    varray<ScreenEffect *> m_screen_effects;
    varray<ScreenEffect *> m_owned_screen_effects; // subset of screen effects that camera needs to delete

    varray<HeadUpDisplay *> m_hud_elements;
    varray<HeadUpDisplay *> m_owned_hud_elements; // subset of hud elements that camera needs to delete
    dec m_hud_offset_percentage;
    bool m_show_hud;

    #ifdef AXN_DEBUG
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
        dec m_center_x_percent;
        dec m_center_y_percent;
        dec m_width_percent;
        dec m_height_percent;
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
