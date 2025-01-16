#ifndef Arrow_hpp
#define Arrow_hpp

#include "Object.hpp"

namespace mtmercy
{

class Arrow : public Object
{
private:
    Arrow( Room *, Coordinate cref tip, Vector cref launch_speed, Color cref feather_color );

public:
    static Arrow tip( Room *, Coordinate cref tip, Vector cref launch_speed, Color cref feather_color );
    static Arrow base( Room *, Coordinate cref base, Vector cref launch_speed, Color cref feather_color );

    virtual void render( ) override;

    virtual void update( ) override;

    Angle angle( ) const;
    void angle( Angle cref );

    dec length( ) const;

protected:
    virtual void ground( TerrainEdge * ) override;
    virtual void react_to_movement( Object *, Vector cref ) override;
    virtual bool collide( Object * ) override;

    virtual void draw_head( );
    virtual void draw_shaft( );
    virtual void draw_feather( );

    dec fade_alpha( ) const;

private:
    dec m_damage;
    Angle m_angle;
    Color m_feather_color;
    Object * m_stuck_object;
};

} // namespace mtmercy

#endif /* Arrow_hpp */
