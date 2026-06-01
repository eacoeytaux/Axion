#ifndef Sign_hpp
#define Sign_hpp

#include "MountMerciless.hpp"
#include "Object.hpp"

namespace mtmercy
{

class Sign : public Object
{

public:

    Sign( Room *, Coordinate cref base );

    virtual void render( ) override;

    virtual bool collide( Object * ) override;

private:

    bool m_reading = false;

    Drawing m_sign_drawing;
    Drawing m_sign_message;

};

} // namespace mtmercy

#endif /* Sign_hpp */
