#ifndef Slider_hpp
#define Slider_hpp

#include "axn.hpp"

namespace axn {
namespace utility {

template <typename T> class Slider {
public:
    Slider( const T & max = T( 0 ) ) // TODO add min
        : m_max_value( max ) {
        value( 0 );
    }

    T max_value( ) const { return m_max_value; }

    T value( ) const { return m_value; }
    Slider & value( const T & value, bool set_new_max_value = false ) {
        Assert( ( !set_new_max_value && ( value <= m_max_value ) ) && ( value >= 0 ),
                "value must be in range" );
        if( set_new_max_value )
            m_max_value = max<T>( m_max_value, value );
        m_value = value;
        return *this;
    }

    float value_percentage( ) const { return ( (float)m_value / (float)m_max_value ); }
    Slider & value_percentage( const float p, bool set_new_max_value = false ) {
        m_value = ( ( m_max_value - m_min_value ) * p ) + m_min_value;
        if( set_new_max_value )
            m_max_value = max<T>( m_max_value, m_value );
        return *this;
    }

    Slider & delta( const T & delta ) {
        value( m_value + delta );
        return *this;
    }

private:
    T m_value;
    T m_max_value;
    T m_min_value;
};

} // namespace utility
} // namespace axn

#endif /* Slider_hpp */
