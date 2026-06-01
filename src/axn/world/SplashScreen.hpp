#ifndef SplashScreen_hpp
#define SplashScreen_hpp

#include "axn/world/World.hpp"

namespace axn
{

class SplashScreen : public World
{

public:

    virtual string name( ) const override { return( "AXION-SplashScreen" ); }

    virtual bool quit( ) const override;

    virtual void input( list<ptr<Input>> cref in_inputs = { } ) override;

protected:

    virtual void create( ) override;

    virtual void complete( ) { m_complete = true; }

private:

    bool m_complete = false;

};

} // namespace axn

#endif /* SplashScreen_hpp */
