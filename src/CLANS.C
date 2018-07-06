/****************************************************************************
 * Realms of Despair (c) 1994 Derek Snider				    *
 * Based on concepts from Dungeons & Dragons (tm) TSR Hobbies Inc.	    *
 * Based on code by various authors.					    *
 * ------------------------------------------------------------------------ *
 *			     Special clan module			    *
 ****************************************************************************/

#include <sys/types.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
/* #include <stdlib.h> */
#include <time.h>
#include "merc.h"

CLAN_DATA * first_clan;
CLAN_DATA * last_clan;

/* local routines */
void	fread_clan	args( ( CLAN_DATA *clan, FILE *fp ) );
bool	load_clan_file	args( ( char *clanfile ) );
void	write_clan_list	args( ( void ) );

/*
 * Get pointer to clan structure from clan name.
 */
CLAN_DATA *get_clan( char *name )
{
    CLAN_DATA *clan;
    
    for ( clan = first_clan; clan; clan = clan->next )
       if ( !str_cmp( name, clan->name ) )
         return clan;
    return NULL;
}


void write_clan_list( )
{
    CLAN_DATA *tclan;
    FILE *fpout;
    char filename[256];

	 sprintf( filename, "%s%s%s", CLAN_DIR, "/", CLAN_LIST );
    fpout = fopen( filename, "w" );
    if ( !fpout )
    {
	bug( "FATAL: cannot open clan.lst for writing!\n\r", 0 );
 	return;
    }	  
    for ( tclan = first_clan; tclan; tclan = tclan->next )
	fprintf( fpout, "%s\n", tclan->filename );
    fprintf( fpout, "$\n" );
    fclose( fpout );
}



/*
 * Save a clan's data to its data file
 */
void save_clan( CLAN_DATA *clan )
{
    FILE *fp;
    char filename[256];
    char buf[MAX_STRING_LENGTH];

    if ( !clan )
    {
	bug( "save_clan: null clan pointer!", 0 );
	return;
    }
        
    if ( !clan->filename || clan->filename[0] == '\0' )
    {
	sprintf( buf, "save_clan: %s has no filename", clan->name );
	bug( buf, 0 );
	return;
    }
 
	 sprintf( filename, "%s%s%s%s",CLAN_DIR,initial(clan->filename), "/",
	 capitalize(clan->filename) );

    fclose( fpReserve );
    if ( ( fp = fopen( filename, "w" ) ) == NULL )
    {
		bug( "save_clan: fopen", 0 );
		perror( filename );
    }
    else
    {
	fprintf( fp, "#CLAN\n" );
	fprintf( fp, "Name         %s~\n",	clan->name		);
	fprintf( fp, "Filename     %s~\n",	clan->filename		);
	fprintf( fp, "Motto        %s~\n",	clan->motto		);
	fprintf( fp, "Description  %s~\n",	clan->description	);
	fprintf( fp, "Deity        %s~\n",	clan->deity		);
	fprintf( fp, "Leader       %s~\n",	clan->leader		);
	fprintf( fp, "NumberOne    %s~\n",	clan->number1		);
	fprintf( fp, "NumberTwo    %s~\n",	clan->number2		);
	fprintf( fp, "PKills       %d\n",	clan->pkills		);
	fprintf( fp, "PDeaths      %d\n",	clan->pdeaths		);
	fprintf( fp, "MKills       %d\n",	clan->mkills		);
	fprintf( fp, "MDeaths      %d\n",	clan->mdeaths		);
	fprintf( fp, "IllegalPK    %d\n",	clan->illegal_pk	);
	fprintf( fp, "Score        %d\n",	clan->score		);
	fprintf( fp, "Type         %d\n",	clan->clan_type		);
	fprintf( fp, "Favour       %d\n",	clan->favour		);
	fprintf( fp, "Strikes      %d\n",	clan->strikes		);
	fprintf( fp, "Members      %d\n",	clan->members		);
	fprintf( fp, "Alignment    %d\n",	clan->alignment		);
	fprintf( fp, "Board        %d\n",	clan->board		);
	fprintf( fp, "ClanObjOne   %d\n",	clan->clanobj1		);
	fprintf( fp, "ClanObjTwo   %d\n",	clan->clanobj2		);
	fprintf( fp, "ClanObjThree %d\n",	clan->clanobj3		);
	fprintf( fp, "Recall       %d\n",	clan->recall		);
	fprintf( fp, "Storeroom    %d\n",	clan->storeroom		);
	fprintf( fp, "GuardOne     %d\n",	clan->guard1		);
	fprintf( fp, "GuardTwo     %d\n",	clan->guard2		);
	fprintf( fp, "End\n\n"						);
	fprintf( fp, "#END\n"						);
    }
    fclose( fp );
    fpReserve = fopen( NULL_FILE, "r" );
    return;
}


/*
 * Read in actual clan data.
 */

#if defined(KEY)
#undef KEY
#endif

#define KEY( literal, field, value )					\
				if ( !str_cmp( word, literal ) )	\
				{					\
				    field  = value;			\
				    fMatch = TRUE;			\
				    break;				\
				}

void fread_clan( CLAN_DATA *clan, FILE *fp )
{
    char buf[MAX_STRING_LENGTH];
    char *word;
    bool fMatch;

    for ( ; ; )
    {
	word   = feof( fp ) ? "End" : fread_word( fp );
	fMatch = FALSE;

	switch ( UPPER(word[0]) )
	{
	case '*':
	    fMatch = TRUE;
	    fread_to_eol( fp );
	    break;

	case 'A':
	    KEY( "Alignment",	clan->alignment,	fread_number( fp ) );
	    break;

	case 'B':
	    KEY( "Board",	clan->board,		fread_number( fp ) );
	    break;

	case 'C':
	    KEY( "ClanObjOne",	clan->clanobj1,		fread_number( fp ) );
	    KEY( "ClanObjTwo",	clan->clanobj2,		fread_number( fp ) );
	    KEY( "ClanObjThree",clan->clanobj3,		fread_number( fp ) );
	    break;

	case 'D':
	    KEY( "Deity",	clan->deity,		fread_string( fp ) );
	    KEY( "Description",	clan->description,	fread_string( fp ) );
	    break;

	case 'E':
	    if ( !str_cmp( word, "End" ) )
	    {
		if (!clan->name)
		  clan->name		= str_dup( "" );
		if (!clan->leader)
		  clan->leader		= str_dup( "" );
		if (!clan->description)
		  clan->description 	= str_dup( "" );
		if (!clan->motto)
		  clan->motto		= str_dup( "" );
		return;
	    }
	    break;
	    
	case 'F':
	    KEY( "Favour",	clan->favour,		fread_number( fp ) );
	    KEY( "Filename",	clan->filename,		fread_string( fp ) );

	case 'G':
	    KEY( "GuardOne",	clan->guard1,		fread_number( fp ) );
	    KEY( "GuardTwo",	clan->guard2,		fread_number( fp ) );
	    break;

	case 'I':
	    KEY( "IllegalPK",	clan->illegal_pk,	fread_number( fp ) );
	    break;

	case 'L':
	    KEY( "Leader",	clan->leader,		fread_string( fp ) );
	    break;

	case 'M':
	    KEY( "MDeaths",	clan->mdeaths,		fread_number( fp ) );
	    KEY( "Members",	clan->members,		fread_number( fp ) );
	    KEY( "MKills",	clan->mkills,		fread_number( fp ) );
	    KEY( "Motto",	clan->motto,		fread_string( fp ) );
	    break;
 
	case 'N':
	    KEY( "Name",	clan->name,		fread_string( fp ) );
	    KEY( "NumberOne",	clan->number1,		fread_string( fp ) );
	    KEY( "NumberTwo",	clan->number2,		fread_string( fp ) );
	    break;

	case 'P':
	    KEY( "PDeaths",	clan->pdeaths,		fread_number( fp ) );
	    KEY( "PKills",	clan->pkills,		fread_number( fp ) );
	    break;

	case 'R':
	    KEY( "Recall",	clan->recall,		fread_number( fp ) );
	    break;

	case 'S':
	    KEY( "Score",	clan->score,		fread_number( fp ) );
	    KEY( "Strikes",	clan->strikes,		fread_number( fp ) );
	    KEY( "Storeroom",	clan->storeroom,	fread_number( fp ) );
	    break;

	case 'T':
	    KEY( "Type",	clan->clan_type,	fread_number( fp ) );
	    break;
	}
	
	if ( !fMatch )
	{
	    sprintf( buf, "Fread_clan: no match: %s", word );
	    bug( buf, 0 );
	}
    }
}


/*
 * Load a clan file
 */

bool load_clan_file( char *clanfile )
{
    char filename[256];
    CLAN_DATA *clan;
    FILE *fp;
    bool found;


    found = FALSE;
    sprintf( filename, "%s%s", CLAN_DIR, clanfile );

    if ( ( fp = fopen( filename, "r" ) ) != NULL )
    {

	found = TRUE;
	for ( ; ; )
	{
	    char letter;
	    char *word;

	    letter = fread_letter( fp );
	    if ( letter == '*' )
	    {
		fread_to_eol( fp );
		continue;
	    }

	    if ( letter != '#' )
	    {
		bug( "Load_clan_file: # not found.", 0 );
		break;
	    }

	    word = fread_word( fp );
	    if ( !str_cmp( word, "CLAN"	) )
	    {
	    	fread_clan( clan, fp );
	    	break;
	    }
	    else
	    if ( !str_cmp( word, "END"	) )
	        break;
	    else
	    {
		bug( "Load_clan_file: bad section.", 0 );
		break;
	    }
	}
	fclose( fp );
    }

    if ( found )
    {
      if ( !first_clan )
        first_clan	= clan;
      else
        last_clan->next = clan;
      clan->next	= NULL;
      clan->prev	= last_clan;
      last_clan		= clan;
    }

	 return found;
}

/*
 * Load in all the clan files.
 */
void load_clans( )
{
    FILE *fpList;
    char *filename;
    char clanlist[256];
    char buf[MAX_STRING_LENGTH];
    
    
    first_clan	= NULL;
    last_clan	= NULL;

    log_string( "Loading clans..." );

    sprintf( clanlist, "%s%s", CLAN_DIR, CLAN_LIST );
    fclose( fpReserve );
    if ( ( fpList = fopen( clanlist, "r" ) ) == NULL )
    {
	perror( clanlist );
	 }

    for ( ; ; )
    {
	filename = feof( fpList ) ? "$" : fread_word( fpList );
	log_string( filename );
	if ( filename[0] == '$' )
	  break;

	if ( !load_clan_file( filename ) )
	{
	  sprintf( buf, "Cannot load clan file: %s", filename );
	  bug( buf, 0 );
	}
    }
    fclose( fpList );
    log_string(" Done clans " );
    fpReserve = fopen( NULL_FILE, "r" );
    return;
}

void do_make( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_INDEX_DATA *pObjIndex;
    OBJ_DATA *obj;
    CLAN_DATA *clan;
    int level;

    if ( IS_NPC( ch ) || !ch->pcdata->clan )
    {
	send_to_char( "Huh?\n\r", ch );
	return;
    }

    clan = ch->pcdata->clan;
    
    if ( str_cmp( ch->name, clan->leader ) )
    {
	send_to_char( "Huh?\n\r", ch );
	return;
    }

	 argument = one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Make what?\n\r", ch );
	return;
    }

    pObjIndex = get_obj_index( clan->clanobj1 );
    level = 10;

    if ( !pObjIndex || !is_name( arg, pObjIndex->name ) )
    {
      pObjIndex = get_obj_index( clan->clanobj2 );
      level = 25;
    }
    if ( !pObjIndex || !is_name( arg, pObjIndex->name ) )
    {
      pObjIndex = get_obj_index( clan->clanobj3 );
      level = 40;
    }

    if ( !pObjIndex || !is_name( arg, pObjIndex->name ) )
    {
	send_to_char( "You don't know how to make that.\n\r", ch );
	return;
    }

    obj = create_object( pObjIndex, level );
    SET_BIT( obj->extra_flags, ITEM_CLANOBJECT );
    if ( CAN_WEAR(obj, ITEM_TAKE) )
      obj_to_char( obj, ch );
    else
      obj_to_room( obj, ch->in_room );
    act( AT_MAGIC, "$n makes $p!", ch, obj, NULL, TO_ROOM );
    act( AT_MAGIC, "You make $p!", ch, obj, NULL, TO_CHAR );
    return;
}

void do_induct( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    CLAN_DATA *clan;

    if ( IS_NPC( ch ) || !ch->pcdata->clan )
    {
	send_to_char( "Huh?\n\r", ch );
	return;
    }

    clan = ch->pcdata->clan;
    
    if ( str_cmp( ch->name, clan->leader )
    &&   str_cmp( ch->name, clan->number1 )
    &&   str_cmp( ch->name, clan->number2 ) )
    {
	send_to_char( "Huh?\n\r", ch );
	return;
    }

    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Induct whom into your clan?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "That player is not here.\n\r", ch);
	return;
    }

    if ( IS_NPC(victim) )
    {
	send_to_char( "Not on NPC's.\n\r", ch );
	return;
    }

    if ( victim->level < 10 )
    {
	send_to_char( "This player is not worth of joining any clan yet.\n\r", ch );
	return;
    }

    if ( victim->level > ch->level )
    {
	send_to_char( "This player is too powerful for you to induct.\n\r", ch );
	return;
    }

    if ( victim->pcdata->clan )
    {
	send_to_char( "This player already belongs to a clan!\n\r", ch );
	return;
    }

    clan->members++;
    victim->pcdata->clan = clan;
    victim->pcdata->clan_name = str_dup( clan->name );
    act( AT_MAGIC, "You induct $N into $t", ch, clan->name, victim, TO_CHAR );
    act( AT_MAGIC, "$n inducts $N into $t", ch, clan->name, victim, TO_ROOM );
    act( AT_MAGIC, "$n inducts you into $t", ch, clan->name, victim, TO_VICT );
    save_char_obj( victim );
    return;
}

void do_outcast( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    CLAN_DATA *clan;

    if ( IS_NPC( ch ) || !ch->pcdata->clan )
    {
	send_to_char( "Huh?\n\r", ch );
	return;
    }

    clan = ch->pcdata->clan;

    if ( str_cmp( ch->name, clan->leader  )
    &&   str_cmp( ch->name, clan->number1 )
    &&   str_cmp( ch->name, clan->deity   ) )
    {
	send_to_char( "Huh?\n\r", ch );
	return;
    }

    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Outcast whom from your clan?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "That player is not here.\n\r", ch);
	return;
    }

    if ( IS_NPC(victim) )
    {
	send_to_char( "Not on NPC's.\n\r", ch );
	return;
    }

    if ( victim == ch )
    {
	send_to_char( "Kick yourself out of your own clan?\n\r", ch );
	return;
    }
 
    if ( victim->level > ch->level )
    {
	send_to_char( "This player is too powerful to kick out of your clan.\n\r", ch );
	return;
    }

    if ( victim->pcdata->clan != ch->pcdata->clan )
    {
	send_to_char( "This player does not belong to your clan!\n\r", ch );
	return;
    }

    --clan->members;
    if ( !str_cmp( victim->name, ch->pcdata->clan->number1 ) )
    {
	ch->pcdata->clan->number1 = str_dup( "" );
	 }
    if ( !str_cmp( victim->name, ch->pcdata->clan->number2 ) )
    {
	ch->pcdata->clan->number2 = str_dup( "" );
    }
    victim->pcdata->clan = NULL;
    victim->pcdata->clan_name = str_dup( "" );
	 act( AT_MAGIC, "You outcast $N from $t", ch, clan->name, victim, TO_CHAR );
    act( AT_MAGIC, "$n outcasts $N from $t", ch, clan->name, victim, TO_ROOM );
    act( AT_MAGIC, "$n outcasts you from $t", ch, clan->name, victim, TO_VICT );
    save_char_obj( victim );
    return;
}

void do_setclan( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    CLAN_DATA *clan;

    if ( IS_NPC( ch ) )
    {
	send_to_char( "Huh?\n\r", ch );
	return;
    }

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if ( arg1[0] == '\0' )
    {
	send_to_char( "Usage: setclan <clan> <field> <deity|leader|number1|number2> <player>\n\r", ch );
	send_to_char( "\n\rField being one of:\n\r", ch );
	send_to_char( " deity leader number1 number2\n\r", ch ); 
	send_to_char( " members board recall storage\n\r", ch );
	send_to_char( " obj1 obj2 obj3 guard1 guard2\n\r", ch );
	if ( get_trust( ch ) >= 54 )
	{
	  send_to_char( " name filename motto desc\n\r", ch );
	  send_to_char( " favour strikes type\n\r", ch );
	}
	return;
    }

    clan = get_clan( arg1 );
    if ( !clan )
    {
	send_to_char( "No such clan.\n\r", ch );
	return;
    }

    if ( !strcmp( arg2, "deity" ) )
    {
	clan->deity = str_dup( argument );
	send_to_char( "Done.\n\r", ch );
	save_clan( clan );
	return;
    }

    if ( !strcmp( arg2, "leader" ) )
    {
	clan->leader = str_dup( argument );
	send_to_char( "Done.\n\r", ch );
	save_clan( clan );
	return;
    }

    if ( !strcmp( arg2, "number1" ) )
    {
	clan->number1 = str_dup( argument );
	send_to_char( "Done.\n\r", ch );
	save_clan( clan );
	return;
    }

    if ( !strcmp( arg2, "number2" ) )
    {
	clan->number2 = str_dup( argument );
	send_to_char( "Done.\n\r", ch );
	save_clan( clan );
	return;
    }

    if ( !strcmp( arg2, "board" ) )
    {
	clan->board = atoi( argument );
	send_to_char( "Done.\n\r", ch );
	save_clan( clan );
	return;
    }

    if ( !strcmp( arg2, "members" ) )
    {
	clan->members = atoi( argument );
	send_to_char( "Done.\n\r", ch );
	save_clan( clan );
	return;
    }

    if ( !strcmp( arg2, "recall" ) )
    {
	clan->recall = atoi( argument );
	send_to_char( "Done.\n\r", ch );
	save_clan( clan );
	return;
    }

    if ( !strcmp( arg2, "storage" ) )
    {
	clan->storeroom = atoi( argument );
	send_to_char( "Done.\n\r", ch );
	save_clan( clan );
	return;
    }

    if ( !strcmp( arg2, "obj1" ) )
    {
	clan->clanobj1 = atoi( argument );
	send_to_char( "Done.\n\r", ch );
	save_clan( clan );
	return;
    }

    if ( !strcmp( arg2, "obj2" ) )
    {
	clan->clanobj2 = atoi( argument );
	send_to_char( "Done.\n\r", ch );
	save_clan( clan );
	return;
    }

    if ( !strcmp( arg2, "obj3" ) )
    {
	clan->clanobj3 = atoi( argument );
	send_to_char( "Done.\n\r", ch );
	save_clan( clan );
	return;
    }

    if ( !strcmp( arg2, "guard1" ) )
    {
	clan->guard1 = atoi( argument );
	send_to_char( "Done.\n\r", ch );
	save_clan( clan );
	return;
    }

    if ( !strcmp( arg2, "guard2" ) )
    {
	clan->guard2 = atoi( argument );
	send_to_char( "Done.\n\r", ch );
	save_clan( clan );
	return;
    }

	 if ( get_trust( ch ) < 54 )
    {
	do_setclan( ch, "" );
	return;
    }
    
    if ( !strcmp( arg2, "type" ) )
    {
	clan->clan_type = atoi( argument );
	send_to_char( "Done.\n\r", ch );
	save_clan( clan );
	return;
    }

    if ( !strcmp( arg2, "name" ) )
    {
	clan->name = str_dup( argument );
	send_to_char( "Done.\n\r", ch );
	save_clan( clan );
	return;
    }

    if ( !strcmp( arg2, "filename" ) )
    {
	clan->filename = str_dup( argument );
	send_to_char( "Done.\n\r", ch );
	save_clan( clan );
	write_clan_list( );
	return;
    }

    if ( !strcmp( arg2, "motto" ) )
    {
	clan->motto = str_dup( argument );
	send_to_char( "Done.\n\r", ch );
	save_clan( clan );
	return;
    }

    if ( !strcmp( arg2, "desc" ) )
    {
	clan->description = str_dup( argument );
	send_to_char( "Done.\n\r", ch );
	save_clan( clan );
	return;
    }

    do_setclan( ch, "" );
    return;
}

void do_showclan( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    CLAN_DATA *clan;

    if ( IS_NPC( ch ) )
    {
	send_to_char( "Huh?\n\r", ch );
	return;
    }

    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Usage: showclan <clan>\n\r", ch );
	return;
    }

    clan = get_clan( arg );
    if ( !clan )
    {
	send_to_char( "No such clan.\n\r", ch );
	return;
    }

    sprintf( buf, "Clan    : %s\n\rFilename: %s\n\rMotto   : %s\n\r",
    			clan->name,
    			clan->filename,
    			clan->motto );
    send_to_char( buf, ch );
    sprintf( buf, "Description: %s\n\rDeity: %s\n\rLeader: %s\n\r",
    			clan->description,
    			clan->deity,
    			clan->leader );
    send_to_char( buf, ch );
    sprintf( buf, "Number1: %s\n\rNumber2: %s\n\rPKills: %6d    PDeaths: %6d\n\r",
    			clan->number1,
    			clan->number2,
    			clan->pkills,
    			clan->pdeaths );
    send_to_char( buf, ch );
    sprintf( buf, "MKills: %6d    MDeaths: %6d\n\r",
    			clan->mkills,
    			clan->mdeaths );
    send_to_char( buf, ch );
    sprintf( buf, "IllegalPK: %-6d Score: %d\n\r",
    			clan->illegal_pk,
    			clan->score );
    send_to_char( buf, ch );
    sprintf( buf, "Type: %d    Favour: %6d  Strikes: %d\n\r",
    			clan->clan_type,
    			clan->favour,
    			clan->strikes );
    send_to_char( buf, ch );
    sprintf( buf, "Members: %3d   Alignment: %d\n\r",
    			clan->members,
    			clan->alignment );
    send_to_char( buf, ch );
    sprintf( buf, "Board: %5d     Object1: %5d Object2: %5d Object3: %5d\n\r",
    			clan->board,
    			clan->clanobj1,
    			clan->clanobj2,
    			clan->clanobj3 );
    send_to_char( buf, ch );
    sprintf( buf, "Recall: %5d  Storeroom: %5d  Guard1: %5d  Guard2: %5d\n\r",
    			clan->recall,
    			clan->storeroom,
    			clan->guard1,
    			clan->guard2 );
    send_to_char( buf, ch );
    return;
}

void do_makeclan( CHAR_DATA *ch, char *argument )
{
    char filename[256];
    CLAN_DATA *clan;
    bool found;

    if ( !argument || argument[0] == '\0' )
    {
	send_to_char( "Usage: makeclan <clan name>\n\r", ch );
	return;
    }

    found = FALSE;
	 sprintf( filename, "%s%s%s", CLAN_DIR, "/", argument );

		  if ( !first_clan )
      first_clan	= clan;
    else
      last_clan->next	= clan;
    clan->next		= NULL;
    clan->prev		= last_clan;
    last_clan		= clan;
    clan->name		= str_dup( argument );
    clan->deity		= str_dup( "" );
    clan->leader	= str_dup( "" );
    clan->number1	= str_dup( "" );
    clan->number2	= str_dup( "" );
}

void do_clans( CHAR_DATA *ch, char *argument )
{
    CLAN_DATA *clan;
    char buf[MAX_STRING_LENGTH];

    if ( !first_clan )
    {
	send_to_char( "There are no clans.\n\r", ch );
	return;
    }

    set_char_color( AT_NOTE, ch );
    send_to_char( "Name             Deity          Leader          PKills     MKills\n\r", ch );
    for ( clan = first_clan; clan; clan = clan->next )
    {
	sprintf( buf, "%-16s %-14s %-14s %7d     %d\n\r", clan->name,
	  clan->deity, clan->leader, clan->pkills, clan->mkills );
	send_to_char( buf, ch );
    }
}
