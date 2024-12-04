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

    virtual void clear( )
    {
        std::list<T>::clear( );
    }

    virtual void reverse( )
    {
        std::list<T>::reverse( );
    }

    virtual void sort( std::function<bool( const T & t1, const T & t2 )> comparator )
    {
        std::list<T>::sort( comparator );
    }

    virtual void erase_if( std::function<bool( const T & t )> checker )
    {
        std::erase_if( *this, checker );
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

public:
    virtual ~queue( ) { }

    uint size( ) const
    {
        return (uint)std::queue<T>::size( );
    }

    T & front( )
    {
        return std::queue<T>::front( );
    }

    const T & front( ) const
    {
        return std::queue<T>::front( );
    }

    void push( const T & t )
    {
        std::queue<T>::push( t );
    }

    void pop( )
    {
        std::queue<T>::pop( );
    }
};

template <typename T>
class stack : public std::stack<T>
{
    using std::stack<T>::stack;

public:
    virtual ~stack( ) { }
};

template <typename Key, typename Compare = std::less<Key>>
class oset : public std::set<Key, Compare>
{
    using std::set<Key, Compare>::set;

public:
    virtual ~oset( ) { }

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
    virtual ~uset( ) { }

    bool contains( const Key & k ) const
    {
        return ( std::unordered_set<Key, Hash, Equal>::find( k ) != std::unordered_set<Key, Hash, Equal>::end( ) );
    }
};

template <typename Key, typename Value, typename Hash = std::hash<Key>, typename Equal = std::equal_to<Key>>
class omap : public std::map<Key, Value, Hash, Equal>
{
    using std::map<Key, Value, Hash, Equal>::map;

public:
    virtual ~omap( ) { }

    bool contains( const Key & k ) const
    {
        return ( std::map<Key, Value, Hash, Equal>::map::find( k ) != std::map<Key, Value, Hash, Equal>::unordered_map::end( ) );
    }
};

template <typename Key, typename Value, typename Hash = std::hash<Key>, typename Equal = std::equal_to<Key>>
class umap : public std::unordered_map<Key, Value, Hash, Equal>
{
    using std::unordered_map<Key, Value, Hash, Equal>::unordered_map;

public:
    virtual ~umap( ) { }

    bool contains( const Key & k ) const
    {
        return ( std::unordered_map<Key, Value, Hash, Equal>::unordered_map::find( k ) != std::unordered_map<Key, Value, Hash, Equal>::unordered_map::end( ) );
    }
};

enum varray_position
{
    NONE,
    FIRST,
    LAST,
    ALL,
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
        {
            return varray( );
        }

        assert_index( index_start );
        assert_index( index_end - 1 );
        varray ret( index_end - index_start );

        for( uint index = index_start; index < index_end; ++index )
        {
            ret.insert_back( index );
        }

        return ret;
    }

    uint size( ) const
    {
        return (uint)std::vector<T>::size( );
    }

    bool contains( const T & t ) const
    {
        return find( t, FIRST ).size( );
    }

    uint contain_count( const T & t ) const
    {
        return find( t, ALL ).size( );
    }

    uint find_first( const T & t ) const
    {
        varray<uint> indices = find( t, FIRST );
        return ( indices.size( ) ? indices.front( ) : 0 );
    }

    uint find_last( const T & t ) const
    {
        varray<uint> indices = find( t, LAST );
        return ( indices.size( ) ? indices.front( ) : 0 );
    }

    varray<uint> find_all( const T & t ) const
    {
        return find( t, ALL );
    }

    varray<uint> find( const T & t, varray_position position ) const
    {
        varray<uint> indices;

        if( position != NONE )
        {
            for( uint i = 0; i < size( ); ++i )
            {
                uint index = ( position == LAST ) ? ( size( ) - 1 - i ) : i;

                if( at( index ) == t )
                {
                    if( ( position == FIRST ) || ( position == LAST ) )
                    {
                        return varray<uint>( { index } );
                    }
                    else if( position == ALL )
                    {
                        indices.insert_back( index );
                    }
                }
            }
        }

        return indices;
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
        return at( 0 );
    }

    const T & front( ) const
    {
        return at( 0 );
    }

    T & back( )
    {
        return at( size( ) - 1 );
    }

    const T & back( ) const
    {
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

    void insert_back( const varray<T> & v )
    {
        insert_varray( v, size( ) );
    }

    void insert_front( const varray<T> & v )
    {
        insert_varray( v, 0 );
    }

    virtual void insert_varray( const varray<T> & v, uint index )
    {
        assert_index( index, false );
        std::vector<T>::insert( begin( ) + index, v.begin( ), v.end( ) );
    }

    void erase_front( )
    {
        erase( 0 );
    }

    void erase_back( )
    {
        erase( size( ) - 1 );
    }

    void erase( uint index )
    {
        erase( index, index + 1 );
    }

    virtual void erase( uint index_start, uint index_end )
    {
        assert_index( index_start );
        assert_index( index_end - 1 );
        std::vector<T>::erase( begin( ) + index_start, begin( ) + index_end );
    }

    virtual void erase( const T & t, varray_position position )
    {
        if( position != NONE )
        {
            uint original_size = size( );
            uint deleted_count = 0;

            varray<uint> indices = find( t, position );
            for( uint i = 0; i < indices.size( ); ++i )
            {
                uint index = ( position == LAST ) ? ( original_size - 1 - i ) : ( i - deleted_count );

                if( at( index ) == t )
                {
                    ++deleted_count;
                    erase( index );
                    if( ( position == FIRST ) || ( position == LAST ) )
                    {
                        return;
                    }
                }
            }
        }
    }

    virtual void erase_if( std::function<bool( const T & t )> checker )
    {
        std::erase_if( *this, checker );
    }

    void resize( uint size, const T & t = T( ) )
    {
        std::vector<T>::resize( size, t );
    }

    void resize_more( uint size, const T & t = T( ) )
    {
        resize( (uint)std::vector<T>::size( ) + size, t );
    }

    void reserve( uint size )
    {
        std::vector<T>::reserve( size );
    }

    void reserve_more( uint size )
    {
        reserve( (uint)std::vector<T>::size( ) + size );
    }

    virtual void clear( bool shrink = false )
    {
        std::vector<T>::clear( );
        if( shrink )
        {
            std::vector<T>::shrink_to_fit( );
        }
    }

    virtual void reverse( )
    {
        std::reverse( begin( ), end( ) );
    }

    virtual void sort( std::function<bool( const T & t1, const T & t2 )> comparator, bool stable = false )
    {
        if( stable )
        {
            std::stable_sort( begin( ), end( ), comparator );
        }
        else
        {
            std::sort( begin( ), end( ), comparator );
        }
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

    bool valid_index( uint index ) const
    {
        return ( index < size( ) );
    }

    T & operator[]( uint index )
    {
        return at( index );
    }

    const T & operator[]( uint index ) const
    {
        return at( index );
    }

    varray operator+( const varray & _v ) const
    {
        varray v = *this;
        v.insert_back( _v );
        return v;
    }

    varray & operator+=( const varray & _v )
    {
        insert_back( _v );
        return *this;
    }

    virtual varray & operator=( const varray & v )
    {
        clear( );
        reserve( v.size( ) );
        insert_back( v );
        return *this;
    }

    bool operator==( const varray & v ) const
    {
        if( size( ) != v.size( ) )
        {
            return false;
        }

        for( uint i = 0; i < size( ); ++i )
        {
            if( at( i ) != v.at( i ) )
            {
                return false;
            }
        }

        return true;
    }

    bool operator!=( const varray & v ) const
    {
        return !( *this == v );
    }

private:
    bool assert_index( uint index, bool exclude_end = true ) const
    {
        return Assert( valid_index( index ) || ( !exclude_end && ( index == size( ) ) ), "varray index (%ui) out of range, varray size: %ui", index, size( ) );
    }
};

} // namespace axn

#endif /* STDc_hpp */
