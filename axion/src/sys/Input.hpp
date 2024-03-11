#ifndef Input_hpp
#define Input_hpp

#include "Geometry.hpp"

namespace axn {
namespace system {

struct Input {
    virtual void polymorphic( ) {}
};

struct KeyInput : public Input {
    typedef char KEY;
    enum DYNAMIC {
        NO_DYNAMIC,
        PRESSED,
        HELD,
        RELEASED,
    };

    KeyInput( KEY k, DYNAMIC d )
        : key( k + ( ( ( k >= 'A' ) && ( k <= 'Z' ) ) ? ( 'A' - 'a' ) : 0 ) ), dynamic( d ) {}

    const KEY key;
    const DYNAMIC dynamic;
};

struct MouseInput : public Input {
    enum BUTTON {
        NO_BUTTON,
        LEFT_BUTTON,
        RIGHT_BUTTON,
    };

    enum DYNAMIC {
        NO_DYNAMIC,
        PRESSED,
        HELD,
        RELEASED,
        MOVE,
    };

    MouseInput( BUTTON b, DYNAMIC d, const Coordinate & p )
        : button( b ), dynamic( d ), position( p ) {}

    const BUTTON button;
    const DYNAMIC dynamic;
    const Coordinate position;
};

struct ControllerInput : public Input {};

struct ControllerButtonInput : public ControllerInput {};
struct ControllerJoystickInput : public ControllerInput {};

} // namespace system
} // namespace axn

#endif /* Input_hpp */
