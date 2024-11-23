#ifndef Error_hpp
#define Error_hpp

#include "Debug.hpp"

namespace axn
{

enum error
{
    no_error = 0,
    error_system,
    error_not_init,
    error_unimplemented,
};

#define return_error( x )    \
    if( error e = (error)x ) \
    return e

} // namespace axn

#endif /* Error_hpp */
