#include "Enemy.hpp"
#include "Player.hpp"

void Enemy::update( )
{
    for_each( player, world( )->players( ) )
    {
        if( target( ) == player )
        {
            if( !target_locked( ) && !in_sight_range( player ) )
            {
                clear_target( );
            }
        }
        else if( in_alert_range( player ) )
        {
            if( !has_target( ) || overrides_target( player ) )
            {
                set_target( player );
            }
        }
    }

    Mob::update( );
}

bool Enemy::overrides_target( const Player * player ) const
{
    return !target_locked( ) && position( ).distance_to( m_target->position( ) ) > position( ).distance_to( player->position( ) );
}

void Enemy::sight_range( const Planc & _range )
{
    Assert( is_pos( _range ) );

    m_sight_range = _range;

    if( !alert_range( ) || ( alert_range( ) > sight_range( ) ) )
    {
        alert_range( sight_range( ) );
    }
}

bool Enemy::in_sight_range( const Player * player ) const
{
    return ( !sight_range( ) || position( ).in_distance_range( player->position( ), sight_range( ) ) );
}

void Enemy::alert_range( const Planc & _range )
{
    Assert( is_pos( _range ) );

    m_alert_range = _range;

    if( sight_range( ) && ( sight_range( ) < alert_range( ) ) )
    {
        sight_range( alert_range( ) );
    }
}

bool Enemy::in_alert_range( const Player * player ) const
{
    return ( !alert_range( ) || position( ).in_distance_range( player->position( ), alert_range( ) ) );
}

#ifdef AXN_DEBUG
Drawing Enemy::debug_overlay( ) const
{
    const Planc SIGHT_LINE_THICKNESS = 0.8;
    const Planc ALERT_LINE_THICKNESS = SIGHT_LINE_THICKNESS;
    const dec ALPHA = 0.75;
    const dec ALPHA_INNER = ALPHA * 0.1;
    const Color SIGHT_LINE_COLOR = GREEN.a( ALPHA );
    const Color ALERT_LINE_COLOR = RED.a( ALPHA );

    Drawing debug_overlay;

    if( Object::draw_physics )
    {
        if( has_target( ) )
        {
            debug_overlay.draw( SIGHT_LINE_COLOR, Line( ORIGIN, target( )->position( ) - position( ) ), SIGHT_LINE_THICKNESS, true );

            if( sight_range( ) )
            {
                debug_overlay.draw( SIGHT_LINE_COLOR.a( ALPHA_INNER ), Polygon::circle( sight_range( ) ) );
                debug_overlay.draw( SIGHT_LINE_COLOR, Polygon::circle( sight_range( ) ), ALERT_LINE_THICKNESS, true );
            }
        }
        else if( alert_range( ) )
        {
            debug_overlay.draw( ALERT_LINE_COLOR.a( ALPHA_INNER ), Polygon::circle( alert_range( ) ) );
            debug_overlay.draw( ALERT_LINE_COLOR, Polygon::circle( alert_range( ) ), ALERT_LINE_THICKNESS, true );
        }
    }

    debug_overlay.draw( Mob::debug_overlay( ) );
    return debug_overlay;
}
#endif
