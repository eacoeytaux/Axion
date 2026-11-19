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

    // C! where an object's center can be while its hitbox touches a node, and which node that is
    // C! an object's ground is a Bumper, a default Bumper (no node) means not grounded
    class Bumper
    {

    private:

        ptr<const Node> m_node;

        Path m_bumpers;

    public:

        Bumper( ) { }

        Bumper( ptr<const Node> in_node, Path cref in_bumpers ) : m_node( in_node ), m_bumpers( in_bumpers ) { }

        ptr<const Node> node( ) const { return( m_node ); }

        Path cref path( ) const { return( m_bumpers ); }

    };

    class Node
    {

    private:

        ptr<const Terrain> m_parent;

        bool m_passable = false;

    public:

        Node( ) { }

        Node( ptr<const Terrain> in_parent, bool in_passable = false ) : m_parent( in_parent ), m_passable( in_passable ) { }

        ptr<const Terrain> parent( ) { return( m_parent ); }

        virtual ptr<Node> front( ) const = 0;
        virtual ptr<Node> back( ) const = 0;

        uint type( ) const { return( m_parent->type( this ) ); }

        bool passable( ) const { return( m_passable ); }

        virtual Angle normal( ) const = 0;

        virtual Point center( ) const = 0; // C! a point on the node itself, its bumper lines all face away from it

        virtual FixedRectangle bounding_box( ) const = 0;

        // C! bumpers are built from the hitbox turned inside out, working that shape out is most of the cost,
        // C! so when building several bumpers for the same hitbox get it once with bumper_shape( ) and pass it in
        static varray<Point> bumper_shape( Hitbox cref in_hitbox );

        virtual Bumper bumpers( varray<Point> cref in_shape ) const = 0;
        Bumper bumpers( Hitbox cref in_hitbox ) const { return( bumpers( bumper_shape( in_hitbox ) ) ); }

        Angle bumper_normal( Line cref in_bumper_line ) const; // C! outward normal of one of this node's bumper lines

    };

    class Vertex;
    class Edge;

    class Vertex : public Node
    {

    private:

        Point m_position;

        ptr<Edge> m_e1;
        ptr<Edge> m_e2;

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

        Point center( ) const override { return( position( ) ); }

        FixedRectangle bounding_box( ) const override;

        using Node::bumpers;
        Bumper bumpers( varray<Point> cref in_shape ) const override;

    };

    class Edge : public Node
    {

    private:

        ptr<Vertex> m_v1;
        ptr<Vertex> m_v2;

    public:

        Edge( ptr<const Terrain> in_parent, ptr<Vertex> in_v1, ptr<Vertex> in_v2, bool in_passable = false ); // todo make constant or variable

        Line line( ) const;
        Vector vector( ) const;

        ptr<Vertex> vertex1( ) const;
        ptr<Vertex> vertex2( ) const;

        ptr<Node> front( ) const override { return( vertex1( ) ); }
        ptr<Node> back( ) const override { return( vertex2( ) ); }

        Angle normal( ) const override;

        Point center( ) const override { return( midpoint( m_v1->position( ), m_v2->position( ) ) ); }

        FixedRectangle bounding_box( ) const override;

        using Node::bumpers;
        Bumper bumpers( varray<Point> cref in_shape ) const override;

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

    Color cref fill_color( ) const { return( m_fill_color ); }

    void traverse_x( span<Planc> cref in_distance_x, function<void( Point cref in_pos, ptr<const Edge> in_edge )> cref in_func ) const;
    void traverse_x( Planc cref in_distance_x, function<void( Point cref in_pos, ptr<const Edge> in_edge )> cref in_func ) const { traverse_x( span<Planc>( in_distance_x, in_distance_x ), in_func ); }

protected:

    Terrain( ptr<Room> in_room, vvarray<Point> cref in_vertices = { } );

    virtual void typeset( ptr<const Node> in_node, uint in_type );

    void make( varray<Point> cref in_positions, Direction in_bound_direction = NO_DIRECTION );

    void fill_color( Color cref in_color, bool in_fill = true );

    void fill( );
    void crop( );

private:

    ptr<Room> m_room;

    vvarray<ptr<Vertex>> m_vertices;
    vvarray<ptr<Edge>> m_edges;

    varray<Polygon> m_terrain;

    umap<ptr<const Node>, uint> m_terrain_type_map;

    Color m_fill_color;

};

} // namespace reality
} // namespace axn

#endif /* Terrain_hpp */
