#include "MountMerciless.hpp"

#include "Climber.hpp"
#include "MohawkMan.hpp"

AXN_NAMESPACES
using mtmercy::MountMerciless;

Player * MountMerciless::create_player( const Coordinate & _position ) {
    return new MohawkMan( this, _position );
}
