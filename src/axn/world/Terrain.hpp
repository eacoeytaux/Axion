#ifndef Terrain_hpp
#define Terrain_hpp

#include "axn/graphics/Visible.hpp"

#include "axn/world/Hitbox.hpp"

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

        ptr<const Node> m_parent = nullptr;

        Path m_bumpers;

    public:

        Bumper( ) { }

        Bumper( ptr<const Node> in_parent, Path cref in_bumpers ) : m_parent( in_parent ), m_bumpers( in_bumpers ) { }

        ptr<const Node> parent( ) const { return( m_parent ); }

        Path cref path( ) const { return( m_bumpers ); }

    };

    class Node
    {

    private:

        ptr<const Terrain> m_parent = nullptr;

        bool m_passable = false;

    public:

        Node( ) { }

        Node( ptr<const Terrain> in_parent ) : m_parent( in_parent ) { }

        ptr<const Terrain> parent( ) { return( m_parent ); }

        virtual ptr<Node> front( ) const = 0;
        virtual ptr<Node> back( ) const = 0;

        uint type( ) const { return( m_parent->type( this ) ); }

        bool passable( ) const { return( m_passable ); }

        virtual Angle normal( ) const = 0;

        virtual FixedRectangle bounding_box( ) const = 0;

        virtual Bumper bumpers( Hitbox cref in_hitbox ) const = 0;

    };

    class Vertex;
    class Edge;

    class Vertex : public Node
    {

    private:

        Point m_position;

        ptr<Edge> m_e1 = nullptr;
        ptr<Edge> m_e2 = nullptr;

    public:

        Vertex( ptr<const Terrain> in_parent, Point cref in_pos );

        Point cref position( ) const;

        ptr<Edge> edge1( ) const;
        ptr<Edge> edge2( ) const;

        void edge1( ptr<Edge> in_edge );
        void edge2( ptr<Edge> in_edge );

        ptr<Node> front( ) const override { return( edge1( ) ); }
        ptr<Node> back( ) const override { return( edge2( ) ); }

        Angle normal( ) const override;

        FixedRectangle bounding_box( ) const override;

        Bumper bumpers( Hitbox cref in_hitbox ) const override;

    };

    class Edge : public Node
    {

    private:

        ptr<Vertex> m_v1 = nullptr;
        ptr<Vertex> m_v2 = nullptr;

    public:

        Edge( ptr<const Terrain> in_parent, ptr<Vertex> in_v1, ptr<Vertex> in_v2, bool in_passable = false ); // todo make constant or variable

        Line line( ) const;
        Vector vector( ) const;

        ptr<Vertex> vertex1( ) const;
        ptr<Vertex> vertex2( ) const;

        ptr<Node> front( ) const override { return( vertex1( ) ); }
        ptr<Node> back( ) const override { return( vertex2( ) ); }

        Angle normal( ) const override;

        FixedRectangle bounding_box( ) const override;

        Bumper bumpers( Hitbox cref in_hitbox ) const override;

    };

public:

    virtual ~Terrain( );

    #if defined( AXN_DEBUG )
    bool draw_debug = true;
    Color draw_debug_color = CYAN;
    virtual Drawing debug_overlay( ) const;
    #endif

    ptr<Room> room( ) const { return( m_room ); }

    vvarray<ptr<Vertex>> cref vertices( ) const;
    vvarray<ptr<Edge>> cref edges( ) const;

    varray<Polygon> cref terrain( ) const;

    virtual uint type( ptr<const Node> t ) const;

    void traverse_x( Span<Planc> cref in_distance_x, function<void( Point cref in_pos, ptr<const Edge> in_edge )> cref in_func ) const;
    void traverse_x( Planc cref in_distance_x, function<void( Point cref in_pos, ptr<const Edge> in_edge )> cref in_func ) const { traverse_x( Span<Planc>( in_distance_x, in_distance_x ), in_func ); }

protected:

    Terrain( ptr<Room> in_room, vvarray<Point> cref in_vertices = { } );

    virtual void typeset( ptr<const Node> in_node, uint in_type );

    void make( varray<Point> cref in_positions, direction in_bound_direction = NO_DIRECTION );

    void fill_terrain( Color cref in_color );
    void crop_terrain( );

private:

    ptr<Room> m_room = nullptr;

    vvarray<ptr<Vertex>> m_vertices;
    vvarray<ptr<Edge>> m_edges;

    varray<Polygon> m_terrain;

    umap<ptr<const Node>, uint> m_terrain_type_map;

};

} // namespace reality
} // namespace axn

#endif /* Terrain_hpp */
