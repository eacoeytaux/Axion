#ifndef Primitives_hpp
#define Primitives_hpp

#include "Includes.hpp"
#include "Error.hpp"
#include "Assert.hpp"

namespace axn {
namespace system {

class ufloat {
private:
    float m_f;
    float get( ) const { return m_f; }
    ufloat & set( float f ) {
        axn::system::Assert( f >= 0 );
        m_f = abs( f );
        return *this;
    }

public:
    ufloat( float f = 0.f ) { set( f ); }

    operator float( ) const { return m_f; }

    ufloat & operator=( const float & f ) {
        set( f );
        return *this;
    }

    ufloat operator+( const float & f ) { return ufloat( get( ) + f ); }
    ufloat operator-( const float & f ) { return ufloat( get( ) - f ); }
    ufloat operator*( const float & f ) { return ufloat( get( ) * f ); }
    ufloat operator/( const float & f ) { return ufloat( get( ) / f ); }

    ufloat & operator+=( const float & f ) {
        *this = ufloat( get( ) + f );
        return *this;
    }
    ufloat & operator-=( const float & f ) {
        *this = ufloat( get( ) - f );
        return *this;
    }
    ufloat & operator*=( const float & f ) {
        *this = ufloat( get( ) * f );
        return *this;
    }
    ufloat & operator/=( const float & f ) {
        *this = ufloat( get( ) / f );
        return *this;
    }

    bool operator==( const float & f ) const { return get( ) == f; }
    bool operator!=( const float & f ) const { return !( *this == f ); }

    bool operator==( const ufloat & uf ) const { return get( ) == uf.get( ); }
    bool operator!=( const ufloat & uf ) const { return !( *this == uf ); }
};

// avoid confusion between std::vector and axn::Vector
template <typename T> class varray : private std::vector<T> {
    using std::vector<T>::vector;

public:
    virtual ~varray( ) { clear( true ); }

    varray copy( ) { return varray( this ); }
    varray copy( uint index_end ) { return copy( 0, index_end ); }
    varray copy( uint index_start, uint index_end ) {
        if( index_start == index_end )
            return varray( );
        assert_index( index_start );
        assert_index( index_end + 1 );
        varray ret( index_end - index_start );
        for( uint index = index_start; index < index_end; ++index )
            ret.push_back( index );
        return ret;
    }

    bool valid_index( uint index ) const { return index < size( ); }

    virtual T & at( uint index ) {
        assert_index( index );
        return std::vector<T>::at( index );
    }
    virtual const T & at( uint index ) const {
        assert_index( index );
        return std::vector<T>::at( index );
    }

    T & front( ) {
        assert_index( 0 );
        return std::vector<T>::at( 0 );
    }
    T & back( ) {
        assert_index( size( ) - 1 );
        return std::vector<T>::at( size( ) - 1 );
    }

    varray<T> & push_back( const T & t ) {
        push( t, size( ) );
        return *this;
    }
    varray<T> & push_front( const T & t ) {
        push_back( t, 0 );
        return *this;
    }

    virtual varray<T> & push( const T & t, uint index ) {
        if( index == size( ) ) {
            std::vector<T>::push_back( t );
        } else {
            assert_index( index );
            std::vector<T>::insert( begin( ) + index, t );
        }
        return *this;
    }

    varray<T> & push_back( const varray<T> & v ) {
        push_varray( v, size( ) );
        return *this;
    }
    varray<T> & push_front( const varray<T> & v ) {
        push_varray( v, 0 );
        return *this;
    }

    virtual varray<T> & push_varray( const varray<T> & v, uint index ) {
        if( index != size( ) )
            assert_index( index );
        std::vector<T>::insert( begin( ) + index, v.begin( ), v.end( ) );
        return *this;
    }

    varray<T> & remove( ) {
        remove( size( ) - 1 );
        return *this;
    }
    varray<T> & remove_front( ) {
        remove( 0 );
        return *this;
    }
    varray<T> & remove_back( ) {
        remove( size( ) - 1 );
        return *this;
    }
    varray<T> & remove( uint index ) {
        remove( index, index + 1 );
        return *this;
    }
    virtual varray<T> & remove( uint index_start, uint index_end ) {
        assert_index( index_start );
        assert_index( index_end - 1 );
        std::vector<T>::erase( begin( ) + index_start, begin( ) + index_end );
        return *this;
    }

    uint size( ) const { return (uint)std::vector<T>::size( ); }

    varray<T> & resize( uint size ) {
        std::vector<T>::resize( size );
        return *this;
    }
    varray<T> & resize_more( uint size ) {
        resize( (uint)std::vector<T>::size( ) + size );
        return *this;
    }

    varray<T> & reserve( uint size ) {
        std::vector<T>::reserve( size );
        return *this;
    }
    varray<T> & reserve_more( uint size ) {
        reserve( (uint)std::vector<T>::size( ) + size );
        return *this;
    }

    virtual varray<T> & clear( bool shrink = false ) {
        std::vector<T>::clear( );
        if( shrink )
            std::vector<T>::shrink_to_fit( );
        return *this;
    }

    virtual varray<T> & reverse( ) {
        std::reverse( begin( ), end( ) );
        return *this;
    }

    virtual varray<T> & sort( std::function<bool( const T & t1, const T & t2 )> comparator ) {
        std::sort( begin( ), end( ), comparator );
        return *this;
    }

    auto begin( ) { return std::vector<T>::begin( ); }
    auto end( ) { return std::vector<T>::end( ); }

    auto begin( ) const { return std::vector<T>::begin( ); }
    auto end( ) const { return std::vector<T>::end( ); }

    T & operator[]( uint index ) { return at( index ); }
    const T & operator[]( uint index ) const { return at( index ); }

    virtual varray & operator=( const varray & v ) {
        clear( );
        reserve( v.size( ) );
        push_back( v );
        return *this;
    }

private:
    bool assert_index( uint index ) const {
        bool valid = valid_index( index );
        Assert( valid, "varray index (%ui) out of range, varray size: %ui", index, size( ) );
        return valid;
    }
};

} // namespace system
} // namespace axn

#endif /* Primitives_hpp */
