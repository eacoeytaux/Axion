#ifndef Damage_hpp
#define Damage_hpp

#include "World.hpp"

namespace axn
{
namespace reality
{

class Damage
{

public:

    Damage( ) { }
    Damage( dec damage ) : m_damage( damage ) { }

    dec value( ) const { return m_damage; }

private:

    dec m_damage = 0.0;
    
};

} // namespace reality
} // namespace axn

#endif /* Damage_hpp */
