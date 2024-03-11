#ifndef Utility_hpp
#define Utility_hpp

#include "Includes.hpp"
#include "Primitives.hpp"

#include "Error.hpp"
#include "Assert.hpp"

#include "Logger.hpp"
#include "Random.hpp"
#include "Clock.hpp"

#include "Primitives.hpp"
#include "Helpers.hpp"

#include "Counter.hpp"
#include "Slider.hpp"

#ifdef AXN_DEBUG
namespace axn {
class Debug {
public:
    static bool active;
};
} // namespace axn
#endif

#define AXN_SKIP_OVER if( false )

#endif /* Utility_hpp */
