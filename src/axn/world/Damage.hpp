#ifndef Damage_hpp
#define Damage_hpp

#include "axn/world/World.hpp"

namespace axn
{
namespace reality
{

class Damage
{

public:

    Damage( ) { }

    Damage( dec in_damage, Vector cref in_force = V0 ) : m_damage( in_damage ), m_force( in_force ) { }

    Damage( Damage cref in_damage, Vector cref in_force = V0 ) : m_damage( in_damage.value( ) ), m_force( in_force ) { }

    dec value( ) const { return( m_damage ); }
    Vector cref force( ) const { return( m_force ); }

    Damage force( Vector cref in_force ) const { return( Damage( *this, in_force ) ); }

private:

    dec m_damage = 0.0;
    Vector m_force = V0;

};

} // namespace reality
} // namespace axn

#endif /* Damage_hpp */
