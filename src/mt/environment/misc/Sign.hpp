#ifndef Sign_hpp
#define Sign_hpp

#include "mt/MountMerciless.hpp"
#include "axn/world/Object.hpp"

namespace mtmercy
{

class Sign : public Object
{

public:

    Sign( ptr<Room> in_room, Point cref in_base );

    virtual void render( ) override;

    virtual bool collide( ptr<Object> in_object ) override;

private:

    bool m_reading = false;

    Drawing m_sign_drawing;
    Drawing m_sign_message;

};

} // namespace mtmercy

#endif /* Sign_hpp */
