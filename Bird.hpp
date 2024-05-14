#ifndef Bird_hpp
#define Bird_hpp

#include "Mob.hpp"

namespace mtmercy
{

class Feather : public Object
{
public:
    virtual ~Feather( ) { }
    Feather( World *, const Coordinate & position );

    virtual const Feather & render( ) const override;

private:
    Color m_color;
};

class Bird : public Mob
{
public:
    virtual ~Bird( ) { }
    Bird( World *, const Coordinate & position );

    virtual const Bird & render( ) const override;
    virtual Bird & update( ) override;

    virtual Bird & die( ) override;

private:
};

} // namespace mtmercy

#endif /* Bird_hpp */
