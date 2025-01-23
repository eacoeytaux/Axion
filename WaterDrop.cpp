#include "WaterDrop.hpp"

using mtmercy::WaterDrop;

WaterDrop::WaterDrop( Room * room, Coordinate cref position ) : Object( room ) { }

void WaterDrop::render( ) { Object::render( ); }
void WaterDrop::update( ) { Object::update( ); }
