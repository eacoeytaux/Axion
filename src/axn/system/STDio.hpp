#ifndef STDio_hpp
#define STDio_hpp

#include "axn/system/OS.hpp"
#include "axn/system/STD.hpp"
#include "axn/system/STDc.hpp"
#include "axn/system/Defines.hpp"
#include "axn/system/Assert.hpp"

namespace axn
{

inline ifstream file_istream( string cref filename ) { return( ifstream( filename.c_str( ) ) ); }
inline ofstream file_ostream( string cref filename ) { return( ofstream( filename.c_str( ) ) ); }

class Serializable
{

public:

    virtual void deserialize( ifstream in_istream ) const { };
    virtual void serialize( ofstream in_ostream ) const { };

};

} // namespace axn

#endif /* STDio_hpp */
