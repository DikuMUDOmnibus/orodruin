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


void echo_to_all( sh_int AT_COLOR, char *argument )
{
	 DESCRIPTOR_DATA *d;

	 if ( !argument || argument[0] == '\0' )
	return;

	 for ( d = descriptor_list; d; d = d->next )
	 {
	if ( d->connected == CON_PLAYING )
	{
		 set_char_color( AT_COLOR, d->character );
		 send_to_char( argument, d->character );
		 send_to_char( "\n\r",   d->character );
	}
	 }
	 return;
}

void do_balzhur( CHAR_DATA *ch, char *argument )
{
	 char arg[MAX_INPUT_LENGTH];
	 char buf[MAX_STRING_LENGTH];
	 CHAR_DATA *victim;
	 OBJ_DATA *obj;
	 OBJ_DATA *obj_next;
	 int sn;

	 argument = one_argument( argument, arg );

	 if ( arg[0] == '\0' )
	 {
	send_to_char( "Who is deserving of such a fate?\n\r", ch );
	return;
	 }

	 if ( ( victim = get_char_world( ch, arg ) ) == NULL )
	 {
	send_to_char( "They aren't playing.\n\r", ch);
	return;
	 }

	 if ( IS_NPC(victim) )
	 {
	send_to_char( "Not on NPC's.\n\r", ch );
	return;
	 }

	 if ( victim->level >= get_trust( ch ) )
	 {
	send_to_char( "I wouldn't even think of that if I were you...\n\r", ch );
	return;
	 }

	send_to_char( "You summon the demon Balzhur to wreak your wrath!\n\r", ch );
	send_to_char( "Balzhur sneers at you evilly, then vanishes in a puff of smoke.\n\r", ch );
	send_to_char( "You hear an ungodly sound in the distance that makes your blood run cold!\n\r", victim );
	sprintf( buf, "Balzhur screams, 'You are MINE %s!!!'", victim->name );
	echo_to_all( AT_GOSSIP, buf );
	victim->level    = 2;
	victim->trust	 = 0;
	victim->exp      = 2000;
	victim->max_hit  = 10;
	victim->max_mana = 100;
	victim->max_move = 100;
	for ( sn = 0; sn < MAX_SKILL; sn++ )
		 victim->pcdata->learned[sn] = 0;
	victim->practice = 0;
	victim->hit      = victim->max_hit;
	victim->mana     = victim->max_mana;
	victim->move     = victim->max_move;
	send_to_char( "The vile demon Balzhur appears before you and smites you down!!\n\r", victim );
	advance_level( victim );
	set_char_color( AT_WHITE, victim );
	send_to_char( "You awake after a long period of time...\n\r", victim );
	extract_char( victim, FALSE );
	 return;
}

void do_restrict( CHAR_DATA *ch, char *argument )
{
	 char arg[MAX_INPUT_LENGTH];
	 char arg2[MAX_INPUT_LENGTH];
	 char buf[MAX_STRING_LENGTH];
	 sh_int level;
	 int cmd;
	 bool found;

	 found = FALSE;

	 argument = one_argument( argument, arg );
	 if ( arg[0] == '\0' )
	 {
		send_to_char( "Restrict which command?\n\r", ch );
		return;
	 }

	 argument = one_argument ( argument, arg2 );
	 if ( arg2[0] == '\0' )
		level = ch->level;
	 else
		level = atoi( arg2 );

	 for ( cmd = 0; cmd_table[cmd].name[0] != '\0'; cmd++ )
	 {
	if ( arg[0] == cmd_table[cmd].name[0]
	&&   !str_prefix( arg, cmd_table[cmd].name )
	&&   cmd_table[cmd].level <= ch->level )
	{
		 found = TRUE;
		 break;
	}
	 }

	 if ( found )
	 {
	cmd_table[cmd].level = level;
	sprintf( buf, "You restrict %s to level %d\n\r",
		cmd_table[cmd].name, level );
	send_to_char( buf, ch );
	sprintf( buf, "Log: %s restricting %s to level %d",
		  ch->name, cmd_table[cmd].name, level );
	NOTIFY( buf, LEVEL_IMMORTAL, WIZ_NOTIFY_LOG );
	log_string( buf );
	 }

	 return;
}

void do_aset( CHAR_DATA *ch, char *argument )
	/* aset by Garion */

{
	 char 	arg1 [MAX_INPUT_LENGTH];
	 char 	arg2 [MAX_INPUT_LENGTH];
	 char 	arg3 [MAX_INPUT_LENGTH];
	 OBJ_DATA	*obj;
	 AFFECT_DATA *paf;
	 int		value;


	 smash_tilde( argument );
	 argument = one_argument( argument, arg1 );
	 argument = one_argument( argument, arg2 );
	 strcpy( arg3, argument );

	 if ( arg1[0] == '\0' || arg2[0] == '\0' || arg3[0] == '\0' )
	 {
	send_to_char("Syntax: aset <object> <field> <value>.\n\r", ch);
	send_to_char("\n\r", ch);
	send_to_char("Field being one of:\n\r", ch);
	send_to_char("\n\r", ch);
	send_to_char("  str int wis dex con hitroll damroll ac\n\r", ch);
	send_to_char("  hp mana move para petri spell breath rod\n\r", ch);
	return;
	 }

	 if ( ( obj = get_obj_world( ch, arg1 ) ) == NULL)
	 {
	send_to_char("Aint no such thing Jack.\n\r", ch);
	return;
	 }

	 if ( !is_number( arg3 ) )
	 {
	send_to_char("Value must be numeric.\n\r", ch);
	return;
	 }

	 value = atoi( arg3 );
	 if ( ( ch->level < MAX_LEVEL ) && ( ( value > 20 ) || ( value < -20 ) ) )
	/* value restrictions for safety... assume the IMP knows better :) */
	 {
	send_to_char("Value range is -20 to 20.\n\r", ch);
	return;
	 }

	 if ( affect_free == NULL )
	 {
	paf = alloc_perm( sizeof( *paf ) );
	 }
	 else
	 {
	paf = affect_free;
	affect_free = affect_free->next;
	 }

	 paf->type		= value;
	 paf->duration	= -1;
	 paf->modifier	= value;
	 paf->bitvector	= 0;
	 paf->next		= obj->affected;

	/* here we go */

	 if ( !str_cmp( arg2, "str" ) )
	 {
	paf->location 	= APPLY_STR;
	obj->affected 	= paf;
	return;
	 }

	 if ( !str_cmp( arg2, "dex" ) )
	 {
	paf->location	= APPLY_DEX;
	obj->affected	= paf;
	return;
	 }

	 if ( !str_cmp( arg2, "int" ) )
	 {
	paf->location	= APPLY_INT;
	obj->affected	= paf;
	return;
	 }

	 if ( !str_cmp( arg2, "wis" ) )
	 {
	paf->location	= APPLY_WIS;
	obj->affected	= paf;
	return;
	 }

	 if ( !str_cmp( arg2, "con" ) )
	 {
	paf->location	= APPLY_CON;
	obj->affected	= paf;
	return;
	 }

	 if ( !str_cmp( arg2, "mana" ) )
	 {
	paf->location 	= APPLY_MANA;
	obj->affected	= paf;
	return;
	 }

	 if ( !str_cmp( arg2, "hp" ) )
	 {
	paf->location	= APPLY_HIT;
	obj->affected	= paf;
	return;
	 }

	 if ( !str_cmp( arg2, "move" ) )
	 {
	paf->location	= APPLY_MOVE;
	obj->affected	= paf;
	return;
	 }

	 if ( !str_cmp( arg2, "ac" ) )
	 {
	paf->location 	= APPLY_AC;
	obj->affected	= paf;
	return;
	 }

	 if ( !str_cmp( arg2, "hitroll" ) )
	 {
	paf->location	= APPLY_HITROLL;
	obj->affected	= paf;
	return;
	 }

	 if ( !str_cmp( arg2, "damroll" ) )
	 {
	paf->location	= APPLY_DAMROLL;
	obj->affected 	= paf;
	return;
	 }

	 if ( !str_cmp( arg2, "para" ) )
	 {
	paf->location	= APPLY_SAVING_PARA;
	obj->affected	= paf;
	return;
	 }

	 if ( !str_cmp( arg2, "rod" ) )
	 {
	paf->location	= APPLY_SAVING_ROD;
	obj->affected	= paf;
	return;
	  }

	 if ( !str_cmp( arg2, "petri" ) )
	 {
	paf->location	= APPLY_SAVING_PETRI;
	obj->affected	= paf;
	return;
	 }

	 if ( !str_cmp( arg2, "breath" ) )
	 {
	paf->location	= APPLY_SAVING_BREATH;
	obj->affected	= paf;
	return;
	 }

	 if ( !str_cmp( arg2, "spell" ) )
	 {
	paf->location	= APPLY_SAVING_SPELL;
	obj->affected	= paf;
	return;
	 }

	 do_aset( ch, "" );
	 return;
}

void do_outfit ( CHAR_DATA *ch, char *argument )
{
	 char arg[MAX_INPUT_LENGTH];
	 CHAR_DATA *victim;
	 OBJ_DATA *obj;

	 argument = one_argument( argument, arg );

	 if ( arg[0] == '\0' )
	 {
	send_to_char( "Outfit whom?\n\r", ch );
	return;
	 }

	 if  ( (victim = get_char_world(ch, arg) ) == NULL )
	 {
	send_to_char("Target not found.\n\r",ch);
	return;
	 }

	 if (victim->level > 5 || IS_NPC(victim))
	 {
	send_to_char("This character is not a newbie!\n\r",ch);
	return;
	 }

	 if ( ( obj = get_eq_char( victim, WEAR_LIGHT ) ) == NULL )
	 {
		  obj = create_object( get_obj_index(OBJ_VNUM_SCHOOL_BANNER), 1);
	obj->cost = 0;
		  obj_to_char( obj, victim );
		  equip_char( victim, obj, WEAR_LIGHT );
	 }

	 if ( ( obj = get_eq_char( victim, WEAR_BODY ) ) == NULL )
	 {
	obj = create_object( get_obj_index(OBJ_VNUM_SCHOOL_VEST), 1);
	obj->cost = 0;
		  obj_to_char( obj, victim );
		  equip_char( victim, obj, WEAR_BODY );
	 }

	 if ( ( obj = get_eq_char( victim, WEAR_SHIELD ) ) == NULL )
	 {
		  obj = create_object( get_obj_index(OBJ_VNUM_SCHOOL_SHIELD),1 );
	obj->cost = 0;
		  obj_to_char( obj, victim );
		  equip_char( victim, obj, WEAR_SHIELD );
	 }

	 if ( ( obj = get_eq_char( victim, WEAR_WIELD ) ) == NULL )
	 {
		  obj = create_object( get_obj_index(class_table[victim->class].weapon), 1);
		  obj_to_char( obj, victim );
		  equip_char( victim, obj, WEAR_WIELD );
	 }

	 act(AT_YELLOW, "You have been equipped by $n.\n\r",ch, NULL, victim, TO_VICT);
}

void do_logcha( CHAR_DATA *ch, char *argument )
{
	 char arg1 [MAX_INPUT_LENGTH];
	 char buf  [MAX_INPUT_LENGTH];

	 smash_tilde( argument );
	 argument = one_argument( argument, arg1 );

	 if ( arg1[0] == '\0' )
	 {
		 if ( IS_SET(ch->act, WIZ_NOTIFY_LOG) )
		 {
			 send_to_char( "Log channel is now OFF.\n\r", ch );
			 REMOVE_BIT(ch->act, WIZ_NOTIFY);
			 REMOVE_BIT(ch->act, WIZ_NOTIFY_LOG);
			 return;
		 }
		 else
		 {
			 send_to_char( "Log channel is now ON.\n\r", ch );
			 SET_BIT(ch->act, WIZ_NOTIFY);
			 SET_BIT(ch->act, WIZ_NOTIFY_LOG);
			 return;
		 }
	 }
	 else
	 {
		send_to_char( "Eh?  I dont understand that!\n\r", ch );
		return;
	 }
}

void do_monitor( CHAR_DATA *ch, char *argument )
{
	 char arg1 [MAX_INPUT_LENGTH];
	 char buf  [MAX_INPUT_LENGTH];

	 smash_tilde( argument );
	 argument = one_argument( argument, arg1 );

	 if ( arg1[0] == '\0' )
	 {
		 if ( IS_SET(ch->act, WIZ_NOTIFY_MONITOR) )
		 {
			 send_to_char( "Monitor is now OFF.\n\r", ch );
			 REMOVE_BIT(ch->act, WIZ_NOTIFY);
			 REMOVE_BIT(ch->act, WIZ_NOTIFY_MONITOR);
			 return;
		 }
		 else
		 {
			 send_to_char( "Monitor is now ON.\n\r", ch );
			 SET_BIT(ch->act, WIZ_NOTIFY);
			 SET_BIT(ch->act, WIZ_NOTIFY_MONITOR);
			 return;
		 }
	 }
	 else
	 {
		send_to_char( "Eh?  I dont understand that!\n\r", ch );
		return;
	 }
}

void do_wizhelp( CHAR_DATA *ch, char *argument )
{
	 char buf[MAX_STRING_LENGTH];
	 char buf1[MAX_STRING_LENGTH];
	 int cmd;
	 int col;

	 buf1[0] = '\0';
	 col = 0;
	 for ( cmd = 0; cmd_table[cmd].name[0] != '\0'; cmd++ )
	 {
		  if ( cmd_table[cmd].level >= LEVEL_HERO
		  &&   cmd_table[cmd].level <= get_trust( ch ) )
	{
		 sprintf( buf, "%-12s", cmd_table[cmd].name );
		 strcat( buf1, buf );
		 if ( ++col % 6 == 0 )
		strcat( buf1, "\n\r" );
	}
	 }

	 if ( col % 6 != 0 )
	strcat( buf1, "\n\r" );
	 send_to_char( buf1, ch );
	 return;
}



void do_bamfin( CHAR_DATA *ch, char *argument )
{
	 char buf[MAX_STRING_LENGTH];
	 if ( !IS_NPC(ch) )
	 {
	smash_tilde( argument );
	if (argument[0] == '\0')
	{
		 sprintf(buf,"Your bamfin is %s\n\r",ch->pcdata->bamfin);
		 send_to_char(buf,ch);
		 return;
	}
	free_string( ch->pcdata->bamfin );
	ch->pcdata->bamfin = str_dup( argument );
	sprintf(buf,"Your bamfin is now %s\n\r",ch->pcdata->bamfin);
	send_to_char(buf,ch);
	 }
	 return;
}



void do_bamfout( CHAR_DATA *ch, char *argument )
{
	 char buf[MAX_STRING_LENGTH];
	 if ( !IS_NPC(ch) )
	 {
	smash_tilde( argument );
	if (argument[0] == '\0')
	  {
			sprintf(buf,"Your bamfout is %s\n\r",ch->pcdata->bamfout);
			send_to_char(buf,ch);
			return;
	  }
	free_string( ch->pcdata->bamfout );
	ch->pcdata->bamfout = str_dup( argument );
	sprintf(buf,"Your bamfout is now %s\n\r",ch->pcdata->bamfout);
	send_to_char(buf,ch);
	 }
	 return;
}



void do_deny( CHAR_DATA *ch, char *argument )
{
	 char arg[MAX_INPUT_LENGTH];
	 CHAR_DATA *victim;

	 one_argument( argument, arg );
	 if ( arg[0] == '\0' )
	 {
	send_to_char( "Deny whom?\n\r", ch );
	return;
	 }

	 if ( ( victim = get_char_world( ch, arg ) ) == NULL )
	 {
	send_to_char( "They aren't here.\n\r", ch );
	return;
	 }

	 if ( IS_NPC(victim) )
	 {
	send_to_char( "Not on NPC's.\n\r", ch );
	return;
	 }

	 if ( get_trust( victim ) >= get_trust( ch ) )
	 {
	send_to_char( "You failed.\n\r", ch );
	return;
	 }

	 SET_BIT(victim->act, PLR_DENY);
	 send_to_char( "You are denied access!\n\r", victim );
	 send_to_char( "OK.\n\r", ch );
	 do_quit( victim, "" );

	 return;
}



void do_disconnect( CHAR_DATA *ch, char *argument )
{
	 char arg[MAX_INPUT_LENGTH];
	 DESCRIPTOR_DATA *d;
	 CHAR_DATA *victim;

	 one_argument( argument, arg );
	 if ( arg[0] == '\0' )
	 {
	send_to_char( "Disconnect whom?\n\r", ch );
	return;
	 }

	 if ( ( victim = get_char_world( ch, arg ) ) == NULL )
	 {
	send_to_char( "They aren't here.\n\r", ch );
	return;
	 }

	 if ( victim->desc == NULL )
	 {
	act(AT_GREY, "$N doesn't have a descriptor.", ch, NULL, victim, TO_CHAR );
	return;
	 }

	 for ( d = descriptor_list; d != NULL; d = d->next )
	 {
	if ( d == victim->desc )
	{
	    close_socket( d );
		 send_to_char( "Ok.\n\r", ch );
	    return;
	}
    }

    bug( "Do_disconnect: desc not found.", 0 );
	 send_to_char( "Descriptor not found!\n\r", ch );
	 return;
}



void do_pardon( CHAR_DATA *ch, char *argument )
{
	 char arg1[MAX_INPUT_LENGTH];
	 char arg2[MAX_INPUT_LENGTH];
	 CHAR_DATA *victim;

	 argument = one_argument( argument, arg1 );
	 argument = one_argument( argument, arg2 );

	 if ( arg1[0] == '\0' || arg2[0] == '\0' )
	 {
	send_to_char( "Syntax: pardon <character> <killer|thief>.\n\r", ch );
	return;
	 }

	 if ( ( victim = get_char_world( ch, arg1 ) ) == NULL )
	 {
	send_to_char( "They aren't here.\n\r", ch );
	return;
	 }

	 if ( IS_NPC(victim) )
	 {
	send_to_char( "Not on NPC's.\n\r", ch );
	return;
	 }

	 if ( !str_cmp( arg2, "killer" ) )
	 {
	if ( IS_SET(victim->act, PLR_KILLER) )
	{
		 REMOVE_BIT( victim->act, PLR_KILLER );
		 send_to_char( "Killer flag removed.\n\r", ch );
		 send_to_char( "You are no longer a KILLER.\n\r", victim );
	}
	return;
	 }

	 if ( !str_cmp( arg2, "thief" ) )
	 {
	if ( IS_SET(victim->act, PLR_THIEF) )
	{
		 REMOVE_BIT( victim->act, PLR_THIEF );
		 send_to_char( "Thief flag removed.\n\r", ch );
		 send_to_char( "You are no longer a THIEF.\n\r", victim );
	}
	return;
	 }

	 send_to_char( "Syntax: pardon <character> <killer|thief>.\n\r", ch );
	 return;
}

void do_echo( CHAR_DATA *ch, char *argument )
{
	 char arg[MAX_INPUT_LENGTH];
	 sh_int AT_COLOR;

	 if ( argument[0] == '\0' )
	 {
	send_to_char( "Echo what?\n\r", ch );
	return;
	 }

	 one_argument( argument, arg );
	 if ( !str_cmp( arg, "thirsty" ) )
		AT_COLOR = AT_THIRSTY;
	 else
	 if ( !str_cmp( arg, "log" ) )
		AT_COLOR = AT_LOG;
	 else
	 if ( !str_cmp( arg, "plain" ) )
		AT_COLOR = AT_PLAIN;
	 else
	 if ( !str_cmp( arg, "hungry" ) )
		AT_COLOR = AT_HUNGRY;
	 else
	 {
		echo_to_all( AT_IMMORT, argument );
		return;
	 }

	 argument = one_argument( argument, arg );
	 echo_to_all( AT_COLOR, argument );
	 return;
}

void echo_to_room( sh_int AT_COLOR, ROOM_INDEX_DATA *room, char *argument )
{
	 CHAR_DATA *vic;

	 for ( vic = room->people; vic; vic = vic->next_in_room )
	 {
	set_char_color( AT_COLOR, vic );
	send_to_char( argument, vic );
	send_to_char( "\n\r",   vic );
	 }
}

void do_recho( CHAR_DATA *ch, char *argument )
{
	 char arg[MAX_INPUT_LENGTH];
	 char buf[MAX_STRING_LENGTH];
	 sh_int AT_COLOR;

	 if ( argument[0] == '\0' )
	 {
	send_to_char( "Recho what?\n\r", ch );
	return;
	 }

	 one_argument( argument, arg );
	 if ( !str_cmp( arg, "Thoric" )
	 ||   !str_cmp( arg, "Caine" )
	 ||   !str_cmp( arg, "Strahd" ) )
	 {
	sprintf( buf, "I don't think %s would like that!\n\r", arg );
	send_to_char( buf, ch );
	return;
	 }
	 else
	 if ( !str_cmp( arg, "thirsty" ) )
		AT_COLOR = AT_THIRSTY;
	 else
	 if ( !str_cmp( arg, "log" ) )
		AT_COLOR = AT_LOG;
	 else
	 if ( !str_cmp( arg, "plain" ) )
		AT_COLOR = AT_PLAIN;
	 else
	 if ( !str_cmp( arg, "hungry" ) )
		AT_COLOR = AT_HUNGRY;
	 else
	 {
		echo_to_room( AT_IMMORT, ch->in_room, argument );
		return;
	 }

	 argument = one_argument( argument, arg );
	 echo_to_room( AT_COLOR, ch->in_room, argument );
	 return;
}


void do_pecho( CHAR_DATA *ch, char *argument )
{
	 char arg[MAX_INPUT_LENGTH];
	 CHAR_DATA *victim;

	 argument = one_argument(argument, arg);

	 if ( argument[0] == '\0' || arg[0] == '\0' )
	 {
	send_to_char("Personal echo what?\n\r", ch);
	return;
	 }

	 if  ( (victim = get_char_world(ch, arg) ) == NULL )
	 {
	send_to_char("Target not found.\n\r",ch);
	return;
	 }

	 if (get_trust(victim) >= get_trust(ch) && get_trust(ch) != MAX_LEVEL)
		  send_to_char( "personal> ",victim);

ansi_color( BLUE, ch );
	 send_to_char(argument,victim);
	 send_to_char("\n\r",victim);
	 send_to_char( "personal> ",ch);
	 send_to_char(argument,ch);
	 send_to_char("\n\r",ch);
ansi_color( GREY, ch );
}

ROOM_INDEX_DATA *find_location( CHAR_DATA *ch, char *arg )
{
	 CHAR_DATA *victim;
	 OBJ_DATA *obj;

	 if ( is_number(arg) )
	return get_room_index( atoi( arg ) );

    if ( ( victim = get_char_world( ch, arg ) ) != NULL )
	return victim->in_room;

	 if ( ( obj = get_obj_world( ch, arg ) ) != NULL )
	return obj->in_room;

    return NULL;
}



void do_transfer( CHAR_DATA *ch, char *argument )
{
	 char arg1[MAX_INPUT_LENGTH];
	 char arg2[MAX_INPUT_LENGTH];
	 ROOM_INDEX_DATA *location;
	 DESCRIPTOR_DATA *d;
	 CHAR_DATA *victim;

	 argument = one_argument( argument, arg1 );
	 argument = one_argument( argument, arg2 );

	 if ( arg1[0] == '\0' )
	 {
	send_to_char( "Transfer whom (and where)?\n\r", ch );
	return;
	 }

	 if ( !str_cmp( arg1, "all" ) )
	 {
	for ( d = descriptor_list; d != NULL; d = d->next )
	{
		 if ( d->connected == CON_PLAYING
		 &&   d->character != ch
		 &&   d->character->in_room != NULL
		 &&   can_see( ch, d->character ) )
		 {
		char buf[MAX_STRING_LENGTH];
		sprintf( buf, "%s %s", d->character->name, arg2 );
		do_transfer( ch, buf );
		 }
	}
	return;
	 }

	 /*
	  * Thanks to Grodyn for the optional location parameter.
	  */
	 if ( arg2[0] == '\0' )
	 {
	location = ch->in_room;
	 }
	 else
	 {
	if ( ( location = find_location( ch, arg2 ) ) == NULL )
	{
		 send_to_char( "No such location.\n\r", ch );
		 return;
	}

	if ( room_is_private( location ) )
	{
		 send_to_char( "That room is private right now.\n\r", ch );
		 return;
	}
	 }

	 if ( ( victim = get_char_world( ch, arg1 ) ) == NULL )
	 {
	send_to_char( "They aren't here.\n\r", ch );
	return;
	 }

	 if ( victim->in_room == NULL )
	 {
	send_to_char( "They are in limbo.\n\r", ch );
	return;
	 }

	 if ( victim->fighting != NULL )
	stop_fighting( victim, TRUE );
	 act(AT_BLUE, "$n has been transferred.  Poor soul.", victim, NULL, NULL, TO_ROOM );
	 char_from_room( victim );
	 char_to_room( victim, location );
	 act(AT_BLUE, "$n appears in a flash of bluish light.", victim, NULL, NULL, TO_ROOM );
	 if ( ch != victim )
{
		  act(AT_BLUE, "$n has transferred you.", ch, NULL, victim, TO_VICT );
}
	 do_look( victim, "auto" );
	 send_to_char( "Ok.\n\r", ch );
}



void do_at( CHAR_DATA *ch, char *argument )
{
	 char arg[MAX_INPUT_LENGTH];
	 ROOM_INDEX_DATA *location;
	 ROOM_INDEX_DATA *original;
	 CHAR_DATA *wch;

	 argument = one_argument( argument, arg );

	 if ( arg[0] == '\0' || argument[0] == '\0' )
	 {
	send_to_char( "At where what?\n\r", ch );
	return;
	 }

	 if ( ( location = find_location( ch, arg ) ) == NULL )
	 {
	send_to_char( "No such location.\n\r", ch );
	return;
	 }

	 if ( room_is_private( location ) )
	 {
	send_to_char( "That room is private right now.\n\r", ch );
	return;
    }

    original = ch->in_room;
	 char_from_room( ch );
    char_to_room( ch, location );
    interpret( ch, argument );

    /*
	  * See if 'ch' still exists before continuing!
     * Handles 'at XXXX quit' case.
     */
    for ( wch = char_list; wch != NULL; wch = wch->next )
    {
	if ( wch == ch )
	{
		 char_from_room( ch );
	    char_to_room( ch, original );
	    break;
	}
    }

	 return;
}



void do_goto( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    ROOM_INDEX_DATA *location;

    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	send_to_char( "Goto where?\n\r", ch );
	return;
    }

    if ( ( location = find_location( ch, arg ) ) == NULL )
	 {
	send_to_char( "No such location.\n\r", ch );
	return;
    }

    if ( room_is_private( location ) )
    {
	send_to_char( "That room is private right now.\n\r", ch );
	return;
    }

    if ( ch->fighting != NULL )
	stop_fighting( ch, TRUE );
    if ( !IS_SET(ch->act, PLR_WIZINVIS) )
    {
	act(AT_GREY, "$n $T.", ch, NULL,
	    (ch->pcdata != NULL && ch->pcdata->bamfout[0] != '\0')
	    ? ch->pcdata->bamfout : "leaves in a swirling mist",  TO_ROOM );
    }

    char_from_room( ch );
	 char_to_room( ch, location );

    if ( !IS_SET(ch->act, PLR_WIZINVIS) )
	 {
	act(AT_GREY, "$n $T.", ch, NULL,
	    (ch->pcdata != NULL && ch->pcdata->bamfin[0] != '\0')
	    ? ch->pcdata->bamfin : "appears in a swirling mist", TO_ROOM );
    }

    do_look( ch, "auto" );
    return;
}



void do_rstat( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char buf1[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    ROOM_INDEX_DATA *location;
    OBJ_DATA *obj;
    CHAR_DATA *rch;
    int door;

    one_argument( argument, arg );
	 location = ( arg[0] == '\0' ) ? ch->in_room : find_location( ch, arg );
	 if ( location == NULL )
    {
	send_to_char( "No such location.\n\r", ch );
	return;
    }

    if ( ch->in_room != location && room_is_private( location ) )
    {
	send_to_char( "That room is private right now.\n\r", ch );
	return;
    }

	 buf1[0] = '\0';

    sprintf( buf, "Name: '%s.'\n\rArea: '%s'.\n\r",
	location->name,
	location->area->name );
    strcat( buf1, buf );

    sprintf( buf,
	"Vnum: %d.  Sector: %d.  Light: %d.\n\r",
	location->vnum,
	location->sector_type,
	location->light );
	 strcat( buf1, buf );

    sprintf( buf,
	"Room flags: %d.\n\rDescription:\n\r%s",
	location->room_flags,
	location->description );
    strcat( buf1, buf );

    if ( location->extra_descr != NULL )
    {
	EXTRA_DESCR_DATA *ed;

	strcat( buf1, "Extra description keywords: '" );
	for ( ed = location->extra_descr; ed; ed = ed->next )
	{
	    strcat( buf1, ed->keyword );
	    if ( ed->next != NULL )
		strcat( buf1, " " );
	}
	strcat( buf1, "'.\n\r" );
    }

    strcat( buf1, "Characters:" );
    for ( rch = location->people; rch; rch = rch->next_in_room )
	 {
ansi_color( BOLD, ch );
ansi_color( PURPLE, ch );
		  strcat( buf1, " " );
	one_argument( rch->name, buf );
	strcat( buf1, buf );
ansi_color( NTEXT, ch );
ansi_color( GREY, ch );
    }

    strcat( buf1, ".\n\rObjects:   " );
    for ( obj = location->contents; obj; obj = obj->next_content )
	 {
ansi_color( BOLD, ch );
ansi_color( RED, ch );
        strcat( buf1, " " );
	one_argument( obj->name, buf );
	strcat( buf1, buf );
ansi_color( NTEXT, ch );
ansi_color( GREY, ch );
    }
    strcat( buf1, ".\n\r" );

    for ( door = 0; door <= 5; door++ )
	 {
	EXIT_DATA *pexit;

	if ( ( pexit = location->exit[door] ) != NULL )
	{
		 sprintf( buf,
		"Door: %d.  To: %d.  Key: %d.  Exit flags: %d.\n\rKeyword: '%s'.  Description: %s",

		door,
		pexit->to_room != NULL ? pexit->to_room->vnum : 0,
	    	pexit->key,
	    	pexit->exit_info,
			pexit->keyword,
	    	pexit->description[0] != '\0'
		    ? pexit->description : "(none).\n\r" );
	    strcat( buf1, buf );
	}
    }

    send_to_char( buf1, ch );
    return;
}



void do_ostat( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char buf1[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    AFFECT_DATA *paf;
	 OBJ_DATA *obj;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
	 {
	send_to_char( "Ostat what?\n\r", ch );
	return;
    }

    buf1[0] = '\0';

    if ( ( obj = get_obj_world( ch, arg ) ) == NULL )
	 {
	send_to_char( "Nothing like that in hell, earth, or heaven.\n\r", ch );
	return;
    }

    sprintf( buf, "Name: %s.\n\r",
	obj->name );
    strcat( buf1, buf );

    sprintf( buf, "Vnum: %d.  Type: %s.\n\r",
	obj->pIndexData->vnum, item_type_name( obj ) );
    strcat( buf1, buf );

	 sprintf( buf, "Short description: %s.\n\rLong description: %s\n\r",
	obj->short_descr, obj->description );
    strcat( buf1, buf );

    sprintf( buf, "Wear bits: %d.  Extra bits: %s.\n\r",
	obj->wear_flags, extra_bit_name( obj->extra_flags ) );
    strcat( buf1, buf );

    sprintf( buf, "Number: %d/%d.  Weight: %d/%d.\n\r",
	1,           get_obj_number( obj ),
	obj->weight, get_obj_weight( obj ) );
    strcat( buf1, buf );

	 sprintf( buf, "Cost: %d.  Timer: %d.  Level: %d.\n\r",
	obj->cost, obj->timer, obj->level );
	 strcat( buf1, buf );

    sprintf( buf,
	"In room: %d.  In object: %s.  Carried by: %s.  Wear_loc: %d.\n\r",
	obj->in_room    == NULL    ?        0 : obj->in_room->vnum,
	obj->in_obj     == NULL    ? "(none)" : obj->in_obj->short_descr,
	obj->carried_by == NULL    ? "(none)" : obj->carried_by->name,
	obj->wear_loc );
    strcat( buf1, buf );
    
    sprintf( buf, "Values: %d %d %d %d.\n\r",
	obj->value[0], obj->value[1], obj->value[2], obj->value[3] );
	 strcat( buf1, buf );

    if ( obj->extra_descr != NULL || obj->pIndexData->extra_descr != NULL )
    {
	EXTRA_DESCR_DATA *ed;

	strcat( buf1, "Extra description keywords: '" );

	for ( ed = obj->extra_descr; ed != NULL; ed = ed->next )
	{
	    strcat( buf1, ed->keyword );
	    if ( ed->next != NULL )
		strcat( buf1, " " );
	}

	for ( ed = obj->pIndexData->extra_descr; ed != NULL; ed = ed->next )
	{
	    strcat( buf1, ed->keyword );
	    if ( ed->next != NULL )
		strcat( buf1, " " );
	}

	strcat( buf1, "'.\n\r" );
    }

	 for ( paf = obj->affected; paf != NULL; paf = paf->next )
    {
        sprintf( buf, "Affects %s by %d.\n\r",
	    affect_loc_name( paf->location ), paf->modifier );
ansi_color( BOLD, ch );
ansi_color( BLUE, ch );
        strcat( buf1, buf );
ansi_color( NTEXT, ch );
ansi_color( GREY, ch );
    }

	 for ( paf = obj->pIndexData->affected; paf != NULL; paf = paf->next )
    {
	sprintf( buf, "Affects %s by %d.\n\r",
	    affect_loc_name( paf->location ), paf->modifier );
ansi_color( BOLD, ch );
ansi_color( GREEN, ch );
        strcat( buf1, buf );
ansi_color( NTEXT, ch );
ansi_color( GREY, ch );
    }

    send_to_char( buf1, ch );
	 return;
}

/*
 * Return ascii name of an act bit vector.
 */
char *act_bit_name( int vector )
{
    static char buf[512];

    buf[0] = '\0';
    if ( vector & ACT_IS_NPC		   ) strcat( buf, " npc"           );
	 if ( vector & ACT_SENTINEL		) strcat( buf, " sentinel"      );
    if ( vector & ACT_SCAVENGER		) strcat( buf, " scavenger"     );
    if ( vector & ACT_AGGRESSIVE		) strcat( buf, " aggressive"    );
    if ( vector & ACT_STAY_AREA		) strcat( buf, " stay-area"     );
	 if ( vector & ACT_WIMPY		   ) strcat( buf, " wimpy"         );
    if ( vector & ACT_PET			   ) strcat( buf, " pet"           );
    if ( vector & ACT_TRAIN		   ) strcat( buf, " train"         );
    if ( vector & ACT_PRACTICE		) strcat( buf, " practice"      );
    return buf+1;
}

void do_mstat( CHAR_DATA *ch, char *argument )
{
	 char buf[MAX_STRING_LENGTH];
	 char buf1[MAX_STRING_LENGTH];
	 char arg[MAX_INPUT_LENGTH];
	 AFFECT_DATA *paf;
	 CHAR_DATA *victim;

	 one_argument( argument, arg );

	 if ( arg[0] == '\0' )
	 {
	send_to_char( "Mstat whom?\n\r", ch );
	return;
	 }

	 if ( ( victim = get_char_world( ch, arg ) ) == NULL )
	 {
	send_to_char( "They aren't here.\n\r", ch );
	return;
	 }

	 buf1[0] = '\0';

	 sprintf( buf, "Name: %s.\n\r",
	victim->name );
	 strcat( buf1, buf );

	 sprintf( buf, "Vnum: %d.  Sex: %s.  Room: %d.\n\r",
	IS_NPC(victim) ? victim->pIndexData->vnum : 0,
	victim->sex == SEX_MALE    ? "male"   :
	victim->sex == SEX_FEMALE  ? "female" : "neutral",
	victim->in_room == NULL    ?        0 : victim->in_room->vnum
	);
	 strcat( buf1, buf );

	 sprintf( buf, "Str: %d.  Int: %d.  Wis: %d.  Dex: %d.  Con: %d.\n\r",
	get_curr_str(victim),
	get_curr_int(victim),
	get_curr_wis(victim),
	get_curr_dex(victim),
	get_curr_con(victim) );
	 strcat( buf1, buf );

	 sprintf( buf, "Hp: %d/%d.  Mana: %d/%d.  Move: %d/%d.  Practices: %d.\n\r",
	victim->hit,         victim->max_hit,
	victim->mana,        victim->max_mana,
	victim->move,        victim->max_move,
	victim->practice );
	 strcat( buf1, buf );

	 sprintf( buf,
	"Lv: %d.  Class: %d.  Align: %d.  AC: %d.  Gold: %d.  Exp: %d.\n\r",
	victim->level,       victim->class,        victim->alignment,
	GET_AC(victim),      victim->gold,         victim->exp );
	 strcat( buf1, buf );

	 sprintf( buf, "Hitroll: %d.  Damroll: %d.  Position: %d.  Wimpy: %d.\n\r",
	GET_HITROLL(victim), GET_DAMROLL(victim),
	victim->position,    victim->wimpy );
	 strcat( buf1, buf );

	 if ( !IS_NPC( victim ) )
	 {
	sprintf( buf, "Page Lines: %d.\n\r", victim->pcdata->pagelen );
	strcat( buf1, buf );
	 }

	 sprintf( buf, "Fighting: %s.\n\r",
		 victim->fighting ? victim->fighting->name : "(none)" );
	 strcat( buf1, buf );

	 if ( !IS_NPC(victim) )
	 {
	sprintf( buf,
		 "Thirst: %d.  Full: %d.  Drunk: %d.  Saving throw: %d.\n\r",
		 victim->pcdata->condition[COND_THIRST],
		 victim->pcdata->condition[COND_FULL],
		 victim->pcdata->condition[COND_DRUNK],
		 victim->saving_throw );
	strcat( buf1, buf );
	 }

	 sprintf( buf, "Carry number: %d.  Carry weight: %d.\n\r",
	victim->carry_number, victim->carry_weight );
	 strcat( buf1, buf );

	 sprintf( buf, "Age: %d.  Played: %d.  Timer: %d.  Act: %s.\n\r",
   get_age( victim ), (int) victim->played, victim->timer,
   act_bit_name(victim->act) );
    strcat( buf1, buf );

    sprintf( buf, "Master: %s.  Leader: %s.  Affected by: %s.\n\r",
	    victim->master      ? victim->master->name   : "(none)",
	    victim->leader      ? victim->leader->name   : "(none)",
	    affect_bit_name( victim->affected_by ) );
    strcat( buf1, buf );


    sprintf( buf, "Short description: %s.\n\rLong  description: %s",
	    victim->short_descr,
	    victim->long_descr[0] != '\0' ? victim->long_descr
	                                  : "(none).\n\r" );

	 if ( IS_NPC(victim) && victim->spec_fun != 0 )
	strcat( buf1, "Mobile has spec fun.\n\r" );

	 if ( IS_NPC(victim) && victim->hunting != NULL )
	 {
		  sprintf(buf, "Hunting victim: %s (%s)\n\r",
					 IS_NPC(victim->hunting) ? victim->hunting->short_descr
													 : victim->hunting->name,
					 IS_NPC(victim->hunting) ? "MOB" : "PLAYER" );
		  strcat(buf1, buf);
	 }

	 for ( paf = victim->affected; paf != NULL; paf = paf->next )
	 {
	sprintf( buf,
		 "Spell: '%s' modifies %s by %d for %d hours with bits %s.\n\r",
		 skill_table[(int) paf->type].name,
		 affect_loc_name( paf->location ),
		 paf->modifier,
		 paf->duration,
		 affect_bit_name( paf->bitvector )
		 );
ansi_color( BOLD, ch );
ansi_color( YELLOW, ch );
        strcat( buf1, buf );
ansi_color( NTEXT, ch );
ansi_color( GREY, ch );
         }

	 send_to_char( buf1, ch );
	 return;
}



void do_mfind( CHAR_DATA *ch, char *argument )
{
    extern int top_mob_index;
    char buf[2*MAX_STRING_LENGTH];
    char buf1[2*MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
	 MOB_INDEX_DATA *pMobIndex;
    int vnum;
    int nMatch;
    bool fAll;
	 bool found;

    one_argument( argument, arg );
    if ( arg[0] == '\0' )
	 {
	send_to_char( "Mfind whom?\n\r", ch );
	return;
    }

    buf1[0] = '\0';
    fAll	= !str_cmp( arg, "all" );
    found	= FALSE;
    nMatch	= 0;

    /*
     * Yeah, so iterating over all vnum's takes 10,000 loops.
     * Get_mob_index is fast, and I don't feel like threading another link.
     * Do you?
     * -- Furey
     */
	 for ( vnum = 0; nMatch < top_mob_index; vnum++ )
    {
	if ( ( pMobIndex = get_mob_index( vnum ) ) != NULL )
	{
	    nMatch++;
	    if ( fAll || is_name( arg, pMobIndex->player_name ) )
		 {
		found = TRUE;
		sprintf( buf, "[%5d] %s\n\r",
		    pMobIndex->vnum, capitalize( pMobIndex->short_descr ) );
ansi_color( PURPLE, ch );
                if (!fAll)
						  strcat( buf1, buf );
                else
                    send_to_char( buf, ch ); 
ansi_color( GREY, ch );

            }
	}
	 }

    if ( !found )
    {
	send_to_char( "Nothing like that in hell, earth, or heaven.\n\r", ch );
	return;
    }

    if(!fAll)
       send_to_char( buf1, ch );
    return;
}



void do_ofind( CHAR_DATA *ch, char *argument )
{
	 extern int top_obj_index;
    char buf[2*MAX_STRING_LENGTH];
    char buf1[2*MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    OBJ_INDEX_DATA *pObjIndex;
    int vnum;
    int nMatch;
    bool fAll;
    bool found;

    one_argument( argument, arg );
    if ( arg[0] == '\0' )
	 {
	send_to_char( "Ofind what?\n\r", ch );
	return;
    }

    buf1[0] = '\0';
    fAll	= !str_cmp( arg, "all" );
    found	= FALSE;
    nMatch	= 0;

	 /*
     * Yeah, so iterating over all vnum's takes 10,000 loops.
	  * Get_obj_index is fast, and I don't feel like threading another link.
     * Do you?
     * -- Furey
     */
    for ( vnum = 0; nMatch < top_obj_index; vnum++ )
    {
	if ( ( pObjIndex = get_obj_index( vnum ) ) != NULL )
	{
	    nMatch++;
	    if ( fAll || is_name( arg, pObjIndex->name ) )
	    {
		found = TRUE;
		sprintf( buf, "[%5d] %s\n\r",
		    pObjIndex->vnum, capitalize( pObjIndex->short_descr ) );
ansi_color( GREEN, ch );
                    if (!fAll)
                        strcat( buf1, buf );
                    else
                        send_to_char( buf, ch ); 
ansi_color( GREY, ch );
//              strcat( buf1, buf );
	    }
	}
    }

    if ( !found )
    {
	send_to_char( "Nothing like that in hell, earth, or heaven.\n\r", ch );
	return;
    }

    if(!fAll)
       send_to_char( buf1, ch );
    return;
}


void do_mwhere( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    bool found;

    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	send_to_char( "Mwhere whom?\n\r", ch );
	return;
    }

    found = FALSE;
    for ( victim = char_list; victim != NULL; victim = victim->next )
    {
	if ( IS_NPC(victim)
	&&   victim->in_room != NULL
	&&   is_name( arg, victim->name ) )
	{
	    found = TRUE;
	    sprintf( buf, "[%5d] %-28s [%5d] %s\n\r",
		victim->pIndexData->vnum,
		victim->short_descr,
		victim->in_room->vnum,
		victim->in_room->name );
	    send_to_char(buf, ch);
	}
    }

    if ( !found )
    {
	act(AT_GREY, "You didn't find any $T.", ch, NULL, arg, TO_CHAR );
	return;
	 }

    return;
}

// @@@
void do_shoplist( CHAR_DATA *ch, char *argument )
{
    SHOP_DATA *pShop;
    char buf[32768];
    char buf1[MAX_STRING_LENGTH];

	 buf[0] = '\0';
    for (pShop = shop_first; pShop; pShop = pShop->next)
       {
       sprintf(buf1, "%-20s opens at %2d closes at %2d buy %4d%% sell %4d%%\n\r",
          get_mob_index( pShop->keeper )->short_descr, pShop->open_hour,
			 pShop->close_hour, pShop->profit_buy, pShop->profit_sell);
       strcat( buf, buf1 );
       }

    send_to_char(buf, ch);
}

// @@@
void do_orphans( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_INPUT_LENGTH];
    char buf1[32768]; // @@@ This is the limit to the response length...
    OBJ_DATA *obj;
	 OBJ_DATA *in_obj;
    bool found;

    buf1[0] = '\0'; // @@@
    found = FALSE;
    for ( obj = object_list; obj != NULL; obj = obj->next )
	 {
	for ( in_obj = obj; in_obj->in_obj != NULL; in_obj = in_obj->in_obj )
	    ;

	if ( in_obj->carried_by == NULL  && in_obj->in_room == NULL)
	{
            found = TRUE;
		 sprintf( buf, "%s\n\r", obj->short_descr );
            buf[0] = UPPER(buf[0]);
            if (strlen(buf1) + strlen(buf) >= sizeof buf1 - 2) // @@@
               break;
            strcat(buf1, buf); // @@@
	}

    }

    if ( !found )
	send_to_char( "Nothing like that in hell, earth, or heaven.\n\r", ch );
    else // @@@
        send_to_char( buf1, ch ); // @@@
	 return;
}

void do_reboo( CHAR_DATA *ch, char *argument )
{
    send_to_char( "If you want to REBOOT, spell it out.\n\r", ch );
    return;
}



void do_reboot( CHAR_DATA *ch, char *argument )
{
	 char buf[MAX_STRING_LENGTH];
    extern bool merc_down;

	 sprintf( buf, "Reboot by %s.", ch->name );
    do_echo( ch, buf );
    merc_down = TRUE;
    return;
}



void do_shutdow( CHAR_DATA *ch, char *argument )
{
	 send_to_char( "If you want to SHUTDOWN, spell it out.\n\r", ch );
    return;
}



void do_shutdown( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    extern bool merc_down;

    sprintf( buf, "Shutdown by %s.", ch->name );
    append_file( ch, SHUTDOWN_FILE, buf );
	 strcat( buf, "\n\r" );
	 do_echo( ch, buf );
    merc_down = TRUE;
    return;
}



void do_snoop( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    DESCRIPTOR_DATA *d;
    CHAR_DATA *victim;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Snoop whom?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
	 }

    if ( victim->desc == NULL )
    {
	send_to_char( "No descriptor to snoop.\n\r", ch );
	return;
    }

    if ( victim == ch )
    {
	send_to_char( "Cancelling all snoops.\n\r", ch );
	for ( d = descriptor_list; d != NULL; d = d->next )
	{
		 if ( d->snoop_by == ch->desc )
		d->snoop_by = NULL;
	}
	return;
    }

    if ( victim->desc->snoop_by != NULL )
    {
	send_to_char( "Busy already.\n\r", ch );
	return;
	 }

    if ( get_trust( victim ) >= get_trust( ch ) )
	 {
	send_to_char( "You failed.\n\r", ch );
	return;
    }

    if ( ch->desc != NULL )
    {
	for ( d = ch->desc->snoop_by; d != NULL; d = d->snoop_by )
	{
		 if ( d->character == victim || d->original == victim )
	    {
		send_to_char( "No snoop loops.\n\r", ch );
		return;
		 }
	}
    }

    victim->desc->snoop_by = ch->desc;
    send_to_char( "Ok.\n\r", ch );
    return;
}



void do_switch( CHAR_DATA *ch, char *argument )
{
	 char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;

    one_argument( argument, arg );
    
    if ( arg[0] == '\0' )
    {
	send_to_char( "Switch into whom?\n\r", ch );
	return;
    }

    if ( ch->desc == NULL )
	return;

    if ( ch->desc->original != NULL )
    {
	send_to_char( "You are already switched.\n\r", ch );
	return;
    }

    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

	 if ( victim == ch )
    {
	send_to_char( "Ok.\n\r", ch );
	return;
    }

    /*
     * Pointed out by Da Pub (What Mud)
     */
    if ( !IS_NPC( victim ) )
    {
        send_to_char( "You cannot switch into a player!\n\r", ch );
	return;
	 }

    if ( victim->desc != NULL )
    {
	send_to_char( "Character in use.\n\r", ch );
	return;
    }

    ch->desc->character = victim;
    ch->desc->original  = ch;
    victim->desc        = ch->desc;
    ch->desc            = NULL;
    send_to_char( "Ok.\n\r", victim );
	 return;
}



void do_return( CHAR_DATA *ch, char *argument )
{
    if ( ch->desc == NULL )
	return;

    if ( ch->desc->original == NULL )
    {
	send_to_char( "You aren't switched.\n\r", ch );
	return;
    }

    send_to_char( "You return to your original body.\n\r", ch );
    ch->desc->character       = ch->desc->original;
    ch->desc->original        = NULL;
    ch->desc->character->desc = ch->desc; 
    ch->desc                  = NULL;
    return;
}



void do_mload( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    MOB_INDEX_DATA *pMobIndex;
    CHAR_DATA *victim;
    
    one_argument( argument, arg );

    if ( arg[0] == '\0' || !is_number(arg) )
    {
	send_to_char( "Syntax: mload <vnum>.\n\r", ch );
	return;
    }

    if ( ( pMobIndex = get_mob_index( atoi( arg ) ) ) == NULL )
    {
	send_to_char( "No mob has that vnum.\n\r", ch );
	return;
    }

    victim = create_mobile( pMobIndex );
    char_to_room( victim, ch->in_room );
	 act(AT_YELLOW, "$n has created $N!", ch, NULL, victim, TO_ROOM );
    send_to_char( "Ok.\n\r", ch );
    return;
}



void do_oload( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    OBJ_INDEX_DATA *pObjIndex;
    OBJ_DATA *obj;
    int level;
 
	 argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if ( arg1[0] == '\0' || !is_number( arg1 ) )
    {
        send_to_char( "Syntax: oload <vnum> <level>.\n\r", ch );
        return;
    }
 
    if ( arg2[0] == '\0' )
    {
	level = get_trust( ch );
	 }
    else
    {
	/*
	 * New feature from Alander.
	 */
        if ( !is_number( arg2 ) )
        {
	    send_to_char( "Syntax: oload <vnum> <level>.\n\r", ch );
	    return;
        }
        level = atoi( arg2 );
	if ( level < 0 || level > get_trust( ch ) )
        {
	    send_to_char( "Limited to your trust level.\n\r", ch );
	    return;
		  }
    }

    if ( ( pObjIndex = get_obj_index( atoi( arg1 ) ) ) == NULL )
    {
	send_to_char( "No object has that vnum.\n\r", ch );
	return;
    }

    obj = create_object( pObjIndex, level );
    if ( CAN_WEAR(obj, ITEM_TAKE) )
    {
	obj_to_char( obj, ch );
	 }
    else
    {
	obj_to_room( obj, ch->in_room );
		  act(AT_YELLOW, "$n has created $p!", ch, obj, NULL, TO_ROOM );
	 }
    send_to_char( "Ok.\n\r", ch );
	 return;
}



void do_purge( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    OBJ_DATA *obj;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	/* 'purge' */
	CHAR_DATA *vnext;
	OBJ_DATA  *obj_next;

	for ( victim = ch->in_room->people; victim != NULL; victim = vnext )
	{
	    vnext = victim->next_in_room;
	    if ( IS_NPC(victim) && victim != ch )
		extract_char( victim, TRUE );
	}

	for ( obj = ch->in_room->contents; obj != NULL; obj = obj_next )
	{
	    obj_next = obj->next_content;
	    extract_obj( obj );
	}

	act(AT_RED, "$n purges the room!", ch, NULL, NULL, TO_ROOM);
	send_to_char( "Ok.\n\r", ch );
	return;
    }

    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( !IS_NPC(victim) )
    {
	send_to_char( "Not on PC's.\n\r", ch );
	return;
    }

    act(AT_RED, "$n purges $N.", ch, NULL, victim, TO_NOTVICT );
    extract_char( victim, TRUE );
	 return;
}



void do_advance( CHAR_DATA *ch, char *argument )
{
	 char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
	 CHAR_DATA *victim;
    OBJ_DATA *obj;
    OBJ_DATA *obj_next;
    int level;
    int iLevel;

	 argument = one_argument( argument, arg1 );
	 argument = one_argument( argument, arg2 );

	 if ( arg1[0] == '\0' || arg2[0] == '\0' || !is_number( arg2 ) )
	 {
	send_to_char( "Syntax: advance <char> <level>.\n\r", ch );
	return;
	 }

	 if ( ( victim = get_char_room( ch, arg1 ) ) == NULL )
	 {
	send_to_char( "That player is not here.\n\r", ch);
	return;
	 }

	 if ( IS_NPC(victim) )
	 {
	send_to_char( "Not on NPC's.\n\r", ch );
	return;
	 }

	 if ( ( level = atoi( arg2 ) ) < 1 || level > MAX_LEVEL )
	 {
	send_to_char( "Level must be 1 to 54.\n\r", ch );
	return;
	 }

	 if ( level > get_trust( ch ) )
	 {
	send_to_char( "Limited to your trust level.\n\r", ch );
	return;
	 }

	 /*
	  * Lower level:
	  *   Reset to level 1.
	  *   Then raise again.
	  *   Currently, an imp can lower another imp.
	  *   -- Swiftest
	  */
	 if ( level <= victim->level )
	 {
	int sn;

	send_to_char( "Lowering a player's level!\n\r", ch );
	set_char_color( AT_YELLOW, victim );
	send_to_char( "Cursed and forsaken! The gods have lowered your level.\n\r", victim );
	victim->level    = 1;
	victim->exp   = 1000;
	victim->max_hit  = 10;
	victim->max_mana = 100;
	victim->max_move = 100;
	for ( sn = 0; sn < MAX_SKILL; sn++ )
		 victim->pcdata->learned[sn] = 0;
	victim->practice = 0;
	victim->hit      = victim->max_hit;
	victim->mana     = victim->max_mana;
	victim->move     = victim->max_move;
	advance_level( victim );
	 }
	 else
	 {
	send_to_char( "Raising a player's level!\n\r", ch );
	if (victim->level >= LEVEL_HERO)
	{
	  set_char_color( AT_YELLOW, victim );
	  act( AT_YELLOW, "$n makes some arcane gestures with $s hands, then points $s finger at you!",
			 ch, NULL, victim, TO_VICT );
	  act( AT_YELLOW, "$n makes some arcane gestures with $s hands, then points $s finger at $N!",
			 ch, NULL, victim, TO_NOTVICT );
	  set_char_color( AT_WHITE, victim );
	  send_to_char( "You suddenly feel very strange...\n\r\n\r", victim );
	  set_char_color( AT_LBLUE, victim );
	}

	switch(level)
	{
	default:
	  send_to_char( "The gods feel fit to raise your level!\n\r", victim );
	  break;
	case 51:
	  set_char_color( AT_WHITE, victim );
	  do_help( victim, "Promo_Angel" );
	  send_to_char( "You awake... all your possessions are gone.\n\r", victim );
	  extract_char( victim, FALSE );
	  break;
	case 52:
	  do_help( victim, "Promo_Saint" );
	  break;
	case 53:
	  do_help( victim, "Promo_Deity" );
	  break;
	case 54:
	  do_help( victim, "Promo_God" );
	}
	 }

	 for ( iLevel = victim->level ; iLevel < level; iLevel++ )
	 {
	if (level < LEVEL_IMMORTAL)
	  send_to_char( "You raise a level!!\n\r", victim );
	victim->level += 1;
	advance_level( victim );
	 }
	 victim->exp   = 1000 * UMAX( 1, victim->level );
	 victim->trust = 0;
	 return;
}



void do_trust( CHAR_DATA *ch, char *argument )
{
	 char arg1[MAX_INPUT_LENGTH];
	 char arg2[MAX_INPUT_LENGTH];
	 CHAR_DATA *victim;
	 int level;

	 argument = one_argument( argument, arg1 );
	 argument = one_argument( argument, arg2 );

	 if ( arg1[0] == '\0' || arg2[0] == '\0' || !is_number( arg2 ) )
	 {
	send_to_char( "Syntax: trust <char> <level>.\n\r", ch );
	return;
	 }

	 if ( ( victim = get_char_room( ch, arg1 ) ) == NULL )
	 {
	send_to_char( "That player is not here.\n\r", ch);
	return;
	 }

	 if ( ( level = atoi( arg2 ) ) < 0 || level > 54 )
	 {
	send_to_char( "Level must be 0 (reset) or 1 to 54.\n\r", ch );
	return;
	 }

	 if ( level > get_trust( ch ) )
	 {
	send_to_char( "Limited to your trust.\n\r", ch );
	return;
	 }

	 victim->trust = level;
	 return;
}



void do_restore( CHAR_DATA *ch, char *argument )
{
	 char arg[MAX_INPUT_LENGTH];
	 CHAR_DATA *victim;

	 one_argument( argument, arg );
	 if ( arg[0] == '\0' )
	 {
	send_to_char( "Restore whom?\n\r", ch );
	return;
	 }

	 if ( !str_cmp( arg, "all" ) )
		{

		  for ( victim = char_list; victim != NULL; victim = victim->next )

			 {
				if ( !IS_NPC(victim) )
				  {
					victim->hit  = victim->max_hit;
					victim->mana = victim->max_mana;
					victim->move = victim->max_move;
					update_pos( victim );
					act(AT_GREEN, "$n has restored you!", ch, NULL, victim, TO_VICT );
				 }
			 }
		 send_to_char( "Ok.\n\r", ch );
		 return;
		}

	 if ( ( victim = get_char_world( ch, arg ) ) == NULL )
	 {
	send_to_char( "They aren't here.\n\r", ch );
	return;
	 }

	 victim->hit  = victim->max_hit;
	 victim->mana = victim->max_mana;
	 victim->move = victim->max_move;
	 update_pos( victim );
	 act(AT_GREEN, "$n has restored you!", ch, NULL, victim, TO_VICT );
	 send_to_char( "Ok.\n\r", ch );
	 return;
}



void do_freeze( CHAR_DATA *ch, char *argument )
{
	 char arg[MAX_INPUT_LENGTH];
	 CHAR_DATA *victim;

	 one_argument( argument, arg );

	 if ( arg[0] == '\0' )
	 {
	send_to_char( "Freeze whom?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( IS_NPC(victim) )
    {
	send_to_char( "Not on NPC's.\n\r", ch );
	return;
    }

    if ( get_trust( victim ) >= get_trust( ch ) )
    {
	send_to_char( "You failed.\n\r", ch );
	return;
    }

    if ( IS_SET(victim->act, PLR_FREEZE) )
    {
	REMOVE_BIT(victim->act, PLR_FREEZE);
	send_to_char( "You can play again.\n\r", victim );
	send_to_char( "FREEZE removed.\n\r", ch );
    }
    else
    {
	SET_BIT(victim->act, PLR_FREEZE);
	send_to_char( "You can't do ANYthing!\n\r", victim );
	send_to_char( "FREEZE set.\n\r", ch );
    }

    save_char_obj( victim );

    return;
}



void do_log( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Log whom?\n\r", ch );
	return;
    }

    if ( !str_cmp( arg, "all" ) )
    {
	if ( fLogAll )
	{
	    fLogAll = FALSE;
	    send_to_char( "Log ALL off.\n\r", ch );
	}
	else
	{
	    fLogAll = TRUE;
	    send_to_char( "Log ALL on.\n\r", ch );
	}
	return;
    }

    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( IS_NPC(victim) )
    {
	send_to_char( "Not on NPC's.\n\r", ch );
	return;
    }

    /*
     * No level check, gods can log anyone.
     */
    if ( IS_SET(victim->act, PLR_LOG) )
    {
	REMOVE_BIT(victim->act, PLR_LOG);
	send_to_char( "LOG removed.\n\r", ch );
    }
    else
    {
	SET_BIT(victim->act, PLR_LOG);
	send_to_char( "LOG set.\n\r", ch );
    }

    return;
}



void do_noemote( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Noemote whom?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( IS_NPC(victim) )
    {
	send_to_char( "Not on NPC's.\n\r", ch );
	return;
    }

    if ( get_trust( victim ) >= get_trust( ch ) )
    {
	send_to_char( "You failed.\n\r", ch );
	return;
    }

    if ( IS_SET(victim->act, PLR_NO_EMOTE) )
    {
	REMOVE_BIT(victim->act, PLR_NO_EMOTE);
	send_to_char( "You can emote again.\n\r", victim );
	send_to_char( "NO_EMOTE removed.\n\r", ch );
    }
    else
    {
	SET_BIT(victim->act, PLR_NO_EMOTE);
	send_to_char( "You can't emote!\n\r", victim );
	send_to_char( "NO_EMOTE set.\n\r", ch );
    }

    return;
}



void do_notell( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Notell whom?", ch );
	return;
    }

    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( IS_NPC(victim) )
    {
	send_to_char( "Not on NPC's.\n\r", ch );
	return;
    }

    if ( get_trust( victim ) >= get_trust( ch ) )
    {
	send_to_char( "You failed.\n\r", ch );
	return;
    }

    if ( IS_SET(victim->act, PLR_NO_TELL) )
    {
	REMOVE_BIT(victim->act, PLR_NO_TELL);
	send_to_char( "You can tell again.\n\r", victim );
	send_to_char( "NO_TELL removed.\n\r", ch );
    }
    else
    {
	SET_BIT(victim->act, PLR_NO_TELL);
	send_to_char( "You can't tell!\n\r", victim );
	send_to_char( "NO_TELL set.\n\r", ch );
    }

    return;
}



void do_silence( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Silence whom?", ch );
	return;
    }

    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( IS_NPC(victim) )
    {
	send_to_char( "Not on NPC's.\n\r", ch );
	return;
    }

    if ( get_trust( victim ) >= get_trust( ch ) )
    {
	send_to_char( "You failed.\n\r", ch );
	return;
    }

    if ( IS_SET(victim->act, PLR_SILENCE) )
    {
	REMOVE_BIT(victim->act, PLR_SILENCE);
	send_to_char( "You can use channels again.\n\r", victim );
	send_to_char( "SILENCE removed.\n\r", ch );
    }
    else
    {
	SET_BIT(victim->act, PLR_SILENCE);
	send_to_char( "You can't use channels!\n\r", victim );
	send_to_char( "SILENCE set.\n\r", ch );
    }

    return;
}



void do_peace( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *rch;

    for ( rch = ch->in_room->people; rch != NULL; rch = rch->next_in_room )
    {
	if ( rch->fighting != NULL )
	    stop_fighting( rch, TRUE );
    }

    send_to_char( "Ok.\n\r", ch );
    return;
}



BAN_DATA *		ban_free;
BAN_DATA *		ban_list;

void do_ban( CHAR_DATA *ch, char *argument )
{

	 char buf[MAX_STRING_LENGTH];
	 char arg[MAX_INPUT_LENGTH];
	 BAN_DATA *pban;

	 if ( IS_NPC(ch) )
	return;

	 one_argument( argument, arg );

	 if ( arg[0] == '\0' )
	 {
	strcpy( buf, "Banned sites:\n\r" );
	for ( pban = ban_list; pban != NULL; pban = pban->next )
	{
		 strcat( buf, pban->name );
		 strcat( buf, "\n\r" );
	}
	send_to_char( buf, ch );
	return;
	 }

	 for ( pban = ban_list; pban != NULL; pban = pban->next )
	 {
	if ( !str_cmp( arg, pban->name ) )
	{
		 send_to_char( "That site is already banned!\n\r", ch );
		 return;
	}
	 }

	 if ( ban_free == NULL )
	 {
	pban		= alloc_perm( sizeof(*pban) );
	 }
	 else
	 {
	pban		= ban_free;
	ban_free	= ban_free->next;
	 }

	 pban->name	= str_dup( arg );
	 pban->next	= ban_list;
	 ban_list	= pban;
	 send_to_char( "Ok.\n\r", ch );
	 return;

}



void do_allow( CHAR_DATA *ch, char *argument )
{
	 char arg[MAX_INPUT_LENGTH];
	 BAN_DATA *prev;
	 BAN_DATA *curr;

	 one_argument( argument, arg );

	 if ( arg[0] == '\0' )
	 {
	send_to_char( "Remove which site from the ban list?\n\r", ch );
	return;
	 }

	 prev = NULL;
	 for ( curr = ban_list; curr != NULL; prev = curr, curr = curr->next )
	 {
	if ( !str_cmp( arg, curr->name ) )
	{
		 if ( prev == NULL )
		ban_list   = ban_list->next;
	    else
		prev->next = curr->next;

	    free_string( curr->name );
	    curr->next	= ban_free;
	    ban_free	= curr;
	    send_to_char( "Ok.\n\r", ch );
	    return;
	}
    }

    send_to_char( "Site is not banned.\n\r", ch );
	 return;
}



void do_wizlock( CHAR_DATA *ch, char *argument )
{
    extern bool wizlock;
    wizlock = !wizlock;

    if ( wizlock )
	send_to_char( "Game wizlocked.\n\r", ch );
    else
	send_to_char( "Game un-wizlocked.\n\r", ch );

    return;
}



void do_slookup( CHAR_DATA *ch, char *argument )
{
    char buf[5*MAX_STRING_LENGTH];
    char buf1[5*MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    int sn;

    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	send_to_char( "Slookup what?\n\r", ch );
	return;
    }

    if ( !str_cmp( arg, "all" ) )
    {
        buf1[0] = '\0';
	for ( sn = 0; sn < MAX_SKILL; sn++ )
	{
		 if ( skill_table[sn].name == NULL )
		break;
	    sprintf( buf, "Sn: %4d Slot: %4d Skill/spell: '%s'\n\r",
		sn, skill_table[sn].slot, skill_table[sn].name );
	    strcat( buf1, buf );
	}
	send_to_char( buf1, ch );
    }
    else
    {
	if ( ( sn = skill_lookup( arg ) ) < 0 )
	{
	    send_to_char( "No such skill or spell.\n\r", ch );
		 return;
	}

	sprintf( buf, "Sn: %4d Slot: %4d Skill/spell: '%s'\n\r",
	    sn, skill_table[sn].slot, skill_table[sn].name );
	send_to_char( buf, ch );
    }

    return;
}



void do_sset( CHAR_DATA *ch, char *argument )
{
    char arg1 [MAX_INPUT_LENGTH];
    char arg2 [MAX_INPUT_LENGTH];
    char arg3 [MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    int value;
    int sn;
    bool fAll;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    argument = one_argument( argument, arg3 );

    if ( arg1[0] == '\0' || arg2[0] == '\0' || arg3[0] == '\0' )
    {
	send_to_char( "Syntax: sset <victim> <skill> <value>\n\r",	ch );
	send_to_char( "or:     sset <victim> all     <value>\n\r",	ch );
	send_to_char( "Skill being any skill or spell.\n\r",		ch );
	return;
    }

    if ( ( victim = get_char_world( ch, arg1 ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
	 }

    if ( IS_NPC(victim) )
    {
	send_to_char( "Not on NPC's.\n\r", ch );
	return;
    }

    fAll = !str_cmp( arg2, "all" );
    sn   = 0;
    if ( !fAll && ( sn = skill_lookup( arg2 ) ) < 0 )
    {
	send_to_char( "No such skill or spell.\n\r", ch );
	return;
    }

    /*
     * Snarf the value.
     */
    if ( !is_number( arg3 ) )
    {
	send_to_char( "Value must be numeric.\n\r", ch );
	return;
    }

    value = atoi( arg3 );
	 if ( value < 0 || value > 100 )
    {
	send_to_char( "Value range is 0 to 100.\n\r", ch );
	return;
    }

    if ( fAll )
    {
	for ( sn = 0; sn < MAX_SKILL; sn++ )
	{
	    if ( skill_table[sn].name != NULL || skill_table[sn].name != NULL )
		victim->pcdata->learned[sn]	= value;
	}
	 }
    else
    {
	victim->pcdata->learned[sn] = value;
    }

    return;
}



void do_mset( CHAR_DATA *ch, char *argument )
{
	 char arg1 [MAX_INPUT_LENGTH];
	 char arg2 [MAX_INPUT_LENGTH];
	 char arg3 [MAX_INPUT_LENGTH];
	 char buf  [MAX_STRING_LENGTH];
	 CHAR_DATA *victim;
	 int value, max;

	 smash_tilde( argument );
	 argument = one_argument( argument, arg1 );
	 argument = one_argument( argument, arg2 );
	 strcpy( arg3, argument );

	 if ( arg1[0] == '\0' || arg2[0] == '\0' )
	 {
	send_to_char( "Syntax: mset <victim> <field>  <value>\n\r",	ch );
	send_to_char( "or:     mset <victim> <string> <value>\n\r",	ch );
	send_to_char( "\n\r",						ch );
	send_to_char( "Field being one of:\n\r",			ch );
ansi_color( BOLD, ch );
ansi_color( RED, ch );
	send_to_char( "  gold hp mana move practice align\n\r",		ch );
	send_to_char( "  thirst drunk full clan",			ch );
	send_to_char( "\n\r",						ch );
ansi_color( NTEXT, ch );
ansi_color( GREY, ch );
		  send_to_char( "\n\r",                                           ch );
	send_to_char( "String being one of:\n\r",			ch );
ansi_color( BOLD, ch );
ansi_color( GREEN, ch );
		  send_to_char( "  name short long description title spec\n\r",   ch );
ansi_color( NTEXT, ch );
ansi_color( GREY, ch );
		  return;
	 }

	 if ( ( victim = get_char_world( ch, arg1 ) ) == NULL )
	 {
	send_to_char( "They aren't here.\n\r", ch );
	return;
	 }

	 /*
	  * Snarf the value (which need not be numeric).
	  */
	 value = is_number( arg3 ) ? atoi( arg3 ) : -1;

	 /*
	  * Set something.
	  */
	 if ( !str_cmp( arg2, "str" ) )
	 {
	if ( IS_NPC(victim) )
	{
		 send_to_char( "Not on NPC's.\n\r", ch );
		 return;
	}

	if ( class_table[ch->class].attr_prime == APPLY_STR )
		 max = 25;
	else
		 max = 18;

	if ( value < 3 || value > max )
	{
		 sprintf( buf, "Strength range is 3 to %d.\n\r", max );
		 send_to_char( buf, ch );
		 return;
	}

	victim->pcdata->perm_str = value;
	return;
	 }

	 if ( !str_cmp( arg2, "int" ) )
	 {
	if ( IS_NPC(victim) )
	{
		 send_to_char( "Not on NPC's.\n\r", ch );
		 return;
	}

	if ( class_table[ch->class].attr_prime == APPLY_INT )
		 max = 25;
	else
		 max = 18;

	if ( value < 3 || value > max )
	{
		 sprintf( buf, "Intelligence range is 3 to %d.\n\r", max );
		 send_to_char( buf, ch );
		 return;
	}

	victim->pcdata->perm_int = value;
	return;
	 }

	 if ( !str_cmp( arg2, "clan" ) )
	 {
	CLAN_DATA *clan;

	if ( ch->level < 54 )
	{
		 send_to_char( "You can't do that.\n\r", ch );
		 return;
	}
	if ( IS_NPC(victim) )
	{
		 send_to_char( "Not on NPC's.\n\r", ch );
		 return;
	}

	if ( !arg3 || arg3[0] == '\0' )
	{
		 victim->pcdata->clan_name	= str_dup( "" );
		 victim->pcdata->clan	= NULL;
		 send_to_char( "Removed from clan.\n\rPlease make sure you adjust that clan's members accordingly.\n\r", ch );
		 return;
	}
	clan = get_clan( arg3 );
	if ( !clan )
	{
		send_to_char( "No such clan.\n\r", ch );
		return;
	}
	victim->pcdata->clan_name = str_dup( arg3 );
	victim->pcdata->clan = clan;
	send_to_char( "Done.\n\rPlease make sure you adjust that clan's members accordingly.\n\r", ch );
	return;
	 }

	 if ( !str_cmp( arg2, "wis" ) )
	 {
	if ( IS_NPC(victim) )
	{
		 send_to_char( "Not on NPC's.\n\r", ch );
		 return;
	}

	if ( class_table[ch->class].attr_prime == APPLY_WIS )
	    max = 25;
	else
	    max = 18;

	if ( value < 3 || value > max )
	{
		 sprintf( buf, "Wisdom range is 3 to %d.\n\r", max );
	    send_to_char( buf, ch );
	    return;
	}

	victim->pcdata->perm_wis = value;
	return;
    }

    if ( !str_cmp( arg2, "dex" ) )
    {
	if ( IS_NPC(victim) )
	{
		 send_to_char( "Not on NPC's.\n\r", ch );
	    return;
	}

	if ( class_table[ch->class].attr_prime == APPLY_DEX )
	    max = 25;
	else
	    max = 18;

	if ( value < 3 || value > max )
	{
	    sprintf( buf, "Dexterity range is 3 to %d.\n\r", max );
	    send_to_char( buf, ch );
		 return;
	}

	victim->pcdata->perm_dex = value;
	return;
    }

    if ( !str_cmp( arg2, "con" ) )
    {
	if ( IS_NPC(victim) )
	{
	    send_to_char( "Not on NPC's.\n\r", ch );
	    return;
	}

	if ( class_table[ch->class].attr_prime == APPLY_CON )
	    max = 25;
	else
	    max = 18;

	if ( value < 3 || value > max )
	{
	    sprintf( buf, "Constitution range is 3 to %d.\n\r", max );
	    send_to_char( buf, ch );
	    return;
	}

	victim->pcdata->perm_con = value;
	return;
    }

    if ( !str_cmp( arg2, "sex" ) )
    {
	if ( value < 0 || value > 2 )
	{
	    send_to_char( "Sex range is 0 to 2.\n\r", ch );
	    return;
	}
	victim->sex = value;
	return;
    }

    if ( !str_cmp( arg2, "class" ) )
    {
	if ( value < 0 || value >= MAX_CLASS )
	{
	    char buf[MAX_STRING_LENGTH];

	    sprintf( buf, "Class range is 0 to %d.\n", MAX_CLASS-1 );
	    send_to_char( buf, ch );
	    return;
	}
	victim->class = value;
	return;
    }

    if ( !str_cmp( arg2, "race" ) )
    {
	if ( value < 0 || value >= MAX_RACE )
	{
	    char buf[MAX_STRING_LENGTH];

	    sprintf( buf, "Race range is 0 to %d.\n", MAX_RACE-1 );
	    send_to_char( buf, ch );
	    return;
	}
	victim->race = value;
	return;
    }

    if ( !str_cmp( arg2, "level" ) )
    {
	if ( !IS_NPC(victim) )
	{
	    send_to_char( "Not on PC's.\n\r", ch );
	    return;
	}

	if ( value < 0 || value > 50 )
	{
	    send_to_char( "Level range is 0 to 50.\n\r", ch );
	    return;
	}
	victim->level = value;
	return;
    }

    if ( !str_cmp( arg2, "gold" ) )
    {
	victim->gold = value;
	return;
	 }

    if ( !str_cmp( arg2, "hp" ) )
    {
	if ( value < -10 || value > 30000 )
	{
	    send_to_char( "Hp range is -10 to 30,000 hit points.\n\r", ch );
	    return;
	}
	victim->max_hit = value;
	return;
    }

	 if ( !str_cmp( arg2, "mana" ) )
    {
	if ( value < 0 || value > 30000 )
	{
	    send_to_char( "Mana range is 0 to 30,000 mana points.\n\r", ch );
	    return;
	}
	victim->max_mana = value;
	return;
    }

    if ( !str_cmp( arg2, "move" ) )
    {
	if ( value < 0 || value > 30000 )
	{
	    send_to_char( "Move range is 0 to 30,000 move points.\n\r", ch );
	    return;
	}
	victim->max_move = value;
	return;
    }

    if ( !str_cmp( arg2, "practice" ) )
    {
	if ( value < 0 || value > 100 )
	{
		 send_to_char( "Practice range is 0 to 100 sessions.\n\r", ch );
	    return;
	}
	victim->practice = value;
	return;
    }

    if ( !str_cmp( arg2, "align" ) )
    {
	if ( value < -1000 || value > 1000 )
	{
	    send_to_char( "Alignment range is -1000 to 1000.\n\r", ch );
	    return;
	}
	victim->alignment = value;
	return;
    }

    if ( !str_cmp( arg2, "thirst" ) )
    {
	if ( IS_NPC(victim) )
	{
	    send_to_char( "Not on NPC's.\n\r", ch );
	    return;
	}

	if ( value < 0 || value > 100 )
	{
	    send_to_char( "Thirst range is 0 to 100.\n\r", ch );
	    return;
	}

	victim->pcdata->condition[COND_THIRST] = value;
	return;
    }

    if ( !str_cmp( arg2, "drunk" ) )
    {
	if ( IS_NPC(victim) )
	{
	    send_to_char( "Not on NPC's.\n\r", ch );
	    return;
	}

	if ( value < 0 || value > 100 )
	{
	    send_to_char( "Drunk range is 0 to 100.\n\r", ch );
	    return;
	}

	victim->pcdata->condition[COND_DRUNK] = value;
	return;
	 }

    if ( !str_cmp( arg2, "full" ) )
    {
	if ( IS_NPC(victim) )
	{
	    send_to_char( "Not on NPC's.\n\r", ch );
	    return;
	}

	if ( value < 0 || value > 100 )
	{
	    send_to_char( "Full range is 0 to 100.\n\r", ch );
		 return;
	}

	victim->pcdata->condition[COND_FULL] = value;
	return;
    }

    if ( !str_cmp( arg2, "name" ) )
    {
	if ( !IS_NPC(victim) )
	{
	    send_to_char( "Not on PC's.\n\r", ch );
	    return;
	}

	free_string( victim->name );
	victim->name = str_dup( arg3 );
	return;
    }

    if ( !str_cmp( arg2, "short" ) )
    {
	free_string( victim->short_descr );
	victim->short_descr = str_dup( arg3 );
	return;
    }

    if ( !str_cmp( arg2, "long" ) )
    {
	free_string( victim->long_descr );
	victim->long_descr = str_dup( arg3 );
	return;
    }

	 if ( !str_cmp( arg2, "title" ) )
	 {
	if ( IS_NPC(victim) )
	{
		 send_to_char( "Not on NPC's.\n\r", ch );
		 return;
	}

	set_title( victim, arg3 );
	return;
	 }

	 if ( !str_cmp( arg2, "spec" ) )
	 {
	if ( !IS_NPC(victim) )
	{
		 send_to_char( "Not on PC's.\n\r", ch );
		 return;
	}

	if ( ( victim->spec_fun = spec_lookup( arg3 ) ) == 0 )
	{
		 send_to_char( "No such spec fun.\n\r", ch );
		 return;
	}

	 else if (!str_cmp(arg2, "hunt"))
	 {
		  CHAR_DATA *hunted = 0;

		  if ( !IS_NPC(victim) )
		  {
				send_to_char( "Not on PC's.\n\r", ch );
				return;
		  }

		  if ( str_cmp( arg3, "." ) )
			 if ( (hunted = get_char_area(victim, arg3)) == NULL )
				{
				  send_to_char("Mob couldn't locate the victim to hunt.\n\r", ch);
				  return;
				}

		  victim->hunting = hunted;
		  return;
	 }

	return;
    }

    /*
     * Generate usage message.
	  */
    do_mset( ch, "" );
    return;
}



void do_oset( CHAR_DATA *ch, char *argument )
{
    char arg1 [MAX_INPUT_LENGTH];
    char arg2 [MAX_INPUT_LENGTH];
    char arg3 [MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
	 int value;

    smash_tilde( argument );
    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    strcpy( arg3, argument );

    if ( arg1[0] == '\0' || arg2[0] == '\0' || arg3[0] == '\0' )
    {
	send_to_char( "Syntax: oset <object> <field>  <value>\n\r",	ch );
	send_to_char( "or:     oset <object> <string> <value>\n\r",	ch );
	send_to_char( "\n\r",						ch );
	send_to_char( "Field being one of:\n\r",			ch );
ansi_color( BOLD, ch );
ansi_color( RED, ch );
        send_to_char( "  value0 value1 value2 value3\n\r",              ch );
	send_to_char( "  extra wear level weight cost timer\n\r",	ch );
ansi_color( NTEXT, ch );
ansi_color( GREY, ch );
        send_to_char( "\n\r",                                           ch );
	send_to_char( "String being one of:\n\r",			ch );
ansi_color( BOLD, ch );
ansi_color( GREEN, ch );
        send_to_char( "  name short long ed\n\r",                       ch );
ansi_color( NTEXT, ch );
ansi_color( GREY, ch );
        return;
    }

    if ( ( obj = get_obj_world( ch, arg1 ) ) == NULL )
    {
	send_to_char( "Nothing like that in hell, earth, or heaven.\n\r", ch );
	return;
    }

    /*
     * Snarf the value (which need not be numeric).
     */
    value = atoi( arg3 );

    /*
     * Set something.
     */
    if ( !str_cmp( arg2, "value0" ) || !str_cmp( arg2, "v0" ) )
    {
	obj->value[0] = value;
	return;
	 }

    if ( !str_cmp( arg2, "value1" ) || !str_cmp( arg2, "v1" ) )
    {
	obj->value[1] = value;
	return;
    }

    if ( !str_cmp( arg2, "value2" ) || !str_cmp( arg2, "v2" ) )
    {
	obj->value[2] = value;
	return;
    }

    if ( !str_cmp( arg2, "value3" ) || !str_cmp( arg2, "v3" ) )
    {
	obj->value[3] = value;
	return;
    }

    if ( !str_cmp( arg2, "extra" ) )
    {
	obj->extra_flags = value;
	return;
    }

	 if ( !str_cmp( arg2, "wear" ) )
    {
	obj->wear_flags = value;
	return;
    }

    if ( !str_cmp( arg2, "level" ) )
    {
	obj->level = value;
	return;
    }
	
    if ( !str_cmp( arg2, "weight" ) )
	 {
	obj->weight = value;
	return;
    }

    if ( !str_cmp( arg2, "cost" ) )
    {
	obj->cost = value;
	return;
    }

    if ( !str_cmp( arg2, "timer" ) )
    {
	obj->timer = value;
	return;
    }
	
    if ( !str_cmp( arg2, "name" ) )
    {
	free_string( obj->name );
	obj->name = str_dup( arg3 );
	return;
    }

    if ( !str_cmp( arg2, "short" ) )
    {
	free_string( obj->short_descr );
	obj->short_descr = str_dup( arg3 );
	return;
    }

    if ( !str_cmp( arg2, "long" ) )
    {
	free_string( obj->description );
	obj->description = str_dup( arg3 );
	return;
    }

    if ( !str_cmp( arg2, "ed" ) )
	 {
	EXTRA_DESCR_DATA *ed;

	argument = one_argument( argument, arg3 );
	if ( argument == NULL )
	{
	    send_to_char( "Syntax: oset <object> ed <keyword> <string>\n\r",
		ch );
	    return;
	}

	if ( extra_descr_free == NULL )
	{
		 ed			= alloc_perm( sizeof(*ed) );
	}
	else
	{
		 ed			= extra_descr_free;
		 extra_descr_free	= extra_descr_free->next;
	}

	ed->keyword		= str_dup( arg3     );
	ed->description		= str_dup( argument );
	ed->next		= obj->extra_descr;
	obj->extra_descr	= ed;
	return;
	 }

	 /*
	  * Generate usage message.
	  */
	 do_oset( ch, "" );
	 return;
}



void do_rset( CHAR_DATA *ch, char *argument )
{
	 char arg1 [MAX_INPUT_LENGTH];
    char arg2 [MAX_INPUT_LENGTH];
    char arg3 [MAX_INPUT_LENGTH];
    ROOM_INDEX_DATA *location;
    int value;

    smash_tilde( argument );
    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    strcpy( arg3, argument );

    if ( arg1[0] == '\0' || arg2[0] == '\0' || arg3[0] == '\0' )
    {
	send_to_char( "Syntax: rset <location> <field> value\n\r",	ch );
	send_to_char( "\n\r",						ch );
	send_to_char( "Field being one of:\n\r",			ch );
ansi_color( BOLD, ch );
ansi_color( BLUE, ch );
        send_to_char( "  flags sector\n\r",                             ch );
ansi_color( NTEXT, ch );
ansi_color( GREY, ch );
        return;
    }

    if ( ( location = find_location( ch, arg1 ) ) == NULL )
    {
	send_to_char( "No such location.\n\r", ch );
	return;
    }

	 /*
     * Snarf the value.
     */
    if ( !is_number( arg3 ) )
    {
	send_to_char( "Value must be numeric.\n\r", ch );
	return;
    }
    value = atoi( arg3 );

    /*
     * Set something.
     */
	 if ( !str_cmp( arg2, "flags" ) )
    {
	location->room_flags	= value;
	return;
    }

    if ( !str_cmp( arg2, "sector" ) )
    {
	location->sector_type	= value;
	return;
    }

    /*
	  * Generate usage message.
     */
    do_rset( ch, "" );
    return;
}


#ifndef WIN32
void do_users( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char buf2[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;
	 int count;

    count	= 0;
    buf[0]	= '\0';
    buf2[0]     = '\0';
    for ( d = descriptor_list; d != NULL; d = d->next )
    {
	if ( d->character != NULL && can_see( ch, d->character ) )
	{
	    count++;
	    sprintf( buf + strlen(buf), "[%3d %2d] %s (" /*"%s@"*/ "%s)\n\r",
		d->descriptor,
		d->connected,
		d->original  ? d->original->name  :
		d->character ? d->character->name : "(none)",
		/*d->host_and_name->username,*/ d->host
		);
	}
    }

    sprintf( buf2, "%d user%s\n\r", count, count == 1 ? "" : "s" );
    strcat( buf, buf2 );
    send_to_char( buf, ch );
    return;
}
#endif


/*
 * Thanks to Grodyn for pointing out bugs in this function.
 */
void do_force( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    int trust;
    int cmd;

    argument = one_argument( argument, arg );

	 if ( arg[0] == '\0' || argument[0] == '\0' )
    {
	send_to_char( "Force whom to do what?\n\r", ch );
	return;
    }

    /*
     * Look for command in command table.
     */
    trust = get_trust( ch );
    for ( cmd = 0; cmd_table[cmd].name[0] != '\0'; cmd++ )
    {
	if ( argument[0] == cmd_table[cmd].name[0]
		 &&   !str_prefix( argument, cmd_table[cmd].name )
	    &&   ( cmd_table[cmd].level > trust 
		  && cmd_table[cmd].level != 41 ) )
	{
	  send_to_char( "You cant even do that yourself!\n\r", ch );
	  return;
	}
    }

    if ( !str_cmp( arg, "all" ) )
    {
	CHAR_DATA *vch;
	CHAR_DATA *vch_next;

	for ( vch = char_list; vch != NULL; vch = vch_next )
	{
	    vch_next = vch->next;

	    if ( !IS_NPC(vch) && get_trust( vch ) < get_trust( ch ) )
	    {
	        MOBtrigger = FALSE;
		act(AT_YELLOW, "$n forces you to '$t'.", ch, argument, vch, TO_VICT );
		interpret( vch, argument );
	    }
	}
    }
	 else
    {
	CHAR_DATA *victim;

	if ( ( victim = get_char_world( ch, arg ) ) == NULL )
	{
	    send_to_char( "They aren't here.\n\r", ch );
	    return;
	}

	if ( victim == ch )
	{
	    send_to_char( "Aye aye, right away!\n\r", ch );
		 return;
	}

	if ( get_trust( victim ) >= get_trust( ch ) )
	{
	    send_to_char( "Do it yourself!\n\r", ch );
	    return;
	}

	MOBtrigger = FALSE;
	act(AT_YELLOW, "$n forces you to '$t'.", ch, argument, victim, TO_VICT );
	interpret( victim, argument );
    }

	 send_to_char( "Ok.\n\r", ch );
	 return;
}



void do_invis( CHAR_DATA *ch, char *argument )
{
	 char arg[MAX_INPUT_LENGTH];
	 sh_int level;

	 if ( IS_NPC(ch) )
	return;

	 argument = one_argument( argument, arg );
	 if ( arg && arg[0] != '\0' )
	 {
	if ( !is_number( arg ) )
	{
		send_to_char( "Usage: invis | invis <level>\n\r", ch );
		return;
	}
	level = atoi( arg );
	if ( level < 2 || level > get_trust( ch ) )
	{
		 send_to_char( "Invalid level.\n\r", ch );
		 return;
	}
	ch->pcdata->wizinvis = level;
	sprintf( arg, "Wizinvis level set to %d.\n\r", level );
	send_to_char( arg, ch );
	return;
	 }

	 if ( ch->pcdata->wizinvis < 2 )
		ch->pcdata->wizinvis = ch->level;

	 if ( IS_SET(ch->act, PLR_WIZINVIS) )
	 {
	REMOVE_BIT(ch->act, PLR_WIZINVIS);
	 act( AT_IMMORT, "$n slowly fades into existence.", ch, NULL, NULL, TO_ROOM );
	send_to_char( "You slowly fade back into existence.\n\r", ch );
	 }
	 else
	 {
	SET_BIT(ch->act, PLR_WIZINVIS);
	 act( AT_IMMORT, "$n slowly fades into thin air.", ch, NULL, NULL, TO_ROOM );
	send_to_char( "You slowly vanish into thin air.\n\r", ch );
	 }

	 return;
}



void do_holylight( CHAR_DATA *ch, char *argument )
{
    if ( IS_NPC(ch) )
	return;

    if ( IS_SET(ch->act, PLR_HOLYLIGHT) )
	 {
	REMOVE_BIT(ch->act, PLR_HOLYLIGHT);
	send_to_char( "Holy light mode off.\n\r", ch );
    }
    else
    {
	SET_BIT(ch->act, PLR_HOLYLIGHT);
	send_to_char( "Holy light mode on.\n\r", ch );
    }

    return;
}

/* Wizify and Wizbit sent in by M. B. King */

void do_wizify( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
  
	 argument = one_argument( argument, arg1  );
    if ( arg1[0] == '\0' )
    {
	send_to_char( "Syntax: wizify <name>\n\r" , ch );
	return;
    }
	 if ( ( victim = get_char_world( ch, arg1 ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r" , ch );
	return;
    }
    if ( IS_NPC( victim ) )
    {
	send_to_char( "Not on mobs.\n\r", ch );
	return;
    }
    victim->wizbit = !victim->wizbit;
    if ( victim->wizbit ) 
    {
ansi_color( BOLD, ch );
        act(AT_GREY, "$N wizified.\n\r", ch, NULL, victim, TO_CHAR );
	act(AT_BLINK, "$n has wizified you!\n\r", ch, NULL, victim, TO_VICT );
ansi_color( NTEXT, ch );
    }
    else
    {
ansi_color( BOLD, ch );
ansi_color( BLACK, ch );
        act(AT_GREY, "$N dewizzed.\n\r", ch, NULL, victim, TO_CHAR );
	act(AT_BLINK, "$n has dewizzed you!\n\r", ch, NULL, victim, TO_VICT );
ansi_color( NTEXT, ch );
ansi_color( GREY, ch );
    }

    do_save( victim, "");
    return;
}

#if 0
/* Idea from Talen of Vego's do_where command */

void do_owhere( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    bool found = FALSE;
    OBJ_DATA *obj;
    OBJ_DATA *in_obj;
    int obj_counter = 1;

    one_argument( argument, arg );

    if( arg[0] == '\0' )
    {
	send_to_char( "Syntax:  owhere <object>.\n\r", ch );
	return;
    }
    else
    {
	for ( obj = object_list; obj != NULL; obj = obj->next )
	{
	    if ( !can_see_obj( ch, obj ) || !is_name( arg, obj->name ) )
			  continue;

		 found = TRUE;

	    for ( in_obj = obj; in_obj->in_obj != NULL;
		 in_obj = in_obj->in_obj )
	        ;

	    if ( in_obj->carried_by != NULL )
	    {
		sprintf( buf, "[%2d] %s carried by %s.\n\r", obj_counter,
			obj->short_descr, PERS( in_obj->carried_by, ch ) );
	    }
		 else
	    {
		sprintf( buf, "[%2d] %s in %s.\n\r", obj_counter,
			obj->short_descr, ( in_obj->in_room == NULL ) ?
			"somewhere" : in_obj->in_room->name );
	    }

	    obj_counter++;
	    buf[0] = UPPER( buf[0] );
	    send_to_char( buf, ch );
	}
    }

	 if ( !found )
	send_to_char(
		"Nothing like that in hell, earth, or heaven.\n\r" , ch );

    return;


}
#else
/* Idea from Talen of Vego's do_where command */

 /*
  * Modified combination of Merc 2.2, Dalton, and Zavod's  "owhere" commands
  */

void do_owhere( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    bool found = FALSE;
    OBJ_DATA *obj;
    OBJ_DATA *in_obj;
    int obj_counter = 1;
 
    one_argument( argument, arg );
 
	 if( arg[0] == '\0' )
      {
		  send_to_char( "Syntax:  owhere <object>.\n\r", ch );
        return;
      }
    
    for ( obj = object_list; obj != NULL; obj = obj->next )
      {
        if ( !can_see_obj( ch, obj ) || !is_name( arg, obj->name ) )
            continue;
 
        found = TRUE;
 
		  for ( in_obj = obj; in_obj->in_obj != NULL;
                in_obj = in_obj->in_obj ) ;

        if ( in_obj->carried_by != NULL )
	  {
        
            sprintf( buf, "%3d [%4d] %-30s [%4d] %s\n\r", 
                obj_counter,
                obj->pIndexData->vnum,
                obj->short_descr,
in_obj->carried_by->in_room == NULL ? 0 : in_obj->carried_by->in_room->vnum,
                PERS( in_obj->carried_by, ch ) );
 
	  }
        else
	  {
 
            sprintf( buf, "%3d [%4d] %-30s [%4d] %s\n\r",
                obj_counter,
                obj->pIndexData->vnum,
                obj->short_descr,
                in_obj->in_room == NULL ? 0 : in_obj->in_room->vnum,
                in_obj->in_room == NULL ? "somewhere" : in_obj->in_room->name );
	  }
            
        obj_counter++;
		  send_to_char( buf, ch );
		}

	 if ( !found )
		  send_to_char(
					 "Nothing like that in hell, earth, or heaven.\n\r" , ch );
	 return;

}
#endif

void do_sstime( CHAR_DATA *ch, char *argument )
{
			  CHAR_DATA *rch;
    extern char      *down_time;
    extern char      *warning1;
    extern char      *warning2;
           char       buf  [ MAX_STRING_LENGTH ];
           char       arg1 [ MAX_INPUT_LENGTH  ];
           char       arg2 [ MAX_INPUT_LENGTH  ];

	 
	 smash_tilde( argument );
	 argument = one_argument( argument, arg1 );
	 strcpy ( arg2, argument );

	 if ( arg1[0] == '\0' || arg2[0] == '\0' ||
	( strlen( arg2 ) != 8 && ( arg2 != "*" && strlen( arg2 ) != 1 ) ) )
      {
	send_to_char( "Syntax: sstime <field> <value>\n\r",               ch );
	send_to_char( "\n\r",                                             ch );
	send_to_char( "Field being one of:\n\r",                          ch );
	send_to_char( "  downtime  1warning  2warning\n\r",               ch );
	send_to_char( "\n\r",                                             ch );
	send_to_char( "Value being format of:\n\r",                       ch );
	send_to_char( "  hh:mm:ss  (military time) or  *  (for off)\n\r", ch );
	send_to_char( "\n\r",                                             ch );
	sprintf( buf,
		"1st warning:  %s\n\r2nd warning:  %s\n\r   Downtime:  %s\n\r",
		warning1, warning2, down_time );
	send_to_char( buf,                                                ch );
	return;
      }

    /* Set something */

    if ( !str_infix( arg1, "downtime" ) )
      {
	free_string( down_time );
	down_time = str_dup( arg2 );
	sprintf( buf, "Downtime is now set to:  %s\n\r", down_time );
	send_to_char( buf, ch );
	return;
      }
    if ( !str_infix( arg1, "1warning" ) )
      {
	free_string( warning1 );
	warning1 = str_dup( arg2 );
	sprintf( buf, "First warning will be given at:  %s\n\r", warning1 );
	send_to_char( buf, ch );
	return;
      }
    if ( !str_infix( arg1, "2warning" ) )
		{
	free_string( warning2 );
	warning2 = str_dup( arg2 );
	sprintf( buf, "Second warning will be given at:  %s\n\r", warning2 );
	send_to_char( buf, ch );
	return;
      }

    /* Generate usage mesage */

	 do_sstime( ch, "" );
	 return;

}

 void do_cmdtable( CHAR_DATA *ch, char *argument )
{
    int cmd;
    char buf[MAX_STRING_LENGTH];
 
	 send_to_char("Commands and Number of Uses This Run\n\r", ch);

	 for ( cmd = 0; cmd_table[cmd].name[0] != '\0'; cmd++ )
	 {
		  /* too many send-to-chars if this is done normally */
		  if ((cmd+1)%4)
			 sprintf(buf,"%-6.6s %4d\t",cmd_table[cmd].name,cmd_table[cmd].num_uses);
		  else
			 sprintf(buf,"%-6.6s %4d\n\r", cmd_table[cmd].name,cmd_table[cmd].num_uses );
		  send_to_char(buf,ch);
	 }
	 return;
}

/* void do_opentournament ( CHAR_DATA *ch, TOURNEY_DATA *tourney, char *argument )
{
	 char arg1[MAX_INPUT_LENGTH];
	 char arg2[MAX_INPUT_LENGTH];
	 char buf[MAX_STRING_LENGTH];
	 int  tourn_hi;
	 int  tourn_lo;

	 argument = one_argument( argument, arg1 );
	 argument = one_argument( argument, arg2 );
	 tourn_lo = atoi( arg1 );  tourn_hi = atoi( arg2 );

	 if ( arg1[0] == '\0' || arg2[0] == '\0' )
	 {
	send_to_char( "Syntax: opentournament <Lowest Level allowed> <Highest Level allowed>.\n\r", ch );
	return;
	 }

	 if ( tourn_lo > tourn_hi )
	 {
	send_to_char( "Unacceptable tournament range.\n\r", ch );
	return;
	 }
	 if ( tourney->open = 0 )
	 {
	tourney->open = 1;
	tourney->low_level = tourn_lo;
	tourney->hi_level = tourn_hi;
	ch->openedtourney = TRUE;
	sprintf( buf,  "%s has opened a tournament for levels %d to %d", ch->name,
	tourn_lo, tourn_hi );
	echo_to_all( AT_IMMORT, buf );
	return;
	 }
	send_to_char( "A tournament is either already open, or is in progress" , ch );
	return;

}

void do_starttournament ( CHAR_DATA *ch, TOURNEY_DATA *tourney )
{
	 char buf[MAX_STRING_LENGTH];

	 if ( !ch->openedtourney = TRUE )
	 {
	 send_to_char( "You have not opened a tournament.", ch );
	 return;
	 }
	 if ( tourney->open = 2 )
	 {
	 send_to_char( "The tournament is already in progress.", ch);
	 return;
	 }
	 tourney->open = 2;
	 sprintf( buf, "%s has closed entry to the tournament, no more players are allowed to join.",
	 ch->name );
	 echo_to_all( AT_IMMORT, buf );
	 return;

}  */
extern ROOM_INDEX_DATA *       room_index_hash         [MAX_KEY_HASH]; /* db.c */

/* Super-AT command:

FOR ALL <action>
FOR MORTALS <action>
FOR GODS <action>
FOR MOBS <action>
FOR EVERYWHERE <action>


Executes action several times, either on ALL players (not including yourself),
MORTALS (including trusted characters), GODS (characters with level higher than
L_HERO), MOBS (Not recommended) or every room (not recommended either!)

If you insert a # in the action, it will be replaced by the name of the target.

If # is a part of the action, the action will be executed for every target
in game. If there is no #, the action will be executed for every room containg
at least one target, but only once per room. # cannot be used with FOR EVERY-
WHERE. # can be anywhere in the action.

Example: 

FOR ALL SMILE -> you will only smile once in a room with 2 players.
FOR ALL TWIDDLE # -> In a room with A and B, you will twiddle A then B.

Destroying the characters this command acts upon MAY cause it to fail. Try to
avoid something like FOR MOBS PURGE (although it actually works at my MUD).

FOR MOBS TRANS 3054 (transfer ALL the mobs to Midgaard temple) does NOT work
though :)

The command works by transporting the character to each of the rooms with 
target in them. Private rooms are not violated.

*/

/* Expand the name of a character into a string that identifies THAT
   character within a room. E.g. the second 'guard' -> 2. guard
*/   
const char * name_expand (CHAR_DATA *ch)
{
	int count = 1;
	CHAR_DATA *rch;
	char name[MAX_INPUT_LENGTH]; /*  HOPEFULLY no mob has a name longer than THAT */

	static char outbuf[MAX_INPUT_LENGTH];	
	
	if (!IS_NPC(ch))
		return ch->name;
		
	one_argument (ch->name, name); /* copy the first word into name */
	
	if (!name[0]) /* weird mob .. no keywords */
	{
		strcpy (outbuf, ""); /* Do not return NULL, just an empty buffer */
		return outbuf;
	}
		
	for (rch = ch->in_room->people; rch && (rch != ch);rch = rch->next_in_room)
		if (is_name (name, rch->name))
			count++;
			

	sprintf (outbuf, "%d.%s", count, name);
	return outbuf;
}


void do_for (CHAR_DATA *ch, char *argument)
{
	char range[MAX_INPUT_LENGTH];
	char buf[MAX_STRING_LENGTH];
	bool fGods = FALSE, fMortals = FALSE, fMobs = FALSE, fEverywhere = FALSE, found;
	ROOM_INDEX_DATA *room, *old_room;
	CHAR_DATA *p, *p_next;
	int i;
	
	argument = one_argument (argument, range);
	
	if (!range[0] || !argument[0]) /* invalid usage? */
	{
		do_help (ch, "for");
		return;
	}
	
	if (!str_prefix("quit", argument))
	{
		send_to_char ("Are you trying to crash the MUD or something?\n\r",ch);
		return;
	}
	
	
	if (!str_cmp (range, "all"))
	{
		fMortals = TRUE;
		fGods = TRUE;
	}
	else if (!str_cmp (range, "gods"))
		fGods = TRUE;
	else if (!str_cmp (range, "mortals"))
		fMortals = TRUE;
	else if (!str_cmp (range, "mobs"))
		fMobs = TRUE;
	else if (!str_cmp (range, "everywhere"))
		fEverywhere = TRUE;
	else
		do_help (ch, "for"); /* show syntax */

	/* do not allow # to make it easier */		
	if (fEverywhere && strchr (argument, '#'))
	{
		send_to_char ("Cannot use FOR EVERYWHERE with the # thingie.\n\r",ch);
		return;
	}
		
	if (strchr (argument, '#')) /* replace # ? */
	{ 
		for (p = char_list; p ; p = p_next)
		{
			p_next = p->next; /* In case someone DOES try to AT MOBS SLAY # */
			found = FALSE;
			
			if (!(p->in_room) || room_is_private(p->in_room) || (p == ch))
				continue;
			
			if (IS_NPC(p) && fMobs)
				found = TRUE;
			else if (!IS_NPC(p) && p->level >= LEVEL_IMMORTAL && fGods)
				found = TRUE;
			else if (!IS_NPC(p) && p->level < LEVEL_IMMORTAL && fMortals)
				found = TRUE;

			/* It looks ugly to me.. but it works :) */				
			if (found) /* p is 'appropriate' */
			{
				char *pSource = argument; /* head of buffer to be parsed */
				char *pDest = buf; /* parse into this */
				
				while (*pSource)
				{
					if (*pSource == '#') /* Replace # with name of target */
					{
						const char *namebuf = name_expand (p);
						
						if (namebuf) /* in case there is no mob name ?? */
							while (*namebuf) /* copy name over */
								*(pDest++) = *(namebuf++);

						pSource++;
					}
					else
						*(pDest++) = *(pSource++);
				} /* while */
				*pDest = '\0'; /* Terminate */
				
				/* Execute */
				old_room = ch->in_room;
				char_from_room (ch);
				char_to_room (ch,p->in_room);
				interpret (ch, buf);
				char_from_room (ch);
				char_to_room (ch,old_room);
				
			} /* if found */
		} /* for every char */
	}
	else /* just for every room with the appropriate people in it */
	{
		for (i = 0; i < MAX_KEY_HASH; i++) /* run through all the buckets */
			for (room = room_index_hash[i] ; room ; room = room->next)
			{
				found = FALSE;
				
				/* Anyone in here at all? */
				if (fEverywhere) /* Everywhere executes always */
					found = TRUE;
				else if (!room->people) /* Skip it if room is empty */
					continue;
					
					
				/* Check if there is anyone here of the requried type */
				/* Stop as soon as a match is found or there are no more ppl in room */
				for (p = room->people; p && !found; p = p->next_in_room)
				{

					if (p == ch) /* do not execute on oneself */
						continue;
						
					if (IS_NPC(p) && fMobs)
						found = TRUE;
					else if (!IS_NPC(p) && (p->level >= LEVEL_IMMORTAL) && fGods)
						found = TRUE;
					else if (!IS_NPC(p) && (p->level <= LEVEL_IMMORTAL) && fMortals)
						found = TRUE;
				} /* for everyone inside the room */
						
				if (found && !room_is_private(room)) /* Any of the required type here AND room not private? */
				{
					/* This may be ineffective. Consider moving character out of old_room
					   once at beginning of command then moving back at the end.
					   This however, is more safe?
					*/
				
					old_room = ch->in_room;
					char_from_room (ch);
					char_to_room (ch, room);
					interpret (ch, argument);
					char_from_room (ch);
					char_to_room (ch, old_room);
				} /* if found */
			} /* for every room in a bucket */
	} /* if strchr */
} /* do_for */
