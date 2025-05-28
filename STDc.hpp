#ifndef STDc_hpp // std containers
#define STDc_hpp

#include "OS.hpp"
#include "STD.hpp"

#include "Assert.hpp"

namespace axn
{

// avoid confusion between std::vector and axn::Vector
template <typename T>
class varray : private std::vector<T>
{

private:

    using std::vector<T>::vector;

public:

    varray copy( ) { return varray( this ); }
    varray copy( uint index_end ) { return copy( 0, index_end ); }
    varray copy( uint index_start, uint index_end )
    {
        assert_index( index_start );

        if( index_start == index_end )
        {
            return varray( );
        }

        assert_index( index_end - 1 );

        varray ret( index_end - index_start );

        for( uint index = index_start; index < index_end; ++index )
        {
            ret.insert_back( index );
        }

        return ret;
    }

    bool empty( ) const { return size( ); }

    uint size( ) const { return (uint)( std::vector<T>::size( ) ); }

    bool contains( const T & t ) const
    {
        for( uint i = 0; i < size( ); ++i )
        {
            if( at( i ) == t )
            {
                return true;
            }
        }

        return false;
    }

    uint contain_count( const T & t ) const
    {
        uint count = 0;

        for( uint i = 0; i < size( ); ++i )
        {
            if( at( i ) == t )
            {
                ++count;
            }
        }

        return count;
    }

    varray<uint> find_indices( const T & t, uint max_count = 0 ) const
    {
        varray<uint> indices;

        for( uint i = 0; i < size( ); ++i )
        {
            if( at( i ) == t )
            {
                indices.insert_back( i );

                if( max_count && indices.size( ) == max_count )
                {
                    break;
                }
            }
        }

        return indices;
    }

    uint find_index_first( const T & t ) const
    {
        for( uint i = 0; i < size( ); ++i )
        {
            if( at( i ) == t )
            {
                return i;
            }
        }

        return size( );
    }

    uint find_index_last( const T & t ) const
    {
        for( uint i = 0; i < size( ); ++i )
        {
            if( at( size( ) - 1 - i ) == t )
            {
                return i;
            }
        }

        return size( );
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

    T & front( ) { return at( 0 ); }
    T & back( ) { return at( size( ) - 1 ); }

    const T & front( ) const { return at( 0 ); }
    const T & back( ) const { return at( size( ) - 1 ); }

    virtual T & insert( uint index ) { return insert( T( ), index ); }
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

    T & insert_front( const T & t = T( ) ) { return insert( t, 0 ); }
    T & insert_back( const T & t = T( ) ) { return insert( t, size( ) ); }

    void insert_front( const varray<T> & v ) { insert( v, 0 ); }
    void insert_back( const varray<T> & v ) { insert( v, size( ) ); }

    virtual void insert( const varray<T> & v, uint index )
    {
        assert_index( index, false );
        std::vector<T>::insert( begin( ) + index, v.begin( ), v.end( ) );
    }

    virtual void remove( const T & t )
    {
        uint deleted_count = 0;

        varray<uint> indices = find_indices( t );
        for( uint i = 0; i < size( ); ++i )
        {
            if( at( i - deleted_count ) == t )
            {
                remove_index( i - deleted_count++ );
            }
        }
    }

    void remove_index( uint index ) { remove_indices( index, index + 1 ); }
    virtual void remove_indices( uint index_start, uint index_end )
    {
        assert_index( index_start );
        assert_index( index_end - 1 );
        std::vector<T>::erase( begin( ) + index_start, begin( ) + index_end );
    }

    void remove_front( ) { remove_index( 0 ); }
    void remove_back( ) { remove_index( size( ) - 1 ); }

    virtual void remove_if( std::function<bool( const T & t )> checker ) { std::erase_if( *this, checker ); }

    void resize( uint size, const T & t = T( ) ) { std::vector<T>::resize( size, t ); }
    void resize_more( uint size, const T & t = T( ) ) { resize( (uint)( std::vector<T>::size( ) ) + size, t ); }

    void reserve( uint size ) { std::vector<T>::reserve( size ); }
    void reserve_more( uint size ) { reserve( (uint)std::vector<T>::size( ) + size ); }

    virtual void clear( bool shrink = false )
    {
        std::vector<T>::clear( );
        if( shrink )
        {
            std::vector<T>::shrink_to_fit( );
        }
    }

    virtual void reverse( ) { std::reverse( begin( ), end( ) ); }

    virtual varray<T> reversed( ) { varray<T> v = *this; v.reverse( ); return v; }

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

    auto begin( ) { return std::vector<T>::begin( ); }
    auto end( ) { return std::vector<T>::end( ); }

    auto begin( ) const { return std::vector<T>::begin( ); }
    auto end( ) const { return std::vector<T>::end( ); }

    bool valid_index( uint index ) const { return ( index < size( ) ); }

    T & operator[]( uint index ) { return at( index ); }
    const T & operator[]( uint index ) const { return at( index ); }

    varray operator+( const T & t ) const { varray v = *this; v.insert_back( t ); return v; }
    varray operator+( const varray & v ) const { varray ret = *this; ret.insert_back( v ); return ret; }

    varray & operator+=( const T & t ) { insert_back( t ); return *this; }
    varray & operator+=( const varray & v ) { insert_back( v ); return *this; }

    virtual varray & operator=( const varray & v )
    {
        clear( );
        reserve( v.size( ) );
        insert_back( v );
        return *this;
    }

    bool operator==( const varray<T> & v ) const { return ( *this == ( std::vector<T> )v ); }
    bool operator!=( const varray<T> & v ) const { return ( *this != ( std::vector<T> )v ); }

private:

    bool assert_index( uint index, bool exclude_end = true ) const
    {
        return Assert( valid_index( index ) || ( !exclude_end && ( index == size( ) ) ), "varray index (%ui) out of range, varray size: %ui", index, size( ) );
    }

};

template <typename T>
class list : private std::list<T>
{

private:

    using std::list<T>::list;

public:

    bool empty( ) const { return size( ); }

    uint size( ) const { return (uint)( std::list<T>::size( ) ); }

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

    const T & front( ) const { return at( 0 ); }
    const T & back( ) const { return at( size( ) - 1 ); }

    T & insert( uint index ) { return insert( T( ), index ); }
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

    T & insert_back( const T & t = T( ) ) { return insert( t, size( ) ); }
    T & insert_front( const T & t = T( ) ) { return insert( t, 0 ); }

    T pop_back( ) { T t = back( ); std::list<T>::pop_back( ); return t; }
    T pop_front( ) { T t = front( ); std::list<T>::pop_front( ); return t; }

    virtual void clear( ) { std::list<T>::clear( ); }

    virtual void reverse( ) { std::list<T>::reverse( ); }
    virtual list<T> reversed( ) { list<T> l = *this; l.reverse( ); return l; }

    virtual void sort( std::function<bool( const T & t1, const T & t2 )> comparator ) { std::list<T>::sort( comparator ); }

    virtual void remove_if( std::function<bool( const T & t )> checker ) { std::erase_if( *this, checker ); }

    auto begin( ) { return std::list<T>::begin( ); }
    auto end( ) { return std::list<T>::end( ); }

    auto begin( ) const { return std::list<T>::begin( ); }
    auto end( ) const { return std::list<T>::end( ); }

    T & operator[]( uint index ) { return at( index ); }
    const T & operator[]( uint index ) const { return at( index ); }

    bool valid_index( uint index ) const { return index < size( ); }

    bool operator==( const list<T> & l ) const { return ( *this == ( std::list<T> )l ); }
    bool operator!=( const list<T> & l ) const { return ( *this != ( std::list<T> )l ); }

private:

    bool assert_index( uint index ) const { return Assert( valid_index( index ), "varray index (%ui) out of range, varray size: %ui", index, size( ) ); }

};

template <typename T>
class queue : private std::queue<T>
{

private:

    using std::queue<T>::queue;

public:

    bool empty( ) const { return size( ); }

    uint size( ) const { return (uint)( std::queue<T>::size( ) ); }

    void push( const T & t ) { std::queue<T>::push( t ); }

    T pop( ) { T t = front( ); std::queue<T>::pop( ); return t; }

    const T & front( ) const { return std::queue<T>::front( ); }
    const T & back( ) const { return std::queue<T>::back( ); }

    bool operator==( const queue<T> & q ) const { return ( *this == ( std::queue<T> )q ); }
    bool operator!=( const queue<T> & q ) const { return ( *this != ( std::queue<T> )q ); }

};

template <typename T>
class stack : private std::stack<T>
{

private:

    using std::stack<T>::stack;

public:

    bool empty( ) const { return size( ); }

    uint size( ) const { return std::stack<T>::size( ); }

    void push( const T & t ) { return std::stack<T>::push( t ); }

    T pop( ) { T t = top( ); std::stack<T>::pop( ); return t; }

    const T & top( ) const { return std::stack<T>::top( ); }
    const T & bottom( ) const { return std::stack<T>::bottom( ); }

    bool operator==( const stack<T> & s ) const { return ( *this == ( std::stack<T> )s ); }
    bool operator!=( const stack<T> & s ) const { return ( *this != ( std::stack<T> )s ); }

};

template <typename Key, typename Compare = std::less<Key>>
class oset : private std::set<Key, Compare>
{

private:

    using std::set<Key, Compare>::set;

public:

    bool empty( ) const { return size( ); }

    uint size( ) const { return std::set<Key, Compare>::size( ); }

    bool contains( const Key & k ) const { return ( std::set<Key, Compare>::find( k ) != std::set<Key, Compare>::end( ) ); }
    
    virtual void insert( const Key & k ) { std::set<Key, Compare>::insert( k ); }

    virtual void clear( ) { std::set<Key, Compare>::clear( ); }

    auto begin( ) { return std::set<Key, Compare>::begin( ); }
    auto end( ) { return std::set<Key, Compare>::end( ); }

    auto begin( ) const { return std::set<Key, Compare>::begin( ); }
    auto end( ) const { return std::set<Key, Compare>::end( ); }

    bool operator==( const oset<Key, Compare> & s ) const { return ( *this == s ); }
    bool operator!=( const oset<Key, Compare> & s ) const { return ( *this != s ); }

};

template <typename Key, typename Hash = std::hash<Key>, typename Equal = std::equal_to<Key>>
class uset : private std::unordered_set<Key, Hash, Equal>
{

private:

    using std::unordered_set<Key, Hash, Equal>::unordered_set;

public:

    bool empty( ) const { return size( ); }

    uint size( ) const { return (uint)( std::unordered_set<Key, Hash, Equal>::size( ) ); }

    bool contains( const Key & k ) const { return ( std::unordered_set<Key, Hash, Equal>::find( k ) != std::unordered_set<Key, Hash, Equal>::end( ) ); }

    void insert( const Key & k ) { std::unordered_set<Key, Hash, Equal>::insert( k ); }

    void remove( const Key & k ) { std::unordered_set<Key, Hash, Equal>::erase( k ); }

    void clear( ) { std::unordered_set<Key, Hash, Equal>::clear( ); }

    auto begin( ) { return std::unordered_set<Key, Hash, Equal>::begin( ); }
    auto end( ) { return std::unordered_set<Key, Hash, Equal>::end( ); }

    auto begin( ) const { return std::unordered_set<Key, Hash, Equal>::begin( ); }
    auto end( ) const { return std::unordered_set<Key, Hash, Equal>::end( ); }

    bool operator==( const uset<Key, Hash, Equal> & s ) const { return ( *this == ( std::unordered_set<Key, Hash, Equal> )s ); }
    bool operator!=( const uset<Key, Hash, Equal> & s ) const { return ( *this != ( std::unordered_set<Key, Hash, Equal> )s ); }

};

template <typename Key, typename Value, typename Hash = std::hash<Key>, typename Equal = std::equal_to<Key>>
class omap : private std::map<Key, Value, Hash, Equal>
{

private:

    using std::map<Key, Value, Hash, Equal>::map;

public:

    bool empty( ) const { return size( ); }

    uint size( ) const { return (uint)( std::map<Key, Value, Hash, Equal>::size( ) ); }

    bool contains( const Key & k ) const { return ( std::map<Key, Value, Hash, Equal>::map::find( k ) != std::map<Key, Value, Hash, Equal>::unordered_map::end( ) ); }

    void insert( const Key & k, const Value & v ) { std::map<Key, Value, Hash, Equal>::insert( k, v ); }

    void remove( const Key & k, const Value & v ) { std::map<Key, Value, Hash, Equal>::erase( k, v ); }

    auto begin( ) { return std::map<Key, Value, Hash, Equal>::begin( ); }
    auto end( ) { return std::map<Key, Value, Hash, Equal>::end( ); }

    auto begin( ) const { return std::map<Key, Value, Hash, Equal>::begin( ); }
    auto end( ) const { return std::map<Key, Value, Hash, Equal>::end( ); }

    bool operator==( const omap<Key, Value, Hash, Equal> & m ) const { return ( *this == ( std::map<Key, Value, Hash, Equal> )m ); }
    bool operator!=( const omap<Key, Value, Hash, Equal> & m ) const { return ( *this != ( std::map<Key, Value, Hash, Equal> )m ); }

};

template <typename Key, typename Value, typename Hash = std::hash<Key>, typename Equal = std::equal_to<Key>>
class umap : public std::unordered_map<Key, Value, Hash, Equal>
{

private:

    using std::unordered_map<Key, Value, Hash, Equal>::unordered_map;

public:

    bool empty( ) const { return size( ); }

    uint size( ) const { return (uint)( std::unordered_map<Key, Value, Hash, Equal>::size( ) ); }

    bool contains( const Key & k ) const { return ( std::unordered_map<Key, Value, Hash, Equal>::unordered_map::find( k ) != std::unordered_map<Key, Value, Hash, Equal>::unordered_map::end( ) ); }

    auto begin( ) { return std::unordered_map<Key, Value, Hash, Equal>::begin( ); }
    auto end( ) { return std::unordered_map<Key, Value, Hash, Equal>::end( ); }

    auto begin( ) const { return std::unordered_map<Key, Value, Hash, Equal>::begin( ); }
    auto end( ) const { return std::unordered_map<Key, Value, Hash, Equal>::end( ); }

    // TODO!
    //T & operator[]( uint index ) { return std::unordered_map<Key, Value, Hash, Equal>::operator[][ index ]; }
    //const T & operator[]( uint index ) const { return std::unordered_map<Key, Value, Hash, Equal>::operator[]( index ); }

    bool operator==( const umap<Key, Value, Hash, Equal> & m ) const { return ( *this == ( std::unordered_map<Key, Value, Hash, Equal> )m ); }
    bool operator!=( const umap<Key, Value, Hash, Equal> & m ) const { return ( *this != ( std::unordered_map<Key, Value, Hash, Equal> )m ); }

};

} // namespace axn

#endif /* STDc_hpp */
