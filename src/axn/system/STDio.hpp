#ifndef STDio_hpp
#define STDio_hpp

#include "axn/system/OS.hpp"
#include "axn/system/STD.hpp"
#include "axn/system/STDc.hpp"
#include "axn/system/Defines.hpp"
#include "axn/system/Assert.hpp"
#include "axn/system/Utility.hpp"

namespace axn
{

inline ifstream file_in( string cref filename ) { return( ifstream( charify( filename ) ) ); }
inline ofstream file_out( string cref filename ) { return( ofstream( charify( filename ) ) ); }

class Serializable
{

public:

    virtual void deserialize( ifstream in_istream ) const { };
    virtual void serialize( ofstream in_ostream ) const { };

};

} // namespace axn

#endif /* STDio_hpp */
