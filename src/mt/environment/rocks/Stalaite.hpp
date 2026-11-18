#ifndef Stalite_hpp
#define Stalite_hpp

#include "mt/MountMerciless.hpp"
#include "axn/world/Object.hpp"

#include "mt/environment/water/WaterDrop.hpp"

namespace mtmercy
{

template( bool hanging ) class Stalaite : public Object
{

public:

    Stalaite( ptr<Room> in_room, Color cref in_color, Point cref in_base, Planc cref in_width, Planc cref in_height, dec in_z = 1.0 ) : Object( in_room, in_base, in_z ), m_width( in_width ), m_height( in_height ), m_color( in_color )
    {
        foreground( true );
        persist_render( true );
        stationary( true );
        solid( false );
        interactive( false );

        space( Polygon( { VectorY( negated( height( ), hanging ) ), PointX( half( negated( -width( ), hanging ) ) ), PointX( half( negated( +width( ), hanging ) ) ) } ) );
        draw( color( ), space( ) );
    }

    Vector tip_offset( ) const { return( VectorY( negated( height( ), hanging ) ) ); }
    Point tip( ) const { return( position( ) + tip_offset( ) ); }

    Planc width( ) const { return( m_width ); }
    Planc height( ) const { return( m_height ); }

    Color cref color( ) { return( m_color ); }

private:

    Planc m_width;
    Planc m_height;

    Color m_color;

};

class Stalagmite : public Stalaite<false>
{

public:

    Stalagmite( ptr<Room> in_room, Color cref in_color, Point cref in_base, Planc cref in_width, Planc cref in_height, dec in_z = 1.0 ) : Stalaite( in_room, in_color, in_base, in_width, in_height, in_z ) { }

};

class Stalactite : public Stalaite<true>
{

public:

    Stalactite( ptr<Room> in_room, Color cref in_color, Point cref in_base, Planc cref in_width, Planc cref in_height, dec in_z = 1.0 ) : Stalaite( in_room, in_color, in_base, in_width, in_height, in_z ) { }
    Stalactite( ptr<Room> in_room, Color cref in_color, Point cref in_base, Planc cref in_width, Planc cref in_height, bool dripping, dec in_z = 1.0 ) : Stalaite( in_room, in_color, in_base, in_width, in_height, in_z )
    {
        if( dripping )
        {
            room( )->add_object( new WaterDropper( room( ), tip( ), z( ) ) );
        }
    }

};

} // namespace mtmercy

#endif /* Stalite_hpp */
