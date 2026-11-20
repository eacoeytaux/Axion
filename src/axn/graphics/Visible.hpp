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

    class LayerOrder : public queue<uint> { }; // determines show order within layer, lower value shows on top of higher values

    Visible( dec in_z = 1.0 ) : m_z( in_z ) { }
    Visible( Drawing cref in_drawing, dec in_z = 1.0 ) : Drawing( in_drawing ), m_z( in_z ) { }

    dec z( ) const { return( m_z ); }
    void z( dec in_z ) { m_z = in_z; }

    virtual void render( )
    {
        m_ever_rendered = true;
        m_needs_render = m_needs_render_always;

        if( !persist_render( ) )
        {
            clear( );
        }
    }

    bool ever_rendered( ) const { return( m_ever_rendered ); }
    bool needs_render( ) { return( m_needs_render || !m_ever_rendered ); }

    FixedRectangle drawing_bounds( dec in_z = 1.0 ) { render( ); return( bounding_box( ) / in_z ); }
    FixedRectangle drawing_bounds_z( ) { return( drawing_bounds( z( ) ) ); }

    bool visible( ) const { return( !empty( ) || !light_sources( ).empty( ) ); } // C! something drawn or giving off light

    varray<LightSource> cref light_sources( ) const { return( m_light_sources ); }
    void clear_light_sources( ) { m_light_sources.clear( ); }
    void move_light_sources( Vector cref in_movement ) { for_each( light_source, m_light_sources ) { light_source.position( light_source.position( ) + in_movement ); } } // C! with the object, when it's moved without updating (going through a door)

    LayerOrder cref layer_order( ) const { return( m_layer_order ); }
    void layer_order( LayerOrder cref in_layer_order ) { m_layer_order = in_layer_order; }

    // todo better name for this func
    virtual bool layer_order_above( ptr<const Visible> in_visible ) const { return( false ); }

    static bool sort( ptr<const Visible> cref in_v1, ptr<const Visible> cref in_v2 )
    {
        if( in_v1->z( ) != in_v2->z( ) )
        {
            // sorting doesn't matter if objects aren't on the same z plane
            return( in_v1->z( ) < in_v2->z( ) );
        }
        else
        {
            LayerOrder cref order1 = in_v1->layer_order( );
            LayerOrder cref order2 = in_v2->layer_order( );

            // C! the first level settles it nearly every time, so the orders are only copied (to pop through) when it doesn't
            return_false_if( is_empty( order1 ) && is_empty( order2 ) );
            return_if( is_empty( order1 ) || is_empty( order2 ), !is_empty( order1 ) );
            return_if( order1.front( ) != order2.front( ), order1.front( ) > order2.front( ) );
            return_if( ( order1.size( ) == order2.size( ) ) && is_one( order1.size( ) ), in_v1->layer_order_above( in_v2 ) );

            LayerOrder o1 = order1;
            LayerOrder o2 = order2;

            while( !is_empty( o1 ) || !is_empty( o2 ) )
            {
                if( !is_empty( o1 ) && !is_empty( o2 ) )
                {
                    if( o1.front( ) == o2.front( ) )
                    {
                        if( ( o1.size( ) == o2.size( ) ) && is_one( o1.size( ) ) )
                        {
                            return( in_v1->layer_order_above( in_v2 ) );
                        }
                        else
                        {
                            // top levels are equal, keep looking

                            o1.pop( ); o2.pop( );
                        }
                    }
                    else
                    {
                        return( o1.front( ) > o2.front( ) );
                    }
                }
                else if( !is_empty( o1 ) )
                {
                    return( true );
                }
                else // if( !is_empty( o2 ) )
                {
                    return( false );
                }
            }
        }

        return( false );
    }

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

    void add_light_source( Point cref in_position, Planc cref in_radius, Planc cref in_flicker = P0, Color cref in_tint = CLEAR ) { m_light_sources.insert_back( LightSource( in_position, in_radius, in_tint, dec( in_flicker ) ) ); }

    LightSource cref add_light_source( LightSource cref in_light_source ) { m_light_sources.insert_back( in_light_source ); return( in_light_source ); }
    varray<LightSource> cref add_light_sources( varray<LightSource> cref in_light_sources ) { m_light_sources.insert_back( in_light_sources ); return( in_light_sources ); }

private:

    dec m_z = 1.0; // 1 = player plane, <1 = background, >1 = foreground
    LayerOrder m_layer_order;

    varray<LightSource> m_light_sources;

    bool m_needs_render = true;
    bool m_needs_render_always = false;
    bool m_persist_render = false;
    bool m_ever_rendered = false;

};

} // namespace graphics
} // namespace axn

#endif /* Visible_hpp */
