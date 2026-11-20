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

// C! something that can be written as one line of text and read back: each type gives a static deserialized( string ) for the reading
class Serializable
{

public:

    virtual ~Serializable( ) { }

    virtual string serialized( ) const = 0; // one line, no newline

};

// C! the words of in_text, split at in_delim, empty ones left out
inline varray<string> split( string cref in_text, char in_delim = ' ' )
{
    string word;
    varray<string> words;

    for_each_const( c, in_text )
    {
        if( c == in_delim )
        {
            if( !word.empty( ) )
            {
                words.insert_back( word );
            }

            word.clear( );
        }
        else
        {
            word += c;
        }
    }

    if( !word.empty( ) ) { words.insert_back( word ); }

    return( words );
}

inline string serialized_dec( dec in_x, dectol in_e = dectol( ) )
{
    char str[ 64 ];
    snprintf( str, sizeof( str ), "%.*f", int( decimal_places( in_e ) ), round_epsilon( in_x ) );

    string text( str );
    if( text.find( '.' ) != string::npos )
    {
        while( text.back( ) == '0' ) { text.pop_back( ); }
        if( text.back( ) == '.' ) { text.pop_back( ); }
    }

    return_if( text == "-0", "0" ); // C! a tiny negative rounds to -0
    return( text );
}

} // namespace axn

#endif /* STDio_hpp */
