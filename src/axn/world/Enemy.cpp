#include "axn/world/Enemy.hpp"

#include "axn/world/Player.hpp"

#if defined( AXN_DEBUG )
var_const( dec ALPHA = ( 0.75 ) );
var_const( dec ALPHA_INNER = ( ALPHA * 0.1 ) );
var_const( Planc SIGHT_LINE_THICKNESS = ( 0.8 ) );
var_const( Planc ALERT_LINE_THICKNESS = ( SIGHT_LINE_THICKNESS ) );
var_const( Color SIGHT_LINE_COLOR = ( GREEN.a( ALPHA ) ) );
var_const( Color ALERT_LINE_COLOR = ( MAGENTA.a( ALPHA ) ) );
#endif

void Enemy::update( )
{
    if( alive( ) )
    {
        for_each( player, room( )->players( ) )
        {
            if( target( ) == player )
            {
                if( ( !target_locked( ) && !in_sight_range( player ) ) || !player->alive( ) )
                {
                    clear_target( );
                }
            }
            else if( player->alive( ) && in_alert_range( player ) )
            {
                if( !has_target( ) || overrides_target( player ) )
                {
                    set_target( player );
                }
            }
        }
    }

    Mob::update( );
}

bool Enemy::overrides_target( ptr<const Object> in_target ) const
{
    return( alive( ) && !target_locked( ) && position( ).closer_than( m_target->position( ), in_target->position( ) ) );
}

void Enemy::sight_range( Planc cref in_range )
{
    Assert( is_pos( in_range ) );

    m_sight_range = in_range;

    if( !alert_range( ) || ( alert_range( ) > sight_range( ) ) )
    {
        alert_range( sight_range( ) );
    }
}

bool Enemy::in_sight_range( ptr<const Player> player ) const
{
    return( alive( ) && !sight_range( ) || position( ).in_distance_range( player->position( ), sight_range( ) ) );
}

void Enemy::alert_range( Planc cref in_range )
{
    Assert( is_pos( in_range ) );

    m_alert_range = in_range;

    if( sight_range( ) && ( sight_range( ) < alert_range( ) ) )
    {
        sight_range( alert_range( ) );
    }
}

bool Enemy::in_alert_range( ptr<const Player> in_player ) const
{
    return( alive( ) && !alert_range( ) || position( ).in_distance_range( in_player->position( ), alert_range( ) ) );
}

#if defined( AXN_DEBUG )
Drawing Enemy::debug_overlay( ) const
{
    Drawing debug_overlay;

    if( Settings::get( Settings::DEBUG_PHYSICS ) )
    {
        if( alive( ) )
        {
            if( alert_range( ) )
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
                else
                {
                    debug_overlay.draw( ALERT_LINE_COLOR.a( ALPHA_INNER ), Polygon::circle( alert_range( ) ) );
                    debug_overlay.draw( ALERT_LINE_COLOR, Polygon::circle( alert_range( ) ), ALERT_LINE_THICKNESS, true );
                }
            }
        }
    }

    return( debug_overlay.draw( Mob::debug_overlay( ) ) );
}
#endif
