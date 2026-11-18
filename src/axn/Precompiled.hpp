#ifndef Precompiled_hpp
#define Precompiled_hpp

// force-included into every .cpp and compiled once per build config (see Axion.vcxproj), so only add headers almost every file uses
// changing any header in here still rebuilds everything, same as before

#include "axn/AXN.hpp"

#include "axn/world/World.hpp"
#include "axn/world/Object.hpp"
#include "axn/world/Terrain.hpp"

#endif /* Precompiled_hpp */
