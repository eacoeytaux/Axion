#ifndef Event_hpp
#define Event_hpp

#include "axn.hpp"

#include "Geometry.hpp"
#include "Object.hpp"

namespace axn {

class Event {
private:
    uint m_processed_count = 0;

public:
    bool processed( ) const { return ( processed_count( ) > 0 ); }
    uint processed_count( ) const { return m_processed_count; }
    virtual Event & process( Object* object = nullptr ) {
        ++m_processed_count;
        return *this;
    }
};

} // namespace axn

#endif /* Event_hpp */
