#ifndef Namespaces_hpp
#define Namespaces_hpp

namespace axn
{

namespace utility { }
namespace geometry { }
namespace graphics { }
namespace audio { }
namespace physics { }
namespace reality { }

} // namespace axn

#define AXN_NAMESPACES             \
    using namespace axn::utility;  \
    using namespace axn::geometry; \
    using namespace axn::graphics; \
    using namespace axn::audio;    \
    using namespace axn::physics;  \
    using namespace axn::reality;  \
    using namespace axn;

AXN_NAMESPACES

#endif /* Namespaces_hpp */
