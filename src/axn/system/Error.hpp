#ifndef Error_hpp
#define Error_hpp

#include "axn/system/STD.hpp"
#include "axn/system/Debug.hpp"

#define catch_error( ... ) catch( exception cref error ) { Log( ERROR_LOG, "error! ( %s )", error.what( ) ); __VA_ARGS__ } macro_end
#define catch_error_return( ... ) catch_error( __VA_ARGS__; return( Error( error.what( ) ) ); ); macro_end
#define catch_error_throw( ... ) catch_error( __VA_ARGS__; throw( error ); ); macro_end

namespace axn
{

class Error
{

private:

    uint m_code = 0;
    uint m_sub_code = 0;

    string m_message = nostring;

public:

    Error( string in_message = nostring ) : m_message( in_message ) { }
    Error( uint in_code, string in_message = nostring ) : m_code( in_code ), m_message( in_message ) { }
    Error( uint in_code, uint in_sub_code, string in_message = nostring ) : m_code( in_code ), m_sub_code( in_sub_code ), m_message( in_message ) { }

    operator bool( ) const { return( code( ) || message( ).length( ) ); }

    operator string( ) const { return( message( ) ); }

    uint code( ) const { return( m_code ); }
    uint sub_code( ) const { return( m_sub_code ); }

    string cref message( ) const { return( m_message ); }

};

var_const( Error no_error( 0 ) );
var_const( Error error_system( 1 ) );
var_const( Error error_not_init( -1 ) );

} // namespace axn

#endif /* Error_hpp */
