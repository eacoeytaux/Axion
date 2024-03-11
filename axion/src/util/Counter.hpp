#ifndef Counter_hpp
#define Counter_hpp

#include "axn.hpp"

namespace axn {
namespace utility {

class Counter {
public:
    virtual ~Counter( ) {}
    Counter( const uint countdown = 0 ) : m_countdown( countdown ) {}

    bool tick( ) {
        if( m_countdown ) {
            return !m_countdown--;
        } else {
            return true;
        }
    }
    Counter & reset( const uint countdown ) {
        m_countdown = countdown;
        return *this;
    }
    uint remaining( ) const { return m_countdown; }

private:
    uint m_countdown;
};

} // namespace utility
} // namespace axn

#endif /* Counter_hpp */
