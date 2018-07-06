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
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"
#include <ctype.h> // @@@ Needed for spec_cast_from_list


/*
 * The following special functions are available for mobiles.
 */
DECLARE_SPEC_FUN(	spec_breath_any		);
DECLARE_SPEC_FUN(	spec_breath_acid	);
DECLARE_SPEC_FUN(	spec_breath_fire	);
DECLARE_SPEC_FUN(	spec_breath_frost	);
DECLARE_SPEC_FUN(	spec_breath_gas		);
DECLARE_SPEC_FUN(	spec_breath_lightning	);
DECLARE_SPEC_FUN(	spec_cast_adept		);
DECLARE_SPEC_FUN(	spec_cast_cleric	);
DECLARE_SPEC_FUN(	spec_cast_judge		);
DECLARE_SPEC_FUN(	spec_cast_mage		);
DECLARE_SPEC_FUN(	spec_cast_undead	);
DECLARE_SPEC_FUN(	spec_executioner	);
DECLARE_SPEC_FUN(	spec_fido		);
DECLARE_SPEC_FUN(	spec_guard		);
DECLARE_SPEC_FUN(	spec_janitor		);
DECLARE_SPEC_FUN(	spec_mayor		);
DECLARE_SPEC_FUN(	spec_poison		);
DECLARE_SPEC_FUN(	spec_thief		);
DECLARE_SPEC_FUN(       spec_cast_psionicist    );
DECLARE_SPEC_FUN(	spec_animated_dead    ); // @@@
DECLARE_SPEC_FUN(	spec_taxidermist    ); // @@@
DECLARE_SPEC_FUN(	spec_capn_swing    ); // @@@
DECLARE_SPEC_FUN(	spec_snake_charm		); // @@@
DECLARE_SPEC_FUN(	spec_hunger		);
DECLARE_SPEC_FUN( spec_cast_from_list ); // @@@ ###
DECLARE_SPEC_FUN( spec_shopkeeper ); // @@@
DECLARE_SPEC_FUN( spec_obj_bribe ); // @@@
DECLARE_SPEC_FUN( spec_agg_no_object ); // @@@
DECLARE_SPEC_FUN( spec_follow_mob ); // @@@
DECLARE_SPEC_FUN( spec_generator ); // @@@
DECLARE_SPEC_FUN( spec_defend_vs_agg ); // @@@
DECLARE_SPEC_FUN( spec_agg_if_object ); // @@@
DECLARE_SPEC_FUN( spec_circuit ); // @@@
DECLARE_SPEC_FUN( spec_hatred ); // @@@


/*
 * Given a name, return the appropriate spec fun.
 */
SPEC_FUN *spec_lookup( const char *name )
{
	 if ( !str_cmp( name, "spec_breath_any"	  ) ) return spec_breath_any;
	 if ( !str_cmp( name, "spec_breath_acid"	  ) ) return spec_breath_acid;
	 if ( !str_cmp( name, "spec_breath_fire"	  ) ) return spec_breath_fire;
	 if ( !str_cmp( name, "spec_breath_frost"	  ) ) return spec_breath_frost;
	 if ( !str_cmp( name, "spec_breath_gas"	  ) ) return spec_breath_gas;
	 if ( !str_cmp( name, "spec_breath_lightning"  ) ) return
							spec_breath_lightning;
	 if ( !str_cmp( name, "spec_cast_adept"	  ) ) return spec_cast_adept;
	 if ( !str_cmp( name, "spec_cast_cleric"	  ) ) return spec_cast_cleric;
	 if ( !str_cmp( name, "spec_cast_judge"	  ) ) return spec_cast_judge;
	 if ( !str_cmp( name, "spec_cast_mage"	  ) ) return spec_cast_mage;
	 if ( !str_cmp( name, "spec_cast_undead"	  ) ) return spec_cast_undead;
	 if ( !str_cmp( name, "spec_executioner"	  ) ) return spec_executioner;
	 if ( !str_cmp( name, "spec_fido"		  ) ) return spec_fido;
	 if ( !str_cmp( name, "spec_guard"		  ) ) return spec_guard;
	 if ( !str_cmp( name, "spec_janitor"		  ) ) return spec_janitor;
	 if ( !str_cmp( name, "spec_mayor"		  ) ) return spec_mayor;
	 if ( !str_cmp( name, "spec_poison"		  ) ) return spec_poison;

	 if ( !str_cmp( name, "spec_cast_psionicist"   ) ) return
																		  spec_cast_psionicist;

	 if ( !str_cmp( name, "spec_thief"		  ) ) return spec_thief;
	 if ( !str_cmp( name, "spec_taxidermist"		  ) ) return spec_taxidermist; // @@@
	 if ( !str_cmp( name, "spec_animated_dead"		  ) ) return spec_animated_dead;
	 if ( !str_cmp( name, "spec_capn_swing"		  ) ) return spec_capn_swing; // @@@
	 if ( !str_cmp( name, "spec_snake_charm"		  ) ) return spec_snake_charm; // @@@
	 if ( !str_cmp( name, "spec_hunger"		  ) ) return spec_hunger; // @@
	 if ( !str_cmp( name, "spec_cast_from_list" )) return spec_cast_from_list; // @@@ ###
	 if ( !str_cmp( name, "spec_shopkeeper" )) return spec_shopkeeper; // @@@ ###
	 if ( !str_cmp( name, "spec_obj_bribe" )) return spec_obj_bribe; // @@@ ###
	 if ( !str_cmp( name, "spec_agg_no_object" )) return spec_agg_no_object; // @@@ ###
	 if ( !str_cmp( name, "spec_follow_mob" )) return spec_follow_mob; // @@@
	 if ( !str_cmp( name, "spec_generator" )) return spec_generator; // @@@
	 if ( !str_cmp( name, "spec_defend_vs_agg" )) return spec_defend_vs_agg; // @@@
	 if ( !str_cmp( name, "spec_agg_if_object" )) return spec_agg_if_object; // @@@
	 if ( !str_cmp( name, "spec_circuit" )) return spec_circuit; // @@@
	 if ( !str_cmp( name, "spec_hatred" )) return spec_hatred; // @@@
	 return 0;
}



/*
 * Core procedure for dragons.
 */
bool dragon( CHAR_DATA *ch, char *spell_name )
{
    CHAR_DATA *victim;
    CHAR_DATA *v_next;
	 int sn;

    if ( ch->position != POS_FIGHTING )
	return FALSE;

    for ( victim = ch->in_room->people; victim != NULL; victim = v_next )
    {
	v_next = victim->next_in_room;
	if ( victim->fighting == ch && number_bits( 2 ) == 0 )
	    break;
    }

	 if ( victim == NULL )
	return FALSE;

    if ( ( sn = skill_lookup( spell_name ) ) < 0 )
	return FALSE;
    (*skill_table[sn].spell_fun) ( sn, ch->level, ch, victim );
    return TRUE;
}



/*
 * Special procedures for mobiles.
 */
bool spec_breath_any( CHAR_DATA *ch )
{
    if ( ch->position != POS_FIGHTING )
	return FALSE;

    switch ( number_bits( 3 ) )
    {
    case 0: return spec_breath_fire		( ch );
    case 1:
	 case 2: return spec_breath_lightning	( ch );
	 case 3: return spec_breath_gas		( ch );
	 case 4: return spec_breath_acid		( ch );
    case 5:
    case 6:
    case 7: return spec_breath_frost		( ch );
    }

    return FALSE;
}



bool spec_breath_acid( CHAR_DATA *ch )
{
    return dragon( ch, "acid breath" );
}



bool spec_breath_fire( CHAR_DATA *ch )
{
	 return dragon( ch, "fire breath" );
}



bool spec_breath_frost( CHAR_DATA *ch )
{
    return dragon( ch, "frost breath" );
}



bool spec_breath_gas( CHAR_DATA *ch )
{
    int sn;

	 if ( ch->position != POS_FIGHTING )
	return FALSE;

    if ( ( sn = skill_lookup( "gas breath" ) ) < 0 )
	return FALSE;
    (*skill_table[sn].spell_fun) ( sn, ch->level, ch, NULL );
	 return TRUE;
}



bool spec_breath_lightning( CHAR_DATA *ch )
{
    return dragon( ch, "lightning breath" );
}



bool spec_cast_adept( CHAR_DATA *ch )
{
    CHAR_DATA *victim;
    CHAR_DATA *v_next;

	 if ( !IS_AWAKE(ch) )
	return FALSE;

    for ( victim = ch->in_room->people; victim != NULL; victim = v_next )
    {
	v_next = victim->next_in_room;
	if ( victim != ch && can_see( ch, victim ) && number_bits( 1 ) == 0 )
	    break;
    }

    if ( victim == NULL )
	return FALSE;

	 switch ( number_bits( 3 ) )
    {
    case 0:
	act(AT_CYAN, "$n utters the word 'tehctah'.", ch, NULL, NULL, TO_ROOM );
	spell_armor( skill_lookup( "armor" ), ch->level, ch, victim );
	return TRUE;

    case 1:
	act(AT_CYAN, "$n utters the word 'nhak'.", ch, NULL, NULL, TO_ROOM );
	spell_bless( skill_lookup( "bless" ), ch->level, ch, victim );
	return TRUE;

	 case 2:
	act(AT_CYAN, "$n utters the word 'yeruf'.", ch, NULL, NULL, TO_ROOM );
	spell_cure_blindness( skill_lookup( "cure blindness" ),
	    ch->level, ch, victim );
	return TRUE;

    case 3:
	act(AT_CYAN, "$n utters the word 'garf'.", ch, NULL, NULL, TO_ROOM );
	spell_cure_light( skill_lookup( "cure light" ),
	    ch->level, ch, victim );
	return TRUE;

	 case 4:
	act(AT_CYAN, "$n utters the words 'rozar'.", ch, NULL, NULL, TO_ROOM );
	spell_cure_poison( skill_lookup( "cure poison" ),
	    ch->level, ch, victim );
	return TRUE;

    case 5:
	act(AT_CYAN, "$n utters the words 'nadroj'.", ch, NULL, NULL, TO_ROOM );
	spell_refresh( skill_lookup( "refresh" ), ch->level, ch, victim );
	return TRUE;

	 }

    return FALSE;
}



bool spec_cast_cleric( CHAR_DATA *ch )
{
    CHAR_DATA *victim;
    CHAR_DATA *v_next;
    char *spell;
	 int sn;

    if ( ch->position != POS_FIGHTING )
	return FALSE;

    for ( victim = ch->in_room->people; victim != NULL; victim = v_next )
    {
	v_next = victim->next_in_room;
	if ( victim->fighting == ch && number_bits( 2 ) == 0 )
	    break;
    }

	 if ( victim == NULL )
	return FALSE;

    for ( ;; )
    {
	int min_level;

	switch ( number_bits( 4 ) )
	{
	case  0: min_level =  0; spell = "blindness";      break;
	case  1: min_level =  3; spell = "cause serious";  break;
	case  2: min_level =  7; spell = "earthquake";     break;
	case  3: min_level =  9; spell = "cause critical"; break;
	case  4: min_level = 10; spell = "dispel evil";    break;
	case  5: min_level = 12; spell = "curse";          break;
	case  6: min_level = 12; spell = "change sex";     break;
	case  7: min_level = 13; spell = "flamestrike";    break;
	case  8:
	case  9:
	case 10: min_level = 15; spell = "harm";           break;
	default: min_level = 16; spell = "dispel magic";   break;
	}

	if ( ch->level >= min_level )
		 break;
	 }

	 if ( ( sn = skill_lookup( spell ) ) < 0 )
	return FALSE;
	 (*skill_table[sn].spell_fun) ( sn, ch->level, ch, victim );
    return TRUE;
}



bool spec_cast_judge( CHAR_DATA *ch )
{
    CHAR_DATA *victim;
    CHAR_DATA *v_next;
    char *spell;
    int sn;

    if ( ch->position != POS_FIGHTING )
	return FALSE;

    for ( victim = ch->in_room->people; victim != NULL; victim = v_next )
    {
	v_next = victim->next_in_room;
	if ( victim->fighting == ch && number_bits( 2 ) == 0 )
	    break;
    }

    if ( victim == NULL )
	return FALSE;

    spell = "high explosive";
    if ( ( sn = skill_lookup( spell ) ) < 0 )
	return FALSE;
    (*skill_table[sn].spell_fun) ( sn, ch->level, ch, victim );
    return TRUE;
}



bool spec_cast_mage( CHAR_DATA *ch )
{
    CHAR_DATA *victim;
    CHAR_DATA *v_next;
    char *spell;
    int sn;

    if ( ch->position != POS_FIGHTING )
	return FALSE;

    for ( victim = ch->in_room->people; victim != NULL; victim = v_next )
    {
	v_next = victim->next_in_room;
	if ( victim->fighting == ch && number_bits( 2 ) == 0 )
	    break;
    }

    if ( victim == NULL )
	return FALSE;

	 for ( ;; )
    {
	int min_level;

	switch ( number_bits( 4 ) )
	{
	case  0: min_level =  0; spell = "blindness";      break;
	case  1: min_level =  3; spell = "chill touch";    break;
	case  2: min_level =  7; spell = "weaken";         break;
	case  3: min_level =  8; spell = "teleport";       break;
	case  4: min_level = 11; spell = "colour spray";   break;
	case  5: min_level = 12; spell = "change sex";     break;
	case  6: min_level = 13; spell = "energy drain";   break;
	case  7:
	case  8:
	case  9: min_level = 15; spell = "fireball";       break;
	default: min_level = 20; spell = "acid blast";     break;
	}

	if ( ch->level >= min_level )
	    break;
    }

    if ( ( sn = skill_lookup( spell ) ) < 0 )
	return FALSE;
	 (*skill_table[sn].spell_fun) ( sn, ch->level, ch, victim );
    return TRUE;
}



bool spec_cast_undead( CHAR_DATA *ch )
{
    CHAR_DATA *victim;
    CHAR_DATA *v_next;
    char *spell;
	 int sn;

    if ( ch->position != POS_FIGHTING )
	return FALSE;

    for ( victim = ch->in_room->people; victim != NULL; victim = v_next )
    {
	v_next = victim->next_in_room;
	if ( victim->fighting == ch && number_bits( 2 ) == 0 )
	    break;
    }

	 if ( victim == NULL )
	return FALSE;

    for ( ;; )
    {
	int min_level;

	switch ( number_bits( 4 ) )
	{
	case  0: min_level =  0; spell = "curse";          break;
	case  1: min_level =  3; spell = "weaken";         break;
	case  2: min_level =  6; spell = "chill touch";    break;
	case  3: min_level =  9; spell = "blindness";      break;
	case  4: min_level = 12; spell = "poison";         break;
	case  5: min_level = 15; spell = "energy drain";   break;
	case  6: min_level = 18; spell = "harm";           break;
	case  7: min_level = 21; spell = "teleport";       break;
	default: min_level = 24; spell = "gate";           break;
	}

	if ( ch->level >= min_level )
		 break;
    }

	 if ( ( sn = skill_lookup( spell ) ) < 0 )
	return FALSE;
    (*skill_table[sn].spell_fun) ( sn, ch->level, ch, victim );
    return TRUE;
}



bool spec_executioner( CHAR_DATA *ch )
{
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;
	 CHAR_DATA *v_next;
    char *crime;

    if ( !IS_AWAKE(ch) || ch->fighting != NULL )
	return FALSE;

    crime = "";
	 for ( victim = ch->in_room->people; victim != NULL; victim = v_next )
    {
	v_next = victim->next_in_room;

	if ( !IS_NPC(victim) && IS_SET(victim->act, PLR_KILLER) )
		 { crime = "KILLER"; break; }

	if ( !IS_NPC(victim) && IS_SET(victim->act, PLR_THIEF) )
	    { crime = "THIEF"; break; }
    }

    if ( victim == NULL )
	return FALSE;

    sprintf( buf, "%s is a %s!  PROTECT THE INNOCENT!  MORE BLOOOOD!!!",
	victim->name, crime );
    do_shout( ch, buf );
	 multi_hit( ch, victim, TYPE_UNDEFINED );
    char_to_room( create_mobile( get_mob_index(MOB_VNUM_CITYGUARD) ),
	ch->in_room );
    char_to_room( create_mobile( get_mob_index(MOB_VNUM_CITYGUARD) ),
	ch->in_room );
	 return TRUE;
}



bool spec_fido( CHAR_DATA *ch )
{
	 OBJ_DATA *corpse;
    OBJ_DATA *c_next;
    OBJ_DATA *obj;
    OBJ_DATA *obj_next;

    if ( !IS_AWAKE(ch) )
	return FALSE;

    for ( corpse = ch->in_room->contents; corpse != NULL; corpse = c_next )
    {
	c_next = corpse->next_content;
	if ( corpse->item_type != ITEM_CORPSE_NPC )
		 continue;

	act(AT_DGREEN, "$n savagely devours a corpse.", ch, NULL, NULL, TO_ROOM );
	for ( obj = corpse->contains; obj; obj = obj_next )
	{
	    obj_next = obj->next_content;
	    obj_from_obj( obj );
	    obj_to_room( obj, ch->in_room );
	}
	extract_obj( corpse );
	return TRUE;
    }

    return FALSE;
}



bool spec_guard( CHAR_DATA *ch )
{
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;
    CHAR_DATA *v_next;
    CHAR_DATA *ech;
	 char *crime;
	 int max_evil;

    if ( !IS_AWAKE(ch) || ch->fighting != NULL )
	return FALSE;

    max_evil = 300;
    ech      = NULL;
    crime    = "";

    for ( victim = ch->in_room->people; victim != NULL; victim = v_next )
    {
	v_next = victim->next_in_room;

	if ( !IS_NPC(victim) && IS_SET(victim->act, PLR_KILLER) )
	    { crime = "KILLER"; break; }

	if ( !IS_NPC(victim) && IS_SET(victim->act, PLR_THIEF) )
	    { crime = "THIEF"; break; }

	if ( victim->fighting != NULL
	&&   victim->fighting != ch
	&&   victim->alignment < max_evil )
	{
		 max_evil = victim->alignment;
	    ech      = victim;
	}
    }

    if ( victim != NULL )
    {
	sprintf( buf, "%s is a %s!  PROTECT THE INNOCENT!!  BANZAI!!",
	    victim->name, crime );
	do_shout( ch, buf );
	multi_hit( ch, victim, TYPE_UNDEFINED );
	return TRUE;
	 }

    if ( ech != NULL )
    {
	act(AT_CYAN, "$n screams 'PROTECT THE INNOCENT!!  BANZAI!!",
	    ch, NULL, NULL, TO_ROOM );
	multi_hit( ch, ech, TYPE_UNDEFINED );
	return TRUE;
    }

    return FALSE;
}



bool spec_janitor( CHAR_DATA *ch )
{
    OBJ_DATA *trash;
    OBJ_DATA *trash_next;

    if ( !IS_AWAKE(ch) )
	return FALSE;

    for ( trash = ch->in_room->contents; trash != NULL; trash = trash_next )
	 {
	trash_next = trash->next_content;
	if ( !IS_SET( trash->wear_flags, ITEM_TAKE ) )
	    continue;
	if ( trash->item_type == ITEM_DRINK_CON
	||   trash->item_type == ITEM_TRASH
	||   trash->cost < 10 )
	{
	    act(AT_CYAN, "$n picks up some trash.", ch, NULL, NULL, TO_ROOM );
	    obj_from_room( trash );
	    obj_to_char( trash, ch );
	    return TRUE;
	}
    }

    return FALSE;
}



bool spec_mayor( CHAR_DATA *ch )
{
    static const char open_path[] =
	"W3a3003b33000c111d0d111Oe333333Oe22c222112212111a1S.";

    static const char close_path[] =
	"W3a3003b33000c111d0d111CE333333CE22c222112212111a1S.";

    static const char *path;
	 static int pos;
    static bool move;

    if ( !move )
    {
	if ( time_info.hour ==  6 )
	{
		 path = open_path;
	    move = TRUE;
	    pos  = 0;
	}

	if ( time_info.hour == 20 )
	{
	    path = close_path;
	    move = TRUE;
	    pos  = 0;
	}
    }

    if ( ch->fighting != NULL )
	return spec_cast_cleric( ch );
	 if ( !move || ch->position < POS_SLEEPING )
	return FALSE;

    switch ( path[pos] )
    {
    case '0':
    case '1':
    case '2':
    case '3':
	move_char( ch, path[pos] - '0' );
	break;

    case 'W':
	ch->position = POS_STANDING;
	act(AT_CYAN, "$n awakens and groans loudly.", ch, NULL, NULL, TO_ROOM );
	break;

    case 'S':
	ch->position = POS_SLEEPING;
	act(AT_CYAN, "$n lies down and falls asleep.", ch, NULL, NULL, TO_ROOM );
	break;

	 case 'a':
	act(AT_CYAN, "$n says 'Hello Honey!'", ch, NULL, NULL, TO_ROOM );
	break;

    case 'b':
	act(AT_CYAN, "$n says 'What a view!  I must do something about that dump!'",
	    ch, NULL, NULL, TO_ROOM );
	break;

    case 'c':
	act(AT_CYAN, "$n says 'Vandals!  Youngsters have no respect for anything!'",
		 ch, NULL, NULL, TO_ROOM );
	break;

    case 'd':
	act(AT_CYAN, "$n says 'Good day, citizens!'", ch, NULL, NULL, TO_ROOM );
	break;

    case 'e':
	act(AT_CYAN, "$n says 'I hereby declare the city of Pelargir open!'",
	    ch, NULL, NULL, TO_ROOM );
	break;

	 case 'E':
	act(AT_CYAN, "$n says 'I hereby declare the city of Midgaard closed!'",
	    ch, NULL, NULL, TO_ROOM );
	break;

    case 'O':
	do_unlock( ch, "gate" );
	do_open( ch, "gate" );
	break;

    case 'C':
	do_close( ch, "gate" );
	do_lock( ch, "gate" );
	break;

    case '.' :
	move = FALSE;
	break;
    }

    pos++;
	 return FALSE;
}



bool spec_poison( CHAR_DATA *ch )
{
    CHAR_DATA *victim;

    if ( ch->position != POS_FIGHTING
    || ( victim = ch->fighting ) == NULL
    ||   number_percent( ) > 2 * ch->level )
	return FALSE;

    act(AT_BLOOD, "You bite $N!",  ch, NULL, victim, TO_CHAR    );
	 act(AT_BLOOD, "$n bites $N!",  ch, NULL, victim, TO_NOTVICT );
    act(AT_BLOOD, "$n bites you!", ch, NULL, victim, TO_VICT    );
    spell_poison( gsn_poison, ch->level, ch, victim );
    return TRUE;
}



bool spec_thief( CHAR_DATA *ch )
{
    CHAR_DATA *victim;
    CHAR_DATA *v_next;
	 int gold;

    if ( ch->position != POS_STANDING )
	return FALSE;

    for ( victim = ch->in_room->people; victim != NULL; victim = v_next )
    {
	v_next = victim->next_in_room;

	if ( IS_NPC(victim)
	||   victim->level >= LEVEL_IMMORTAL
	||   number_bits( 2 ) != 0
	||   !can_see( ch, victim ) )	/* Thx Glop */
	    continue;

	if ( IS_AWAKE(victim) && number_range( 0, ch->level ) == 0 )
	{
		 act(AT_GREEN, "You discover $n's hands in your wallet!",
		ch, NULL, victim, TO_VICT );
	    act(AT_GREEN, "$N discovers $n's hands in $S wallet!",
		ch, NULL, victim, TO_NOTVICT );
	    return TRUE;
	}
	else
	{
	    gold = victim->gold * number_range( 1, 20 ) / 100;
	    ch->gold     += 7 * gold / 8;
	    victim->gold -= gold;
	    return TRUE;
	}
    }

    return FALSE;
}

// @@@
bool spec_taxidermist( CHAR_DATA *ch )
{
    OBJ_DATA *inv;
	 int sn;

    if ( ch->position != POS_STANDING )
	return FALSE;

    if ( ch->pIndexData->pShop == 0 ||
         time_info.hour < ch->pIndexData->pShop->open_hour ||
         time_info.hour > ch->pIndexData->pShop->close_hour )
       return FALSE;

    for ( inv = ch->carrying; inv != NULL; inv = inv->next_content )
    {
    if (inv->item_type == ITEM_CORPSE_NPC)
       {
       if ( ( sn = skill_lookup( "make purse" ) ) >= 0 )
          (*skill_table[sn].spell_fun) ( sn, ch->level, ch, inv );
       return TRUE;
       }
    else if (inv->wear_loc == WEAR_NONE && number_percent() < 5)
       {
       act(AT_CYAN, "$n suggests you buy $p.", ch, inv, NULL, TO_ROOM );
		 return TRUE;
		 }
    }

    return FALSE;
}

// @@@
/* The start and end piers are hard-coded into Captain Swing.  If we need more
   ferries we will have to give him something that encodes the vnums and the
   current ferry state information */
#define ROOM_VNUM_START 5950
#define ROOM_VNUM_FINISH 5953
#define OBJ_VNUM_TICKET 5953

bool spec_capn_swing( CHAR_DATA *ch )
{
    static int state;                   /* Boat's current state, 0-11 */
    static EXIT_DATA *exit_to_boat;     /* A 'doorless' door onto the boat,
                                           disconnected when the ferry leaves
                                           and reconnected when it returns. */

    int has_ticket;                     /* Does the PC/Mob have a ticket? */
	 CHAR_DATA *passenger;               /* A passenger */
	 ROOM_INDEX_DATA *boat = ch->in_room->exit[1]->to_room; /* The boat room */
    OBJ_DATA *obj;                      /* An inv object */
    CHAR_DATA *sea_hag;                 /* A mob which boards the ship at sea */
    CHAR_DATA *person;                  /* A person standing on the dock */
    int i;

    /* @@@ */
    for (i = 0; i < 10; i++)
      	obj_to_room( create_money( 50 ),  boat);

    switch(state) {
    case 0: /* Check to see if anyone is on the boat */
		 for ( passenger = boat->people;
             passenger;
             passenger = passenger->next_in_room )
          {
          state = 1;
          send_to_char("Captain Swing says 'All aboard?'.\n\r", passenger);
          }
       break;
    case 1: /* Warn chars on boat they are going to Mahn-Tor */
		 state = 0;
       for ( passenger = boat->people;
             passenger;
				 passenger = passenger->next_in_room )
          {
          state = 2;
          send_to_char("Captain Swing says 'Last call for Mahn-Tor!'.\n\r",
             passenger);
          }
       break;

    case 2: /* Check for tickets and leave the dock */
       state = 0;
       for ( passenger = boat->people;
             passenger;
				 passenger = passenger->next_in_room )
          {
          /* We can't require immortals to have a ticket, as they might not
             be able to afford one, and also might be able to kill Swing,
             leaving the boat adrift until he repops. */
          if ( IS_IMMORTAL(passenger) )
             {
				 has_ticket = 1;
             state = 3;
             send_to_char( "Captain Swing has comped you.\n\r",
                passenger );
             }
			 else
             {
             /* Check for a valid ferry ticket. */
             has_ticket = 0;
             for ( obj = passenger->carrying; obj; obj = obj->next_content )
                {
                if ( obj->pIndexData->vnum == OBJ_VNUM_TICKET )
        	       {
                   has_ticket = 1;
                   state = 3;
                   extract_obj( obj );
                   send_to_char( "Captain Swing collects your ticket.\n\r",
							 passenger );
                   }
                }
             }

			 if (!has_ticket)
             {
             send_to_char( "Captain Swing notices you have no ticket and hurls"
                           " you into the air.\n\r", passenger );
             act(AT_PINK, "$n is hurtled off by an angry Captain Swing.",
                passenger, NULL, NULL, TO_ROOM );
             char_from_room( passenger );
				 char_to_room( passenger, boat->exit[3]->to_room );
             act(AT_DBLUE, "$n crashes to the ground.", passenger, NULL, NULL, TO_ROOM );
             send_to_char( "You hit the ground, hard!\n\r", passenger);
             do_look( passenger, "auto" );
             }
          }

       /* Close and lock the pick-proof door, to prevent players from trying to
          kill Captain Swing.  They shouldn't even be able to see him unless
          they teleport into his room... */
       SET_BIT(boat->exit[3]->exit_info, EX_CLOSED|EX_LOCKED);
       for ( passenger = boat->people;
				 passenger;
             passenger = passenger->next_in_room )
          send_to_char("Captain Swing has secured the door.\n\r"
				 "You feel the boat begin to rock as it leaves the dock.\n\r",
             passenger);

       /* Remove the boat from the dock's list of exits */
       exit_to_boat = get_room_index( ROOM_VNUM_START )->exit[1];
       get_room_index( ROOM_VNUM_START )->exit[1] = 0;

       /* Let people on the dock know the boat has left without them */
       for ( person = get_room_index( ROOM_VNUM_START )->people;
				 person;
             person = person->next_in_room )
          send_to_char("A ship pulls away from the dock.\n\r", person);

       state = 3;
       break;

    case 3: /* Boring.  Makes the boat ride take a long time */
    case 4:
    case 5:
    case 6:
    case 8:
	 case 9:
		 for ( passenger = boat->people;
             passenger;
             passenger = passenger->next_in_room )
          send_to_char("You begin to feel seasick as"
             " the ferry rocks up and down.\n\r", passenger);


       state++;
       break;

    case 7: /* A sea monster invades the ship! */
		 if ( number_percent() < 33 )
          {
          sea_hag = create_mobile( get_mob_index(7041) );
          char_to_room( sea_hag, boat );
          act(AT_BLUE, "The porthole opens and $n climbs through!", sea_hag, NULL, NULL,
             TO_ROOM );
          }

       state++;
       break;

	 case 10: /* The boat lands at the far pier.  (There is no way to board) */
		 REMOVE_BIT(boat->exit[3]->exit_info, EX_CLOSED|EX_LOCKED);
       boat->exit[3]->to_room = get_room_index( ROOM_VNUM_FINISH );
       for ( passenger = boat->people;
             passenger;
             passenger = passenger->next_in_room )
          send_to_char("You feel a gentle thud as the ferry docks.\n\r"
             "Captain Swing opens the door and motions you to leave.\n\r",
             passenger);

       for ( person = get_room_index( ROOM_VNUM_FINISH )->people;
             person;
             person = person->next_in_room )
			 send_to_char("A ship pulls into the dock.\n\r", person);

       state = 11;
       break;

    case 11: /* Get passengers off the boat so it can return to Midgaard */
       state = 0;
       for ( passenger = boat->people; passenger;
                                         passenger = passenger->next_in_room )
			 {
          /* The boat will not return until all passengers debark */
          state = 11;
			 send_to_char("Captain Swing reminds you to debark promptly.\n\r",
             passenger);
          }

       /* Did the boat return? */
       if (state == 0)
          {
          /* The boat is now back at the start pier */
          boat->exit[3]->to_room = get_room_index( ROOM_VNUM_START );

          /* Add the boat to the dock's list of exits */
          get_room_index( ROOM_VNUM_START )->exit[1] = exit_to_boat;

          for ( person = get_room_index( ROOM_VNUM_START )->people;
                person;
                person = person->next_in_room )
             send_to_char("A ship pulls into the dock.\n\r", person);
          }

		 break;
    }

    return FALSE;
}

bool spec_snake_charm( CHAR_DATA *ch )
{
    CHAR_DATA *victim;
    CHAR_DATA *v_next;
    char *spell;
    int sn;

    if ( ch->position != POS_FIGHTING )
       {
       switch ( number_bits( 3 ) ) {
       case 0:
			 do_order( ch, "all sing charmer" ); /* a chance to get free here */
          break;
       case 1:
          do_order( ch,
             "all chat 'The snake charmer area is pretty cool.  "
				 "I'm getting a lot of experience really fast!" );
          break;
       case 2:
          do_order( ch,
             "all chat 'YES!  I just got 327xp for killing the snake charmer!");
          break;
       case 3:
			 do_order( ch, "all remove dagger" );
          do_order( ch, "all give dagger charmer" );
          break;
       case 4:
          do_order( ch, "all remove sword" );
          do_order( ch, "all give sword charmer" );
          break;
       case 5:
          do_order( ch, "all remove mace" );
          do_order( ch, "all give mace charmer" );
          break;
       case 6:
			 do_order( ch, "all drop all" );
          break;
       case 7:
			 do_order( ch, "all cast 'cure light' charmer" );
          break;
       };

       return TRUE;
       }

    for ( victim = ch->in_room->people; victim != NULL; victim = v_next )
    {
	v_next = victim->next_in_room;
	if ( victim->fighting == ch && number_bits( 2 ) == 0 )
	    break;
    }

    if ( victim == NULL )
	return FALSE;

	act(AT_CYAN, "$n begins playing a new, beautiful song.", ch, NULL, NULL, TO_ROOM );
    spell_charm_person(gsn_charm_person, ch->level, ch, victim );
    if (IS_AFFECTED(victim, AFF_CHARM))
       stop_fighting( victim, TRUE );
//       do_flee( victim, "" );
//       do_rest( victim, "" );
//       do_order( ch, "all rest" );

    return TRUE;
}

bool spec_hunger( CHAR_DATA *ch )
{
    CHAR_DATA *victim;
    OBJ_DATA *obj;
    OBJ_DATA *obj_next;

    if ( !IS_AWAKE(ch) )
	return FALSE;

    for ( victim = ch->in_room->people; victim; victim = victim->next_in_room )
	if ( number_bits( 2 ) == 0 )
	    break;

    if ( victim == NULL )
	return FALSE;

	 if ( !IS_NPC(victim) )
		 victim->pcdata->condition[COND_FULL] = 0;

    for ( obj = victim->carrying; obj; obj = obj_next )
       {
       obj_next = obj->next_content;
       if ( obj->item_type == ITEM_FOOD )
          {
          extract_obj( obj );
          act(AT_RED, "Chomp!", ch, NULL, NULL, TO_ROOM );
          }
       }

	 act(AT_YELLOW, "$n looks a little healthier.", ch, NULL, NULL, TO_ROOM );
    return TRUE;
}

// @@@ ###
/*
   spec_cast_from_list-
      I copied the basic structure from spec_cast_undead.  The first time any
      mobile with this vnum gets pulsed, the cargo for this mobile vnum is
		initialized.  From then on, the mobile will cast spells in it's cargo.
*/

struct string_cargo_struct {
   int ncount;
   char *string_list[16];      /* Allow up to 16 spells / mobile vnum */
};

void check_init_cargo( CHAR_DATA *ch )
{
    struct string_cargo_struct *cls;

    if (ch->pIndexData->cargo == 0)
       {
       /* remove the @ from the mobile's description */
		 char *q = strchr(ch->pIndexData->description, '@');
       int ncount = 0;

       // Allocate the cargo
       ch->pIndexData->cargo = cls = alloc_perm( sizeof (*cls) );

       /* Is there a spell cargo? */
		 if (q)
          {
          /* count the spells */
          while (*q)
             {
				 *q++ = '\0';  /* Break descr after @ into spell text */
             while (isspace(*q))
                q++;
             if (*q)
                {
                if (ncount < 16)
                   cls->string_list[ncount++] = q;
                while (*q && *q != '\n')
//                   {
//                   if (*q == '_') /* fix _ in spell names to a space */
//                      *q = ' ';
                   q++;
//                   }
                }
             }
          }

		 cls->ncount = ncount;
       }
}

bool spec_cast_from_list( CHAR_DATA *ch )
{
    CHAR_DATA *victim;
	 CHAR_DATA *v_next;
    char *spell;
    int sn;
    struct string_cargo_struct *cls;
    int nwhich;

    /* Do we need to initialize cargo? */
    check_init_cargo(ch);

    cls = ch->pIndexData->cargo;
    if ( ch->position != POS_FIGHTING || cls->ncount == 0)
	return FALSE;

    for ( victim = ch->in_room->people; victim != NULL; victim = v_next )
    {
	v_next = victim->next_in_room;
	if ( victim->fighting == ch && number_bits( 2 ) == 0 )
	    break;
    }

    if ( victim == NULL )
	return FALSE;

    // Calculate the spell randomly from mobile's table
	 nwhich = number_range(0, cls->ncount-1);
    sn = skill_lookup( cls->string_list[nwhich] );
    if (sn < 0)
       return FALSE;

    /* If the spell is defensive, the mobile casts on itself.  This allows it
       to heal itself, bless itself, etc.  All other spells are cast on the
       character the mob is fighting.  For offensive spells that is cool.  For
       SELF spells it's not cool, if the mob casts 'STONE SKIN' say it will be
       on the victim, not itself.  This is to allow TELEPORT to teleport the
       victim away, like mages do.  Good idea?  I don't know...
       If the spell has no target, like 'EARTHQUAKE', it is probably a bad idea
		 to have the mobile cast it...  The spell is probably coded to hit all
		 NPCs... */
	 if (skill_table[sn].target == TAR_CHAR_DEFENSIVE)
       (*skill_table[sn].spell_fun) ( sn, ch->level, ch, ch );
    else
       (*skill_table[sn].spell_fun) ( sn, ch->level, ch, victim );
    return TRUE;
}

bool spec_shopkeeper( CHAR_DATA *ch )
{
    OBJ_DATA *inv;
	 struct string_cargo_struct *cls;

    /* Make sure the cargo was safely initialized */
    check_init_cargo(ch);

    if ( !IS_AWAKE(ch) || ch->position != POS_STANDING )
	return FALSE;

    cls = ch->pIndexData->cargo;
    for ( inv = ch->carrying; inv != NULL; inv = inv->next_content )
    {
	 int npercent = (cls->ncount >= 2) ? atoi(cls->string_list[1]) : 5;
	 if (inv->wear_loc == WEAR_NONE && number_percent() < npercent)
       {
       if (cls->ncount)
          act(AT_CYAN, cls->string_list[0], ch, inv, NULL, TO_ROOM );
       else
          act(AT_CYAN, "$n suggests you buy a $p.", ch, inv, NULL, TO_ROOM );
       return TRUE;
       }
    }

    return FALSE;
}

// @@@ Modify to take a list of objects
bool spec_obj_bribe( CHAR_DATA *ch )
{
    OBJ_DATA *inv;
    int nobjvnum;
    struct string_cargo_struct *cls;
    char *p;
    char buf[MAX_STRING_LENGTH];
	 sh_int old_trust;

    /* Make sure the cargo was safely initialized */
	 check_init_cargo(ch);

    if ( ch->position != POS_STANDING )
	return FALSE;

    cls = ch->pIndexData->cargo;
    if (!IS_AWAKE(ch) || cls->ncount < 2)
       return FALSE;

    for (p = cls->string_list[0]; *p; )
    {
       nobjvnum = atoi(p);
		 for ( inv = ch->carrying; inv != NULL; inv = inv->next_content )
       {
       if (inv->pIndexData->vnum == nobjvnum)
          {
          int nwhich = number_range(1, cls->ncount-1);
          char *q;
          extract_obj( inv );
			 q = p = cls->string_list[nwhich];
          if (*p)
             for ( ;; )
                {
                if (*p == ';' || *p == '\0')
						 {
                   memcpy(buf, q, p-q);
                   buf[p-q] = '\0';
                   old_trust = ch->trust;
                   ch->trust = 55;        /* Safe on TFC? */
                   interpret(ch, buf);
                   ch->trust = old_trust;
                   if (*p == '\0')
                      return TRUE;

                   q = p+1;
                   }

                p++;
                }
          }
       }

       while (*p && *p != ' ')
          p++;
       if (*p)
         p++;
    }

	 return FALSE;
}

OBJ_DATA *is_wielding(CHAR_DATA *victim, char *vnum_list)
{
    OBJ_DATA *obj;
    int nobjvnum;

    while (*vnum_list)
    {
       nobjvnum = atoi(vnum_list);
       for ( obj = victim->carrying; obj != NULL; obj = obj->next_content )
		 {
          if ( obj->pIndexData->vnum == nobjvnum && obj->wear_loc != -1)
             return obj;
		 }

       while (*vnum_list && *vnum_list != ' ')
          vnum_list++;
       if (*vnum_list == ' ')
          vnum_list++;
   }

   return 0;
}

bool spec_agg_no_object( CHAR_DATA *ch )
{
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;
    CHAR_DATA *v_next;
    struct string_cargo_struct *cls;
//    OBJ_DATA *obj;
//    int nobjvnum;

    /* Make sure the cargo was safely initialized */
	 check_init_cargo(ch);

    cls = ch->pIndexData->cargo;
    if (!IS_AWAKE(ch) || cls->ncount < 2 || ch->fighting != NULL)
       return FALSE;

//    nobjvnum = atoi(cls->string_list[0]);
    for ( victim = ch->in_room->people; victim != NULL; victim = v_next )
    {
    v_next = victim->next_in_room;

    if (!IS_NPC(victim))
		 if (!is_wielding(victim, cls->string_list[0]))
       {
//	act( "$n screams 'PROTECT THE INNOCENT!!  BANZAI!!",
       act(AT_CYAN, cls->string_list[1], ch, NULL, NULL, TO_ROOM );
	    multi_hit( ch, victim, TYPE_UNDEFINED );
       return TRUE;
       }
    }

    return FALSE;
}

bool spec_follow_mob( CHAR_DATA *ch )
{
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;
    CHAR_DATA *v_next;
    struct string_cargo_struct *cls;
    OBJ_DATA *obj;
    int nmobvnum;

    /* Make sure the cargo was safely initialized */
    check_init_cargo(ch);

	 cls = ch->pIndexData->cargo;
    if (!IS_AWAKE(ch) || cls->ncount < 1 || ch->fighting != NULL || ch->master)
       return FALSE;

    nmobvnum = atoi(cls->string_list[0]);
    for ( victim = ch->in_room->people; victim != NULL; victim = v_next )
    {
    v_next = victim->next_in_room;

	 if (IS_NPC(victim) && victim->pIndexData->vnum == nmobvnum)
       {
       add_follower( ch, victim );
		 break;
       }
    }

    return FALSE;
}

struct char_cargo_struct {
   int timer;
   int state;
};

/*
   Blight-generator:  Makes mobs aggressive.  (Ages...  starts out as some kind
      of low-level and when it gets to high level it changes its descr and
      starts up.--maybe)
*/
// @@@ Modify this to take a list...
bool spec_generator( CHAR_DATA *ch )
{
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;
    CHAR_DATA *v_next;
    struct string_cargo_struct *cls;
	 OBJ_DATA *obj;
    struct char_cargo_struct *mobcargo = ch->cargo;

    /* Make sure the cargo was safely initialized */
    check_init_cargo(ch);

    /* If this mob is freshly popped, initialize the character cargo */
    if (mobcargo == 0)
       {
       mobcargo = ch->cargo = calloc(1, sizeof (struct char_cargo_struct));
       mobcargo->timer = 0;
       mobcargo->state = 0;
		 }

    cls = ch->pIndexData->cargo;
    if (!IS_AWAKE(ch) || cls->ncount < 1 || ch->fighting != NULL)
       return FALSE;

    if (mobcargo->state < cls->ncount-1)
       {
       if (mobcargo->timer++ >= 10)
          {
          char buf[128];
          sprintf(buf, "New state is %d", ++mobcargo->state); // @@@
			 free_string( ch->description );
   	    ch->description = str_dup( buf );
          act(AT_CYAN, "You here the sound of stretching and tearing.",
             ch, NULL, NULL, TO_ROOM );
          ch->level += 30 / cls->ncount + 1;
          ch->max_hit = ch->level * 8 + number_range(
				   ch->level * ch->level / 4,
				   ch->level * ch->level );

          mobcargo->timer = 0;
          }
       }
	 else
       for ( victim = ch->in_room->people; victim != NULL; victim = v_next )
       {
			 v_next = victim->next_in_room;

          if (IS_NPC(victim) && !saves_spell(ch->level, victim))
          {
             victim->act |= ACT_AGGRESSIVE;
             victim->act &= ~ACT_WIMPY; /* Add Super-Wimp if desired */
             victim->alignment = -1000;
          }
       }

    return FALSE;
}

/*
   Blight-fighter:  A cityguard that attacks aggressive mobs.
*/
// @@@ Modify this to take a list...
bool spec_defend_vs_agg( CHAR_DATA *ch )
{
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;
	 CHAR_DATA *v_next;
	 struct string_cargo_struct *cls;
    OBJ_DATA *obj;
    int nobjvnum;

    /* Make sure the cargo was safely initialized */
    check_init_cargo(ch);

    cls = ch->pIndexData->cargo;
    if (!IS_AWAKE(ch) || ch->fighting != NULL)
       return FALSE;

	 for ( victim = ch->in_room->people; victim != NULL; victim = v_next )
    {
    v_next = victim->next_in_room;

    if (victim != ch && IS_NPC(victim) &&
        (victim->act & ACT_AGGRESSIVE) == ACT_AGGRESSIVE)
       {
       if (cls->ncount < 1)
          act(AT_CYAN, "$n screams 'PROTECT THE INNOCENT!!  BANZAI!!", ch, NULL, NULL,
             TO_ROOM);
       else
			 act(AT_CYAN, cls->string_list[0], ch, NULL, NULL, TO_ROOM );

	    multi_hit( ch, victim, TYPE_UNDEFINED );
       return TRUE;
       }
    }

    return FALSE;
}

/*
   Agg-if-object:  Attacks players who have an item in their inventory, say
   alcohol or they have stolen some holy relic.
*/
// @@@ Modify this to take a list...
bool spec_agg_if_object( CHAR_DATA *ch )
{
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;
    CHAR_DATA *v_next;
    struct string_cargo_struct *cls;
    OBJ_DATA *obj;
	 int nobjvnum;

    /* Make sure the cargo was safely initialized */
	 check_init_cargo(ch);

    cls = ch->pIndexData->cargo;
    if (!IS_AWAKE(ch) || cls->ncount < 2 || ch->fighting != NULL)
       return FALSE;

    nobjvnum = atoi(cls->string_list[0]);
    for ( victim = ch->in_room->people; victim != NULL; victim = v_next )
    {
       v_next = victim->next_in_room;

    /* @@@ This needs to be replaced with a no-hassle check */
	 if (!IS_IMMORTAL(victim))
    {
       for ( obj = victim->carrying; obj != NULL; obj = obj->next_content )
       {
       if ( obj->pIndexData->vnum == nobjvnum )
          break;
       }

       if (obj)
       {
       act(AT_CYAN, cls->string_list[1], ch, NULL, NULL, TO_ROOM );
	    multi_hit( ch, victim, TYPE_UNDEFINED );
		 return TRUE;
       }
    }
    }

    return FALSE;
}

bool spec_circuit( CHAR_DATA *ch )
{
    OBJ_DATA *inv;
    int nobjvnum;
	 struct string_cargo_struct *cls;
    struct char_cargo_struct *mobcargo = ch->cargo;

    /* Make sure the cargo was safely initialized */
    check_init_cargo(ch);

    /* If this mob is freshly popped, initialize the character cargo */
    if (mobcargo == 0)
       {
       mobcargo = ch->cargo = calloc(1, sizeof (struct char_cargo_struct));
       mobcargo->timer = 0;
       mobcargo->state = 0;
		 }

    // Is the mob able to perform it's next action?
    cls = ch->pIndexData->cargo;
    if (!IS_AWAKE(ch) || cls->ncount < 1 || ch->fighting != NULL)
       return FALSE;

    // Is it time for a state change?
    // @@@ 10 is hardcoded...
    if (mobcargo->timer++ >= 10)
       {
       interpret(ch, cls->string_list[mobcargo->state]);
		 mobcargo->timer = 0;
       if (mobcargo->state < cls->ncount-1)
          mobcargo->state++;
		 else
          mobcargo->state = 0;

       return TRUE;
       }

    return FALSE;
}

/* This spec-fun lets mobs act the way they do on some muds--Remebering who
   they last fought against and going aggressive against that player.
*/
bool spec_hatred( CHAR_DATA *ch )
{
    char *char_enemy = ch->cargo;   /* Store current hated char in cargo */
    CHAR_DATA *victim, *v_next;

    if (!IS_AWAKE(ch))              /* Mob can't hate while asleep */
       return FALSE;

    if (ch->fighting)               /* Is the mobile fighting? */
		 {
		 /* If the mobile is fighting a player, check and see if it is a player
          the mobile doesn't currently hate. */
       if (!IS_NPC(ch->fighting) &&
			 (!char_enemy ||
           str_cmp(ch->fighting->name, char_enemy) == TRUE))
          {
          free_string( char_enemy );
          ch->cargo = str_dup( ch->fighting->name );
          }
       }

	 /* The mobile is not fighting.  Does the mobile hate anyone? */
    else if (char_enemy)
       {
       /* Check the room for the player the mobile hates: */
       for ( victim = ch->in_room->people; victim != NULL; victim = v_next )
         {
         v_next = victim->next_in_room;

         /* @@@ A no-hassle check is needed here. */
         if (!IS_NPC(victim) && str_cmp(victim->name, char_enemy) == FALSE)
            {
				act( AT_CYAN,"$n screams with rage!", ch, NULL, NULL, TO_ROOM );
				multi_hit( ch, victim, TYPE_UNDEFINED );
            return TRUE;
            }
         }
       }

    return FALSE;
}

/*
 * Psionicist spec_fun by Thelonius for EnvyMud.
 */
bool spec_cast_psionicist( CHAR_DATA *ch )
{
    CHAR_DATA *victim;
    char      *spell;
    int        sn;

    if ( ch->position != POS_FIGHTING )
        return FALSE;

	 for ( victim = ch->in_room->people; victim; victim = victim->next_in_room )
    {
        if ( victim->fighting == ch && can_see( ch, victim )
		 && number_bits( 2 ) == 0 )
            break;
    }

    if ( !victim )
        return FALSE;

    for ( ; ; )
    {
        int min_level;

		  switch ( number_bits( 4 ) )
		  {
		  case  0: min_level =  0; spell = "mind thrust";          break;
		  case  1: min_level =  4; spell = "psychic drain";        break;
		  case  2: min_level =  6; spell = "agitation";            break;
		  case  3: min_level =  8; spell = "psychic crush";        break;
		  case  4: min_level =  9; spell = "project force";        break;
		  case  5: min_level = 13; spell = "ego whip";             break;
		  case  6: min_level = 14; spell = "energy drain";         break;
		  case  7:
		  case  8: min_level = 17; spell = "psionic blast";        break;
		  case  9: min_level = 20; spell = "detonate";             break;
	case 10: min_level = 27; spell = "disintegrate";         break;
		  default: min_level = 25; spell = "ultrablast";           break;
		  }

		  if ( ch->level >= min_level )
				break;
	 }

	 if ( ( sn = skill_lookup( spell ) ) < 0 )
		  return FALSE;
	 (*skill_table[sn].spell_fun) ( sn, ch->level, ch, victim );
	 return TRUE;
}

#if 0
/* This current version of spec_animated_dead just emotes.  It could be
	enhanced in all kinds of ways...  Tying in HPs with the charm-master,
	turning on the charm master, etc.  When the charm wears off, the mobile
	turns to dust.
*/
bool spec_animated_dead( CHAR_DATA *ch )
{
	if ( !IS_AFFECTED(ch, AFF_CHARM) )
		{
		act(AT_CYAN, "$n crumbles into dust.", ch, NULL, NULL, TO_ROOM );

		stop_fighting( ch, TRUE );
		/* Uncomment the following line if you have mobprogs */
		 mprog_death_trigger( ch );

		extract_char( ch, TRUE );

		return TRUE;
		}

	switch ( number_bits(5) )
	{
	case 0:
		act(AT_BLOOD, "Another gobbet of lose flesh falls off $n.", ch, NULL, NULL, TO_ROOM );
		return TRUE;
	case 1:
		act(AT_BLOOD, "$n lets loose with a blood-curdling wail.", ch, NULL, NULL, TO_ROOM );
		return TRUE;
	case 2:
		act(AT_BLOOD, "$n's odor is getting very bad.  You gag.", ch, NULL, NULL, TO_ROOM );
		return TRUE;
	default:
		break;
	}

	return FALSE;
}
#else
bool spec_animated_dead( CHAR_DATA *ch )
{
	act(AT_GREY, "$n crumbles into dust.", ch, NULL, NULL, TO_ROOM );

	stop_fighting( ch, TRUE );
	/* Uncomment the following line if you have mobprogs */
	 mprog_death_trigger( ch );

	extract_char( ch, TRUE );

	return TRUE;
}
#endif
