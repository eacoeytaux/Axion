#include "IcePick.hpp"

AXN_NAMESPACES
using axn::reality::IcePick;

const planc HANDLE_LENGTH = 16.f;
const ufloat HANDLE_THICKNESS = 5.f;
const planc HEAD_OFFSET = 8.f;
const planc HEAD_LENGTH = 18.f;
const Color HANDLE_COLOR = Color::rgb(198,145,70);
const Color METAL_COLOR = GRAY_MID;

IcePick::IcePick( World * world, const Coordinate & _position )
: Object( world, _position )
{ }

const IcePick & IcePick::render( ) const {
    Object::render( );
    
    Coordinate handle_base = position() + Vector( (planc)-10.f, (planc)-8.f );
    draw( HANDLE_COLOR, Line( handle_base, handle_base - Vector( (planc)0.f, HANDLE_LENGTH ) ), HANDLE_THICKNESS );

    Shape head = Shape( { handle_base - Vector( HEAD_OFFSET, HANDLE_LENGTH + HANDLE_THICKNESS * 1.5f ), handle_base - Vector( HEAD_OFFSET, HANDLE_LENGTH ), handle_base - Vector( HEAD_OFFSET - HEAD_LENGTH, HANDLE_LENGTH ) } );
    draw( METAL_COLOR, head );
    
    return *this;
}
