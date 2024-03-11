#ifndef Identifiable_hpp
#define Identifiable_hpp

#include "axn.hpp"

namespace axn {
namespace utility {

class Identifiable {
private:
    uint m_id;

public:
    Identifiable( ) {
        static uint total_ids = 0;
        static bool overflow = false;
        m_id = ++total_ids;
        if( !total_ids )
            overflow = true;
        Assert( !overflow );
    }
    uint id( ) const { return m_id; }
};

} // namespace utility
} // namespace axn

#endif /* Identifiable_hpp */
