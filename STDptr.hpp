#ifndef STDptr_hpp
#define STDptr_hpp

#include "OS.hpp"
#include "STD.hpp"

#include "Assert.hpp"

namespace axn
{

template <class T>
class ptr : public std::shared_ptr<T *>
{
    using std::shared_ptr<T>::shared_ptr;

public:

    operator bool( ) const { return ( std::shared_ptr<T *>::get( ) != nullptr ); }
};

template <class T>
ptr<T> make_ptr( T * t )
{
    return ptr<T *>( t );
}

template <class T, class U>
ptr<T> is_ptr( const ptr<U> & u ) noexcept
{
    return std::dynamic_pointer_cast<T *>( u );
}

} // namespace axn

#endif /* STDptr_hpp */
