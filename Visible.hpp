#ifndef Visible_hpp
#define Visible_hpp

#include "Drawing.hpp"
#include "LightSource.hpp"

namespace axn
{
namespace graphics
{

class Visible : public Drawing
{

public:

    Visible( dec z = 1.0 ) : m_z( z ) { }
    Visible( Drawing cref drawing, dec z = 1.0 ) : Drawing( drawing ), m_z( z ) { }

    dec z( ) const { return m_z; }
    void z( dec z ) { m_z = z; }

    void layer_position( queue<uint> layer_position ) { m_layer_position = layer_position; }

    virtual void render( )
    {
        m_ever_rendered = true;
        if( !persist_render( ) )
        {
            clear( );
        }
    }

    bool ever_rendered( ) const { return m_ever_rendered; }
    bool needs_render( ) { return ( m_needs_render || !m_ever_rendered ); }

    bool visible( ) const { return polygon_count( ) || light_sources( ).size( ); }

    const varray<LightSource> & light_sources( ) const { return m_light_sources; }
    void clear_light_sources( ) { m_light_sources.clear( ); }

    queue<uint> layer_position( ) const { return m_layer_position; }

    static bool sort( const Visible * cref v1, const Visible * cref v2 )
    {
        if( v1->z( ) != v2->z( ) )
        {
            // sorting doesn't matter if objects aren't on the same z plane
            return ( v1->z( ) < v2->z( ) );
        }
        else
        {
            queue<uint> s1 = v1->layer_position( );
            queue<uint> s2 = v2->layer_position( );

            while( s1.size( ) || s2.size( ) )
            {
                if( s1.size( ) && s2.size( ) )
                {
                    if( s1.front( ) == s2.front( ) )
                    {
                        // top levels are equal, keep looking
                        s1.pop( );
                        s2.pop( );
                    }
                    else
                    {
                        return ( s1.front( ) > s2.front( ) );
                    }
                }
                else if( s1.size( ) )
                {
                    return true;
                }
                else // if( s2.size( ) )
                {
                    return false;
                }
            }
        }

        return false;
    }

protected:

    bool persist_render( ) { return m_persist_render; }
    void persist_render( bool persist_render ) { m_persist_render = persist_render; }

    void needs_render( bool needs_render ) { m_needs_render = needs_render || m_needs_render || m_needs_render_always; }

    void needs_render_always( bool needs_render_always )
    {
        m_needs_render_always = needs_render_always;
        if( m_needs_render_always )
        {
            needs_render( true );
        }
    }

    void add_light_source( Coordinate cref position, Planc radius, Color cref tint = TRANSPARENT, dec flicker = 0.0 ) { m_light_sources.insert_back( LightSource( position, radius ) ); }

    void add_light_source( LightSource cref light_source ) { m_light_sources.insert_back( light_source ); }
    void add_light_sources( varray<LightSource> cref light_sources ) { m_light_sources.insert_back( light_sources ); }

private:

    dec m_z = 1.0;                // 1 = player plane, <1 = background, >1 = foreground
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
