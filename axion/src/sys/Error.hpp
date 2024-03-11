#ifndef Error_hpp
#define Error_hpp

#define return_err( x )     \
    if( err e = (err)x )    \
    return e

namespace axn {
namespace system {

enum err { no_error = 0, error_not_init, error_unimplemented, error_todo };

}
} // namespace axn

#endif /* Error_hpp */
