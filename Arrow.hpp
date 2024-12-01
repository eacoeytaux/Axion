#ifndef Arrow_hpp
#define Arrow_hpp

#include "Object.hpp"

namespace mtmercy
{

class Arrow : public Object
{
public:
    virtual ~Arrow( ) { }
    Arrow( World *, const Coordinate & tip, const Vector & launch_speed, const Color & feather_color );

    virtual void render( ) override;

    virtual void update( ) override;

    Angle angle( ) const;
    void angle( const Angle & );

    dec length( ) const;

protected:
    virtual void ground( TerrainEdge * ) override;
    virtual void react_to_movement( Object *, const Vector & ) override;
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
