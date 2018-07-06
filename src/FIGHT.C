/***************************************************************************
 *  Original Diku Mud copyright (C) 1990, 1991 by Sebastian Hammer,        *
 *  Michael Seifert, Hans Henrik St{rfeldt, Tom Madsen, and Katja Nyboe.   *
 *                                                                         *
 *  Merc Diku Mud improvments copyright (C) 1992, 1993 by Michael          *
 *  Chastain, Michael Quan, and Mitchell Tse.                              *
 *                                                                         *
 *  In order to use any part of this Merc Diku Mud, you must comply with   *
 *  both the original Diku license in 'license.doc' as well the Merc       *
 *  license in 'license.txt'.  In particular, you may not remove either of *
 *  these copyright notices.                                               *
 *                                                                         *
 *  Much time and thought has gone into this software and you are          *
 *  benefitting.  We hope that you share your changes too.  What goes      *
 *  around, comes around.                                                  *
 ***************************************************************************/

#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "merc.h"



/*
 * Local functions.
 */
bool	check_dodge	args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
void	check_killer	args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
bool	check_parry	args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
void	dam_message	args( ( CHAR_DATA *ch, CHAR_DATA *victim, int dam,
			    int dt ) );
void	death_cry	args( ( CHAR_DATA *ch ) );
void	group_gain	args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
int	xp_compute	args( ( CHAR_DATA *gch, CHAR_DATA *victim ) );
bool	is_safe		args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
bool  is_wielding_poisoned args( ( CHAR_DATA *ch ) );
void	make_corpse	args( ( CHAR_DATA *ch ) );
void	one_hit		args( ( CHAR_DATA *ch, CHAR_DATA *victim, int dt ) );
void	raw_kill	args( ( CHAR_DATA *victim ) );
void	set_fighting	args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
void	disarm		args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
void	trip		args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );



/*
 * Control the fights going on.
 * Called periodically by update_handler.
 */
void violence_update( void )
{
    CHAR_DATA *ch;
//    CHAR_DATA *ch_next;
    CHAR_DATA *victim;
    CHAR_DATA *rch;
    CHAR_DATA *rch_next;

    for ( ch = char_list; ch != NULL; ch = ch->next )
    {

        /*
         * Hunting mobs.
         */
        if ( IS_NPC(ch)
            && ch->fighting == NULL
            && IS_AWAKE(ch)
            && ch->hunting != NULL )
          {
            hunt_victim(ch);
            continue;
          }

//	ch_next	= ch->next;

	if ( ( victim = ch->fighting ) == NULL || ch->in_room == NULL )
	    continue;

	if ( IS_AWAKE(ch) && ch->in_room == victim->in_room )
	    multi_hit( ch, victim, TYPE_UNDEFINED );
	else
	    stop_fighting( ch, FALSE );

	if ( ( victim = ch->fighting ) == NULL )
	    continue;

        mprog_hitprcnt_trigger( ch, victim );
        mprog_fight_trigger( ch, victim );

	/*
	 * Fun for the whole family!
	 */
	for ( rch = ch->in_room->people; rch != NULL; rch = rch_next )
	{
	    rch_next = rch->next_in_room;

	    if ( IS_AWAKE(rch) && rch->fighting == NULL )
	    {
		/*
		 * PC's auto-assist others in their group.
		 */
		if ( !IS_NPC(ch) || IS_AFFECTED(ch, AFF_CHARM) )
		{
		    if ( ( !IS_NPC(rch) || IS_AFFECTED(rch, AFF_CHARM) )
		    &&   is_same_group(ch, rch) )
			multi_hit( rch, victim, TYPE_UNDEFINED );
		    continue;
		}

		/*
		 * NPC's assist NPC's of same type or 12.5% chance regardless.
		 */
		if ( IS_NPC(rch) && !IS_AFFECTED(rch, AFF_CHARM) )
		{
		    if ( rch->pIndexData == ch->pIndexData
		    ||   number_bits( 3 ) == 0 )
		    {
			CHAR_DATA *vch;
			CHAR_DATA *target;
			int number;

			target = NULL;
			number = 0;
			for ( vch = ch->in_room->people; vch; vch = vch->next )
			{
			    if ( can_see( rch, vch )
			    &&   is_same_group( vch, victim )
			    &&   number_range( 0, number ) == 0 )
			    {
				target = vch;
				number++;
			    }
			}

			if ( target != NULL )
			{
			   if ( ( ( ( target->level - rch->level <= 4 )
   			       && ( target->level - rch->level >= -4 ) )
			      && !( IS_GOOD ( rch ) && IS_GOOD ( target ) ) )
			   || ( IS_EVIL ( rch ) || IS_EVIL ( target ) ) )
			     multi_hit( rch, target, TYPE_UNDEFINED );
			}
		    }
		}
	    }
	}
    }

    return;
}



/*
 * Do one group of attacks.
 */
void multi_hit( CHAR_DATA *ch, CHAR_DATA *victim, int dt )
{
    int     chance;

    one_hit( ch, victim, dt );
    if ( ch->fighting != victim || dt == gsn_backstab )
	return;

    chance = IS_NPC(ch) ? ch->level : ch->pcdata->learned[gsn_second_attack]/2;
    if ( number_percent( ) < chance )
    {
	one_hit( ch, victim, dt );
	if ( ch->fighting != victim )
	    return;
    }

    chance = IS_NPC(ch) ? ch->level : ch->pcdata->learned[gsn_third_attack]/4;
    if ( number_percent( ) < chance )
    {
	one_hit( ch, victim, dt );
	if ( ch->fighting != victim )
	    return;
    }

    chance = IS_NPC(ch) ? ch->level / 2 : 0;
    if ( number_percent( ) < chance )
	one_hit( ch, victim, dt );
    
    return;
}



/*
 * Hit one guy once.
 */
void one_hit( CHAR_DATA *ch, CHAR_DATA *victim, int dt )
{
    OBJ_DATA *wield;
    int victim_ac;
    int thac0;
    int thac0_00;
    int thac0_32;
    int dam;
    int diceroll;

    /*
     * Can't beat a dead char!
     * Guard against weird room-leavings.
     */
    if ( victim->position == POS_DEAD || ch->in_room != victim->in_room )
	return;

    /*
     * Figure out the type of damage message.
     */
    wield = get_eq_char( ch, WEAR_WIELD );
    if ( dt == TYPE_UNDEFINED )
    {
	dt = TYPE_HIT;
	if ( wield != NULL && wield->item_type == ITEM_WEAPON )
	    dt += wield->value[3];
    }

    /*
     * Calculate to-hit-armor-class-0 versus armor.
     */
    if ( IS_NPC(ch) )
    {
	thac0_00 = 20;
	thac0_32 =  0;
    }
    else
    {
	thac0_00 = class_table[ch->class].thac0_00;
	thac0_32 = class_table[ch->class].thac0_32;
    }
    thac0     = interpolate( ch->level, thac0_00, thac0_32 ) - GET_HITROLL(ch);
    victim_ac = UMAX( -15, GET_AC(victim) / 10 );
    if ( !can_see( ch, victim ) )
	victim_ac -= 4;

    /*
     * The moment of excitement!
     */
    while ( ( diceroll = number_bits( 5 ) ) >= 20 )
	;

    if ( diceroll == 0
    || ( diceroll != 19 && diceroll < thac0 - victim_ac ) )
    {
	/* Miss. */
	damage( ch, victim, 0, dt );
	tail_chain( );
	return;
    }

    /*
     * Hit.
     * Calc damage.
     */
    if ( IS_NPC(ch) )
    {
	dam = number_range( ch->level / 2, ch->level * 3 / 2 );
	if ( wield != NULL )
	    dam += dam / 2;
    }
    else
    {
	if ( wield != NULL )
	    dam = number_range( wield->value[1], wield->value[2] );
	else
	    dam = number_range( 1, 4 );
    }

    /*
     * Bonuses.
     */
    dam += GET_DAMROLL(ch);
    if ( wield && IS_SET( wield->extra_flags, ITEM_POISONED ) )
        dam += dam / 4;
	 if ( !IS_NPC(ch) && ch->pcdata->learned[gsn_enhanced_damage] > 0 )
	dam += dam * ch->pcdata->learned[gsn_enhanced_damage] / 150;
    if ( !IS_AWAKE(victim) )
	dam *= 2;
    if ( dt == gsn_backstab )
	dam *= 2 + ch->level / 8;

    if ( dam <= 0 )
	dam = 1;

    damage( ch, victim, dam, dt );
    tail_chain( );
    return;
}



/*
 * Inflict damage from a hit.
 */
void damage( CHAR_DATA *ch, CHAR_DATA *victim, int dam, int dt )
{
    if ( victim->position == POS_DEAD )
	return;

    /*
     * Stop up any residual loopholes.
     */
    if ( dam > 1000 )
    {
	bug( "Damage: %d: more than 1000 points!", dam );
	dam = 1000;
    }

    if ( victim != ch )
    {
	/*
	 * Certain attacks are forbidden.
	 * Most other attacks are returned.
	 */
	if ( is_safe( ch, victim ) )
	    return;
	check_killer( ch, victim );

	if ( victim->position > POS_STUNNED )
	{
	    if ( victim->fighting == NULL )
		set_fighting( victim, ch );
	    victim->position = POS_FIGHTING;
	}

	if ( victim->position > POS_STUNNED )
	{
	    if ( ch->fighting == NULL )
		set_fighting( ch, victim );

	    /*
	     * If victim is charmed, ch might attack victim's master.
	     */
	    if ( IS_NPC(ch)
	    &&   IS_NPC(victim)
	    &&   IS_AFFECTED(victim, AFF_CHARM)
	    &&   victim->master != NULL
	    &&   victim->master->in_room == ch->in_room
	    &&   number_bits( 3 ) == 0 )
	    {
		stop_fighting( ch, FALSE );
		multi_hit( ch, victim->master, TYPE_UNDEFINED );
		return;
	    }
	}

	/*
	 * More charm stuff.
	 */
	if ( victim->master == ch )
	    stop_follower( victim );

	/*
	 * Inviso attacks ... not.
	 */
	if ( IS_AFFECTED(ch, AFF_INVISIBLE) )
	{
	    affect_strip( ch, gsn_invis );
	    affect_strip( ch, gsn_mass_invis );
	    REMOVE_BIT( ch->affected_by, AFF_INVISIBLE );
				act(AT_WHITE, "$n fades into existence.", ch, NULL, NULL, TO_ROOM );
		  }

	/*
	 * Damage modifiers.
	 */
	if ( IS_AFFECTED(victim, AFF_SANCTUARY) )
	    dam /= 2;

	if ( IS_AFFECTED(victim, AFF_PROTECT) && IS_EVIL(ch) )
	    dam -= dam / 4;

	if ( dam < 0 )
	    dam = 0;

	/*
	 * Check for disarm, trip, parry, and dodge.
	 */
	if ( dt >= TYPE_HIT )
	{
	    if ( IS_NPC(ch) && number_percent( ) < ch->level / 2 )
		disarm( ch, victim );
	    if ( IS_NPC(ch) && number_percent( ) < ch->level / 2 )
		trip( ch, victim );
	    if ( check_parry( ch, victim ) )
		return;
	    if ( check_dodge( ch, victim ) )
		return;
	}

	dam_message( ch, victim, dam, dt );
    }

    /*
     * Hurt the victim.
     * Inform the victim of his new state.
     */
    victim->hit -= dam;
    if ( !IS_NPC(victim)
    &&   victim->level >= LEVEL_IMMORTAL
    &&   victim->hit < 1 )
	victim->hit = 1;
    if ( dam > 0 && dt > TYPE_HIT
	&& is_wielding_poisoned( ch )
	&& !saves_spell( ch->level, victim ) )
    {
	AFFECT_DATA af;

	af.type      = gsn_poison;
	af.duration  = 1;
	af.location  = APPLY_STR;
	af.modifier  = -2;
	af.bitvector = AFF_POISON;
	affect_join( victim, &af );
    }

	 update_pos( victim );

    switch( victim->position )
    {
    case POS_MORTAL:
		  act(AT_BLOOD, "$n is mortally wounded, and will die soon, if not aided.",
		 victim, NULL, NULL, TO_ROOM );
ansi_color( RED, ch );
		  send_to_char(
	    "You are mortally wounded, and will die soon, if not aided.\n\r",
	    victim );
ansi_color( GREY, ch );
        break;

    case POS_INCAP:
		  act(AT_RED, "$n is incapacitated and will slowly die, if not aided.",
		 victim, NULL, NULL, TO_ROOM );
ansi_color( BOLD, ch );
ansi_color( RED, ch );
		  send_to_char(
	    "You are incapacitated and will slowly die, if not aided.\n\r",
	    victim );
ansi_color( NTEXT, ch );
ansi_color( GREY, ch );
        break;

    case POS_STUNNED:
		  act(AT_GREEN, "$n is stunned, but will probably recover.",
		 victim, NULL, NULL, TO_ROOM );
ansi_color( BOLD, ch );
ansi_color( GREEN, ch );
        send_to_char("You are stunned, but will probably recover.\n\r",
	    victim );
ansi_color( NTEXT, ch );
ansi_color( GREY, ch );
        break;

	 case POS_DEAD:
		  act(AT_RED, "$n is DEAD!!", victim, 0, 0, TO_ROOM );
ansi_color( BOLD, ch );
        send_to_char( "You have been KILLED!!\n\r\n\r", victim );
	send_to_char( "Your soul rises up out of you body, and you see a bright\n\r", victim);
	send_to_char( "light pulsating in front of you.  In a blinding flash you\n\r", victim);
	send_to_char( "are whisked into the center of the light.  As you come to\n\r", victim);
	send_to_char( "things are not the same as before.\n\r\n\r", victim );
ansi_color( NTEXT, ch );
        break;

    default:
	if ( dam > victim->max_hit / 4 )
{
ansi_color( BOLD, ch );
ansi_color( RED, ch );
            send_to_char( "That really did HURT!\n\r", victim );
ansi_color( NTEXT, ch );
ansi_color( GREY, ch );
}
        if ( victim->hit < victim->max_hit / 4 )
{
ansi_color( BOLD, ch );
ansi_color( RED, ch );
            send_to_char( "You sure are BLEEDING!\n\r", victim );
ansi_color( NTEXT, ch );
ansi_color( GREY, ch );
}
        break;
    }

    /*
     * Sleep spells and extremely wounded folks.
     */
    if ( !IS_AWAKE(victim) )
	stop_fighting( victim, FALSE );

    /*
     * Payoff for killing things.
     */
    if ( victim->position == POS_DEAD )
    {
	group_gain( ch, victim );

	if ( !IS_NPC(victim) )
	{
		 sprintf( log_buf, "Log: %s killed by %s at %d",
		victim->name,
		(IS_NPC(ch) ? ch->short_descr : ch->name),
		victim->in_room->vnum );
		 log_string( log_buf );
		 NOTIFY( log_buf, LEVEL_IMMORTAL, WIZ_NOTIFY_LOG );
		sprintf( log_buf, "Monitor: %s killed by %s at %d", victim->name,
		(IS_NPC(ch) ? ch->short_descr : ch->name),
		victim->in_room->vnum );
ansi_color( BOLD, ch );
ansi_color( PURPLE, ch );
 NOTIFY( log_buf, LEVEL_IMMORTAL, WIZ_NOTIFY_MONITOR );
ansi_color( NTEXT, ch );
ansi_color( GREY, ch );
		 /*
		  * Dying penalty:
		  * 1/2 way back to previous level.
		  */
		 if ( victim->exp > 1000 * victim->level )
		gain_exp( victim, (1000 * victim->level - victim->exp)/2 );
	}

	raw_kill( victim );

	if ( !IS_NPC(ch) && IS_NPC(victim) )
	{
	    if ( IS_SET(ch->act, PLR_AUTOLOOT) )
		do_get( ch, "all corpse" );
	    else
		do_look( ch, "in corpse" );

	    if ( IS_SET(ch->act, PLR_AUTOSAC) )
		do_sacrifice( ch, "corpse" );
	}

	return;
    }

    if ( victim == ch )
	return;

    /*
     * Take care of link dead people.
     */
    if ( !IS_NPC(victim) && victim->desc == NULL )
    {
	if ( number_range( 0, victim->wait ) == 0 )
	{
	    do_recall( victim, "" );
	    return;
	}
    }

    /*
     * Wimp out?
     */
    if ( IS_NPC(victim) && dam > 0 )
    {
	if ( ( IS_SET(victim->act, ACT_WIMPY) && number_bits( 1 ) == 0
	&&   victim->hit < victim->max_hit / 2 )
	||   ( IS_AFFECTED(victim, AFF_CHARM) && victim->master != NULL
	&&     victim->master->in_room != victim->in_room ) )
	    do_flee( victim, "" );
    }

    if ( !IS_NPC(victim)
    &&   victim->hit > 0
    &&   victim->hit <= victim->wimpy
    &&   victim->wait == 0 )
	do_flee( victim, "" );

    tail_chain( );
    return;
}



bool is_safe( CHAR_DATA *ch, CHAR_DATA *victim )
{
    if ( IS_NPC(ch) || IS_NPC(victim) )
	return FALSE;

    if ( get_age( ch ) < 21 )
    {
	send_to_char( "You aren't old enough.\n\r", ch );
	return TRUE;
    }

    if ( IS_SET( victim->act, PLR_KILLER ) )
        return FALSE;

    if ( ch->level >= victim->level )
    {
	send_to_char( "You may not attack a lower level player.\n\r", ch );
	return TRUE;
    }

    return FALSE;
}



/*
 * See if an attack justifies a KILLER flag.
 */
void check_killer( CHAR_DATA *ch, CHAR_DATA *victim )
{
    /*
     * Follow charm thread to responsible character.
     * Attacking someone's charmed char is hostile!
     */
    while ( IS_AFFECTED(victim, AFF_CHARM) && victim->master != NULL )
	victim = victim->master;

    /*
     * NPC's are fair game.
     * So are killers and thieves.
     */
    if ( IS_NPC(victim)
    ||   IS_SET(victim->act, PLR_KILLER)
    ||   IS_SET(victim->act, PLR_THIEF) )
	return;

    /*
     * Charm-o-rama.
     */
    if ( IS_SET(ch->affected_by, AFF_CHARM) )
    {
	if ( ch->master == NULL )
	{
	    char buf[MAX_STRING_LENGTH];

	    sprintf( buf, "Check_killer: %s bad AFF_CHARM",
		IS_NPC(ch) ? ch->short_descr : ch->name );
	    bug( buf, 0 );
	    affect_strip( ch, gsn_charm_person );
	    REMOVE_BIT( ch->affected_by, AFF_CHARM );
	    return;
	}

ansi_color( BOLD, ch );
        send_to_char( "*** You are now a KILLER!! ***\n\r", ch->master );
ansi_color( NTEXT, ch );
        SET_BIT(ch->master->act, PLR_KILLER);
	stop_follower( ch );
	return;
    }

    /*
     * NPC's are cool of course (as long as not charmed).
     * Hitting yourself is cool too (bleeding).
     * So is being immortal (Alander's idea).
     * And current killers stay as they are.
     */
    if ( IS_NPC(ch)
    ||   ch == victim
    ||   ch->level >= LEVEL_IMMORTAL
    ||   IS_SET(ch->act, PLR_KILLER) )
	return;

ansi_color( BOLD, ch );
    send_to_char( "*** You are now a KILLER!! ***\n\r", ch );
ansi_color( NTEXT, ch );
    SET_BIT(ch->act, PLR_KILLER);
    save_char_obj( ch );
    return;
}


/*
 * Check to see if weapon is poisoned.
 */
bool is_wielding_poisoned( CHAR_DATA *ch )
{
    OBJ_DATA *obj;

    if ( ( obj = get_eq_char( ch, WEAR_WIELD ) )
	&& IS_SET( obj->extra_flags, ITEM_POISONED ) )
        return TRUE;

    return FALSE;

}

/*
 * Check for parry.
 */
bool check_parry( CHAR_DATA *ch, CHAR_DATA *victim )
{
    int chance;

    if ( !IS_AWAKE(victim) )
	return FALSE;

    if ( IS_NPC(victim) )
    {
	/* Tuan was here.  :) */
	chance	= UMIN( 60, 2 * victim->level );
    }
    else
    {
	if ( get_eq_char( victim, WEAR_WIELD ) == NULL )
	    return FALSE;
	chance	= victim->pcdata->learned[gsn_parry] / 2;
    }

    if ( number_percent( ) >= chance + victim->level - ch->level )
	return FALSE;

	 act(AT_GREEN, "You parry $n's attack.",  ch, NULL, victim, TO_VICT    );
	 act(AT_GREEN, "$N parries your attack.", ch, NULL, victim, TO_CHAR    );
	 return TRUE;
         ansi_color( NTEXT, ch );
         ansi_color( GREY, ch );
}



/*
 * Check for dodge.
 */
bool check_dodge( CHAR_DATA *ch, CHAR_DATA *victim )
{
    int chance;

    if ( !IS_AWAKE(victim) )
	return FALSE;

    if ( IS_NPC(victim) )
	/* Tuan was here.  :) */
        chance  = UMIN( 60, 2 * victim->level );
    else
        chance  = victim->pcdata->learned[gsn_dodge] / 2;

    if ( number_percent( ) >= chance + victim->level - ch->level )
        return FALSE;

	 act(AT_GREEN, "You dodge $n's attack.", ch, NULL, victim, TO_VICT    );
	 act(AT_GREEN, "$N dodges your attack.", ch, NULL, victim, TO_CHAR    );
	 return TRUE;
         ansi_color( NTEXT, ch );
         ansi_color( GREY, ch );
}



/*
 * Set position of a victim.
 */
void update_pos( CHAR_DATA *victim )
{
    if ( victim->hit > 0 )
    {
    	if ( victim->position <= POS_STUNNED )
	    victim->position = POS_STANDING;
	return;
    }

    if ( IS_NPC(victim) || victim->hit <= -11 )
    {
	victim->position = POS_DEAD;
	return;
    }

         if ( victim->hit <= -6 ) victim->position = POS_MORTAL;
    else if ( victim->hit <= -3 ) victim->position = POS_INCAP;
    else                          victim->position = POS_STUNNED;

    return;
}



/*
 * Start fights.
 */
void set_fighting( CHAR_DATA *ch, CHAR_DATA *victim )
{
    if ( ch->fighting != NULL )
    {
	bug( "Set_fighting: already fighting", 0 );
	return;
    }

    if ( IS_AFFECTED(ch, AFF_SLEEP) )
	affect_strip( ch, gsn_sleep );

    ch->fighting = victim;
    ch->position = POS_FIGHTING;

    return;
}



/*
 * Stop fights.
 */
void stop_fighting( CHAR_DATA *ch, bool fBoth )
{
    CHAR_DATA *fch;

    for ( fch = char_list; fch != NULL; fch = fch->next )
    {
	if ( fch == ch || ( fBoth && fch->fighting == ch ) )
	{
	    fch->fighting	= NULL;
	    fch->position	= POS_STANDING;
	    update_pos( fch );
	}
    }

    return;
}



/*
 * Make a corpse out of a character.
 */
void make_corpse( CHAR_DATA *ch )
{
    char buf[MAX_STRING_LENGTH];
    OBJ_DATA *corpse;
    OBJ_DATA *obj;
    OBJ_DATA *obj_next;
    ROOM_INDEX_DATA *location;
	 char *name;

    if ( IS_NPC(ch) )
    {
	name		= ch->short_descr;
	corpse		= create_object(get_obj_index(OBJ_VNUM_CORPSE_NPC), 0);
   // @@@ ECS - Made all corpses have value 3 gp
   corpse->cost   = 3;
	corpse->timer	= number_range( 2, 4 );
	if ( ch->gold > 0 )
	{
	    obj_to_obj( create_money( ch->gold ), corpse );
	    ch->gold = 0;
	}
    }
    else
    {
	name		= ch->name;
	corpse		= create_object(get_obj_index(OBJ_VNUM_CORPSE_PC), 0);
	corpse->timer	= number_range( 25, 40 );
    }

    sprintf( buf, corpse->short_descr, name );
    free_string( corpse->short_descr );
    corpse->short_descr = str_dup( buf );

    sprintf( buf, corpse->description, name );
    free_string( corpse->description );
    corpse->description = str_dup( buf );

    for ( obj = ch->carrying; obj != NULL; obj = obj_next )
    {
	obj_next = obj->next_content;
	obj_from_char( obj );
	if ( IS_SET( obj->extra_flags, ITEM_INVENTORY ) )
	    extract_obj( obj );
	else
	    obj_to_obj( obj, corpse );
    }

    if ( IS_NPC( ch ) )
	obj_to_room( corpse, ch->in_room );
    else
    {
        if ( ch->level > LEVEL_HERO )
	    obj_to_room( corpse, ch->in_room );
	else
	  if ( !( location = get_room_index( ROOM_VNUM_GRAVEYARD_A ) ) )
	  {
	      bug( "Graveyard A does not exist!", NULL );
	      obj_to_room( corpse, ch->in_room );
	  }
	  else
	      obj_to_room( corpse, location );
    }

    return;
}



/*
 * Improved Death_cry contributed by Diavolo.
 */
void death_cry( CHAR_DATA *ch )
{
    ROOM_INDEX_DATA *was_in_room;
    char *msg;
    int door;
    int vnum;

    vnum = 0;
    switch ( number_bits( 4 ) )
    {
    default: msg  = "You hear $n's death cry.";				break;
    case  0: msg  = "$n hits the ground ... DEAD.";			break;
    case  1: msg  = "$n splatters blood on your armor.";		break;
    case  2: msg  = "You see $n's soul escape to the afterlife.";
	     vnum = OBJ_VNUM_FINAL_TURD;				break;
    case  3: msg  = "$n's severed head plops on the ground.";
	     vnum = OBJ_VNUM_SEVERED_HEAD;				break;
    case  4: msg  = "$n's heart is torn from $s chest.";
	     vnum = OBJ_VNUM_TORN_HEART;				break;
    case  5: msg  = "$n's arm is sliced from $s dead body.";
	     vnum = OBJ_VNUM_SLICED_ARM;				break;
    case  6: msg  = "$n's leg is sliced from $s dead body.";
	     vnum = OBJ_VNUM_SLICED_LEG;				break;
    }

	 act(AT_ORANGE, msg, ch, NULL, NULL, TO_ROOM );

    if ( vnum != 0 )
    {
	char buf[MAX_STRING_LENGTH];
	OBJ_DATA *obj;
	char *name;

	name		= IS_NPC(ch) ? ch->short_descr : ch->name;
	obj		= create_object( get_obj_index( vnum ), 0 );
	obj->timer	= number_range( 4, 7 );

	sprintf( buf, obj->short_descr, name );
	free_string( obj->short_descr );
	obj->short_descr = str_dup( buf );

	sprintf( buf, obj->description, name );
	free_string( obj->description );
	obj->description = str_dup( buf );

	obj_to_room( obj, ch->in_room );
	 }

	 if ( IS_NPC(ch) )
	msg = "You hear something's death cry.";
	 else
	msg = "You hear someone's death cry.";

	 was_in_room = ch->in_room;
	 for ( door = 0; door <= 5; door++ )
	 {
	EXIT_DATA *pexit;

	if ( ( pexit = was_in_room->exit[door] ) != NULL
	&&   pexit->to_room != NULL
	&&   pexit->to_room != was_in_room )
	{
		 ch->in_room = pexit->to_room;
				act(AT_BLOOD, msg, ch, NULL, NULL, TO_ROOM );
		  }
	 }
    ch->in_room = was_in_room;

    return;
}



void raw_kill( CHAR_DATA *victim )
{
    stop_fighting( victim, TRUE );
    mprog_death_trigger( victim );
    make_corpse( victim );

    if ( IS_NPC(victim) )
    {
	victim->pIndexData->killed++;
	kill_table[URANGE(0, victim->level, MAX_LEVEL-1)].killed++;
	extract_char( victim, TRUE );
	return;
    }

    extract_char( victim, FALSE );
    while ( victim->affected )
	affect_remove( victim, victim->affected );
    victim->affected_by = race_table[victim->race].bitvector; // ###
//    victim->affected_by	= 0;
    victim->armor	= 100;
    victim->position	= POS_RESTING;
    victim->hit		= UMAX( 1, victim->hit  );
    victim->mana	= UMAX( 1, victim->mana );
    victim->move	= UMAX( 1, victim->move );
    save_char_obj( victim );
    return;
}



void group_gain( CHAR_DATA *ch, CHAR_DATA *victim )
{
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *gch;
    CHAR_DATA *lch;
    int xp;
    int members;

    /*
     * Monsters don't get kill xp's or alignment changes.
     * P-killing doesn't help either.
     * Dying of mortal wounds or poison doesn't give xp to anyone!
     */
    if ( IS_NPC(ch) || !IS_NPC(victim) || victim == ch )
	return;
    
    members = 0;
    for ( gch = ch->in_room->people; gch != NULL; gch = gch->next_in_room )
    {
	if ( is_same_group( gch, ch ) )
	    members++;
    }

    if ( members == 0 )
    {
	bug( "Group_gain: members.", members );
	members = 1;
    }

    lch = (ch->leader != NULL) ? ch->leader : ch;

    for ( gch = ch->in_room->people; gch != NULL; gch = gch->next_in_room )
    {
	OBJ_DATA *obj;
	OBJ_DATA *obj_next;

	if ( !is_same_group( gch, ch ) )
	    continue;

	if ( gch->level - lch->level >= 6 )
	{
	    send_to_char( "You are too high for this group.\n\r", gch );
	    continue;
	}

	if ( gch->level - lch->level <= -6 )
	{
	    send_to_char( "You are too low for this group.\n\r", gch );
	    continue;
	}

	xp = xp_compute( gch, victim ) / members;
        sprintf( buf, "You receive %d experience points.\n\r", xp );
ansi_color( BOLD, ch );
ansi_color( PURPLE, ch );
        send_to_char( buf, gch );
ansi_color( NTEXT, ch );
ansi_color( GREY, ch );
        gain_exp( gch, xp );

	for ( obj = ch->carrying; obj != NULL; obj = obj_next )
	{
	    obj_next = obj->next_content;
	    if ( obj->wear_loc == WEAR_NONE )
		continue;

	    if ( ( IS_OBJ_STAT(obj, ITEM_ANTI_EVIL)    && IS_EVIL(ch)    )
	    ||   ( IS_OBJ_STAT(obj, ITEM_ANTI_GOOD)    && IS_GOOD(ch)    )
	    ||   ( IS_OBJ_STAT(obj, ITEM_ANTI_NEUTRAL) && IS_NEUTRAL(ch) ) )
	    {
					 act(AT_YELLOW, "You are zapped by $p.", ch, obj, NULL, TO_CHAR );
					 act(AT_YELLOW, "$n is zapped by $p.",   ch, obj, NULL, TO_ROOM );
					 obj_from_char( obj );
		obj_to_room( obj, ch->in_room );
	    }
	}
    }

    return;
}



/*
 * Compute xp for a kill.
 * Also adjust alignment of killer.
 * Edit this function to change xp computations.
 */
int xp_compute( CHAR_DATA *gch, CHAR_DATA *victim )
{
    int align;
    int xp;
    int extra;
    int level;
    int number;

    xp    = 300 - URANGE( -3, gch->level - victim->level, 6 ) * 50;
    align = gch->alignment - victim->alignment;

    if ( align >  500 )
    {
	gch->alignment  = UMIN( gch->alignment + (align-500)/4,  1000 );
	xp = 5 * xp / 4;
    }
    else if ( align < -500 )
    {
	gch->alignment  = UMAX( gch->alignment + (align+500)/4, -1000 );
    }
    else
    {
	gch->alignment -= gch->alignment / 4;
	xp = 3 * xp / 4;
    }

    /*
     * Adjust for popularity of target:
     *   -1/8 for each target over  'par' (down to -100%)
     *   +1/8 for each target under 'par' (  up to + 25%)
     */
    level  = URANGE( 0, victim->level, MAX_LEVEL - 1 );
    number = UMAX( 1, kill_table[level].number );
    extra  = victim->pIndexData->killed - kill_table[level].killed / number;
    xp    -= xp * URANGE( -2, extra, 8 ) / 8;

    xp     = number_range( xp * 3 / 4, xp * 5 / 4 );
    xp     = UMAX( 0, xp );

    return xp;
}



void dam_message( CHAR_DATA *ch, CHAR_DATA *victim, int dam, int dt )
{
    static char * const attack_table[] =
    {
	"hit",
	"slice",  "stab",  "slash", "whip", "claw",
	"blast",  "pound", "crush", "grep", "bite",
	"pierce", "suction", "chop" // @@@ Chop is from envy
    };

    char buf1[256], buf2[256], buf3[256];
    const char *vs;
	 const char *vp;
    const char *attack;
			  char          buf          [ MAX_STRING_LENGTH ];
			  char          buf4         [ 256 ];
           char          buf5         [ 256 ];
	 char punct;

         if ( dam ==   0 ) { vs = "miss";       vp = "misses";          }
    else if ( dam <=   4 ) { vs = "scratch";    vp = "scratches";       }
    else if ( dam <=   8 ) { vs = "bruise";     vp = "bruises";         }
    else if ( dam <=  12 ) { vs = "hit";        vp = "hits";            }
    else if ( dam <=  16 ) { vs = "injure";     vp = "injures";         }
    else if ( dam <=  20 ) { vs = "wound";      vp = "wounds";          }
    else if ( dam <=  24 ) { vs = "maul";       vp = "mauls";           }
    else if ( dam <=  28 ) { vs = "decimate";   vp = "decimates";       }
    else if ( dam <=  32 ) { vs = "devastate";  vp = "devastates";      }
    else if ( dam <=  36 ) { vs = "maim";       vp = "maims";           }
    else if ( dam <=  40 ) { vs = "MUTILATE";   vp = "MUTILATES";       }
    else if ( dam <=  44 ) { vs = "* DISEMBOWEL *"; vp = "* DISEMBOWELS *";     }
    else if ( dam <=  48 ) { vs = "< EVISCERATE >"; vp = "< EVISCERATES >";     }
    else if ( dam <=  52 ) { vs = "** MASSACRE **";   vp = "** MASSACRES **";       }
    else if ( dam <= 100 ) { vs = "== OBLITERATE ==";
                             vp = "== OBLITERATES ==";                 }
    else                   { vs = "<<< DECAPITATE >>>";
                             vp = "<<< DECAPITATES >>>";                }

    punct   = (dam <= 24) ? '.' : '!';

	 if ( dt == TYPE_HIT )
	 {
	sprintf( buf3, "$n %s $N%c",  vp, punct );
	sprintf( buf1, "You %s $N%c", vs, punct );
	sprintf( buf2, "$n %s you%c", vp, punct );
	sprintf( buf4, "You %s yourself%c", vs, punct );
	sprintf( buf5, "$n %s $mself%c",    vp, punct );
	 }
	 else
	 {
	if ( dt >= 0 && dt < MAX_SKILL )
		 attack	= skill_table[dt].noun_damage;
	else if ( dt >= TYPE_HIT
	&& dt < TYPE_HIT + sizeof(attack_table)/sizeof(attack_table[0]) )
		 attack	= attack_table[dt - TYPE_HIT];
	else
	{
		 bug( "Dam_message: bad dt %d.", dt );
		 dt  = TYPE_HIT;
		 attack  = attack_table[0];
	}
	if ( dt > TYPE_HIT && is_wielding_poisoned( ch ) )
	{
		 sprintf( buf1, "Your poisoned %s %s $N%c",  attack, vp, punct );
		 sprintf( buf2, "$n's poisoned %s %s you%c", attack, vp, punct );
		 sprintf( buf3, "$n's poisoned %s %s $N%c",  attack, vp, punct );
		 sprintf( buf4, "Your poisoned %s %s you%c", attack, vp, punct );
		 sprintf( buf5, "$n's poisoned %s %s $n%c",  attack, vp, punct );
	}
	else
	{
		 sprintf( buf1, "Your %s %s $N%c",  attack, vp, punct );
		 sprintf( buf2, "$n's %s %s you%c", attack, vp, punct );
		 sprintf( buf3, "$n's %s %s $N%c",  attack, vp, punct );
		 sprintf( buf4, "Your %s %s you%c", attack, vp, punct );
		 sprintf( buf5, "$n's %s %s $n%c",  attack, vp, punct );
	}
		 }

	 if ( victim != ch )
	 {
		  act(AT_BLUE, buf1, ch, NULL, victim, TO_CHAR    );
		  act(AT_BLUE, buf2, ch, NULL, victim, TO_VICT    );
		  act(AT_BLUE, buf3, ch, NULL, victim, TO_NOTVICT );
			}
	 else
	 {
		  act(AT_BLUE, buf4, ch, NULL, victim, TO_CHAR    );
		  act(AT_BLUE, buf5, ch, NULL, victim, TO_ROOM    );
	 }
         ansi_color( NTEXT, ch );
         ansi_color( GREY, ch );
         return;
}



/*
 * Disarm a creature.
 * Caller must check for successful attack.
 */
void disarm( CHAR_DATA *ch, CHAR_DATA *victim )
{
    OBJ_DATA *obj;

    if ( ( obj = get_eq_char( victim, WEAR_WIELD ) ) == NULL )
	return;

    if ( get_eq_char( ch, WEAR_WIELD ) == NULL /*&& number_bits( 1 ) == 0*/ )
	return;

	 act(AT_WHITE, "$n DISARMS you!", ch, NULL, victim, TO_VICT    );
	 act(AT_WHITE, "You disarm $N!",  ch, NULL, victim, TO_CHAR    );
	 act(AT_WHITE, "$n DISARMS $N!",  ch, NULL, victim, TO_NOTVICT );

    obj_from_char( obj );
    if ( IS_NPC(victim) )
	obj_to_char( obj, victim );
    else
	obj_to_room( obj, victim->in_room );

    ansi_color( NTEXT, ch );
    ansi_color( GREY, ch );
    return;
}



/*
 * Trip a creature.
 * Caller must check for successful attack.
 */
void trip( CHAR_DATA *ch, CHAR_DATA *victim )
{
    if ( victim->wait == 0 )
	 {
		  act(AT_YELLOW, "$n trips you and you go down!", ch, NULL, victim, TO_VICT    );
		  act(AT_YELLOW, "You trip $N and $N goes down!", ch, NULL, victim, TO_CHAR    );
		  act(AT_YELLOW, "$n trips $N and $N goes down!", ch, NULL, victim, TO_NOTVICT );

	WAIT_STATE( ch,     2 * PULSE_VIOLENCE );
	WAIT_STATE( victim, 2 * PULSE_VIOLENCE );
	victim->position = POS_RESTING;
    }
         ansi_color( NTEXT, ch );
         ansi_color( GREY, ch );

    return;
}



void do_kill( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Kill whom?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( !IS_NPC(victim) )
    {
	if ( !IS_SET(victim->act, PLR_KILLER)
	&&   !IS_SET(victim->act, PLR_THIEF) )
	{
	    send_to_char( "You must MURDER a player.\n\r", ch );
	    return;
	}
    }
    else
    {
	if ( IS_AFFECTED(victim, AFF_CHARM) && victim->master != NULL )
	{
	    send_to_char( "You must MURDER a charmed creature.\n\r", ch );
	    return;
	}
    }

    if ( victim == ch )
    {
	send_to_char( "You hit yourself.  Ouch!\n\r", ch );
	multi_hit( ch, ch, TYPE_UNDEFINED );
	return;
    }

    if ( is_safe( ch, victim ) )
	return;

    if ( IS_AFFECTED(ch, AFF_CHARM) && ch->master == victim )
    {
	act(AT_GREY, "$N is your beloved master.", ch, NULL, victim, TO_CHAR );
	return;
    }

    if ( ch->position == POS_FIGHTING )
    {
	send_to_char( "You do the best you can!\n\r", ch );
	return;
    }

    WAIT_STATE( ch, 1 * PULSE_VIOLENCE );
    check_killer( ch, victim );
    multi_hit( ch, victim, TYPE_UNDEFINED );
    return;
}



void do_murde( CHAR_DATA *ch, char *argument )
{
    send_to_char( "If you want to MURDER, spell it out.\n\r", ch );
    return;
}



void do_murder( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Murder whom?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( victim == ch )
    {
	send_to_char( "Suicide is a mortal sin.\n\r", ch );
	return;
    }

    if ( is_safe( ch, victim ) )
	return;

    if ( IS_AFFECTED(ch, AFF_CHARM) && ch->master == victim )
    {
	act(AT_GREY, "$N is your beloved master.", ch, NULL, victim, TO_CHAR );
	return;
    }

    if ( ch->position == POS_FIGHTING )
    {
	send_to_char( "You do the best you can!\n\r", ch );
	return;
    }

    WAIT_STATE( ch, 1 * PULSE_VIOLENCE );
    sprintf( buf, "Help!  I am being attacked by %s!", ch->name );
    do_shout( victim, buf );
    check_killer( ch, victim );
    multi_hit( ch, victim, TYPE_UNDEFINED );
    return;
}



void do_backstab( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    OBJ_DATA *obj;

    if ( !IS_NPC(ch)
    &&   ch->level < skill_table[gsn_backstab].skill_level[ch->class] )
    {
	send_to_char(
	    "You better leave the assassin trade to thieves.\n\r", ch );
	return;
    }

    one_argument( argument, arg );
    
    if ( arg[0] == '\0' )
    {
	send_to_char( "Backstab whom?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( victim == ch )
    {
	send_to_char( "How can you sneak up on yourself?\n\r", ch );
	return;
    }

    if ( is_safe( ch, victim ) )
      return;

    if ( ( obj = get_eq_char( ch, WEAR_WIELD ) ) == NULL
    ||   obj->value[3] != 11 )
    {
	send_to_char( "You need to wield a piercing weapon.\n\r", ch );
	return;
    }

    if ( victim->fighting != NULL )
    {
	send_to_char( "You can't backstab a fighting person.\n\r", ch );
	return;
    }

    if ( victim->hit < victim->max_hit )
    {
	act(AT_GREEN, "$N is hurt and suspicious ... you can't sneak up.",
	    ch, NULL, victim, TO_CHAR );
	return;
    }

    check_killer( ch, victim );
    WAIT_STATE( ch, skill_table[gsn_backstab].beats );
    if ( !IS_AWAKE(victim)
    ||   IS_NPC(ch)
    ||   number_percent( ) < ch->pcdata->learned[gsn_backstab] )
	multi_hit( ch, victim, gsn_backstab );
    else
	damage( ch, victim, 0, gsn_backstab );

    return;
}



void do_flee( CHAR_DATA *ch, char *argument )
{
    ROOM_INDEX_DATA *was_in;
    ROOM_INDEX_DATA *now_in;
    CHAR_DATA *victim;
    int attempt;

    if ( ( victim = ch->fighting ) == NULL )
    {
	if ( ch->position == POS_FIGHTING )
	    ch->position = POS_STANDING;
	send_to_char( "You aren't fighting anyone.\n\r", ch );
	return;
    }

    was_in = ch->in_room;
    for ( attempt = 0; attempt < 6; attempt++ )
    {
	EXIT_DATA *pexit;
	int door;

	door = number_door( );
	if ( ( pexit = was_in->exit[door] ) == 0
	    ||   pexit->to_room == NULL
	    ||   IS_SET(pexit->exit_info, EX_CLOSED)
	    || ( IS_NPC(ch)
		&& ( IS_SET(pexit->to_room->room_flags, ROOM_NO_MOB)
		    || ( IS_SET(ch->act, ACT_STAY_AREA)
			&& pexit->to_room->area != ch->in_room->area ) ) ) )
	    continue;

	move_char( ch, door );
	if ( ( now_in = ch->in_room ) == was_in )
	    continue;

	ch->in_room = was_in;
		  act(AT_YELLOW, "$n has fled!", ch, NULL, NULL, TO_ROOM );
		  ch->in_room = now_in;

	if ( !IS_NPC(ch) )
	{
ansi_color( BOLD, ch );
ansi_color( YELLOW, ch );
            send_to_char( "You flee from combat!  You lose 25 exps.\n\r", ch );
ansi_color( NTEXT, ch );
ansi_color( GREY, ch );
            gain_exp( ch, -25 );
	}

	stop_fighting( ch, TRUE );
	return;
    }

ansi_color( BOLD, ch );
    send_to_char( "You failed!  You lose 10 exps.\n\r", ch );
ansi_color( NTEXT, ch );
    gain_exp( ch, -10 );
    return;
}



void do_rescue( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    CHAR_DATA *fch;

    if ( !IS_NPC(ch)
    &&   ch->level < skill_table[gsn_rescue].skill_level[ch->class] )
    {
	send_to_char(
	    "You better leave the heroic acts to warriors.\n\r", ch );
	return;
    }

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Rescue whom?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( victim == ch )
    {
	send_to_char( "What about fleeing instead?\n\r", ch );
	return;
    }

    if ( !IS_NPC(ch) && IS_NPC(victim) )
    {
	send_to_char( "Doesn't need your help!\n\r", ch );
	return;
    }

    if ( ch->fighting == victim )
    {
	send_to_char( "Too late.\n\r", ch );
	return;
    }

    if ( ( fch = victim->fighting ) == NULL )
    {
	send_to_char( "That person is not fighting right now.\n\r", ch );
	return;
    }

    WAIT_STATE( ch, skill_table[gsn_rescue].beats );
    if ( !IS_NPC(ch) && number_percent( ) > ch->pcdata->learned[gsn_rescue] )
    {
	send_to_char( "You fail the rescue.\n\r", ch );
	return;
	 }

	 act(AT_GREEN, "You rescue $N!",  ch, NULL, victim, TO_CHAR    );
	 act(AT_GREEN, "$n rescues you!", ch, NULL, victim, TO_VICT    );
	 act(AT_GREEN, "$n rescues $N!",  ch, NULL, victim, TO_NOTVICT );

    stop_fighting( fch, FALSE );
    stop_fighting( victim, FALSE );

    check_killer( ch, fch );
    set_fighting( ch, fch );
    set_fighting( fch, ch );
    return;
}



void do_kick( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;

    if ( !IS_NPC(ch)
    &&   ch->level < skill_table[gsn_kick].skill_level[ch->class] )
    {
	send_to_char(
	    "You better leave the martial arts to fighters.\n\r", ch );
	return;
    }

    if ( ( victim = ch->fighting ) == NULL )
    {
	send_to_char( "You aren't fighting anyone.\n\r", ch );
	return;
    }

    WAIT_STATE( ch, skill_table[gsn_kick].beats );
    if ( IS_NPC(ch) || number_percent( ) < ch->pcdata->learned[gsn_kick] )
	damage( ch, victim, number_range( 1, ch->level ), gsn_kick );
    else
	damage( ch, victim, 0, gsn_kick );

    return;
}




void do_disarm( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    OBJ_DATA *obj;
    int percent;

    if ( !IS_NPC(ch)
    &&   ch->level < skill_table[gsn_disarm].skill_level[ch->class] )
    {
	send_to_char( "You don't know how to disarm opponents.\n\r", ch );
	return;
    }

    if ( get_eq_char( ch, WEAR_WIELD ) == NULL )
    {
	send_to_char( "You must wield a weapon to disarm.\n\r", ch );
	return;
    }

    if ( ( victim = ch->fighting ) == NULL )
    {
	send_to_char( "You aren't fighting anyone.\n\r", ch );
	return;
    }

    if ( ( obj = get_eq_char( victim, WEAR_WIELD ) ) == NULL )
    {
	send_to_char( "Your opponent is not wielding a weapon.\n\r", ch );
	return;
    }

    WAIT_STATE( ch, skill_table[gsn_disarm].beats );
    percent = number_percent( ) + victim->level - ch->level;
    if ( IS_NPC(ch) || percent < ch->pcdata->learned[gsn_disarm] * 2 / 3 )
	disarm( ch, victim );
    else
	send_to_char( "You failed.\n\r", ch );
    return;
}



void do_sla( CHAR_DATA *ch, char *argument )
{
    send_to_char( "If you want to SLAY, spell it out.\n\r", ch );
    return;
}



void do_slay( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char arg[MAX_INPUT_LENGTH];

    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	send_to_char( "Slay whom?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( ch == victim )
    {
	send_to_char( "Suicide is a mortal sin.\n\r", ch );
	return;
    }

    if ( !IS_NPC(victim) && victim->level >= ch->level )
    {
	send_to_char( "You failed.\n\r", ch );
	return;
    }

	 act(AT_RED, "You slay $M in cold blood!",  ch, NULL, victim, TO_CHAR    );
	 act(AT_RED, "$n slays you in cold blood!", ch, NULL, victim, TO_VICT    );
	 act(AT_RED, "$n slays $N in cold blood!",  ch, NULL, victim, TO_NOTVICT );
	 raw_kill( victim );
	 return;
}

void do_berserk( CHAR_DATA *ch, char *argument)
{
	 int chance, hp_percent;

	 if (!IS_NPC(ch) && ch->level < skill_table[gsn_berserk].skill_level[ch->class])
	 {
	send_to_char("You turn red in the face, but nothing happens.\n\r",ch);
	return;
	 }

	 if (IS_AFFECTED(ch,AFF_BERSERK) || is_affected(ch,gsn_berserk))
		  {
	send_to_char("You get a little madder.\n\r",ch);
	return;
	 }


	 if (ch->mana < 50)
	 {
	send_to_char("You can't get up enough energy.\n\r",ch);
	return;
	 }

	 /* modifiers */

	 /* fighting */
	 if (ch->position == POS_FIGHTING)
	chance += 10;

	 /* damage -- below 50% of hp helps, above hurts */
	 hp_percent = 100 * ch->hit/ch->max_hit;
	 chance += 25 - hp_percent/2;




	{ AFFECT_DATA af;

	WAIT_STATE(ch,PULSE_VIOLENCE);
	ch->mana -= 50;
	ch->move /= 2;

	/* heal a little damage */
	ch->hit += ch->level * 2;
	ch->hit = UMIN(ch->hit,ch->max_hit);

	ansi_color( RED, ch);
	ansi_color( BOLD, ch );
	send_to_char("Your pulse races as you are consumned by rage!\n\r",ch);
	ansi_color( NTEXT, ch );
	ansi_color( GREY, ch );
	act(AT_RED,"$n gets a wild look in $s eyes.",ch,NULL,NULL,TO_ROOM);

	af.type		= gsn_berserk;
	af.duration	= number_fuzzy(ch->level / 8);
	af.modifier	= UMAX(1,ch->level/5);
	af.bitvector 	= AFF_BERSERK;

	af.location	= APPLY_HITROLL;
	affect_to_char(ch,&af);

	af.location	= APPLY_DAMROLL;
	affect_to_char(ch,&af);

	af.modifier	= UMAX(10,10 * (ch->level/5));
	af.location	= APPLY_AC;
	affect_to_char(ch,&af);
	  }

}


