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

#define for_each( x, v ) for( auto & x: v )

#define for_range_selector( x, r, i, func, ... ) func
#define for_range_i( r ) for( uint i = 0; i < (uint)r; ++i )
#define for_range_x( i, r ) for( uint i = 0; i < (uint)r; ++i )
#define for_range( ... ) for_range_selector(, ##__VA_ARGS__, for_range_x( __VA_ARGS__ ), for_range_i( __VA_ARGS__ ) )

#define static_once( )                      \
    static bool b_set_##__COUNTER__ = true; \
    if( b_set_##__COUNTER__ && !( b_set_##__COUNTER__ = false ) || b_set_##__COUNTER__ )

#define static_setup( Class, name )    \
    static Class name;                 \
    static bool name##_not_set = true; \
    if( name##_not_set && !( name##_not_set = false ) || name##_not_set )

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

// -- double util functions --

const double DEFAULT_FLOAT_EPSILON = 0.00000001;

inline bool is_infinity( const double d )
{
    return ( d == INFINITY_POSITIVE ) || ( d == INFINITY_NEGATIVE );
}

inline bool double_eq( const double x, const double y, const double epsilon = DEFAULT_FLOAT_EPSILON )
{
    if( abs( x - y ) < epsilon )
        return true; // equals!
    else
        return false;
}

inline bool double_neq( const double x, const double y, const double epsilon = DEFAULT_FLOAT_EPSILON )
{
    return !double_eq( x, y, epsilon );
}

inline bool double_gt( const double x, const double y, const double epsilon = DEFAULT_FLOAT_EPSILON )
{
    return ( ( x - epsilon ) > y );
}

inline bool double_ge( const double x, const double y, const double epsilon = DEFAULT_FLOAT_EPSILON )
{
    return ( ( ( x - epsilon ) > y ) || double_eq( x, y ) );
}

inline bool double_lt( const double x, const double y, const double epsilon = DEFAULT_FLOAT_EPSILON )
{
    return ( ( x + epsilon ) < y );
}

inline bool double_le( const double x, const double y, const double epsilon = DEFAULT_FLOAT_EPSILON )
{
    return ( ( ( x + epsilon ) < y ) || double_eq( x, y ) );
}

// -- misc. util functions --

template <typename T>
void swap_values( T & t1, T & t2 )
{
    T tmp = t1;
    t1 = t2;
    t2 = tmp;
}

template <typename T>
T half( const T & t )
{
    return t / 2.0;
}

template <typename T>
void halve( T & t )
{
    t = t / T( 2 );
}

template <typename T>
T pow( const T & t, const uint p )
{
    if( !p )
        return T( );
    else if( p == 1 )
        return t;

    T ret = t;
    for_range( p - 1 ) ret *= t;
    return ret;
}

template <typename T>
T & square( T & t )
{
    return ( t *= t );
}

template <typename T>
T squared( const T & t )
{
    return ( t * t );
}

template <typename T>
T & cube( T & t )
{
    return ( t *= t * t );
}

template <typename T>
T cubed( const T & t )
{
    return ( t * t * t );
}

template <typename T>
const T & min( const T & t1, const T & t2 )
{
    if( t1 <= t2 )
        return t1;
    else
        return t2;
}

template <typename T>
const T & max( const T & t1, const T & t2 )
{
    if( t1 >= t2 )
        return t1;
    else
        return t2;
}

template <typename T>
const T & min( const varray<T> & list )
{
    Assert( list.size( ), "list must not be empty" );
    T & ret = list[ 0 ];
    for_each( t, list ) if( t < ret ) ret = t;
    return ret;
}

template <typename T>
const T & max( const varray<T> & list )
{
    Assert( list.size( ), "list must not be empty" );
    T & ret = list[ 0 ];
    for_each( t, list ) if( t > ret ) ret = t;
    return ret;
}

template <typename T>
T & flip( T & t, bool f = true )
{
    if( f )
        t = -t;
    return t;
}

template <typename T>
T flipped( const T & t, bool f = true )
{
    if( f )
        return -t;
    else
        return t;
}

// -- labels --

template <typename T>
inline string Label( const T & t )
{
    return typeid( t ).name( );
}

template <typename T>
inline string Label( )
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
        static uint total_ids = 0;
        m_id = ++total_ids;
        Assert( m_id != 0 );
    }

    operator uint( ) const { return m_id; }

    bool operator==( const Identifiable & i ) const { return id( ) == i.id( ); }
    bool operator!=( const Identifiable & i ) const { return id( ) != i.id( ); }

    struct Hasher
    {
        size_t operator( )( const Identifiable & i ) const { return hash<int>( )( i.id( ) ); }
    };
};

template <typename T>
class IDset : public set<Identifiable, Identifiable::Hasher>
{
};

template <typename T>
class IDuset : public uset<Identifiable, Identifiable::Hasher>
{
};

template <typename T>
class IDmap : public map<Identifiable, T, Identifiable::Hasher>
{
};

template <typename T>
class IDumap : public umap<Identifiable, T, Identifiable::Hasher>
{
};

template <typename T>
class Span
{
public:
    virtual ~Span( ){ };
    Span( const T & max ) : m_min( max ), m_max( max ) { }
    Span( const T & min, const T & max ) : m_min( min ), m_max( max )
    {
        if( m_min > m_max )
        {
            swap_values( m_min, m_max );
        }
    };

    const T & min( ) const { return m_min; }
    const T & max( ) const { return m_max; }

    T range( ) const { return T( m_max - m_min ); }

    template <typename T2>
    Span & operator=( const varray<T2> & v )
    {
        Assert( ( v.size( ) == 1 ) || ( v.size( ) == 2 ), "varray must have exactly 1 or 2 elements" );

        m_min = (T)v[ 0 ];

        if( v.size( ) == 1 )
        {
            m_max = m_min;
        }
        else
        {
            m_max = (T)v[ 1 ];

            if( m_min > m_max )
            {
                T tmp = m_min;
                m_min = m_max;
                m_max = tmp;
            }
        }

        return *this;
    }

private:
    T m_min, m_max;
};

template <typename T>
class Slider
{
public:
    Slider( const T & min, const T & max ) : m_min_value( ( min < max ) ? min : max ), m_max_value( ( min < max ) ? max : min )
    {
        value( min_value( ) );
    }

    Slider( const T & max = T( 0 ) ) : m_min_value( 0 ), m_max_value( max )
    {
        value( min_value( ) );
    }

    const T & value( ) const { return m_value; }
    Slider & value( const T & value, bool set_new_min_max_value = false )
    {
        Assert( ( set_new_min_max_value || ( ( value >= min_value( ) ) && ( value <= max_value( ) ) ) ), "value must be in range" );

        m_value = value;

        if( set_new_min_max_value )
        {
            if( m_value > max_value( ) )
                max_value( m_value );
            else if( m_value < min_value( ) )
                min_value( m_value );
        }

        return *this;
    }

    const T & min_value( ) const { return m_min_value; }
    Slider & min_value( const T & min_value )
    {
        m_min_value = min_value;

        if( m_value < m_min_value )
            value( m_min_value );

        return *this;
    }

    const T & max_value( ) const { return m_max_value; }
    Slider & max_value( const T & max_value )
    {
        m_max_value = max_value;

        if( m_value > m_max_value )
            value( m_max_value );

        return *this;
    }

    double value_percentage( ) const { return ( (double)( m_value - m_min_value ) / (double)( m_max_value - m_min_value ) ); }
    Slider & value_percentage( const double p, bool set_new_min_max_value = false )
    {
        return value( ( ( m_max_value - m_min_value ) * p ) + m_min_value, set_new_min_max_value );
    }

    Slider & delta( const T & delta, bool set_new_min_max_value = false )
    {
        return value( m_value + delta, set_new_min_max_value );
    }

private:
    T m_value;
    T m_min_value;
    T m_max_value;
};

class Counter
{
public:
    virtual ~Counter( ) { }
    Counter( const uint countdown = 0 ) : m_countdown( countdown ) { }

    bool tick( )
    {
        if( m_countdown )
        {
            return !m_countdown--;
        }
        else
        {
            return true;
        }
    }
    Counter & reset( const uint countdown )
    {
        m_countdown = countdown;
        return *this;
    }
    uint remaining( ) const { return m_countdown; }

private:
    uint m_countdown;
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
    return in_range<T>( t, T( 0 ), high, inclusive, inclusive );
}

template <typename T>
bool in_range( const T & t, const Span<T> & span, bool inclusive = true )
{
    return in_range<T>( t, span.min( ), span.max( ), inclusive, inclusive );
}

} // namespace utility
} // namespace axn

#endif /* Utility_hpp */
