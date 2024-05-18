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

template <typename T>
class queue : public std::queue<T>
{
    using std::queue<T>::queue;
};

template <typename T>
class stack : public std::stack<T>
{
    using std::stack<T>::stack;
};

template <typename Key, typename Compare = std::less<Key>>
class set : public std::set<Key, Compare>
{
    using std::set<Key, Compare>::set;

public:
    bool contains( const Key & k ) const
    {
        return ( std::set<Key, Compare>::find( k ) != std::set<Key, Compare>::end( ) );
    }
};

template <typename Key, typename Hash = std::hash<Key>, typename Equal = std::equal_to<Key>>
class uset : public std::unordered_set<Key, Hash, Equal>
{
    using std::unordered_set<Key, Hash, Equal>::unordered_set;

public:
    bool contains( const Key & k ) const
    {
        return ( std::unordered_set<Key, Hash, Equal>::find( k ) != std::unordered_set<Key, Hash, Equal>::end( ) );
    }
};

using std::map;
template <typename Key, typename Value, typename Hash = std::hash<Key>, typename Equal = std::equal_to<Key>>
class umap : public std::unordered_map<Key, Value, Hash, Equal>
{
public:
    bool contains( const Key & k ) const
    {
        return ( std::unordered_map<Key, Value, Hash, Equal>::unordered_map::find( k ) != std::unordered_map<Key, Value, Hash, Equal>::unordered_map::end( ) );
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
        assert_index( index_end - 1 );
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
        return insert( t, size( ) );
    }

    T & insert_front( const T & t = T( ) )
    {
        return insert( t, 0 );
    }

    varray<T> & insert_back( const varray<T> & v )
    {
        return insert_varray( v, size( ) );
    }

    varray<T> & insert_front( const varray<T> & v )
    {
        return insert_varray( v, 0 );
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
        return remove( 0 );
    }

    varray<T> & remove_back( )
    {
        return remove( size( ) - 1 );
    }

    varray<T> & remove( uint index )
    {
        return remove( index, index + 1 );
    }

    virtual varray<T> & remove( uint index_start, uint index_end )
    {
        assert_index( index_start );
        assert_index( index_end - 1 );
        std::vector<T>::erase( begin( ) + index_start, begin( ) + index_end );
        return *this;
    }

    varray<T> & resize( uint size, const T & t = T( ) )
    {
        std::vector<T>::resize( size, t );
        return *this;
    }

    varray<T> & resize_more( uint size, const T & t = T( ) )
    {
        resize( (uint)std::vector<T>::size( ) + size, t );
        return *this;
    }

    varray<T> & reserve( uint size )
    {
        std::vector<T>::reserve( size );
        return *this;
    }

    varray<T> & reserve_more( uint size )
    {
        return reserve( (uint)std::vector<T>::size( ) + size );
    }

    virtual varray<T> & clear( bool shrink = false )
    {
        std::vector<T>::clear( );
        if (shrink)
        {
            std::vector<T>::shrink_to_fit();
        }
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
        return ( index < size( ) );
    }

private:
    bool assert_index( uint index ) const
    {
        return Assert( valid_index( index ), "varray index (%ui) out of range, varray size: %ui", index, size( ) );
    }
};

} // namespace axn

#endif /* STDc_hpp */
