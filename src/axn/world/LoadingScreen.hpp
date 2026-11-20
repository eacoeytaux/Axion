#ifndef LoadingScreen_hpp
#define LoadingScreen_hpp

#include "axn/world/World.hpp"

namespace axn
{

// C! a black screen with a spinner, which Engine shows in place of a world while that world is loading( )
// C! (see World::start_loading( )). It runs on the main loop while the load may run on its own thread, so it mustn't
// C! use anything the load does (Random, or the loading world itself); see etc/threading-plan.md
class LoadingScreen : public World
{

public:

    virtual string name( ) const override { return( "LoadingScreen" ); }

protected:

    virtual void create( ) override;

    virtual bool loads_in_background( ) const override { return( false ); }

};

} // namespace axn

#endif /* LoadingScreen_hpp */
