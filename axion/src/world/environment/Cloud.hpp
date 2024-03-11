#ifndef Cloud_hpp
#define Cloud_hpp

#include "axn.hpp"
#include "Object.hpp"

namespace axn {
namespace reality {

class Cloud : public Object {
private:
    struct Puff {
    public:
        Puff( const planc & radius, const Vector & center_offset );
        planc m_radius;
        Vector m_center_offset;
    };

public:
    virtual ~Cloud( ) {}
    Cloud( World* );

    virtual const Cloud & render( ) const override;

    static planc max_dx( );
    static planc max_dy( );

private:
    varray<Puff> m_puffs;
};

} // namespace reality
} // namespace axn

#endif /* Cloud_hpp */
