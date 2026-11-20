#ifndef STDptr_hpp
#define STDptr_hpp

#include "axn/system/OS.hpp"
#include "axn/system/STD.hpp"
#include "axn/system/Defines.hpp"
#include "axn/system/Assert.hpp"

namespace axn
{

#define ptr ptr

#define cast( in, ... ) ptr( in ).template as<__VA_ARGS__>( )
#define cast_const( in, ... ) cast( in, const __VA_ARGS__ )

#define cast_as( in, out, ... ) ptr<__VA_ARGS__> out = cast( in, __VA_ARGS__ )
#define cast_const_as( in, out, ... ) ptr<__VA_ARGS__> out = cast_const( in, __VA_ARGS__ )

classT ptr
{

private:

    mutable T1 * m_ptr = nullptr;

public:

    ptr( T1 * in_ptr = nullptr ) : m_ptr( in_ptr ) { }

    T1 * raw( ) const { return( m_ptr ); } // C! the pointer itself, null or not: the same as converting it, but says so where null is expected

    T1 & operator*( ) const { Assert( !null( ) ); return( *m_ptr ); }
    T1 * operator->( ) const { Assert( !null( ) ); return( m_ptr ); }

    operator T1 * ( ) const { return( m_ptr ); } // C! no null check: a null ptr turning into a null pointer is fine (comparing, hashing), unlike * and -> which use it

    operator bool( ) const { return( !null( ) ); }

    bool null( ) const { return( m_ptr == nullptr ); }

    // C! todo? make a define similar to typeT but like typeT2?

    // C! defined after the class, IntelliSense reports ptr<T2> as an incomplete type (E0070) while ptr is still being defined
    template( typename T2 ) operator ptr<T2>( ) const;

    template( typename T2 ) ptr<T2> as( ) const;

    equal_default( ptr<T1> );

};

typeT template( typename T2 ) ptr<T1>::operator ptr<T2>( ) const { return_null_if( null( ) ); return( as<T2>( ) ); }

typeT template( typename T2 ) ptr<T2> ptr<T1>::as( ) const { return_null_if( null( ) ); return( dynamic_cast< T2 * >( m_ptr ) ); }

typeT2 inline bool same( ) { return( std::same_as<T1, T2> ); }
typeT2 inline bool same( T1 cref in_x1, T2 cref in_x2 ) { return( same<T1, T2>( ) ); }
typeT2 inline bool same( ptr<T1> in_x1, ptr<T2> in_x2 ) { return( same<T1, T2>( ) ); }

typeT2 inline bool common( ) { return( std::common_with<T1, T2> ); }
typeT2 inline bool common( T1 cref in_x1, T2 cref in_x2 ) { return( common<T1, T2>( ) ); }
typeT2 inline bool common( ptr<T1> in_x1, ptr<T2> in_x2 ) { return( common<T1, T2>( ) ); }

typeT2 inline bool derived( ) { return( std::derived_from<T1, T2> ); }
typeT2 inline bool derived( T1 cref in_x1, T2 cref in_x2 ) { return( derived<T1, T2>( ) ); }
typeT2 inline bool derived( ptr<T1> in_x1, ptr<T2> in_x2 ) { return( derived<T1, T2>( ) ); }

typeT2 inline bool derives( ) { return( std::derived_from<T2, T1> ); }
typeT2 inline bool derives( T1 cref in_x1, T2 cref in_x2 ) { return( derives<T1, T2>( ) ); }
typeT2 inline bool derives( ptr<T1> in_x1, ptr<T2> in_x2 ) { return( derives<T1, T2>( ) ); }

typeT inline ptr<T1> new_ptr( ) { return( ptr( new T1( ) ) ); }

} // namespace axn

structT std::hash<axn::ptr<T1>> { size_t operator( )( axn::ptr<T1> in_ptr ) const { return( std::hash<T1 *>( )( in_ptr.raw( ) ) ); } };

#endif /* STDptr_hpp */
