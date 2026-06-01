#ifndef TrainingGrounds_hpp
#define TrainingGrounds_hpp

#include "Room.hpp"

namespace mtmercy
{

class TrainingGrounds : public Room
{

public:

    TrainingGrounds( World * world ) : Room( world ) { }

protected:

    virtual void create( ) override;

    virtual Terrain * generate_terrain( ) override;

    virtual Player * add_player( Coordinate cref position ) override;
    virtual Player * create_player( Coordinate cref position ) override;

};

} // namespace mtmercy

#endif /* TrainingGrounds_hpp */
