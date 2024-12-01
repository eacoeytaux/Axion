#ifndef Utility_hpp
#define Utility_hpp

#include "OS.hpp"
#include "STD.hpp"
#include "STDc.hpp"
#include "STDptr.hpp"

#include "Error.hpp"
#include "Assert.hpp"

#include "Namespaces.hpp"

namespace axn
{
namespace utility
{

// -- defines --

#define for_each( x, container ) \
    for( auto & x: container )

#define for_range( i, range ) \
    for( uint i = 0; i < (uint)range; ++i )

#define do_until_break( ) \
    while( true )

#define while_max( b, max )                 \
    uint while_max_count_##__COUNTER__ = 0; \
    while( ( b ) && ( while_max_count_##__COUNTER__++ < max ) )

#define static_once( )                      \
    static bool b_set_##__COUNTER__ = true; \
    if( b_set_##__COUNTER__ && !( b_set_##__COUNTER__ = false ) )

#define static_setup( Class, name )    \
    static Class name;                 \
    static bool name##_not_set = true; \
    if( name##_not_set && !( name##_not_set = false ) )

#define default_equal( Class )                        \
    bool operator==( const Class & c ) const = default; \
    bool operator!=( const Class & c ) const { return !( *this == c ); }

#define default_non_equal( Class ) \
    bool operator!=( const Class & c ) const { return !( *this == c ); }

#define default_virtual_equal( Class )                        \
    virtual bool operator==( const Class & c ) const = default; \
    virtual bool operator!=( const Class & c ) const { return !( *this == c ); }

#define default_virtual_non_equal( Class ) \
    virtual bool operator!=( const Class & c ) const { return !( *this == c ); }

inline void nothing( ) { }

// -- safe delete --

template <typename T>
void safe_delete( T *& t )
{
    if( t && dynamic_cast<T *>( t ) )
    {
        delete t;
    }

    t = nullptr;
}

// -- dec util functions --

const dec DEFAULT_DECIMAL_EPSILON = 0.0001;

inline bool is_int( const dec d )
{
    return ( d == rint( d ) );
}

inline bool is_zero( const dec d )
{
    return ( d == ZERO );
}

inline bool is_infinity( const dec d )
{
    return ( d == INFINITY_POSITIVE ) || ( d == INFINITY_NEGATIVE );
}

inline bool dec_eq( const dec x, const dec y, const dec epsilon = DEFAULT_DECIMAL_EPSILON )
{
    return ( abs( x - y ) < epsilon );
}

inline bool dec_neq( const dec x, const dec y, const dec epsilon = DEFAULT_DECIMAL_EPSILON )
{
    return ( !dec_eq( x, y, epsilon ) );
}

inline bool dec_gt( const dec x, const dec y, const dec epsilon = DEFAULT_DECIMAL_EPSILON )
{
    return ( ( x - epsilon ) > y );
}

inline bool dec_ge( const dec x, const dec y, const dec epsilon = DEFAULT_DECIMAL_EPSILON )
{
    return ( ( ( x - epsilon ) > y ) || dec_eq( x, y ) );
}

inline bool dec_lt( const dec x, const dec y, const dec epsilon = DEFAULT_DECIMAL_EPSILON )
{
    return ( ( x + epsilon ) < y );
}

inline bool dec_le( const dec x, const dec y, const dec epsilon = DEFAULT_DECIMAL_EPSILON )
{
    return ( ( ( x + epsilon ) < y ) || dec_eq( x, y ) );
}

inline bool divisible( const dec x, const dec y )
{
    return !is_zero( remainder( x, y ) );
}

inline dec remainder_pos( const dec x, const dec y )
{
    dec r = remainder( x, y );
    while( dec_lt( r, ZERO ) )
    {
        r += y;
    }
    return r;
}

// -- misc. util functions --

template <typename T>
bool is_zero( const T & t )
{
    return ( t == ZERO );
}

template <typename T>
bool is_positive( const T & t )
{
    return ( t > ZERO );
}

template <typename T>
bool is_negative( const T & t )
{
    return ( t < ZERO );
}

template <typename T>
void swap_values( T & t1, T & t2 )
{
    T t0 = t1;
    t1 = t2;
    t2 = t0;
}

template <typename T>
T inverse( const T & t )
{
    Assert( (bool)t, "cannot divide by zero" );
    return ( ONE / t );
}

template <typename T>
void invert( T & t )
{
    Assert( (bool)t, "cannot divide by zero" );
    return t = ( ONE / t );
}

template <typename T>
T half( const T & t )
{
    return ( t / TWO );
}

//template <typename T>
//T pow( const T & t, const uint p )
//{
//    if( !p )
//    {
//        return ONE;
//    }
//    else if( p == ONE )
//    {
//        return t;
//    }
//
//    T ret = t;
//    for_range( i, p - ONE ) ret *= t;
//    return ret;
//}

template <typename T>
T square( const T & t )
{
    return ( t * t );
}

template <typename T>
T cube( const T & t )
{
    return ( t * t * t );
}

template <typename T>
const T & min( const T & t1, const T & t2 )
{
    if( t1 <= t2 )
    {
        return t1;
    }
    else
    {
        return t2;
    }
}

template <typename T>
const T & max( const T & t1, const T & t2 )
{
    if( t1 >= t2 )
    {
        return t1;
    }
    else
    {
        return t2;
    }
}

template <typename T>
const T & min( const varray<T> & list )
{
    Assert( list.size( ), "list must not be empty" );

    int min_i = ZERO;

    for_range( i, list.size( ) - ONE )
    {
        if( list[ i + ONE ] < list[ min_i ] )
        {
            min_i = i + ONE;
        }
    }

    return list[ min_i ];
}

template <typename T>
const T & max( const varray<T> & list )
{
    Assert( list.size( ), "list must not be empty" );

    int max_i = ZERO;

    for_range( i, list.size( ) - ONE )
    {
        if( list[ i + ONE ] > list[ max_i ] )
        {
            max_i = i + ONE;
        }
    }

    return list[ max_i ];
}

template <typename T>
T negative( const T & t, bool f = true )
{
    return f ? -t : t;
}

template <typename T>
T log_base( const T & base, const T & t )
{
    return ( log( t ) / log( base ) );
}

inline uint factorial( const uint n )
{
    uint f = ONE;
    if( n )
    {
        for_range( i, n - ONE )
        {
            f *= ( i + TWO );
        }
    }
    return f;
}

inline uint pascal( const uint row, const uint col )
{
    Assert( col <= row );

    if( !row || !col || ( col == row ) )
    {
        return ONE;
    }
    else if( ( col == ONE ) || ( col == ( row - ONE ) ) )
    {
        return row;
    }
    else
    {
        return ( factorial( row ) / ( factorial( col ) * factorial( row - col ) ) );
    }
}

inline varray<uint> pascal( const uint row )
{
    varray<uint> v( row + ONE );

    for_range( i, half( row ) + ONE )
    {
        v[ i ] = v[ row - i ] = pascal( row, i );
    }

    return v;
}

// -- labels --

template <typename T>
inline string Label( const T & t )
{
    return typeid( t ).name( );
}

template <typename T>
inline const string & Label( )
{
    static const string label = Label( T( ) );
    return label;
}

// -- util classes --

class Identifiable
{
private:
    uint m_id;

public:
    uint id( ) const { return m_id; }

    virtual ~Identifiable( ) { }
    Identifiable( )
    {
        static uint total_ids = ZERO;
        m_id = ++total_ids;
        Assert( m_id != ZERO );
    }

    operator uint( ) const { return m_id; }

    default_equal( Identifiable );

    struct Hasher
    {
        size_t operator( )( const Identifiable & i ) const { return hash<int>( )( i.id( ) ); }
    };
};

class IDoset : public oset<Identifiable, Identifiable::Hasher>
{
};

class IDuset : public uset<Identifiable, Identifiable::Hasher>
{
};

template <typename T>
class IDomap : public omap<Identifiable, T, Identifiable::Hasher>
{
};

template <typename T>
class IDumap : public umap<Identifiable, T, Identifiable::Hasher>
{
};

class Counter
{
private:
    uint m_countdown_top;
    uint m_countdown_remaining;

public:
    virtual ~Counter( ) { }
    Counter( const uint countdown = ZERO ) : m_countdown_top( countdown ), m_countdown_remaining( countdown ) { }

    bool tick( )
    {
        if( m_countdown_remaining )
        {
            return !m_countdown_remaining--;
        }
        else
        {
            return true;
        }
    }

    void reset( )
    {
        m_countdown_remaining = m_countdown_top;
    }

    void reset( const uint countdown )
    {
        set( countdown );
        reset( );
    }

    void set( const uint countdown )
    {
        m_countdown_top = countdown;
    }

    uint remaining( ) const { return m_countdown_remaining; }

    default_equal( Counter );
};

template <typename T>
class Span
{
private:
    T m_min, m_max;

public:
    virtual ~Span( ) { }

    Span( ) : m_min( T( ZERO ) ), m_max( T( ZERO ) ) { }
    Span( const T & value ) : m_min( value ), m_max( value ) { }
    Span( const T & min_value, const T & max_value ) : m_min( min_value ), m_max( max_value )
    {
        if( m_min > m_max )
        {
            swap_values( m_min, m_max );
        }
    };

    const T & min( ) const { return m_min; }
    const T & max( ) const { return m_max; }

    T range( ) const { return T( max( ) - min( ) ); }

    template <typename T2>
    Span & operator=( const varray<T2> & v )
    {
        Assert( ( v.size( ) == ONE ) || ( v.size( ) == 2 ), "varray must have exactly 1 or 2 elements" );

        m_min = T( v[ ZERO ] );

        if( v.size( ) == ONE )
        {
            m_max = m_min;
        }
        else
        {
            m_max = T( v[ ONE ] );

            if( m_min > m_max )
            {
                swap_values( m_min, m_max );
            }
        }

        return *this;
    }

    default_equal( Span );
};

template <typename T>
class Slider
{
private:
    T m_value, m_min_value, m_max_value;

public:
    virtual ~Slider( ) { }

    Slider( ) : m_min_value( ZERO ), m_max_value( ZERO ) { value( min( ) ); }
    Slider( const T & max_value ) : m_min_value( ZERO ), m_max_value( max_value ) { value( min( ) ); }
    Slider( const T & min_value, const T & max_value ) : m_min_value( ( min_value < max_value ) ? min_value : max_value ), m_max_value( ( min_value < max_value ) ? max_value : min_value ) { value( min( ) ); }

    const T & value( ) const { return m_value; }
    void value( const T & value, bool set_new_min_max_value = false )
    {
        Assert( ( set_new_min_max_value || ( ( value >= min( ) ) && ( value <= max( ) ) ) ), "value must be in range" );

        m_value = value;

        if( set_new_min_max_value )
        {
            if( m_value > max( ) )
            {
                max( m_value );
            }

            if( m_value < min( ) )
            {
                min( m_value );
            }
        }
    }

    const T & min( ) const { return m_min_value; }
    void min( const T & min_value )
    {
        m_min_value = min_value;

        if( m_value < m_min_value )
        {
            value( m_min_value );
        }
    }

    const T & max( ) const { return m_max_value; }
    void max( const T & max_value )
    {
        m_max_value = max_value;

        if( m_value > m_max_value )
        {
            value( m_max_value );
        }
    }

    T range( ) const { return T( max( ) - min( ) ); }

    dec value_percentage( ) const { return ( (dec)( m_value - m_min_value ) / (dec)( m_max_value - m_min_value ) ); }
    void value_percentage( const dec p, bool set_new_min_max_value = false )
    {
        value( ( ( m_max_value - m_min_value ) * p ) + m_min_value, set_new_min_max_value );
    }

    void delta( const T & delta, bool set_new_min_max_value = false )
    {
        value( m_value + delta, set_new_min_max_value );
    }

    default_equal( Slider );
};

// -- range util functions --

template <typename T>
bool in_range( const T & t, const T & low, const T & high, bool include_low, bool include_high )
{
    T l = low;
    T h = high;

    bool il = include_low;
    bool ih = include_high;

    if( l > h )
    {
        swap_values<T>( l, h );
        swap_values<bool>( il, ih );
    }

    return ( ( t > l ) && ( t < h ) ) || ( il && ( t == l ) ) || ( ih && ( t == h ) );
}

template <typename T>
bool in_range( const T & t, const T & low, const T & high, bool inclusive = true )
{
    return in_range<T>( t, low, high, inclusive, inclusive );
}

template <typename T>
bool in_range( const T & t, const T & high, bool inclusive = true )
{
    return in_range<T>( t, T( ZERO ), high, inclusive, inclusive );
}

template <typename T>
bool in_range( const T & t, const Span<T> & span, bool inclusive = true )
{
    return in_range<T>( t, span.min( ), span.max( ), inclusive, inclusive );
}

} // namespace utility
} // namespace axn

#endif /* Utility_hpp */
