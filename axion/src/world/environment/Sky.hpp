#ifndef Sky_hpp
#define Sky_hpp

#include "axn.hpp"
#include "Background.hpp"
#include "Cloud.hpp"

namespace axn {
namespace reality {

class Sky : public Background {
public:
    virtual ~Sky( ) {}
    Sky( World*, const int cloud_regen_rate = 0 );

    virtual const Sky & render( ) const override;
    virtual Sky & update( ) override;

private:
    varray<Cloud*> m_clouds;
    int m_cloud_regen_rate; // 0 means never, 1 means most clouds, high number
                            // means few
    Counter m_cloud_timer;
    Color m_top_color;
    Color m_bottom_color;
};

} // namespace reality
} // namespace axn

#endif /* Sky_hpp */
