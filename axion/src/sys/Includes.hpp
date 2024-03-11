#ifndef Namespaces_hpp
#define Namespaces_hpp

// #define _USE_MATH_DEFINES
#include <cmath>
#include <memory>
#include <iostream>
#include <vector>
#include <stack>
#include <queue>
#include <list>
#include <unordered_map>

namespace axn {

// axn namespaces
namespace system {}
namespace utility {}
namespace geometry {}
namespace graphics {}
namespace audio {}
namespace physics {}
namespace reality {}

// general std
using std::dynamic_pointer_cast;
using std::list;
using std::make_shared;
using std::queue;
using std::shared_ptr;
using std::stack;
using std::string;
using std::unordered_map;

// system std
using std::cout;
using std::endl;
using std::exception;
using std::runtime_error;

// math std
using std::acos;
using std::asin;
using std::atan;
using std::atan2;
using std::cos;
using std::sin;
using std::sqrt;
using std::tan;

const float POSITIVE_INFINITY = INFINITY;
const float NEGATIVE_INFINITY = -INFINITY;

} // namespace axn

#define AXN_NAMESPACES              \
    using namespace axn::system;    \
    using namespace axn::utility;   \
    using namespace axn::geometry;  \
    using namespace axn::graphics;  \
    using namespace axn::audio;     \
    using namespace axn::physics;   \
    using namespace axn::reality;   \
    using namespace axn;

AXN_NAMESPACES

#endif /* Namespaces_hpp */
