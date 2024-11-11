#ifndef Colors_hpp
#define Colors_hpp

#include "Graphics.hpp"

namespace mtmercy
{

class Colors
{
private:
    Colors() { }
    
public:
    virtual ~Colors() { }
    
    static void add( const string &, const Color & );
    static const Color & lookup( const string & );
    
private:
    static umap<string, Color> color_map;
};

}

#endif /* Colors_hpp */
