#ifndef Error_hpp
#define Error_hpp

#include "axn/system/Debug.hpp"

namespace axn
{

enum error
{
    no_error = 0,
    error_system,
    error_not_init,
    error_unimplemented,
};

} // namespace axn

#endif /* Error_hpp */
