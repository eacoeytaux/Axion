#ifndef STDc_hpp // std containers
#define STDc_hpp

#include "axn/system/OS.hpp"
#include "axn/system/STD.hpp"
#include "axn/system/STDptr.hpp"
#include "axn/system/Defines.hpp"
#include "axn/system/Assert.hpp"

namespace axn
{

// avoid confusion between std::vector and axn::Vector
classT varray : private std::vector<T1>
{

private:

    using std::vector<T1>::vector;

public:

    T1 ref at( uint in_index ) { assert_index( in_index, false ); return( std::vector<T1>::at( in_index ) ); }
    T1 cref at( uint in_index ) const { assert_index( in_index, false ); return( std::vector<T1>::at( in_index ) ); }

    T1 ref front( ) { return( at( 0 ) ); }
    T1 cref front( ) const { return( at( 0 ) ); }

    T1 ref back( ) { return( at( size( ) - 1 ) ); }
    T1 cref back( ) const { return( at( size( ) - 1 ) ); }

    T1 ref insert( T1 cref in_elem = T1( ) ) { return( insert_back( in_elem ) ); }

    T1 ref insert_at( T1 cref in_elem, uint in_index ) { assert_index( in_index, true ); if( in_index == size( ) ) { std::vector<T1>::push_back( in_elem ); } else { std::vector<T1>::insert( begin( ) + in_index, in_elem ); } return( at( in_index ) ); }
    T1 ref insert_at( uint in_index ) { return( insert_at( T1( ), in_index ) ); }

    T1 ref insert_front( T1 cref in_elem = T1( ) ) { return( insert_at( in_elem, 0 ) ); }
    T1 ref insert_back( T1 cref in_elem = T1( ) ) { return( insert_at( in_elem, size( ) ) ); }

    varray ref insert( varray<T1> cref in_arr ) { return( insert_back( in_arr ) ); }

    varray ref insert_at( varray<T1> cref in_arr, uint in_index ) { assert_index( in_index, true ); std::vector<T1>::insert( begin( ) + in_index, in_arr.begin( ), in_arr.end( ) ); rethis; }

    varray ref insert_front( varray<T1> cref in_arr ) { insert_at( in_arr, 0 ); rethis; }
    varray ref insert_back( varray<T1> cref in_arr ) { insert_at( in_arr, size( ) ); rethis; }

    varray ref remove( T1 cref in_elem ) { uint deleted_count = 0; varray<uint> indices = find_indices( in_elem ); for_range( i, size( ) ) { if( at( i - deleted_count ) == in_elem ) { remove_at( i - deleted_count++ ); } } rethis; }

    varray ref remove_at( uint in_index ) { remove_at( in_index, in_index + 1 ); rethis; }
    varray ref remove_at( uint in_index_start, uint in_index_end ) { assert_index( in_index_start ); assert_index( in_index_end - 1 ); std::vector<T1>::erase( begin( ) + in_index_start, begin( ) + in_index_end ); rethis; }

    varray ref remove_front( ) { remove_at( 0 ); rethis; }
    varray ref remove_back( ) { remove_at( size( ) - 1 ); rethis; }

    varray ref remove_if( std::function<bool( T1 cref in_elem )> cref checker ) { std::erase_if( ( *this ), checker ); rethis; }

    varray ref resize( uint in_size, T1 cref in_fill_elem = T1( ) ) { std::vector<T1>::resize( in_size, in_fill_elem ); rethis; }
    varray ref resize_more( uint in_size, T1 cref in_fill_elem = T1( ) ) { resize( size( ) + in_size, in_fill_elem ); rethis; }

    varray ref reserve( uint in_size ) { std::vector<T1>::reserve( in_size ); rethis; }
    varray ref reserve_more( uint in_size ) { reserve( size( ) + in_size ); rethis; }

    varray ref clear( bool in_shrink = false ) { std::vector<T1>::clear( ); if( in_shrink ) { std::vector<T1>::shrink_to_fit( ); } rethis; }

    uint size( ) const { return( uint( std::vector<T1>::size( ) ) ); }

    bool empty( ) const { return( std::vector<T1>::empty( ) ); }

    bool contains( T1 cref in_elem ) const { for_range( i, size( ) ) { return_true_if( at( i ) == in_elem ); } return( false ); }

    uint count( T1 cref in_elem ) const { uint count = 0; for_range( i, size( ) ) { if( at( i ) == in_elem ) { count++; } } return( count ); }

    varray<uint> find_indices( T1 cref in_elem, uint in_max_count = 0 ) const { varray<uint> indices; for_range( i, size( ) ) { if( at( i ) == ( in_elem ) ) { indices.insert_back( i ); break_if( in_max_count && ( indices.size( ) == in_max_count ) ); } } return( indices ); }

    uint find_index_first( T1 cref in_elem ) const { for_range( i, size( ) ) { return_if( ( at( i ) == in_elem ), i ); } return( size( ) ); }
    uint find_index_last( T1 cref in_elem ) const { for_range( i, size( ) ) { return_if( ( at( size( ) - i - 1 ) == in_elem ), i ); } return( size( ) ); }

    auto begin( ) { return( std::vector<T1>::begin( ) ); }
    auto end( ) { return( std::vector<T1>::end( ) ); }

    auto begin( ) const { return( std::vector<T1>::begin( ) ); }
    auto end( ) const { return( std::vector<T1>::end( ) ); }

    varray ref fill( T1 cref in_elem ) { for_range( i, size( ) ) { at( i ) = in_elem; } rethis; }

    varray ref reverse( ) { std::reverse( begin( ), end( ) ); rethis; }
    varray reversed( ) const { varray<T1> ret = ( *this ); ret.reverse( ); return( ret ); }

    varray ref sort( std::function<bool( T1 cref in_x1, T1 cref in_x2 )> cref comparator, bool in_stable = false ) { if( in_stable ) { std::stable_sort( begin( ), end( ), comparator ); } else { std::sort( begin( ), end( ), comparator ); } rethis; }
    varray sorted( std::function<bool( T1 cref in_x1, T1 cref in_x2 )> cref comparator, bool in_stable = false ) const { varray<T1> ret = ( *this ); ret.sort( comparator, in_stable ); return( ret ); }

    bool valid_index( uint in_index, bool in_include_end = false ) const { return( ( in_index < size( ) ) || ( in_include_end && ( in_index == size( ) ) ) ); }
    bool assert_index( uint in_index, bool in_include_end = false ) const { Assert( valid_index( in_index, in_include_end ), "index ( %ui ) out of range, size: %ui", in_index, size( ) ); return( valid_index( in_index, in_include_end ) ); }

    T1 ref operator[ ]( uint in_index ) { return( at( in_index ) ); }
    T1 cref operator[ ]( uint in_index ) const { return( at( in_index ) ); }

    varray ref operator+=( T1 cref in_elem ) { insert( in_elem ); rethis; }
    varray ref operator+=( varray cref in_elem ) { insert( in_elem ); rethis; }

    varray operator+( T1 cref in_elem ) const { varray ret = ( *this ); ret.insert( in_elem ); return( ret ); }
    varray operator+( varray cref in_elem ) const { varray ret = ( *this ); ret.insert( in_elem ); return( ret ); }

    default_equal( varray<T1> );

};

classT vvarray : public varray<varray<T1>>{ };


// fixed array
templated( uint N, typename T1 )
class carray : private std::array<T1, N>
{

private:

    using std::array<T1, N>::array;

public:

    T1 ref at( uint in_index ) { assert_index( in_index, false ); return( std::array<T1, N>::at( in_index ) ); }
    T1 cref at( uint in_index ) const { assert_index( in_index, false ); return( std::array<T1, N>::at( in_index ) ); }

    T1 ref front( ) { return( at( 0 ) ); }
    T1 cref front( ) const { return( at( 0 ) ); }

    T1 ref back( ) { return( at( size( ) - 1 ) ); }
    T1 cref back( ) const { return( at( size( ) - 1 ) ); }

    uint size( ) const { return( uint( std::array<T1, N>::size( ) ) ); }

    bool empty( ) const { return( std::array<T1, N>::empty( ) ); }

    bool contains( T1 cref in_elem ) const { for_range( i, size( ) ) { return_true_if( at( i ) == in_elem ); } return( false ); }

    uint count( T1 cref in_elem ) const { uint count = 0; for_range( i, size( ) ) { if( at( i ) == in_elem ) { count++; } } return( count ); }

    varray<uint> find_indices( T1 cref in_elem, uint in_max_count = 0 ) const { varray<uint> indices; for_range( i, size( ) ) { if( at( i ) == ( in_elem ) ) { indices.insert_back( i ); break_if( in_max_count && ( indices.size( ) == in_max_count ) ); } } return( indices ); }

    uint find_index_first( T1 cref in_elem ) const { for_range( i, size( ) ) { return_if( ( at( i ) == in_elem ), i ); } return( size( ) ); }
    uint find_index_last( T1 cref in_elem ) const { for_range( i, size( ) ) { return_if( ( at( size( ) - i - 1 ) == in_elem ), i ); } return( size( ) ); }

    auto begin( ) { return( std::array<T1, N>::begin( ) ); }
    auto end( ) { return( std::array<T1, N>::end( ) ); }

    auto begin( ) const { return( std::array<T1, N>::begin( ) ); }
    auto end( ) const { return( std::array<T1, N>::end( ) ); }

    carray ref fill( T1 cref in_elem ) { for_range( i, size( ) ) { at( i ) = in_elem; } rethis; }

    carray ref reverse( ) { std::reverse( begin( ), end( ) ); rethis; }
    carray reversed( ) const { carray<N, T1> ret = ( *this ); ret.reverse( ); return( ret ); }

    carray ref sort( std::function<bool( T1 cref in_x1, T1 cref in_x2 )> cref comparator, bool in_stable = false ) { if( in_stable ) { std::stable_sort( begin( ), end( ), comparator ); } else { std::sort( begin( ), end( ), comparator ); } rethis; }
    carray sorted( std::function<bool( T1 cref in_x1, T1 cref in_x2 )> cref comparator, bool in_stable = false ) const { carray<N, T1> ret = ( *this ); ret.sort( comparator, in_stable ); return( ret ); }

    bool valid_index( uint in_index ) const { return( in_index < size( ) ); }
    bool assert_index( uint in_index ) const { Assert( valid_index( in_index ), "index ( %ui ) out of range, size: %ui", in_index, size( ) ); return( valid_index( in_index ) ); }

    T1 ref operator[ ]( uint in_index ) { return( at( in_index ) ); }
    T1 cref operator[ ]( uint in_index ) const { return( at( in_index ) ); }

    default_equal( carray<N, T1> );

};

classT queue : private std::queue<T1>
{

private:

    using std::queue<T1>::queue;

public:

    T1 pop( ) { T1 x = front( ); std::queue<T1>::pop( ); return( x ); }

    T1 ref push( T1 cref in_elem = T1( ) ) { std::queue<T1>::push( in_elem ); return( front( ) ); }

    T1 ref front( ) { return( std::queue<T1>::front( ) ); }
    T1 cref front( ) const { return( std::queue<T1>::front( ) ); }

    T1 ref back( ) { return( std::queue<T1>::back( ) ); }
    T1 cref back( ) const { return( std::queue<T1>::back( ) ); }

    queue ref clear( ) { while( size( ) ) { pop( ); }; rethis; }

    uint size( ) const { return( uint( std::queue<T1>::size( ) ) ); }

    bool empty( ) const { return( std::queue<T1>::empty( ) ); }

    default_equal( queue<T1> );

};

classT stack : private std::stack<T1>
{

private:

    using std::stack<T1>::stack;

public:

    T1 pop( ) { T1 x = top( ); std::stack<T1>::pop( ); return( x ); }

    T1 ref push( T1 cref in_elem = T1( ) ) { std::stack<T1>::push( in_elem ); return( top( ) ); }

    T1 ref top( ) { return( std::stack<T1>::top( ) ); }
    T1 cref top( ) const { return( std::stack<T1>::top( ) ); }

    T1 ref bottom( ) { return( std::stack<T1>::bottom( ) ); }
    T1 cref bottom( ) const { return( std::stack<T1>::bottom( ) ); }

    stack ref clear( ) { while( size( ) ) { pop( ); }; rethis; }

    uint size( ) const { return( uint( std::stack<T1>::size( ) ) ); }

    bool empty( ) const { return( std::stack<T1>::empty( ) ); }

    default_equal( stack<T1> );

};

classT list : private std::list<T1>
{

private:

    using std::list<T1>::list;

public:

    T1 ref at( uint in_index ) { assert_index( in_index, false ); auto it = std::list<T1>::begin( ); do_count( in_index ) { it++; } return( *it ); }
    T1 cref at( uint in_index ) const { assert_index( in_index, false ); auto it = std::list<T1>::begin( ); do_count( in_index ) { it++; } return( *it ); }

    T1 ref front( ) { return( at( 0 ) ); }
    T1 cref front( ) const { return( at( 0 ) ); }

    T1 ref back( ) { return( at( size( ) - 1 ) ); }
    T1 cref back( ) const { return( at( size( ) - 1 ) ); }

    T1 ref insert( T1 cref in_elem = T1( ) ) { return( insert_back( in_elem ) ); }

    T1 ref insert_at( T1 cref in_elem, uint in_index ) { assert_index( in_index, true ); if( !in_index ) { std::list<T1>::push_front( in_elem ); } else if( in_index == size( ) ) { std::list<T1>::push_back( in_elem ); } else { assert_index( in_index ); auto it = std::list<T1>::begin( ); do_count( in_index ) { it++; } std::list<T1>::insert( it, in_elem ); } return( at( in_index ) ); }
    T1 ref insert_at( uint in_index ) { return( insert_at( T1( ), in_index ) ); }

    T1 ref insert_front( T1 cref in_elem = T1( ) ) { return( insert_at( in_elem, 0 ) ); }
    T1 ref insert_back( T1 cref in_elem = T1( ) ) { return( insert_at( in_elem, size( ) ) ); }

    list ref remove( T1 cref in_elem ) { std::list<T1>::remove( in_elem ); rethis; }

    list ref remove_at( uint in_index ) { assert_index( in_index, false ); auto it = std::list<T1>::begin( ); do_count( in_index ) { it++; } std::list<T1>::erase( it ); rethis; }

    list ref remove_front( ) { std::list<T1>::pop_front( ); rethis; }
    list ref remove_back( ) { std::list<T1>::pop_back( ); rethis; }

    list ref remove_if( std::function<bool( T1 cref in_elem )> cref checker ) { std::erase_if( ( *this ), checker ); rethis; }

    list ref clear( ) { std::list<T1>::clear( ); rethis; }

    uint size( ) const { return( uint( std::list<T1>::size( ) ) ); }

    bool empty( ) const { return( std::list<T1>::empty( ) ); }

    auto begin( ) { return( std::list<T1>::begin( ) ); }
    auto end( ) { return( std::list<T1>::end( ) ); }

    auto begin( ) const { return( std::list<T1>::begin( ) ); }
    auto end( ) const { return( std::list<T1>::end( ) ); }

    list ref reverse( ) { std::list<T1>::reverse( ); rethis; }
    list reversed( ) const { list<T1> ret = ( *this ); ret.reverse( ); return( ret ); }

    list ref sort( std::function<bool( T1 cref in_x1, T1 cref in_x2 )> cref comparator ) { std::list<T1>::sort( comparator ); rethis; }
    list sorted( std::function<bool( T1 cref in_x1, T1 cref in_x2 )> cref comparator ) const { list<T1> ret = ( *this ); ret.sort( comparator ); return( ret ); }

    bool valid_index( uint in_index, bool in_include_end = false ) const { return( ( in_index < size( ) ) || ( in_include_end && ( in_index == size( ) ) ) ); }
    bool assert_index( uint in_index, bool in_include_end = false ) const { Assert( valid_index( in_index, in_include_end ), "index ( %ui ) out of range, size: %ui", in_index, size( ) ); return( valid_index( in_index, in_include_end ) ); }

    T1 ref operator[ ]( uint in_index ) { return( at( in_index ) ); }
    T1 cref operator[ ]( uint in_index ) const { return( at( in_index ) ); }

    default_equal( list<T1> );

};

classT oset : private std::set<T1>
{

private:

    using std::set<T1>::set;

public:

    bool insert( T1 cref in_key ) { std::set<T1>::insert( in_key ); return( true ); }

    oset ref remove( T1 cref in_key ) { std::set<T1>::erase( in_key ); rethis; }

    oset ref clear( ) { std::set<T1>::clear( ); rethis; }

    uint size( ) const { return( uint( std::set<T1>::size( ) ) ); }

    bool empty( ) const { return( std::set<T1>::empty( ) ); }

    bool contains( T1 cref in_key ) const { return( std::set<T1>::find( in_key ) != std::set<T1>::end( ) ); }

    auto begin( ) { return( std::set<T1>::begin( ) ); }
    auto end( ) { return( std::set<T1>::end( ) ); }

    auto begin( ) const { return( std::set<T1>::begin( ) ); }
    auto end( ) const { return( std::set<T1>::end( ) ); }

    bool operator[ ]( T1 cref in_key ) const { return( contains( in_key ) ); }

    default_equal( oset<T1> );

};

classT uset : private std::unordered_set<T1>
{

private:

    using std::unordered_set<T1>::unordered_set;

public:

    bool insert( T1 cref in_key ) { std::unordered_set<T1>::insert( in_key ); return( true ); }

    uset ref remove( T1 cref in_key ) { std::unordered_set<T1>::erase( in_key ); rethis; }

    uset ref clear( ) { std::unordered_set<T1>::clear( ); rethis; }

    uint size( ) const { return( uint( std::unordered_set<T1>::size( ) ) ); }

    bool empty( ) const { return( std::unordered_set<T1>::empty( ) ); }

    bool contains( T1 cref in_key ) const { return( std::unordered_set<T1>::find( in_key ) != std::unordered_set<T1>::end( ) ); }

    auto begin( ) { return( std::unordered_set<T1>::begin( ) ); }
    auto end( ) { return( std::unordered_set<T1>::end( ) ); }

    auto begin( ) const { return( std::unordered_set<T1>::begin( ) ); }
    auto end( ) const { return( std::unordered_set<T1>::end( ) ); }

    bool operator[ ]( T1 cref in_key ) const { return( contains( in_key ) ); }

    default_equal( uset<T1> );

};

classT2 omap : private std::map<T1, T2>, private oset<T1>
{

private:

    using std::map<T1, T2>::map;

public:

    uset<T1> cref keys( ) const { return( *this ); }

    T2 ref at( T1 cref in_key ) { return( std::unordered_map<T1, T2>::at( in_key ) ); }
    T2 cref at( T1 cref in_key ) const { return( std::unordered_map<T1, T2>::at( in_key ) ); }

    T2 ref insert( T1 cref in_key, T2 cref in_value = T2( ) ) { std::map<T1, T2>::insert( in_key, in_value ); oset<T1>::insert( in_key ); return( at( in_key ) ); }

    omap ref remove( T1 cref in_key ) { std::map<T1, T2>::erase( in_key ); oset<T1>::remove( in_key ); rethis; }

    omap ref clear( ) const { std::map<T1, T2>::clear( ); oset<T1>::clear( ); rethis; }

    uint size( ) const { return( uint( std::map<T1, T2>::size( ) ) ); }

    bool empty( ) const { return( std::map<T1, T2>::empty( ) ); }

    bool contains( T1 cref in_key ) const { return( std::map<T1, T2>::find( in_key ) != std::map<T1, T2>::end( ) ); }

    auto begin( ) { return( std::map<T1, T2>::begin( ) ); }
    auto end( ) { return( std::map<T1, T2>::end( ) ); }

    auto begin( ) const { return( std::map<T1, T2>::begin( ) ); }
    auto end( ) const { return( std::map<T1, T2>::end( ) ); }

    T2 ref operator[ ]( T1 cref in_key ) { return( at( in_key ) ); }
    T2 cref operator[ ]( T1 cref in_key ) const { return( at( in_key ) ); }

    default_equal( omap<T1, T2> );

};

classT2 umap : private std::unordered_map<T1, T2>, private uset<T1>
{

private:

    using std::unordered_map<T1, T2>::unordered_map;

public:

    uset<T1> cref keys( ) const { return( *this ); }

    T2 ref at( T1 cref in_key ) { return( std::unordered_map<T1, T2>::at( in_key ) ); }
    T2 cref at( T1 cref in_key ) const { return( std::unordered_map<T1, T2>::at( in_key ) ); }

    T2 ref insert( T1 cref in_key, T2 cref in_value = T2( ) ) { std::unordered_map<T1, T2>::insert( { in_key, in_value } ); uset<T1>::insert( in_key ); return( at( in_key ) ); }

    umap ref remove( T1 cref in_key ) { std::unordered_map<T1, T2>::erase( in_key ); uset<T1>::remove( in_key ); rethis; }

    umap ref clear( ) const { std::map<T1, T2>::clear( ); uset<T1>::clear( ); rethis; }

    uint size( ) const { return( uint( std::unordered_map<T1, T2>::size( ) ) ); }

    bool empty( ) const { return( std::unordered_map<T1, T2>::empty( ) ); }

    bool contains( T1 cref in_key ) const { return( std::unordered_map<T1, T2>::find( in_key ) != std::unordered_map<T1, T2>::end( ) ); }

    auto begin( ) { return( std::unordered_map<T1, T2>::begin( ) ); }
    auto end( ) { return( std::unordered_map<T1, T2>::end( ) ); }

    auto begin( ) const { return( std::unordered_map<T1, T2>::begin( ) ); }
    auto end( ) const { return( std::unordered_map<T1, T2>::end( ) ); }

    T2 ref operator[ ]( T1 cref in_key ) { return( at( in_key ) ); }
    T2 cref operator[ ]( T1 cref in_key ) const { return( at( in_key ) ); }

    default_equal( umap<T1, T2> );

};

} // namespace axn

#endif /* STDc_hpp */
