#ifndef Cloud_hpp
#define Cloud_hpp

#include "Object.hpp"

namespace mtmercy
{

class Cloud : public Object
{
private:
    struct Puff
    {
        Planc radius;
        Vector center_offset;

        default_equal( Puff );
    };

public:
    Cloud( Room * );

    void render( ) override;
    void update( ) override;

private:
    Drawing m_cloud_drawing;
    static Planc max_dx( );
    static Planc max_dy( );
};

} // namespace mtmercy

#endif /* Cloud_hpp */
