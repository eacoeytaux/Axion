#ifndef Plains_hpp
#define Plains_hpp

#include "Room.hpp"

namespace mtmercy
{

class Plains : public Room
{

public:

    Plains( World * world ) : Room( world ) { }
    
protected:

    virtual void create( ) override;
    
    virtual Terrain * generate_terrain( ) override;

    virtual Player * add_player( Coordinate cref position ) override;
    virtual Player * create_player( Coordinate cref position ) override;
};

} // namespace mtmercy

#endif /* Plains_hpp */
