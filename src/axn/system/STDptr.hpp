#ifndef STDptr_hpp
#define STDptr_hpp

#include "axn/system/OS.hpp"
#include "axn/system/STD.hpp"
#include "axn/system/Defines.hpp"
#include "axn/system/Assert.hpp"

#define ptr ptr

#define cast( Class, in ) in.as<Class>( )
#define cast_const( Class, in ) cast( const Class, in )

#define cast_as( out, Class, in ) ptr<Class> out = cast( Class, in )
#define cast_const_as( out, Class, in ) ptr<const Class> out = cast_const( Class, in )

namespace axn
{

classT ptr
{

protected:

    mutable T1 * m_ptr;

public:

    ptr( T1 * in_ptr = nullptr ) : m_ptr( in_ptr ) { }

    T1 * get( ) const { return( m_ptr ); }

    T1 * operator->( ) const { return( get( ) ); }
    T1 & operator*( ) const { return( *get( ) ); }

    typeT ptr<T1> as( ) const { return( dynamic_cast<T1 *>( get( ) ) ); }
    typeT operator ptr<T1>( ) const { return( dynamic_cast<T1 *>( get( ) ) ); }

    operator T1 * ( ) const { return( get( ) ); }

    operator bool( ) const { return( !null( ) ); }
    bool null( ) const { return( get( ) == nullptr ); }

    void del( ) const { delete( get( ) ); m_ptr = nullptr; }

    bool operator==( ptr<T1> in_other ) const { return( get( ) == in_other.get( ) ); }
    bool operator!=( ptr<T1> in_other ) const { return( get( ) != in_other.get( ) ); }

};

structT hash<ptr<T1>> { size_t operator( )( axn::ptr<T1> in_ptr ) const { return( hash<T1 *>( )( in_ptr.get( ) ) ); } };

} // namespace axn

#endif /* STDptr_hpp */
