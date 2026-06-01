#ifndef Namespaces_hpp
#define Namespaces_hpp

namespace axn
{

namespace audio { }
namespace graphics { }
namespace geometry { }
namespace physics { }
namespace reality { }
namespace utility { }

} // namespace axn

#define AXN_NAMESPACES             \
    using namespace axn::audio;    \
    using namespace axn::graphics; \
    using namespace axn::geometry; \
    using namespace axn::physics;  \
    using namespace axn::reality;  \
    using namespace axn::utility;  \
    using namespace axn;

AXN_NAMESPACES

#endif /* Namespaces_hpp */
