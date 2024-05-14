#ifndef STDc_hpp // std containers
#define STDc_hpp

#include "OS.hpp"
#include "STD.hpp"

#include "Assert.hpp"

namespace axn
{

using std::hash;

template <typename T>
class list : private std::list<T>
{
    using std::list<T>::list;

public:
    virtual ~list( ) { clear( ); }

    uint size( ) const
    {
        return (uint)std::list<T>::size( );
    }

    virtual T & at( uint index )
    {
        assert_index( index );
        auto it = std::list<T>::begin( );
        for( uint i = 0; i < index; ++i )
        {
            ++it;
        }
        return *it;
    }

    virtual const T & at( uint index ) const
    {
        assert_index( index );
        auto it = std::list<T>::cbegin( );
        for( uint i = 0; i < index; ++i )
        {
            ++it;
        }
        return *it;
    }

    T & front( )
    {
        assert_index( 0 );
        return at( 0 );
    }

    const T & front( ) const
    {
        assert_index( 0 );
        return at( 0 );
    }

    T & back( )
    {
        assert_index( size( ) - 1 );
        return at( size( ) - 1 );
    }

    const T & back( ) const
    {
        assert_index( size( ) - 1 );
        return at( size( ) - 1 );
    }

    T & insert( uint index )
    {
        return insert( T( ), index );
    }

    T & insert( const T & t, uint index )
    {
        if( !index )
        {
            std::list<T>::push_front( t );
        }
        else if( index == size( ) )
        {
            std::list<T>::push_back( t );
        }
        else
        {
            assert_index( index );
            auto it = std::list<T>::begin( );
            for( uint i = 0; i < index; ++i )
            {
                ++it;
            }
            std::list<T>::insert( it, t );
        }

        return at( index );
    }

    T & insert_back( const T & t = T( ) )
    {
        return insert( t, size( ) );
    }

    T & insert_front( const T & t = T( ) )
    {
        return insert( t, 0 );
    }

    virtual list<T> & clear( )
    {
        std::list<T>::clear( );
        return *this;
    }

    virtual list<T> & reverse( )
    {
        std::list<T>::reverse( );
        return *this;
    }

    virtual list<T> & sort( std::function<bool( const T & t1, const T & t2 )> comparator )
    {
        std::list<T>::sort( comparator );
        return *this;
    }

    virtual list<T> & remove_if( std::function<bool( const T & t )> checker )
    {
        std::list<T>::remove_if( checker );
        return *this;
    }

    auto begin( )
    {
        return std::list<T>::begin( );
    }

    auto end( )
    {
        return std::list<T>::end( );
    }

    auto begin( ) const
    {
        return std::list<T>::begin( );
    }

    auto end( ) const
    {
        return std::list<T>::end( );
    }

    T & operator[]( uint index )
    {
        return at( index );
    }

    const T & operator[]( uint index ) const
    {
        return at( index );
    }

    bool valid_index( uint index ) const
    {
        return index < size( );
    }

private:
    bool assert_index( uint index ) const
    {
        return Assert( valid_index( index ), "varray index (%ui) out of range, varray size: %ui", index, size( ) );
    }
};

using std::queue;
using std::stack;

template <typename K, typename C = std::less<K>>
class set : public std::set<K, C>
{
    using std::set<K, C>::set;

public:
    bool contains( const K & k ) const
    {
        return ( std::set<K, C>::find( k ) != std::set<K, C>::end( ) );
    }
};

template <typename K, typename H = std::hash<K>, typename E = std::equal_to<K>>
class uset : public std::unordered_set<K, H, E>
{
    using std::unordered_set<K, H, E>::unordered_set;

public:
    bool contains( const K & k ) const
    {
        return ( std::unordered_set<K, H, E>::find( k ) != std::unordered_set<K, H, E>::end( ) );
    }
};

using std::map;
template <typename K, typename V, typename H = std::hash<K>, typename E = std::equal_to<K>>
class umap : public std::unordered_map<K, V, H, E>
{
    using std::unordered_map<K, V, H, E>::unordered_map;

public:
    bool contains( const K & k ) const
    {
        return ( std::unordered_map<K, V, H, E>::find( k ) != std::unordered_map<K, V, H, E>::end( ) );
    }
};

// avoid confusion between std::vector and axn::Vector
template <typename T>
class varray : private std::vector<T>
{
    using std::vector<T>::vector;

public:
    virtual ~varray( ) { clear( true ); }

    varray copy( ) { return varray( this ); }
    varray copy( uint index_end ) { return copy( 0, index_end ); }
    varray copy( uint index_start, uint index_end )
    {
        if( index_start == index_end )
            return varray( );
        assert_index( index_start );
        assert_index( index_end + 1 );
        varray ret( index_end - index_start );
        for( uint index = index_start; index < index_end; ++index )
            ret.insert_back( index );
        return ret;
    }

    uint size( ) const
    {
        return (uint)std::vector<T>::size( );
    }

    virtual T & at( uint index )
    {
        assert_index( index );
        return std::vector<T>::at( index );
    }

    virtual const T & at( uint index ) const
    {
        assert_index( index );
        return std::vector<T>::at( index );
    }

    T & front( )
    {
        assert_index( 0 );
        return at( 0 );
    }

    const T & front( ) const
    {
        assert_index( 0 );
        return at( 0 );
    }

    T & back( )
    {
        assert_index( size( ) - 1 );
        return at( size( ) - 1 );
    }

    const T & back( ) const
    {
        assert_index( size( ) - 1 );
        return at( size( ) - 1 );
    }

    virtual T & insert( uint index )
    {
        return insert( T( ), index );
    }

    virtual T & insert( const T & t, uint index )
    {
        if( index == size( ) )
        {
            std::vector<T>::push_back( t );
        }
        else
        {
            assert_index( index );
            std::vector<T>::insert( begin( ) + index, t );
        }

        return at( index );
    }

    T & insert_back( const T & t = T( ) )
    {
        insert( t, size( ) );
        return back( );
    }

    T & insert_front( const T & t = T( ) )
    {
        insert( t, 0 );
        return front( );
    }

    varray<T> & insert_back( const varray<T> & v )
    {
        insert_varray( v, size( ) );
        return *this;
    }

    varray<T> & insert_front( const varray<T> & v )
    {
        insert_varray( v, 0 );
        return *this;
    }

    virtual varray<T> & insert_varray( const varray<T> & v, uint index )
    {
        if( index != size( ) )
            assert_index( index );
        std::vector<T>::insert( begin( ) + index, v.begin( ), v.end( ) );
        return *this;
    }

    varray<T> & remove_front( )
    {
        remove( 0 );
        return *this;
    }

    varray<T> & remove_back( )
    {
        remove( size( ) - 1 );
        return *this;
    }

    varray<T> & remove( uint index )
    {
        remove( index, index + 1 );
        return *this;
    }

    virtual varray<T> & remove( uint index_start, uint index_end )
    {
        assert_index( index_start );
        assert_index( index_end - 1 );
        std::vector<T>::erase( begin( ) + index_start, begin( ) + index_end );
        return *this;
    }

    varray<T> & resize( uint size )
    {
        std::vector<T>::resize( size );
        return *this;
    }

    varray<T> & resize_more( uint size )
    {
        resize( (uint)std::vector<T>::size( ) + size );
        return *this;
    }

    varray<T> & reserve( uint size )
    {
        std::vector<T>::reserve( size );
        return *this;
    }

    varray<T> & reserve_more( uint size )
    {
        reserve( (uint)std::vector<T>::size( ) + size );
        return *this;
    }

    virtual varray<T> & clear( bool shrink = false )
    {
        std::vector<T>::clear( );
        if( shrink )
            std::vector<T>::shrink_to_fit( );
        return *this;
    }

    virtual varray<T> & reverse( )
    {
        std::reverse( begin( ), end( ) );
        return *this;
    }

    virtual varray<T> & sort( std::function<bool( const T & t1, const T & t2 )> comparator )
    {
        std::sort( begin( ), end( ), comparator );
        return *this;
    }

    auto begin( )
    {
        return std::vector<T>::begin( );
    }

    auto end( )
    {
        return std::vector<T>::end( );
    }

    auto begin( ) const
    {
        return std::vector<T>::begin( );
    }

    auto end( ) const
    {
        return std::vector<T>::end( );
    }

    T & operator[]( uint index )
    {
        return at( index );
    }

    const T & operator[]( uint index ) const
    {
        return at( index );
    }

    virtual varray & operator=( const varray & v )
    {
        clear( );
        reserve( v.size( ) );
        insert_back( v );
        return *this;
    }

    bool valid_index( uint index ) const
    {
        return index < size( );
    }

private:
    bool assert_index( uint index ) const
    {
        return Assert( valid_index( index ), "varray index (%ui) out of range, varray size: %ui", index, size( ) );
    }
};

} // namespace axn

#endif /* STDc_hpp */
