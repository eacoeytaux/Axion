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

    virtual const Arrow & render( ) const override;
    virtual Arrow & update( ) override;

    Angle angle( ) const;
    Arrow & angle( const Angle & );

    dec length( ) const;

protected:
    virtual Arrow & ground( TerrainEdge * ) override;
    virtual Arrow & react_to_movement( Object *, const Vector & ) override;
    virtual bool collide( Object * ) override;

    virtual void render_head( ) const;
    virtual void render_shaft( ) const;
    virtual void render_feather( ) const;

    dec fade_alpha( ) const;

private:
    dec m_damage;
    Angle m_angle;
    Color m_feather_color;
    Object * m_stuck_object;
};

} // namespace mtmercy

#endif /* Arrow_hpp */
