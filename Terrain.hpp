#ifndef Terrain_hpp
#define Terrain_hpp

#include "Visible.hpp"

namespace axn
{
namespace reality
{

class Room;

class Terrain : public Visible
{

public:

    class Node;

    class Bumper
    {

    private:

        const Node * m_parent = nullptr;

        Path m_bumpers;

    public:

        Bumper( ) { }

        Bumper( const Node * parent, Path cref bumpers ) : m_parent( parent ), m_bumpers( bumpers ) { }

        const Node * parent( ) const { return ( m_parent ); }

        Path cref path( ) const { return ( m_bumpers ); }

    };

    class Node
    {

    private:

        const Terrain * m_parent = nullptr;

        bool m_passable = false;

    public:

        Node( ) { }

        Node( const Terrain * parent ) : m_parent( parent ) { }

        const Terrain * parent( ) { return ( m_parent ); }

        virtual Node * front( ) const = 0;
        virtual Node * back( ) const = 0;

        uint type( ) const { return ( m_parent->type( this ) ); }

        bool passable( ) const { return ( m_passable ); }

        virtual Angle normal( ) const = 0;

        virtual FixedRectangle bounding_box( ) const = 0;

        virtual Bumper bumpers( Polygon cref ) const = 0;

    };

    class Vertex;
    class Edge;

    class Vertex : public Node
    {

    private:

        Coordinate m_position;

        Edge * m_e1 = nullptr;
        Edge * m_e2 = nullptr;

    public:

        Vertex( const Terrain * parent, Coordinate cref pos );

        Coordinate cref position( ) const;

        Edge * edge1( ) const;
        Edge * edge2( ) const;

        void edge1( Edge * );
        void edge2( Edge * );

        Node * front( ) const override { return edge1( ); }
        Node * back( ) const override { return edge2( ); }

        Angle normal( ) const override;

        FixedRectangle bounding_box( ) const override;

        Bumper bumpers( Polygon cref ) const override;

    };

    class Edge : public Node
    {

    private:

        Vertex * m_v1 = nullptr;
        Vertex * m_v2 = nullptr;

    public:

        Edge( const Terrain * parent, Vertex * v1, Vertex * v2, bool passable = false ); // todo make constant or variable

        Line line( ) const;
        Vector vector( ) const;

        Vertex * vertex1( ) const;
        Vertex * vertex2( ) const;

        Node * front( ) const override { return vertex1( ); }
        Node * back( ) const override { return vertex2( ); }

        Angle normal( ) const override;

        FixedRectangle bounding_box( ) const override;

        Bumper bumpers( Polygon cref ) const override;

    };

public:

    virtual ~Terrain( );

    #if defined ( AXN_DEBUG )
    bool draw_debug = true;
    Color draw_debug_color = CYAN;
    virtual Drawing debug_overlay( ) const;
    #endif

    Room * room( ) const { return m_room; }

    varray<varray<Vertex *>> cref vertices( ) const;
    varray<varray<Edge *>> cref edges( ) const;

    varray<Polygon> cref terrain( ) const;

    virtual uint type( const Node * t ) const;

    void traverse_x( const Span<Planc> & _distance_x, const function<void( Coordinate cref, const Edge * )> & f ) const;
    void traverse_x( Planc cref _distance_x, const function<void( Coordinate cref, const Edge * )> & f ) const { traverse_x( Span<Planc>( _distance_x, _distance_x ), f ); }

protected:

    Terrain( Room *, const varray<varray<Coordinate>> & vertices = { } );

    virtual void typeset( const Node * t, uint type );

    void make( varray<Coordinate> cref positions, Direction bound_direction = NO_DIRECTION );

    void crop_terrain( );
    void fill_terrain( Color cref );

private:

    Room * m_room = nullptr;

    varray<varray<Vertex *>> m_vertices;
    varray<varray<Edge *>> m_edges;

    varray<Polygon> m_terrain;

    umap<const Node *, uint> m_terrain_type_map;

};

} // namespace reality
} // namespace axn

#endif /* Terrain_hpp */
