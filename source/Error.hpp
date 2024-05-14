#ifndef Error_hpp
#define Error_hpp

#include "Debug.hpp"

namespace axn
{

// TODO

enum error
{
    no_error = 0,
    error_not_init,
    error_unimplemented,
    error_todo,
};

#define return_error( x )    \
    if( error e = (error)x ) \
    return e

} // namespace axn

#endif /* Error_hpp */
