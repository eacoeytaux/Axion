#ifndef Sky_hpp
#define Sky_hpp

#include "MountMerciless.hpp"
#include "Cloud.hpp"

namespace mtmercy
{

class Sky : public Object
{

public:

    Sky( Room * );

    virtual void render( ) override;
    virtual void update( ) override;

private:

    bool m_has_clouds;
    varray<Cloud *> m_clouds;

    Countdown m_cloud_timer;
    Color m_top_color;
    Color m_mid_color;
    Color m_bottom_color;
};

} // namespace mtmercy

#endif /* Sky_hpp */
