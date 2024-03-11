#ifndef Constants_hpp
#define Constants_hpp

#include "axn.hpp"
#include "Geometry.hpp"
#include "Graphics.hpp"

namespace axn {

const planc METER = 32.f;
const Vector GRAVITY = Vector( (planc)0.f, (planc)-2.5f );
const ufloat AIR_RESISTANCE = 0.001;

} // namespace axn

#endif /* Constants_hpp */
