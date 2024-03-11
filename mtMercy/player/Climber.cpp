#include "Climber.hpp"

#include "Arrow.hpp"

AXN_NAMESPACES
using mtmercy::Climber;

Climber::Climber( World * world, const Coordinate & _position )
: Player( world, _position )
, m_hook( world, position(), position() )
{
    space( Shape::rectangle( WIDTH, HEIGHT ) );
    
    m_movement_speed = DEFAULT_SPEED;
    
    m_reload_timer.reset( 0 );
    m_rope_max_length = DEFAULT_ROPE_MAX_LENGTH;
    m_hook_growth_speed = DEFAULT_HOOK_GROWTH_SPEED;
    m_hook_retract_speed = DEFAULT_HOOK_RETRACT_SPEED;
    //m_hook.max_length( m_rope_max_length );
    
    m_arrow_feather_color = Random::rng_Color();
    
    //border( BLACK, 2.f );
}

Climber & Climber::input( Input * _input ) {
    if ( KeyInput * key_input = dynamic_cast<KeyInput*>( _input ) ) {
        KeyInput::KEY key = key_input->key;
        KeyInput::DYNAMIC dynamic = key_input->dynamic;
        
        bool pressed = ( dynamic == KeyInput::PRESSED );
        bool held = ( dynamic == KeyInput::HELD );
        
        bool down = ( pressed || held );
        
        switch ( key ) {
            case ' ': {
                if ( down ) jump( true );
                else jump( false );
                break;
            }
            case 'w': {
                if ( down ) looking_up(  true );
                else looking_up( false );
                break;
            }
            case 's': {
                if ( down ) looking_down( true );
                else looking_down( false );
                break;
            }
            case 'a': {
                if ( down ) moving_left( true );
                else moving_left( false );
                break;
            }
            case 'd': {
                if ( down ) moving_right( true );
                else moving_right( false );
                break;
            }
            case 'g': {
                if (  pressed ) god( !god(), 666 );
                break;
            }
            default: {
                break;
            }
        } return *this;
    }
    
    if (MouseInput * mouse_input = dynamic_cast<MouseInput*>( _input )) {
        MouseInput::BUTTON button = mouse_input->button;
        MouseInput::DYNAMIC dynamic = mouse_input->dynamic;
        
        bool move = ( dynamic == MouseInput::MOVE );
        bool pressed = ( dynamic == MouseInput::PRESSED );
        bool released = ( dynamic == MouseInput::RELEASED );
        
        if ( move ) {
            aim( Angle( position(), mouse_input->position ) );
        } else {
            if ( released ) {
                release_bow( );
            } else if ( pressed ) {
                if ( button == MouseInput::LEFT_BUTTON ) {
                    fire_arrow( );
                } else if ( button == MouseInput::RIGHT_BUTTON ) {
                    fire_hook( );
                }
            }
        } return *this;
    }
    
    return *this;
}

Climber & Climber::update( ) {
    Player::update( );
    
    m_reload_timer.tick( );
    
    if ( m_firing_arrow || m_firing_hook ) {
        drawing_dirty( true );
        if ( !m_reload_timer.remaining() ) {
            if ( m_firing_arrow ) {
                //static SoundClip twang( "twang.wav" );
                //twang.play( 0.5f );
                world()->add_object(new Arrow( world(), position() + VectorA( aim_angle(), 32.f ), VectorA( aim_angle() /* + ( ( Random::r_Angle() / 16.f ) - PI_1D16 ) */, 32.f ) + velocity(), m_arrow_feather_color ) );
                
                m_arrow_feather_color = Random::rng_Color();
            } else {
                m_hook.fire( VectorA( aim_angle(), 32.f ) );
            }
            m_reload_timer.reset( RELOAD_TIME );
        }
    } else if ( !m_firing_hook ) {
        drawing_dirty( true );
        m_hook.retract( );
        //m_hook.position( position() + Vector( aim_angle(), 32 ) );
        //m_hook.angle( aim_angle() );
    }
    
    m_hook.base( position(), aim_angle() );
    m_hook.update();
    
    return *this;
}

Climber & Climber::adjust_velocity( ) {
    if ( !gravity_ratio() ) { // can fly?
        if ( m_looking_up ) {
            if ( m_moving_right && !m_moving_left ) {
                add_velocity( VectorA( Angle( PI_1D4 ), m_movement_speed ) - Vector( m_movement_speed, (planc)0.f ) );
            } else if ( m_moving_left && !m_moving_right ) {
                add_velocity( VectorA( Angle( PI_3D4 ), m_movement_speed ) - Vector( -m_movement_speed, (planc)0.f ) );
            } else {
                add_velocity( Vector( (planc)0.f, m_movement_speed ) );
            }
        }
        if ( m_looking_down ) {
            if ( m_moving_right && !m_moving_left ) {
                add_velocity( VectorA( Angle( PI_7D4 ), m_movement_speed ) - Vector( m_movement_speed, (planc)0.f ) );
            } else if ( m_moving_left && !m_moving_right ) {
                add_velocity( VectorA( Angle( PI_5D4 ), m_movement_speed ) - Vector( -m_movement_speed, (planc)0.f ) );
            } else {
                add_velocity( Vector( (planc)0.f, -m_movement_speed ) );
            }
        }
    }
        
    if ( m_moving_right ) {
        add_velocity( Vector( m_movement_speed, (planc)0.f ) );
    }
    if ( m_moving_left ) {
        add_velocity( Vector( -m_movement_speed, (planc)0.f ) );
    }
    
    Player::adjust_velocity( );
    
    if ( m_ground ) m_jump_reset_timer.tick();
    if ( m_jumping ) {
        if ( !m_jump_reset_timer.remaining() && m_ground ) {
            m_jumping_timer.reset( JUMP_MAX_DURATION );
            m_jump_reset_timer.reset( JUMP_RESET_WAIT_TIME );
            m_jump_degradation = 0.f;
        }
        
        if ( !m_jumping_timer.tick() ) {
            add_velocity( ( Vector( (planc)0.f, JUMP_STRENGTH ) - GRAVITY ) * ( 1.f - m_jump_degradation ) );
            m_jump_degradation = 1.f - ( JUMP_DEGRATION_RATIO * (  1.f - m_jump_degradation ) );
            m_ground = nullptr;
        }
    } else {
        m_jumping_timer.reset( 0 );
    }
    
    velocity( velocity() * 0.9 );
    
    return *this;
}

bool Climber::moving_right( ) const {
    return m_moving_right && !m_moving_left;
}

Climber & Climber::moving_right( const bool _moving ) {
    m_moving_right = _moving;
    drawing_dirty( true );
    return *this;
}

bool Climber::moving_left( ) const {
    return m_moving_left && !m_moving_right;
}

Climber & Climber::moving_left( const bool _moving ) {
    m_moving_left = _moving;
    drawing_dirty( true );
    return *this;
}

bool Climber::looking_up( ) const {
    return m_looking_up && !m_looking_down;
}

Climber & Climber::looking_up( const bool _looking ) {
    m_looking_up = _looking;
    drawing_dirty( true );
    return *this;
}

bool Climber::looking_down( ) const {
    return m_looking_down && !m_looking_up;
}

Climber & Climber::looking_down( const bool _looking ) {
    m_looking_down = _looking;
    drawing_dirty( true );
    return *this;
}

Climber & Climber::jump( const bool _jumping ) {
    m_jumping = _jumping;
    drawing_dirty( true );
    return *this;
}

Angle Climber::aim_angle( ) const {
    return m_aim_angle;
}

Climber & Climber::aim( const Angle & _aim_angle ) {
    m_aim_angle = _aim_angle;
    drawing_dirty( true );
    return *this;
}

Climber & Climber::fire_hook( ) {
    m_firing_hook = true;
    drawing_dirty( true );
    return *this;
}

Climber & Climber::fire_arrow() {
    m_firing_arrow = true;
    drawing_dirty(true);
    return *this;
}

Climber & Climber::release_bow( ) {
    m_firing_hook = false;
    m_firing_arrow = false;
    drawing_dirty( true );
    return *this;
}

Coordinate Climber::head_center( ) const {
    return Coordinate( 0.f, BODY_HEIGHT + NECK_LENGTH );
}

Coordinate Climber::face_center( ) const {
    return head_center() + VectorA( aim_angle(), ( HEAD_RADIUS - FACE_RADIUS ) / 2.f );
}

Coordinate Climber::front_shoulder( ) const {
    bool mirror = ( ( aim_angle() > PI_1D2 ) && ( aim_angle() < PI_3D2 ) );
    Coordinate shoulder = Coordinate( -( BODY_WIDTH.half() ) + 4.f, ( BODY_HEIGHT.half() ) - ( ARM_WIDTH.half() ) );
    if ( mirror ) shoulder.mirror_y( );
    return shoulder;
}

Coordinate Climber::front_hand( ) const {
    bool mirror = ( ( aim_angle() > PI_1D2 ) && ( aim_angle() < PI_3D2 ) );
    Coordinate hand = VectorA( aim_angle() + ( PI_1D4 * ( mirror ? 1.f : -1.f ) ), (planc)12.f ).destination();
    return hand;
}

Coordinate Climber::back_shoulder( ) const {
    bool mirror = ( ( aim_angle() > PI_1D2 ) && ( aim_angle() < PI_3D2 ) );
    Coordinate shoulder = front_shoulder();
    if (!mirror) shoulder.mirror_y( );
    return shoulder;
}

Coordinate Climber::back_hand( ) const {
    bool mirror = ( ( aim_angle() > PI_1D2 ) && ( aim_angle() < PI_3D2 ) );
    Coordinate hand = VectorA( aim_angle() - ( PI_1D8 * ( mirror ? -1.f : 1.f ) ), 24.f ).destination();
    return hand;
}

Coordinate Climber::front_hip( ) const {
    return Coordinate( ( PANTS_WIDTH.half() - LEG_WIDTH.half() ) * ( facing_left() ? 1.f : -1.f ), -BODY_HEIGHT.half() );
}

Coordinate Climber::front_foot( ) const {
    return front_hip() - Vector( (planc)0.f, LEG_HEIGHT );
}

Coordinate Climber::back_hip( ) const {
    return Coordinate( ( PANTS_WIDTH.half() - LEG_WIDTH.half() ) * ( facing_left() ? -1.f : 1.f ), -BODY_HEIGHT.half() );
}

Coordinate Climber::back_foot( ) const {
    return back_hip() - Vector( (planc)0.f, LEG_HEIGHT );
}
