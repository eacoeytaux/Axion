#ifndef Visible_hpp
#define Visible_hpp

#include "axn/graphics/Drawing.hpp"
#include "axn/graphics/Lighting.hpp"

namespace axn
{
namespace graphics
{

class Visible : public Drawing
{

public:

    Visible( dec in_z = 1.0 ) : m_z( in_z ) { }
    Visible( Drawing cref in_drawing, dec in_z = 1.0 ) : Drawing( in_drawing ), m_z( in_z ) { }

    dec z( ) const { return( m_z ); }
    void z( dec in_z ) { m_z = in_z; }

    virtual void render( )
    {
        m_ever_rendered = true;
        if( !persist_render( ) )
        {
            clear( );
        }
    }

    bool ever_rendered( ) const { return( m_ever_rendered ); }
    bool needs_render( ) { return( m_needs_render || !m_ever_rendered ); }

    FixedRectangle drawing_bounds( ) { render( ); return( bounding_box( ) ); }

    bool visible( ) const { return( true ); } // TODO polygon_count( ) || light_sources( ).size( ); }

    varray<LightSource> cref light_sources( ) const { return( m_light_sources ); }
    void clear_light_sources( ) { m_light_sources.clear( ); }

    void layer_position( queue<uint> in_layer_position ) { m_layer_position = in_layer_position; }
    queue<uint> layer_position( ) const { return( m_layer_position ); }

    virtual bool layer_above( ptr<const Visible> in_visible ) const { return( false ); }
    bool layer_position_above( ptr<const Visible> in_visible ) const
    {
        // TODO sort of queue<int> should happen as its own function

        if( z( ) != in_visible->z( ) )
        {
            // sorting doesn't matter if objects aren't on the same z plane
            return( z( ) < in_visible->z( ) );
        }
        else
        {
            queue<uint> s1 = layer_position( );

            queue<uint> s2 = in_visible->layer_position( );

            while( s1.size( ) || s2.size( ) )
            {
                if( s1.size( ) && s2.size( ) )
                {
                    if( s1.front( ) == s2.front( ) )
                    {
                        if( ( s1.size( ) == s2.size( ) ) && is_one( s1.size( ) ) )
                        {
                            return( layer_above( in_visible ) );
                        }
                        else
                        {
                            // top levels are equal, keep looking

                            s1.pop( );
                            s2.pop( );
                        }
                    }
                    else
                    {
                        return( s1.front( ) > s2.front( ) );
                    }
                }
                else if( s1.size( ) )
                {
                    return( true );
                }
                else // if( s2.size( ) )
                {
                    return( false );
                }
            }

        }

        return( false );
    }

    static bool sort( ptr<const Visible> cref in_v1, ptr<const Visible> cref in_v2 ) { return( in_v1->layer_position_above( in_v2 ) ); }

protected:

    bool persist_render( ) { return( m_persist_render ); }
    void persist_render( bool in_persist_render ) { m_persist_render = in_persist_render; }

    void needs_render( bool in_needs_render ) { m_needs_render = in_needs_render || m_needs_render || m_needs_render_always; }

    void needs_render_always( bool in_needs_render_always )
    {
        m_needs_render_always = in_needs_render_always;
        if( m_needs_render_always )
        {
            needs_render( true );
        }
    }

    void add_light_source( Point cref in_position, Planc cref in_radius, Planc cref in_flicker = 0.0, Color cref in_tint = TRANSPARENT ) { m_light_sources.insert_back( LightSource( in_position, in_radius ) ); }

    LightSource cref add_light_source( LightSource cref in_light_source ) { m_light_sources.insert_back( in_light_source ); return( in_light_source ); }
    varray<LightSource> cref add_light_sources( varray<LightSource> cref in_light_sources ) { m_light_sources.insert_back( in_light_sources ); return( in_light_sources ); }

private:

    dec m_z = 1.0;                // 1 = player plane, 1- = background, 1+ = foreground
    queue<uint> m_layer_position; // determines show order within layer, lower value shows on top of higher values

    varray<LightSource> m_light_sources;

    bool m_needs_render = true;
    bool m_needs_render_always = false;
    bool m_persist_render = false;
    bool m_ever_rendered = false;

};

} // namespace graphics
} // namespace axn

#endif /* Visible_hpp */
