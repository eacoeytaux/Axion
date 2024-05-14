#ifndef Visible_hpp
#define Visible_hpp

#include "Geometry.hpp"
#include "Color.hpp"
#include "Drawing.hpp"
#include "LightSource.hpp"

namespace axn
{
namespace graphics
{

class Visible
{
public:
    virtual ~Visible( ) { }

    Visible( double z = 1.0, uint layer_position = 0 ) { *this = Visible( Drawing( ), z, layer_position ); }
    Visible( const Drawing & drawing, double z = 1.0, uint layer_position = 0 )
    {
        m_drawing = drawing;
        this->z( z );
        this->layer_position( layer_position );
    }

    virtual const Visible & render( ) const
    {
        m_ever_rendered = true;
        if( !persist_drawing( ) )
            clear_drawing( );
        return *this;
    }

    bool ever_rendered( ) const { return m_ever_rendered; }
    bool needs_render( ) const { return ( drawing_dirty( ) || !ever_rendered( ) ); }

    const Drawing & drawing( ) const
    {
        if( !m_drawing_always_dirty )
            drawing_dirty( false );
        return m_drawing;
    }

    const Visible & clear_drawing( bool reserve_mem = true ) const
    {
        m_drawing.clear( reserve_mem );
        return *this;
    }

    bool visible( ) const { return drawing( ).polygon_count( ) || lights( ).size( ); }

    bool drawing_dirty( ) const { return m_drawing_dirty; }

    const varray<LightSource> & lights( ) const { return m_lights; }
    Visible & clear_light_sources( )
    {
        m_lights.clear( );
        return *this;
    }

    uint layer_position( ) const { return m_layer_position; }
    double z( ) const { return m_z; }

protected:
    const Visible & z( double z ) const
    {
        m_z = z;
        return *this;
    }

    const Visible & layer_position( uint layer_position ) const
    {
        m_layer_position = layer_position;
        return *this;
    }

    const Visible & add_light_source( const Coordinate & position, Planc radius, const Color & tint = TRANSPARENT, double flicker = 0.0 ) const
    {
        m_lights.insert_back( LightSource( position, radius ) );
        return *this;
    }

    const Visible & move_drawing( const Vector & offset ) const
    {
        m_drawing.move( offset );
        return *this;
    }

    const Visible & position_drawing( const Coordinate & position ) const
    {
        m_drawing.center( position );
        return *this;
    }

    const Visible & drawing( const Drawing & drawing ) const
    {
        m_drawing = drawing;
        return *this;
    }

    const Visible & draw( const Drawing & drawing ) const
    {
        m_drawing.draw( drawing );
        return *this;
    }

    const Visible & draw( const varray<Color> & colors, const Shape & shape, double thickness = FILLED ) const
    {
        m_drawing.draw( colors, shape, thickness );
        return *this;
    }
    const Visible & draw( const Color & color, const Shape & shape, double thickness = FILLED ) const
    {
        m_drawing.draw( color, shape, thickness );
        return *this;
    }

    const Visible & draw( const Color & color1, const Color & color2, const Line & line, double thickness = FILLED ) const
    {
        m_drawing.draw( color1, color2, line, thickness );
        return *this;
    }

    const Visible & draw( const Color & color, const Line & line, double thickness = FILLED ) const
    {
        m_drawing.draw( color, line, thickness );
        return *this;
    }

#ifdef AXN_DEBUG
    const Visible & draw( const Color & color, const Vector & vector, uint arrow_head_length, double thickness ) const
    {
        m_drawing.draw( color, vector, arrow_head_length, thickness );
        return *this;
    }
#endif

    bool has_border( ) const { return m_drawing.has_border( ); }
    Color border_color( ) const { return m_drawing.border_color( ); }
    Planc border_width( ) const { return m_drawing.border_width( ); }
    Visible & border_color( const Color & color )
    {
        m_drawing.border_color( color );
        return *this;
    }
    Visible & border_width( Planc width )
    {
        m_drawing.border_width( width );
        return *this;
    }
    Visible & border( const Color & color, const Planc & width )
    {
        m_drawing.border( color, width );
        return *this;
    }

    bool persist_drawing( ) const { return m_persist_drawing; }
    const Visible & persist_drawing( bool persist_drawing ) const
    {
        m_persist_drawing = persist_drawing;
        return *this;
    }

    const Visible & drawing_dirty( bool drawing_dirty ) const
    {
        m_drawing_dirty = drawing_dirty || m_drawing_always_dirty;
        return *this;
    }
    const Visible & drawing_always_dirty( bool drawing_always_dirty ) const
    {
        m_drawing_always_dirty = drawing_always_dirty;
        if( m_drawing_always_dirty )
            drawing_dirty( true );
        return *this;
    }

private:
    mutable double m_z = 1.0;     // 1 = player plane, [0,1) = background, >1 = foreground
    mutable int m_layer_position; // determines show order within layer, higher value shows on top of lower values

    mutable varray<LightSource> m_lights;

    mutable Drawing m_drawing;
    mutable bool m_drawing_dirty = true;
    mutable bool m_drawing_always_dirty = false; // TODO this necessary?
    mutable bool m_persist_drawing = false;
    mutable bool m_ever_rendered = false;
};

} // namespace graphics
} // namespace axn

#endif /* Visible_hpp */
