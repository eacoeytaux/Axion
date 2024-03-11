#include "MohawkMan.hpp"

#include "Arrow.hpp" // TODO

AXN_NAMESPACES
using mtmercy::MohawkMan;

const MohawkMan & MohawkMan::render( ) const {
    Player::render( );
    
    // TODO draw in world?
    //m_hook.render();
    //draw(m_hook.drawing());
    
    // legs
    {
        static_setup( Drawing, boot_drawing ) {
            Shape boot_base = Shape::rectangle( LEG_WIDTH + 7.f, 5.f, Coordinate( 1.5f, 0.f ) );
            Shape boot_shin = Shape::rectangle( LEG_WIDTH + 4.f, 9.f, Coordinate( 0.f, 2.f ) );
            Shape boot_sole = Shape::rectangle( LEG_WIDTH + 7.f, 2.f, Coordinate( 1.5f, -3.f ) );
            Line lace_1 = Line( Coordinate( 5.f, 5.f ), Coordinate( -3.f, 5.f ) );
            Line lace_2 = Line( Coordinate( 5.f, 3.f ), Coordinate( -3.f, 3.f ) );
            boot_drawing.add( SOLE_COLOR, boot_sole );
            boot_drawing.add( BOOT_COLOR, boot_shin );
            boot_drawing.add( LACE_COLOR, lace_1, 1.f );
            boot_drawing.add( LACE_COLOR, lace_2, 1.f );
            boot_drawing.add( BOOT_COLOR, boot_base );
        }
        
        // back leg
        {
            Coordinate hip = back_hip();
            Coordinate foot = back_hip() - Vector( (planc)0.f, LEG_HEIGHT );
            
            if ( m_ground && ( moving_left() ^ moving_right() ) ) {
                Vector foot_offset( Coordinate( (world()->age() % 16) - 8.f, (planc)0.f ) );
                if ( moving_left() ) foot += foot_offset;
                else if ( moving_right() ) foot -= foot_offset;
            }
            
            Line right_leg = Line( hip, foot );
            draw( PANTS_COLOR, right_leg, (ufloat)LEG_WIDTH );
            
            Drawing boot_drawing_back = boot_drawing;
            if ( facing_left() ) boot_drawing_back.mirror_y();
            boot_drawing_back.move( foot );
            draw( boot_drawing_back );
        }
        
        // front leg
        {
            Coordinate hip = front_hip();
            Coordinate foot = front_foot();
            
            if ( m_ground && ( moving_left() ^ moving_right() ) ) {
                Vector foot_offset( Coordinate( ((world()->age() + 8) % 16) - 8.f, (planc)0.f ) );
                if ( moving_left() ) foot += foot_offset;
                else if ( moving_right() ) foot -= foot_offset;
            }
            
            Line left_leg = Line( hip, foot );
            draw( PANTS_COLOR, left_leg, (ufloat)LEG_WIDTH );
            
            Drawing boot_drawing_front = boot_drawing;
            if ( facing_left() ) boot_drawing_front.mirror_y();
            boot_drawing_front.move( foot );
            draw( boot_drawing_front );
        }
    }
    
    // body
    static_setup( Drawing, body_drawing ) {
        Coordinate waist = Coordinate( 0.f, -BODY_HEIGHT.half() );

        // pants base
        body_drawing.add( PANTS_COLOR, Shape( { Coordinate( PANTS_WIDTH.half(), PANTS_HEIGHT.half() ), Coordinate( -PANTS_WIDTH.half(), PANTS_HEIGHT.half() ), Coordinate( -PANTS_WIDTH / 4.f, -PANTS_HEIGHT.half() ), Coordinate( PANTS_WIDTH / 4.f, -PANTS_HEIGHT.half() ) } ) + waist + Vector( (planc)0.f, -PANTS_HEIGHT.half() ) );

        // undershirt
        body_drawing.add( GRAY_LIGHT, Shape::rectangle( BODY_WIDTH.half(), (ufloat)BODY_HEIGHT ) );

        // belt
        body_drawing.add( BELT_COLOR, Line( waist + Vector( PANTS_WIDTH.half(), 0.f ), waist + Vector( -PANTS_WIDTH.half() ) ), (ufloat)BELT_THICKNESS );
        body_drawing.add( BELT_BUCKLE_COLOR, Shape::rectangle( BELT_BUCKLE_WIDTH, BELT_BUCKLE_WIDTH, waist ) );

        // jacket
        body_drawing.add( JACKET_COLOR, Shape::rectangle( BODY_WIDTH / 3.f, BODY_HEIGHT, Coordinate( BODY_WIDTH / 3.5f, 0.f ) ) );
        body_drawing.add( JACKET_COLOR, Shape::rectangle( BODY_WIDTH / 3.f, BODY_HEIGHT, Coordinate( -BODY_WIDTH / 3.5f, 0.f ) ) );
    }
    
    draw( body_drawing );
    
    // back arm
    {
        Coordinate shoulder = back_shoulder();
        Coordinate hand = back_hand();
        Line arm = Line( shoulder, hand );
        draw( JACKET_BACK_COLOR, Shape::circle( ARM_WIDTH.half(), shoulder ) );
        draw( JACKET_BACK_COLOR, arm, (ufloat)ARM_WIDTH );
        draw( SKIN_COLOR, Shape::circle( ARM_WIDTH.half(), hand ) );
    }
    
    static_setup( Drawing, head_drawing ) {
        head_drawing.add( SKIN_COLOR, Shape::circle( HEAD_RADIUS, head_center() ) );
    
        // shaved hair
        varray<Coordinate> top_half_coordinates = Shape::circle( HEAD_RADIUS, head_center() ).coordinates();
        uint count = ( top_half_coordinates.size() / 2 ) + 1;
        // TODO wtf?
        for_range ( count ) top_half_coordinates.remove( floor( top_half_coordinates.size() / 2 ) );
        Shape top_half = Shape( top_half_coordinates );
        head_drawing.add( HAIR_COLOR.a( 0.375f ), top_half );
    }
    
    draw( head_drawing );
        
    // face
    {
        draw( SKIN_COLOR, Shape::circle( FACE_RADIUS, face_center() ) );
        
        // mohawk
        {
            Coordinate top = head_center() + Vector( (planc)0.f, HEAD_RADIUS + 3.f );
            Coordinate top_right = top + Vector( (planc)5.f, (planc)0.f );
            Coordinate top_left = top + Vector( (planc)-5.f, (planc)0.f );
            
            Coordinate bottom = face_center() + Vector( (planc)0.f, FACE_RADIUS - 4.f );
            Coordinate bottom_right = bottom + Vector( (planc)5.f, (planc)0.f );
            Coordinate bottom_left = bottom + Vector( (planc)-5.f, (planc)0.f );
            
            draw( HAIR_COLOR, Shape( { top_right, top_left, bottom_left, bottom_right } ) );
        }
        
        // beard
        draw( HAIR_COLOR, Shape::rectangle( 8.f, 8.f, face_center() + Vector( (planc)0.f, (planc)-8.f ) ) );
        draw( HAIR_COLOR, Shape::rectangle( FACE_RADIUS * 2.f, 12.f, face_center() + Vector( (planc)0.f, (planc)-12.f ) ) );
        
        // eyes
        if ( m_blink_wait_counter.remaining() && m_blink_wait_counter.remaining() ) {
            draw( EYE_COLOR, Shape::rectangle( 3.f, 3.f, face_center() + Coordinate(EYE_SPACING, 0.f ) ) );
            draw( EYE_COLOR, Shape::rectangle( 3.f, 3.f, face_center() + Coordinate(-EYE_SPACING, 0.f ) ) );
        } else {
            draw( EYE_COLOR, Shape::rectangle( 3.f, 1.f, face_center() + Coordinate(EYE_SPACING, -1.f ) ) );
            draw( EYE_COLOR, Shape::rectangle( 3.f, 1.f, face_center() + Coordinate(-EYE_SPACING, -1.f ) ) );
        }
    }
    
    // crossbow
    {
        Shape crossbow_shape = Shape( { Coordinate( 6.f, 0.f ), Coordinate( 0.f, 0.f ), Coordinate( -2.f, -2.f ), Coordinate( -4.f, -2.f ), Coordinate( -4.f, -4.f ), Coordinate( 1.f, -1.f ), Coordinate( 5.f, -1.f ) } );
        crossbow_shape.scale( 5.f );
        Line crossbow_handle = Line( Coordinate( 22.f, 0.f ), Coordinate( 22.f, -12.f ) );
        
        if ( ( aim_angle() > PI_1D2 ) && ( aim_angle() < PI_3D2 ) ) {
            crossbow_shape.mirror_x( );
            crossbow_handle.mirror_x( );
        }
        
        crossbow_shape.rotate( aim_angle() );
        crossbow_handle.rotate( aim_angle() );
        
        draw( CROSSBOW_COLOR, crossbow_shape  );
        draw( CROSSBOW_COLOR, crossbow_handle, (ufloat)4.f );
        
        Arrow arrow( world(), position(), VectorA(aim_angle(), 1.f), m_arrow_feather_color ); arrow.render();
        Drawing arrow_drawing = arrow.drawing();
        arrow_drawing.move( VectorA( aim_angle(), arrow.length() ) );
        draw( arrow_drawing );
    }
    
    // front arm
    {
        Coordinate shoulder = front_shoulder();
        Coordinate hand = front_hand();
        
        Joint lower_arm = Joint( shoulder, UPPER_ARM_LENGTH, LOWER_ARM_LENGTH, hand, !( ( aim_angle() > PI_1D2 ) && ( aim_angle() < PI_3D2 ) ) );
        draw( JACKET_FRONT_COLOR, Shape::circle( ARM_WIDTH.half(), shoulder ) );
        draw( JACKET_FRONT_COLOR, Line( lower_arm.c1(), lower_arm.joint() ), (ufloat)ARM_WIDTH );
        
        draw( SKIN_COLOR, Shape::circle( ARM_WIDTH.half(), hand ) );
        
        Joint upper_arm = Joint( shoulder, UPPER_ARM_LENGTH, LOWER_ARM_LENGTH, hand, !( ( aim_angle() > PI_1D2 ) && ( aim_angle() < PI_3D2 ) ) );
        draw( JACKET_FRONT_COLOR, Shape::circle( ARM_WIDTH.half(), upper_arm.joint() ) );
        draw( JACKET_FRONT_COLOR, Line( upper_arm.joint(), upper_arm.c2() ), (ufloat)ARM_WIDTH );
    }
    
    return *this;
}
