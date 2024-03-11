#include "Hook.hpp"

AXN_NAMESPACES
using axn::reality::Hook;

const planc HOOK_LENGTH = 26.f;
const planc HOOK_THICKNESS = 5.f;
const Angle HOOK_ANGLE = Angle( PI_1D4 );
const Color HOOK_COLOR = Color::rgb( 0x9C9C9C );

const planc ROPE_WIDTH = 4.f;
const Color ROPE_BASE_COLOR = Color::rgb( 0xDAA420 );
const Color ROPE_ALT_COLOR = Color::rgb( 0xB9870F );

const planc RETRACT_SPEED = 13.f;

Hook::Hook( World * world, const Coordinate & _hook_tip, const Coordinate & _base, const Angle & _angle )
: Object( world )
{
    position( _hook_tip );
    m_base = _base;
    m_angle = _angle;
    
    m_retract_speed = RETRACT_SPEED;
}

const Hook & Hook::render( ) const {
    Object::render( );
    
    Coordinate tip = hook_tip() - Hook::base();
    Coordinate base = hook_base() - Hook::base();
    Angle hook_angle( base, tip );

    draw( HOOK_COLOR, Shape::circle( HOOK_THICKNESS, base ) );
    draw( HOOK_COLOR, Line( tip, base ), HOOK_THICKNESS );

    draw( HOOK_COLOR, Shape::triangle( tip + VectorA( hook_angle, HOOK_THICKNESS * 2.5f ), tip + VectorA( hook_angle + PI_1D2, HOOK_THICKNESS * 1.25f ), tip + VectorA( hook_angle - PI_1D2, HOOK_THICKNESS * 1.25f ) ) );

    //draw( HOOK_COLOR, Square( HOOK_THICKNESS, tip, hook_angle + PI_1D4 ) );
    //draw( HOOK_COLOR, Line( tip, tip + Vector( hook_angle + ( PI * 0.64f ), HOOK_HEAD_LENGTH ) ), HOOK_THICKNESS );
    //draw( HOOK_COLOR, Line( tip, tip + Vector( hook_angle - ( PI * 0.64f ), HOOK_HEAD_LENGTH ) ), HOOK_THICKNESS );
    
    Vector rope_vector( base );
    
    // draw rope base
    switch ( m_state ) {
        case FIRING:
            // TODO sine wave
        case RETRACTING:
            draw( ROPE_BASE_COLOR, Line( rope_vector.origin(), rope_vector.destination() ), ROPE_WIDTH );
            break;
        default:
            break;
    }
    
    // draw rope detail coils
    Vector rope_chunk = VectorA( rope_vector.angle(), ROPE_WIDTH );
    for_range ( (uint)( rope_vector.magnitude() / ROPE_WIDTH.half() )) {
        Shape rope_strip_rect = Shape::rectangle( ROPE_WIDTH, ROPE_WIDTH, rope_vector.origin() + ( rope_chunk * i * 2 ), rope_chunk.angle() );
        draw( ROPE_ALT_COLOR, rope_strip_rect );
    }
    
    return *this;
}

Hook & Hook::update( ) {
    float remaining_percentage = 1.f;
    while ( remaining_percentage ) {
        if ( m_state == LOADED ) {
            gravity_ratio( 0.f );
            velocity( Vector( (planc)0.f, (planc)0.f ) );
        } else {
            gravity_ratio( 0.f );
            if ( m_state == FIRING ) {
                length( base().distance( position() ) );
                if ( length() > max_length() ) {
                    m_state = RETRACTING;
                    // TODO adjust for overshot with remaining percentage
                }
            } else if ( m_state == RETRACTING ) {
                //gravity_ratio( 0.25f );
                length( length() - m_retract_speed );
                if ( length() <= 0.f ) m_state = LOADED;
                else position( VectorA( Angle( base(), position() ), length() ).destination() );
            }
        }
        
        Angle delta;
        if ( velocity().magnitude() ) delta = velocity().angle() - m_angle;
        m_angle = Angle( base(), hook_tip() );
        
        remaining_percentage = 0.f; // TODO
    }
    
    Object::update( );
    return *this;
}

Angle Hook::angle( ) const {
    return m_angle;
}

Hook & Hook::angle( const Angle & _angle ) {
    m_angle = _angle;
    return *this;
}

Coordinate Hook::hook_tip( ) const {
    return position();
}

Coordinate Hook::hook_base( ) const {
    if ( m_state == LOADED ) return hook_tip() - VectorA( angle(), HOOK_LENGTH );
    else return hook_tip() - VectorA( Angle( base(), hook_tip() ), HOOK_LENGTH );
}

Coordinate Hook::base( ) const {
    return m_base;
}
    
Hook & Hook::base( const Coordinate & _base, const Angle & _angle ) {
    m_base = _base;
    if ( m_state == LOADED ) {
        angle( _angle );
        position( _base + VectorA( angle(), HOOK_LENGTH ) );
    }
    return *this;
}

planc Hook::length( ) const {
    return m_length;
}

Hook & Hook::length( const planc & _length ) {
    m_length = _length;
    return *this;
}

planc Hook::max_length( ) const {
    return m_max_length;
}

Hook & Hook::max_length( const planc & _max_length ) {
    m_max_length = _max_length;
    return *this;
}

Hook & Hook::fire( const Vector & _launch_speed ) {
    switch ( m_state ) {
        case LOADED:
            velocity( _launch_speed );
            m_state = FIRING;
            //static SoundClip twang( "twang.wav" );
            //twang.play( );
            break;
        case FIRING:
        case HOOKED:
        case RETRACTING:
            break;
        default:
            Assert( false, "invalid_state" );
            break;
    }
    
    return *this;
}

Hook & Hook::retract( ) {
    switch ( m_state ) {
        case LOADED: {
            break;
        }
        case FIRING:
        case HOOKED: {
            m_state = RETRACTING;
            break;
        }
        case RETRACTING: {
            break;
        }
        default: {
            Assert( false, "invalid_state" );
            break;
        }
    }
    
    return *this;
}

Hook & Hook::load( ) {
    m_state = LOADED;
    m_ground = nullptr;
    return *this;
}
