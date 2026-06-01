#ifndef Cloud_hpp
#define Cloud_hpp

#include "mt/MountMerciless.hpp"
#include "axn/world/Object.hpp"

namespace mtmercy
{

class Cloud : public Object
{

public:

    Cloud( ptr<Room> in_room );

    void render( ) override;
    void update( ) override;

private:

    Drawing m_cloud_drawing;

    static Planc max_dx( );
    static Planc max_dy( );

};

} // namespace mtmercy

#endif /* Cloud_hpp */
