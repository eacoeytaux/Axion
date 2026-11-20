#ifndef Listener_hpp
#define Listener_hpp

#include "axn/AXN.hpp"

namespace axn
{
namespace audio
{

class Listener
{

protected:

    Listener( declmp in_volume = 1.0 ) : m_volume( in_volume ) { }

public:

    bool active( ) const { return( m_active ); }
    Listener ref active( bool in_active ) { m_active = in_active; rethis; }

    declmp volume( ) const { return( m_volume ); }
    Listener ref volume( declmp in_volume ) { m_volume = in_volume; rethis; }

    virtual Point listening_position( ) const = 0;

private:

    bool m_active = true;
    declmp m_volume = 1.0;

};

} // namespace audio
} // namespace axn

#endif /* Listener_hpp */
