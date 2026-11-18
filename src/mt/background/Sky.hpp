#ifndef Sky_hpp
#define Sky_hpp

#include "mt/MountMerciless.hpp"
#include "mt/background/Cloud.hpp"

namespace mtmercy
{

class Sky : public Object
{

public:

    Sky( ptr<Room> in_room );

    virtual void render( ) override;
    virtual void update( ) override;

protected:

    virtual ptr<Cloud> add_cloud( );

private:

    bool m_has_clouds;

    varray<ptr<Cloud>> m_clouds;

    Countdown m_cloud_timer;

    Color m_top_color;
    Color m_mid_color;
    Color m_bottom_color;

};

} // namespace mtmercy

#endif /* Sky_hpp */
