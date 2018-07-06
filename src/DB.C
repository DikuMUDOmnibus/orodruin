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
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"

#if !defined(macintosh)
extern	int	_filbuf		args( (FILE *) );
#endif

/*
 * Globals.
 */
HELP_DATA *		help_first;
HELP_DATA *		help_last;

SHOP_DATA *		shop_first;
SHOP_DATA *		shop_last;

CHAR_DATA *		char_free;
EXTRA_DESCR_DATA *	extra_descr_free;
NOTE_DATA *		note_free;
OBJ_DATA *		obj_free;
PC_DATA *		pcdata_free;

char			bug_buf		[2*MAX_INPUT_LENGTH];
CHAR_DATA *		char_list;
char *			help_greeting;
char			log_buf		[2*MAX_INPUT_LENGTH];
KILL_DATA		kill_table	[MAX_LEVEL];
NOTE_DATA *		note_list;
OBJ_DATA *		object_list;
TIME_INFO_DATA		time_info;
WEATHER_DATA		weather_info;
char *                  down_time;
char *                  warning1;
char *                  warning2;

sh_int			gsn_backstab;
sh_int			gsn_dodge;
sh_int			gsn_hide;
sh_int			gsn_peek;
sh_int			gsn_pick_lock;
sh_int			gsn_sneak;
sh_int			gsn_steal;
int            gsn_poison_weapon;

int            gsn_bash;
sh_int  	   	gsn_berserk;
sh_int			gsn_disarm;
sh_int			gsn_enhanced_damage;
sh_int			gsn_kick;
sh_int			gsn_parry;
sh_int			gsn_rescue;
sh_int			gsn_second_attack;
sh_int			gsn_third_attack;

sh_int			gsn_blindness;
sh_int			gsn_charm_person;
sh_int			gsn_curse;
sh_int			gsn_invis;
sh_int			gsn_mass_invis;
sh_int			gsn_poison;
sh_int			gsn_sleep;
sh_int         gsn_hunt;

int                     gsn_sset;

/*
 * Psionicist gsn's.
 */
int                     gsn_chameleon;
int                     gsn_domination;
int                     gsn_heighten;
int                     gsn_shadow;





/*
 * Locals.
 */
MOB_INDEX_DATA *	mob_index_hash		[MAX_KEY_HASH];
OBJ_INDEX_DATA *	obj_index_hash		[MAX_KEY_HASH];
ROOM_INDEX_DATA *	room_index_hash		[MAX_KEY_HASH];
char *			string_hash		[MAX_KEY_HASH];

AREA_DATA *		area_first;
AREA_DATA *		area_last;

char *			string_space;
char *			top_string;
char			str_empty	[1];

int			top_affect;
int			top_area;
int			top_ed;
int			top_exit;
int			top_help;
int			top_mob_index;
int			top_obj_index;
int			top_reset;
int			top_room;
int			top_shop;

/*
 * MOBprogram locals
*/

int 		mprog_name_to_type	args( ( char* name ) );
MPROG_DATA *	mprog_file_read	args( ( char* f, MPROG_DATA* mprg, 
                                        MOB_INDEX_DATA *pMobIndex ) );
void		load_mobprogs           args( ( FILE* fp ) );
void   		mprog_read_programs     args( ( FILE* fp,
					MOB_INDEX_DATA *pMobIndex ) );

// Windows version locals
int fIgnoreUnconnected = TRUE;


/*
 * Memory management.
 * Increase MAX_STRING if you have too.
 * Tune the others only if you understand what you're doing.
 */
#define			MAX_STRING	1500000
#define			MAX_PERM_BLOCK	131072
#define			MAX_MEM_LIST	11

void *			rgFreeList	[MAX_MEM_LIST];
const int		rgSizeList	[MAX_MEM_LIST]	=
{
    16, 32, 64, 128, 256, 1024, 2048, 4096, 8192, 16384, 32768-64
};

int			nAllocString;
int			sAllocString;
int			nAllocPerm;
int			sAllocPerm;



/*
 * Semi-locals.
 */
bool			fBootDb;
FILE *			fpArea;
char			strArea[MAX_INPUT_LENGTH];



/*
 * Local booting procedures.
 */
void	init_mm		args( ( void ) );

void	load_area	args( ( FILE *fp ) );
void	load_helps	args( ( FILE *fp ) );
void	load_mobiles	args( ( FILE *fp ) );
void	load_objects	args( ( FILE *fp ) );
void	load_resets	args( ( FILE *fp ) );
void	load_rooms	args( ( FILE *fp ) );
void	load_shops	args( ( FILE *fp ) );
void	load_specials	args( ( FILE *fp ) );
void	load_notes	args( ( void ) );

void	fix_exits	args( ( void ) );

void	reset_area	args( ( AREA_DATA * pArea ) );



/*
 * Big mama top level function.
 */
void boot_db( void )
{
	 /*
	  * Init some data space stuff.
	  */
	 {
	if ( ( string_space = calloc( 1, MAX_STRING ) ) == NULL )
	{
		 bug( "Boot_db: can't alloc %d string space.", MAX_STRING );
		/* exit( 1 ); */
	}
	top_string	= string_space;
	fBootDb		= TRUE;
	 }

	 /*
	  * Init random number generator.
	  */
	 {
	init_mm( );
	 }

	 /*
	  * Set time and weather.
	  */
	 {
	long lhour, lday, lmonth;

	lhour		= (current_time - 650336715)
			/ (PULSE_TICK / PULSE_PER_SECOND);
	time_info.hour	= lhour  % 24;
	lday		= lhour  / 24;
	time_info.day	= lday   % 35;
	lmonth		= lday   / 35;
	time_info.month	= lmonth % 17;
	time_info.year	= lmonth / 17;

		  if ( time_info.hour <  5 ) weather_info.sunlight = SUN_DARK;
	else if ( time_info.hour <  6 ) weather_info.sunlight = SUN_RISE;
	else if ( time_info.hour < 19 ) weather_info.sunlight = SUN_LIGHT;
	else if ( time_info.hour < 20 ) weather_info.sunlight = SUN_SET;
	else                            weather_info.sunlight = SUN_DARK;

	weather_info.change	= 0;
	weather_info.mmhg	= 960;
	if ( time_info.month >= 7 && time_info.month <=12 )
		 weather_info.mmhg += number_range( 1, 50 );
	else
		 weather_info.mmhg += number_range( 1, 80 );

		  if ( weather_info.mmhg <=  980 ) weather_info.sky = SKY_LIGHTNING;
	else if ( weather_info.mmhg <= 1000 ) weather_info.sky = SKY_RAINING;
	else if ( weather_info.mmhg <= 1020 ) weather_info.sky = SKY_CLOUDY;
	else                                  weather_info.sky = SKY_CLOUDLESS;

	 }

	 /*
	  * Assign gsn's for skills which have them.
	  */
	 {
	int sn;

	for ( sn = 0; sn < MAX_SKILL; sn++ )
	{
		 if ( skill_table[sn].pgsn != NULL )
		*skill_table[sn].pgsn = sn;
	}
	 }

	 /*
	  * Read in all the area files.
	  */
	 {
	FILE *fpList;

	if ( ( fpList = fopen( AREA_LIST, "r" ) ) == NULL )
	{
		 perror( AREA_LIST );
		 exit( 1 );
	}

	for ( ; ; )
	{
		 strcpy( strArea, fread_word( fpList ) );
		 if ( strArea[0] == '$' )
		break;

		 if ( strArea[0] == '-' )
		 {
		fpArea = stdin;
		 }
		 else
		 {
		if ( ( fpArea = fopen( strArea, "r" ) ) == NULL )
		{
			 perror( strArea );
			 exit( 1 );
		}
		 }

		 for ( ; ; )
		 {
		char *word;

		if ( fread_letter( fpArea ) != '#' )
		{
			 bug( "Boot_db: # not found.", 0 );
			 exit( 1 );
		}

		word = fread_word( fpArea );

			  if ( word[0] == '$'               )                 break;
		else if ( !str_cmp( word, "AREA"     ) ) load_area    (fpArea);
		else if ( !str_cmp( word, "HELPS"    ) ) load_helps   (fpArea);
		else if ( !str_cmp( word, "MOBILES"  ) ) load_mobiles (fpArea);
		else if ( !str_cmp( word, "MOBPROGS" ) ) load_mobprogs(fpArea);
		else if ( !str_cmp( word, "OBJECTS"  ) ) load_objects (fpArea);
		else if ( !str_cmp( word, "RESETS"   ) ) load_resets  (fpArea);
		else if ( !str_cmp( word, "ROOMS"    ) ) load_rooms   (fpArea);
		else if ( !str_cmp( word, "SHOPS"    ) ) load_shops   (fpArea);
		else if ( !str_cmp( word, "SPECIALS" ) ) load_specials(fpArea);
		else
		{
			 bug( "Boot_db: bad section name.", 0 );
			 exit( 1 );
		}
		 }

		 if ( fpArea != stdin )
		fclose( fpArea );
		 fpArea = NULL;
	}
	fclose( fpList );
	 }

	 /*
	  * Fix up exits.
	  * Declare db booting over.
	  * Reset all areas once.
	  * Load up the notes file.
	  * Set the MOBtrigger.
	  */
	 {
	fix_exits( );
	fBootDb	= FALSE;
	area_update( );
	load_notes( );
	log_string( "Loading clans" );
	load_clans( );
	MOBtrigger = TRUE;
	 }

	 return;
}



/*
 * Snarf an 'area' header line.
 */
void load_area( FILE *fp )
{
	 AREA_DATA *pArea;

	 pArea		= alloc_perm( sizeof(*pArea) );
	 pArea->reset_first	= NULL;
	 pArea->reset_last	= NULL;
	 pArea->name		= fread_string( fp );
    pArea->age		= 15;
    pArea->nplayer	= 0;

    if ( area_first == NULL )
	area_first = pArea;
    if ( area_last  != NULL )
	area_last->next = pArea;
    area_last	= pArea;
    pArea->next	= NULL;

    top_area++;
    return;
}



/*
 * Snarf a help section.
 */
void load_helps( FILE *fp )
{
    HELP_DATA *pHelp;

    for ( ; ; )
    {
	pHelp		= alloc_perm( sizeof(*pHelp) );
	pHelp->level	= fread_number( fp );
	pHelp->keyword	= fread_string( fp );
	if ( pHelp->keyword[0] == '$' )
	    break;
	pHelp->text	= fread_string( fp );

	if ( !str_cmp( pHelp->keyword, "greeting" ) )
	    help_greeting = pHelp->text;

	if ( help_first == NULL )
	    help_first = pHelp;
	if ( help_last  != NULL )
	    help_last->next = pHelp;

	help_last	= pHelp;
	pHelp->next	= NULL;
	top_help++;
    }

    return;
}



/*
 * Snarf a mob section.
 */
void load_mobiles( FILE *fp )
{
    MOB_INDEX_DATA *pMobIndex;

    for ( ; ; )
    {
	sh_int vnum;
	char letter;
	int iHash;

	letter				= fread_letter( fp );
	if ( letter != '#' )
	{
	    bug( "Load_mobiles: # not found.", 0 );
	    exit( 1 );
	}

	vnum				= fread_number( fp );
	if ( vnum == 0 )
	    break;

	fBootDb = FALSE;
	if ( get_mob_index( vnum ) != NULL )
	{
	    bug( "Load_mobiles: vnum %d duplicated.", vnum );
	    exit( 1 );
	}
	fBootDb = TRUE;

	pMobIndex			= alloc_perm( sizeof(*pMobIndex) );
	pMobIndex->vnum			= vnum;
	pMobIndex->player_name		= fread_string( fp );
	pMobIndex->short_descr		= fread_string( fp );
	pMobIndex->long_descr		= fread_string( fp );
	pMobIndex->description		= fread_string( fp );

	pMobIndex->long_descr[0]	= UPPER(pMobIndex->long_descr[0]);
	pMobIndex->description[0]	= UPPER(pMobIndex->description[0]);

	pMobIndex->act			= fread_number( fp ) | ACT_IS_NPC;
	pMobIndex->affected_by		= fread_number( fp );
	pMobIndex->pShop		= NULL;
	pMobIndex->alignment		= fread_number( fp );
	letter				= fread_letter( fp );
	pMobIndex->level		= number_fuzzy( fread_number( fp ) );

	/*
	 * The unused stuff is for imps who want to use the old-style
	 * stats-in-files method.
	 */
	pMobIndex->hitroll		= fread_number( fp );	/* Unused */
	pMobIndex->ac			= fread_number( fp );	/* Unused */
	pMobIndex->hitnodice		= fread_number( fp );	/* Unused */
	/* 'd'		*/		  fread_letter( fp );	/* Unused */
	pMobIndex->hitsizedice		= fread_number( fp );	/* Unused */
	/* '+'		*/		  fread_letter( fp );	/* Unused */
	pMobIndex->hitplus		= fread_number( fp );	/* Unused */
	pMobIndex->damnodice		= fread_number( fp );	/* Unused */
	/* 'd'		*/		  fread_letter( fp );	/* Unused */
	pMobIndex->damsizedice		= fread_number( fp );	/* Unused */
	/* '+'		*/		  fread_letter( fp );	/* Unused */
	pMobIndex->damplus		= fread_number( fp );	/* Unused */
	pMobIndex->gold			= fread_number( fp );	/* Unused */
	/* xp can't be used! */		  fread_number( fp );	/* Unused */
	/* position	*/		  fread_number( fp );	/* Unused */
	/* start pos	*/		  fread_number( fp );	/* Unused */

	/*
	 * Back to meaningful values.
	 */
	pMobIndex->sex			= fread_number( fp );
   pMobIndex->cargo     = 0; // @@@ ### Needed?

	if ( letter != 'S' )
	{
	    bug( "Load_mobiles: vnum %d non-S.", vnum );
	    exit( 1 );
	}

	letter=fread_letter(fp);
	if (letter=='>')
	{
	    ungetc(letter,fp);
	    mprog_read_programs(fp,pMobIndex);
	}
	else ungetc(letter,fp);
	iHash			= vnum % MAX_KEY_HASH;
	pMobIndex->next		= mob_index_hash[iHash];
	mob_index_hash[iHash]	= pMobIndex;
	top_mob_index++;
	kill_table[URANGE(0, pMobIndex->level, MAX_LEVEL-1)].number++;
    }

    return;
}



/*
 * Snarf an obj section.
 */
void load_objects( FILE *fp )
{
    OBJ_INDEX_DATA *pObjIndex;

    for ( ; ; )
    {
	sh_int vnum;
	char letter;
	int iHash;

	letter				= fread_letter( fp );
	if ( letter != '#' )
	{
	    bug( "Load_objects: # not found.", 0 );
	    exit( 1 );
	}

	vnum				= fread_number( fp );
	if ( vnum == 0 )
	    break;

	fBootDb = FALSE;
	if ( get_obj_index( vnum ) != NULL )
	{
	    bug( "Load_objects: vnum %d duplicated.", vnum );
	    exit( 1 );
	}
	fBootDb = TRUE;

	pObjIndex			= alloc_perm( sizeof(*pObjIndex) );
	pObjIndex->vnum			= vnum;
	pObjIndex->name			= fread_string( fp );
	pObjIndex->short_descr		= fread_string( fp );
	pObjIndex->description		= fread_string( fp );
	/* Action description */	  fread_string( fp );

	pObjIndex->short_descr[0]	= LOWER(pObjIndex->short_descr[0]);
	pObjIndex->description[0]	= UPPER(pObjIndex->description[0]);

	pObjIndex->item_type		= fread_number( fp );
	pObjIndex->extra_flags		= fread_number( fp );
	pObjIndex->wear_flags		= fread_number( fp );
	pObjIndex->value[0]		= fread_number( fp );
	pObjIndex->value[1]		= fread_number( fp );
	pObjIndex->value[2]		= fread_number( fp );
	pObjIndex->value[3]		= fread_number( fp );
	pObjIndex->weight		= fread_number( fp );
	pObjIndex->cost			= fread_number( fp );	/* Unused */
	/* Cost per day */		  fread_number( fp );

	if ( pObjIndex->item_type == ITEM_POTION )
	    SET_BIT(pObjIndex->extra_flags, ITEM_NODROP);

	for ( ; ; )
	{
	    char letter;

	    letter = fread_letter( fp );

	    if ( letter == 'A' )
	    {
		AFFECT_DATA *paf;

		paf			= alloc_perm( sizeof(*paf) );
		paf->type		= -1;
		paf->duration		= -1;
		paf->location		= fread_number( fp );
		paf->modifier		= fread_number( fp );
		paf->bitvector		= 0;
		paf->next		= pObjIndex->affected;
		pObjIndex->affected	= paf;
		top_affect++;
	    }

	    else if ( letter == 'E' )
	    {
		EXTRA_DESCR_DATA *ed;

		ed			= alloc_perm( sizeof(*ed) );
		ed->keyword		= fread_string( fp );
		ed->description		= fread_string( fp );
		ed->next		= pObjIndex->extra_descr;
		pObjIndex->extra_descr	= ed;
		top_ed++;
	    }

	    else
	    {
		ungetc( letter, fp );
		break;
	    }
	}

	/*
	 * Translate spell "slot numbers" to internal "skill numbers."
	 */
	switch ( pObjIndex->item_type )
	{
	case ITEM_PILL:
	case ITEM_POTION:
	case ITEM_SCROLL:
	    pObjIndex->value[1] = slot_lookup( pObjIndex->value[1] );
	    pObjIndex->value[2] = slot_lookup( pObjIndex->value[2] );
	    pObjIndex->value[3] = slot_lookup( pObjIndex->value[3] );
	    break;

	case ITEM_STAFF:
	case ITEM_WAND:
	    pObjIndex->value[3] = slot_lookup( pObjIndex->value[3] );
	    break;
	}

	iHash			= vnum % MAX_KEY_HASH;
	pObjIndex->next		= obj_index_hash[iHash];
	obj_index_hash[iHash]	= pObjIndex;
	top_obj_index++;
    }

    return;
}



/*
 * Snarf a reset section.
 */
void load_resets( FILE *fp )
{
    RESET_DATA *pReset;

    if ( area_last == NULL )
    {
	bug( "Load_resets: no #AREA seen yet.", 0 );
	exit( 1 );
    }

    for ( ; ; )
    {
	ROOM_INDEX_DATA *pRoomIndex;
	EXIT_DATA *pexit;
	char letter;

	if ( ( letter = fread_letter( fp ) ) == 'S' )
	    break;

	if ( letter == '*' )
	{
	    fread_to_eol( fp );
	    continue;
	}

	pReset		= alloc_perm( sizeof(*pReset) );
	pReset->command	= letter;
	/* if_flag */	  fread_number( fp );
	pReset->arg1	= fread_number( fp );
	pReset->arg2	= fread_number( fp );
	pReset->arg3	= (letter == 'G' || letter == 'R' || letter == 'C' || letter == 'F')
			    ? 0 : fread_number( fp );
			  fread_to_eol( fp );

	/*
	 * Validate parameters.
	 * We're calling the index functions for the side effect.
	 */
	switch ( letter )
	{
	default:
	    bug( "Load_resets: bad command '%c'.", letter );
	    exit( 1 );
	    break;

	case 'M':
	    get_mob_index  ( pReset->arg1 );
	    get_room_index ( pReset->arg3 );
	    break;

	case 'O':
	    get_obj_index  ( pReset->arg1 );
	    get_room_index ( pReset->arg3 );
	    break;

	case 'P':
	    get_obj_index  ( pReset->arg1 );
	    get_obj_index  ( pReset->arg3 );
	    break;

	case 'G':
	case 'E':
	    get_obj_index  ( pReset->arg1 );
	    break;

	case 'D':
	    pRoomIndex = get_room_index( pReset->arg1 );

	    if ( pReset->arg2 < 0
	    ||   pReset->arg2 > 5
	    || ( pexit = pRoomIndex->exit[pReset->arg2] ) == NULL
	    || !IS_SET( pexit->exit_info, EX_ISDOOR ) )
	    {
		bug( "Load_resets: 'D': exit %d not door.", pReset->arg2 );
		exit( 1 );
	    }

	    if ( pReset->arg3 < 0 || pReset->arg3 > 2 )
	    {
		bug( "Load_resets: 'D': bad 'locks': %d.", pReset->arg3 );
		exit( 1 );
	    }

	    break;

	case 'R':
	    /*pRoomIndex		=*/ get_room_index( pReset->arg1 );

	    if ( pReset->arg2 < 0 || pReset->arg2 > 6 )
	    {
		bug( "Load_resets: 'R': bad exit %d.", pReset->arg2 );
		exit( 1 );
	    }

	    break;
   case 'C':
   case 'F':
      break;
	}

	if ( area_last->reset_first == NULL )
	    area_last->reset_first	= pReset;
	if ( area_last->reset_last  != NULL )
	    area_last->reset_last->next	= pReset;
	    
	area_last->reset_last	= pReset;
	pReset->next		= NULL;
	top_reset++;
    }

    return;
}



/*
 * Snarf a room section.
 */
void load_rooms( FILE *fp )
{
    ROOM_INDEX_DATA *pRoomIndex;

    if ( area_last == NULL )
    {
	bug( "Load_resets: no #AREA seen yet.", 0 );
	exit( 1 );
    }

    for ( ; ; )
    {
	sh_int vnum;
	char letter;
	int door;
	int iHash;

	letter				= fread_letter( fp );
	if ( letter != '#' )
	{
	    bug( "Load_rooms: # not found.", 0 );
	    exit( 1 );
	}

	vnum				= fread_number( fp );
	if ( vnum == 0 )
	    break;

	fBootDb = FALSE;
	if ( get_room_index( vnum ) != NULL )
	{
	    bug( "Load_rooms: vnum %d duplicated.", vnum );
	    exit( 1 );
	}
	fBootDb = TRUE;

	pRoomIndex			= alloc_perm( sizeof(*pRoomIndex) );
	pRoomIndex->people		= NULL;
	pRoomIndex->contents		= NULL;
	pRoomIndex->extra_descr		= NULL;
	pRoomIndex->area		= area_last;
	pRoomIndex->vnum		= vnum;
	pRoomIndex->name		= fread_string( fp );
	pRoomIndex->description		= fread_string( fp );
	/* Area number */		  fread_number( fp );
	pRoomIndex->room_flags		= fread_number( fp );
	pRoomIndex->sector_type		= fread_number( fp );
	pRoomIndex->light		= 0;
	for ( door = 0; door <= 5; door++ )
	    pRoomIndex->exit[door] = NULL;

	for ( ; ; )
	{
	    letter = fread_letter( fp );

	    if ( letter == 'S' )
		break;

	    if ( letter == 'D' )
	    {
		EXIT_DATA *pexit;
		int locks;

		door = fread_number( fp );
		if ( door < 0 || door > 5 )
		{
		    bug( "Fread_rooms: vnum %d has bad door number.", vnum );
		    exit( 1 );
		}

		pexit			= alloc_perm( sizeof(*pexit) );
		pexit->description	= fread_string( fp );
		pexit->keyword		= fread_string( fp );
		pexit->exit_info	= 0;
		locks			= fread_number( fp );
		pexit->key		= fread_number( fp );
		pexit->vnum		= fread_number( fp );

		switch ( locks )
		{
		case 1: pexit->exit_info = EX_ISDOOR;                break;
		case 2: pexit->exit_info = EX_ISDOOR | EX_PICKPROOF; break;
		}

		pRoomIndex->exit[door]	= pexit;
		top_exit++;
	    }
	    else if ( letter == 'E' )
	    {
		EXTRA_DESCR_DATA *ed;

		ed			= alloc_perm( sizeof(*ed) );
		ed->keyword		= fread_string( fp );
		ed->description		= fread_string( fp );
		ed->next		= pRoomIndex->extra_descr;
		pRoomIndex->extra_descr	= ed;
		top_ed++;
	    }
	    else
	    {
		bug( "Load_rooms: vnum %d has flag not 'DES'.", vnum );
		exit( 1 );
	    }
	}

	iHash			= vnum % MAX_KEY_HASH;
	pRoomIndex->next	= room_index_hash[iHash];
	room_index_hash[iHash]	= pRoomIndex;
	top_room++;
    }

    return;
}



/*
 * Snarf a shop section.
 */
void load_shops( FILE *fp )
{
    SHOP_DATA *pShop;

    for ( ; ; )
    {
	MOB_INDEX_DATA *pMobIndex;
	int iTrade;

	pShop			= alloc_perm( sizeof(*pShop) );
	pShop->keeper		= fread_number( fp );
	if ( pShop->keeper == 0 )
	    break;
	for ( iTrade = 0; iTrade < MAX_TRADE; iTrade++ )
	    pShop->buy_type[iTrade]	= fread_number( fp );
	pShop->profit_buy	= fread_number( fp );
	pShop->profit_sell	= fread_number( fp );
	pShop->open_hour	= fread_number( fp );
	pShop->close_hour	= fread_number( fp );
				  fread_to_eol( fp );
	pMobIndex		= get_mob_index( pShop->keeper );
	pMobIndex->pShop	= pShop;

	if ( shop_first == NULL )
	    shop_first = pShop;
	if ( shop_last  != NULL )
	    shop_last->next = pShop;

	shop_last	= pShop;
	pShop->next	= NULL;
	top_shop++;
    }

    return;
}



/*
 * Snarf spec proc declarations.
 */
void load_specials( FILE *fp )
{
    for ( ; ; )
    {
	MOB_INDEX_DATA *pMobIndex;
	char letter;

	switch ( letter = fread_letter( fp ) )
	{
	default:
	    bug( "Load_specials: letter '%c' not *MS.", letter );
	    exit( 1 );

	case 'S':
	    return;

	case '*':
	    break;

	case 'M':
	    pMobIndex		= get_mob_index	( fread_number ( fp ) );
	    pMobIndex->spec_fun	= spec_lookup	( fread_word   ( fp ) );
	    if ( pMobIndex->spec_fun == 0 )
	    {
		bug( "Load_specials: 'M': vnum %d.", pMobIndex->vnum );
		exit( 1 );
	    }
	    break;
	}

	fread_to_eol( fp );
    }
}



/*
 * Snarf notes file.
 */
void load_notes( void )
{
    FILE *fp;
    NOTE_DATA *pnotelast;

    if ( ( fp = fopen( NOTE_FILE, "r" ) ) == NULL )
	return;

    pnotelast = NULL;
    for ( ; ; )
    {
	NOTE_DATA *pnote;
	char letter;

	do
	{
	    letter = getc( fp );
	    if ( feof(fp) )
	    {
		fclose( fp );
		return;
	    }
	}
	while ( isspace(letter) );
	ungetc( letter, fp );

	pnote		  = alloc_perm( sizeof(*pnote) );

	if ( str_cmp( fread_word( fp ), "sender" ) )
	    break;
	pnote->sender	  = fread_string( fp );

	if ( str_cmp( fread_word( fp ), "date" ) )
	    break;
	pnote->date	  = fread_string( fp );

	if ( str_cmp( fread_word( fp ), "stamp" ) )
	    break;
	pnote->date_stamp = fread_number( fp );

	if ( str_cmp( fread_word( fp ), "to" ) )
	    break;
	pnote->to_list	  = fread_string( fp );

	if ( str_cmp( fread_word( fp ), "subject" ) )
	    break;
	pnote->subject	  = fread_string( fp );

	if ( str_cmp( fread_word( fp ), "text" ) )
	    break;
	pnote->text	  = fread_string( fp );

	if ( note_list == NULL )
	    note_list		= pnote;
	else
	    pnotelast->next	= pnote;

	pnotelast	  = pnote;
    }

    strcpy( strArea, NOTE_FILE );
    fpArea = fp;
    bug( "Load_notes: bad key word.", 0 );
    exit( 1 );
    return;
}

void load_down_time( void )
{
    FILE *fp;

    down_time = str_dup ( "*" );
    warning1  = str_dup ( "*" );
    warning2  = str_dup ( "*" );

    if ( !( fp = fopen( DOWN_TIME_FILE, "r" ) ) )
        return;

    for ( ; ; )
    {
        char *word;
	char  letter;

	do
	{
	    letter = getc( fp );
	    if ( feof( fp ) )
	    {
		fclose( fp );
		return;
	    }
	}
	while ( isspace( letter ) );
	ungetc( letter, fp );
	
	word = fread_word( fp );

	if ( !str_cmp( word, "DOWNTIME" ) )
	{
	    free_string( down_time );
	    down_time = fread_string( fp );
	}
	if ( !str_cmp( word, "WARNINGA" ) )
	{
	    free_string( warning1 );
	    warning1 = fread_string( fp );
	}
	if ( !str_cmp( word, "WARNINGB" ) )
	{
	    free_string( warning2 );
	    warning2 = fread_string( fp );
	}
    }
}


/*
 * Translate all room exits from virtual to real.
 * Has to be done after all rooms are read in.
 * Check for bad reverse exits.
 */
void fix_exits( void )
{
    extern const sh_int rev_dir [];
    char buf[MAX_STRING_LENGTH];
    ROOM_INDEX_DATA *pRoomIndex;
    ROOM_INDEX_DATA *to_room;
    EXIT_DATA *pexit;
    EXIT_DATA *pexit_rev;
    int iHash;
    int door;

    for ( iHash = 0; iHash < MAX_KEY_HASH; iHash++ )
    {
	for ( pRoomIndex  = room_index_hash[iHash];
	      pRoomIndex != NULL;
	      pRoomIndex  = pRoomIndex->next )
	{
	    bool fexit;

	    fexit = FALSE;
	    for ( door = 0; door <= 5; door++ )
	    {
		if ( ( pexit = pRoomIndex->exit[door] ) != NULL )
		{
		    fexit = TRUE;
		    if ( pexit->vnum <= 0 )
			pexit->to_room = NULL;
		    else
			pexit->to_room = get_room_index( pexit->vnum );
		}
	    }

	    if ( !fexit )
		SET_BIT( pRoomIndex->room_flags, ROOM_NO_MOB );
	}
    }
#if 0
    for ( iHash = 0; iHash < MAX_KEY_HASH; iHash++ )
    {
	for ( pRoomIndex  = room_index_hash[iHash];
	      pRoomIndex != NULL;
	      pRoomIndex  = pRoomIndex->next )
	{
	    for ( door = 0; door <= 5; door++ )
	    {
		if ( ( pexit     = pRoomIndex->exit[door]       ) != NULL
		&&   ( to_room   = pexit->to_room               ) != NULL
		&&   ( pexit_rev = to_room->exit[rev_dir[door]] ) != NULL
		&&   pexit_rev->to_room != pRoomIndex )
		{
		    sprintf( buf, "Fix_exits: %d:%d -> %d:%d -> %d.",
			pRoomIndex->vnum, door,
			to_room->vnum,    rev_dir[door],
			(pexit_rev->to_room == NULL)
			    ? 0 : pexit_rev->to_room->vnum );
		    bug( buf, 0 );
		}
	    }
	}
    }
#endif
    return;
}



/*
 * Repopulate areas periodically.
 */
void area_update( void )
{
    AREA_DATA *pArea;

    for ( pArea = area_first; pArea != NULL; pArea = pArea->next )
    {
	CHAR_DATA *pch;

	if ( ++pArea->age < 3 )
	    continue;

	/*
	 * Check for PC's.
	 */
	if ( pArea->nplayer > 0 && pArea->age == 15 - 1 )
	{
	    for ( pch = char_list; pch != NULL; pch = pch->next )
	    {
		if ( !IS_NPC(pch)
		&&   IS_AWAKE(pch)
		&&   pch->in_room != NULL
		&&   pch->in_room->area == pArea )
		{
		    send_to_char( "The hair on the back of your neck stands on end.\n\r",
			pch );
		}
	    }
	}

	/*
	 * Check age and reset.
	 * Note: Mud School resets every 3 minutes (not 15).
	 */
	if ( pArea->nplayer == 0 || pArea->age >= 15 )
	{
	    ROOM_INDEX_DATA *pRoomIndex;

	    reset_area( pArea );
	    pArea->age = number_range( 0, 3 );
	    pRoomIndex = get_room_index( ROOM_VNUM_SCHOOL );
	    if ( pRoomIndex != NULL && pArea == pRoomIndex->area )
		pArea->age = 15 - 3;
	}
    }

    return;
}



/*
 * Reset one area.
 */
void reset_area( AREA_DATA *pArea )
{
    RESET_DATA *pReset;
    CHAR_DATA *mob;
    bool last;
    int level;

    mob 	= NULL;
    last	= TRUE;
    level	= 0;
    for ( pReset = pArea->reset_first; pReset != NULL; pReset = pReset->next )
    {
	ROOM_INDEX_DATA *pRoomIndex;
	MOB_INDEX_DATA *pMobIndex;
	OBJ_INDEX_DATA *pObjIndex;
	OBJ_INDEX_DATA *pObjToIndex;
	EXIT_DATA *pexit;
	OBJ_DATA *obj;
	OBJ_DATA *obj_to;

	switch ( pReset->command )
	{
	default:
	    bug( "Reset_area: bad command %c.", pReset->command );
	    break;

	case 'M':
	    if ( ( pMobIndex = get_mob_index( pReset->arg1 ) ) == NULL )
	    {
		bug( "Reset_area: 'M': bad vnum %d.", pReset->arg1 );
		continue;
	    }

	    if ( ( pRoomIndex = get_room_index( pReset->arg3 ) ) == NULL )
	    {
		bug( "Reset_area: 'R': bad vnum %d.", pReset->arg3 );
		continue;
	    }

	    level = URANGE( 0, pMobIndex->level - 2, LEVEL_HERO );
	    if ( pMobIndex->count >= pReset->arg2 )
	    {
		last = FALSE;
		break;
	    }

	    mob = create_mobile( pMobIndex );

	    /*
	     * Check for pet shop.
	     */
	    {
		ROOM_INDEX_DATA *pRoomIndexPrev;
		pRoomIndexPrev = get_room_index( pRoomIndex->vnum - 1 );
		if ( pRoomIndexPrev != NULL
		&&   IS_SET(pRoomIndexPrev->room_flags, ROOM_PET_SHOP) )
		    SET_BIT(mob->act, ACT_PET);
	    }

	    if ( room_is_dark( pRoomIndex ) )
		SET_BIT(mob->affected_by, AFF_INFRARED);

	    char_to_room( mob, pRoomIndex );
	    level = URANGE( 0, mob->level - 2, LEVEL_HERO );
	    last  = TRUE;
	    break;

	case 'O':
	    if ( ( pObjIndex = get_obj_index( pReset->arg1 ) ) == NULL )
	    {
		bug( "Reset_area: 'O': bad vnum %d.", pReset->arg1 );
		continue;
	    }

	    if ( ( pRoomIndex = get_room_index( pReset->arg3 ) ) == NULL )
	    {
		bug( "Reset_area: 'R': bad vnum %d.", pReset->arg3 );
		continue;
	    }

	    if ( pArea->nplayer > 0
	    ||   count_obj_list( pObjIndex, pRoomIndex->contents ) > 0 )
	    {
		last = FALSE;
		break;
	    }

	    obj       = create_object( pObjIndex, number_fuzzy( level ) );
	    obj->cost = 0;
	    obj_to_room( obj, pRoomIndex );
	    last = TRUE;
	    break;

	case 'P':
	    if ( ( pObjIndex = get_obj_index( pReset->arg1 ) ) == NULL )
	    {
		bug( "Reset_area: 'P': bad vnum %d.", pReset->arg1 );
		continue;
	    }

	    if ( ( pObjToIndex = get_obj_index( pReset->arg3 ) ) == NULL )
	    {
		bug( "Reset_area: 'P': bad vnum %d.", pReset->arg3 );
		continue;
	    }

	    if ( pArea->nplayer > 0
	    || ( obj_to = get_obj_type( pObjToIndex ) ) == NULL
	    ||   count_obj_list( pObjIndex, obj_to->contains ) > 0 )
	    {
		last = FALSE;
		break;
	    }

	    obj = create_object( pObjIndex, number_fuzzy( obj_to->level ) );
	    obj_to_obj( obj, obj_to );
	    last = TRUE;
	    break;

	case 'G':
	case 'E':
	    if ( ( pObjIndex = get_obj_index( pReset->arg1 ) ) == NULL )
	    {
		bug( "Reset_area: 'E' or 'G': bad vnum %d.", pReset->arg1 );
		continue;
	    }

	    if ( !last )
		break;

	    if ( mob == NULL )
	    {
		bug( "Reset_area: 'E' or 'G': null mob for vnum %d.",
		    pReset->arg1 );
		last = FALSE;
		break;
	    }

	    if ( mob->pIndexData->pShop != NULL )
	    {
		int olevel;

		switch ( pObjIndex->item_type )
		{
		default:		olevel = 0;                      break;
		case ITEM_PILL:		olevel = number_range(  0, 10 ); break;
		case ITEM_POTION:	olevel = number_range(  0, 10 ); break;
		case ITEM_SCROLL:	olevel = number_range(  5, 15 ); break;
		case ITEM_WAND:		olevel = number_range( 10, 20 ); break;
		case ITEM_STAFF:	olevel = number_range( 15, 25 ); break;
		case ITEM_ARMOR:	olevel = number_range(  5, 15 ); break;
		case ITEM_WEAPON:	olevel = number_range(  5, 15 ); break;
		}

		obj = create_object( pObjIndex, olevel );
		SET_BIT( obj->extra_flags, ITEM_INVENTORY );
	    }
	    else
	    {
		obj = create_object( pObjIndex, number_fuzzy( level ) );
	    }
	    obj_to_char( obj, mob );
	    if ( pReset->command == 'E' )
		equip_char( mob, obj, pReset->arg3 );
	    last = TRUE;
	    break;

	case 'D':
	    if ( ( pRoomIndex = get_room_index( pReset->arg1 ) ) == NULL )
	    {
		bug( "Reset_area: 'D': bad vnum %d.", pReset->arg1 );
		continue;
	    }

	    if ( ( pexit = pRoomIndex->exit[pReset->arg2] ) == NULL )
		break;

	    switch ( pReset->arg3 )
	    {
	    case 0:
		REMOVE_BIT( pexit->exit_info, EX_CLOSED );
		REMOVE_BIT( pexit->exit_info, EX_LOCKED );
		break;

	    case 1:
		SET_BIT(    pexit->exit_info, EX_CLOSED );
		REMOVE_BIT( pexit->exit_info, EX_LOCKED );
		break;

	    case 2:
		SET_BIT(    pexit->exit_info, EX_CLOSED );
		SET_BIT(    pexit->exit_info, EX_LOCKED );
		break;
	    }

	    last = TRUE;
	    break;

	case 'R':
	    if ( ( pRoomIndex = get_room_index( pReset->arg1 ) ) == NULL )
	    {
		bug( "Reset_area: 'R': bad vnum %d.", pReset->arg1 );
		continue;
	    }

	    {
		int d0;
		int d1;

		for ( d0 = 0; d0 < pReset->arg2 - 1; d0++ )
		{
		    d1                   = number_range( d0, pReset->arg2-1 );
		    pexit                = pRoomIndex->exit[d0];
		    pRoomIndex->exit[d0] = pRoomIndex->exit[d1];
		    pRoomIndex->exit[d1] = pexit;
		}
	    }
	    break;
   case 'C':
   case 'F':
      break;
	}
    }

    return;
}

#define nelems(a) (sizeof (a)/sizeof (a)[0])

// Calculate a meaningful modifier and amount
void random_apply( OBJ_DATA *obj, CHAR_DATA *mob )
{
   static int attrib_types[] = { APPLY_STR, APPLY_DEX, APPLY_DEX, APPLY_INT,
      APPLY_INT, APPLY_WIS, APPLY_CON, APPLY_CON, APPLY_CON };
   static int power_types[] = { APPLY_MANA, APPLY_HIT, APPLY_MOVE, APPLY_AC };
   static int combat_types[] = { APPLY_HITROLL, APPLY_HITROLL, APPLY_DAMROLL,
      APPLY_SAVING_SPELL, APPLY_SAVING_SPELL, APPLY_SAVING_BREATH };

   AFFECT_DATA *paf = alloc_perm( sizeof(*paf) );
   paf->type	     = -1;
   paf->duration  = -1;
   paf->bitvector = 0;
   paf->next	     = obj->affected;
   obj->affected  = paf;
   switch (number_bits(2)) {
   case 0:
      paf->location  = attrib_types[number_range(0, nelems(attrib_types)-1)];
      paf->modifier  = 1;
      break;
   case 1:
      paf->location  = power_types[number_range(0, nelems(power_types)-1)];
      paf->modifier  = number_range(mob->level/2, mob->level);
      break;
   case 2:
   case 3:
      paf->location  = combat_types[number_range(0, nelems(combat_types)-1)];
      paf->modifier  = number_range(1, mob->level/6+1);
      break;
   }

   SET_BIT(obj->extra_flags, ITEM_MAGIC);

   // Is item cursed?
   if (number_percent() <= 5)
      {
      paf->modifier = -paf->modifier;
      SET_BIT(obj->extra_flags, ITEM_NODROP);
      if (number_percent() <= 15)
         SET_BIT(obj->extra_flags, ITEM_NOREMOVE);
      }
}

// Jewelry stuff
static char *adj1[] = { "splendid", "ancient", "dusty", "scratched",
   "flawed", "burnt", "heavy", "gilded", "spooky", "flaming", "plain",
   "ornate", "inscrutable", "obscene", "wrinkled" };
static char *adj2[] = { "diamond", "emerald", "topaz", "wooden", "jade",
   "white gold", "onyx", "tin", "glass", "marble", "black", "granite" };

#define MASK_IGNORE     (1<<TAR_IGNORE)
#define MASK_OFFENSIVE  (1<<TAR_CHAR_OFFENSIVE)
#define MASK_DEFENSIVE  (1<<TAR_CHAR_DEFENSIVE)
#define MASK_SELF       (1<<TAR_CHAR_SELF)
#define MASK_INV        (1<<TAR_OBJ_INV)

#define CLASS_MAGE       0
#define CLASS_CLERIC     1
#define CLASS_RANGER     5
// Returns a clerical or magical spell of the appropriate (masked) type
int random_spell( int level, int mask, sh_int *type )
{
	extern const	struct skill_type skill_table[MAX_SKILL];

	for ( ;; )
		{
		int skill_no = number_range(0, MAX_SKILL-1);
		if ((skill_table[skill_no].skill_level[CLASS_MAGE] <= level ||
			  skill_table[skill_no].skill_level[CLASS_CLERIC] <= level ||
			  skill_table[skill_no].skill_level[CLASS_RANGER] <= level) &&
			 skill_table[skill_no].spell_fun &&         /* an actual spell? */
			 mask & (1<<skill_table[skill_no].target))  /* appropriate? */
			{
			*type = skill_table[skill_no].target;
			return skill_no;
			}
		}
}

// Wands/Staves/Potions/Pills and bags
void wield_random_magic( CHAR_DATA *mob )
{
	int item_type = number_range(48, 53);  /* Get template obj from LIMBO.ARE */
	OBJ_INDEX_DATA *pObjIndex = get_obj_index( item_type );
	OBJ_DATA *obj = create_object( pObjIndex, number_fuzzy( mob->level ) );
	sh_int type;
	int    n_adj1 = number_range(0, nelems(adj1)-1);
	int    n_adj2 = number_range(0, nelems(adj2)-1);

	// Potion stuff
	static char *potions[] = { "bottle", "snifter", "jar", "bulb", "potion" };

	// Pill stuff
	static char *pills[] = { "bag of dust", "honeycomb", "root", "leaf",
		"crystal", "powder packet", "seed" };

	// Bag types (no wearable belts/backpacks... yet)
	static char *skins[] = { "lamia skin", "cloth", "leather", "tough leather",
		"zebra-skin", "gilded" };
	static char *bags[] = { "bag", "valise", "pail", "basket", "jar",
		"box", "sack" };

	char buffer[256];
	char buf[MAX_STRING_LENGTH];
	char name[MAX_STRING_LENGTH];

	switch (item_type) {
	case 48: /* scroll */
		sprintf(buffer, "%s %s scroll", adj1[n_adj1], adj2[n_adj2]);
		obj->value[0] = number_range(obj->level / 2+1, obj->level * 3 / 2+1); /* level */
		obj->value[0] = URANGE(1, obj->value[0], 36);
		obj->value[1] = random_spell(obj->value[0],
			MASK_IGNORE|MASK_OFFENSIVE|MASK_DEFENSIVE|MASK_SELF, &type);
		if (number_percent() < 50)
			obj->value[2] = random_spell(obj->value[0], 1<<type, &type);
		break;
	case 49: /* wand */
		sprintf(buffer, "%s %s wand", adj1[n_adj1], adj2[n_adj2]);
		obj->value[0] = number_range(obj->level / 3 + 1, obj->level * 3 / 2);  /* level */
		obj->value[0] = URANGE(1, obj->value[0], 36);
		obj->value[1] = number_fuzzy(obj->level / 2 + 3); /* max charges */
		obj->value[2] = number_range(1, obj->value[1]);   /* charges left */
		obj->value[3] = random_spell(obj->value[0], MASK_OFFENSIVE, &type);
		break;
	case 50: /* staff */
		sprintf(buffer, "%s %s staff", adj1[n_adj1], adj2[n_adj2]);
		obj->value[0] = number_range(obj->level / 4 + 1, obj->level * 3 / 2);  /* level */
		obj->value[0] = URANGE(1, obj->value[0], 36);
		obj->value[1] = number_fuzzy(obj->level / 2 + 3); /* max charges */
		obj->value[2] = number_range(1, obj->value[1]);   /* charges left */
      obj->value[3] = random_spell(obj->value[0],
         MASK_IGNORE|MASK_OFFENSIVE|MASK_DEFENSIVE|MASK_SELF, &type);
      break;
   case 51: /* potion */
      sprintf(buffer, "%s %s %s", adj1[n_adj1], adj2[n_adj2],
         potions[number_range(0, nelems(potions)-1)]);
      obj->value[0] = number_range(obj->level / 2+1, obj->level * 3 / 2+1); /* level */
      obj->value[0] = URANGE(1, obj->value[0], 36);
      obj->value[1] = random_spell(obj->value[0], MASK_DEFENSIVE, &type);
      if (number_percent() < 50)
         obj->value[2] = random_spell(obj->value[0], MASK_DEFENSIVE, &type);
      break;
   case 52: /* pill */
      sprintf(buffer, "%s %s %s", adj1[n_adj1], adj2[n_adj2],
         pills[number_range(0, nelems(pills)-1)]);
      obj->value[0] = number_range(obj->level / 2+1, obj->level * 3 / 2+1); /* level */
      obj->value[0] = URANGE(1, obj->value[0], 36);
      obj->value[1] = random_spell(obj->value[0], MASK_DEFENSIVE, &type);
      if (number_percent() < 50)
         obj->value[2] = random_spell(obj->value[0], MASK_DEFENSIVE, &type);
      break;
   case 53: /* bag */
      sprintf(buffer, "%s %s %s", adj1[n_adj1],
         skins[number_range(0, nelems(skins)-1)],
         bags[number_range(0, nelems(bags)-1)]);
      obj->value[0] = number_range(mob->level, mob->level * 25); /* weight */
      obj->value[1] = number_range(0, 1);
      obj->value[2] = -1;
      break;
   };

   // Generate the description strings
   free_string( obj->name );
   obj->name = str_dup( buffer );

   sprintf( buf, "a%s %s",
      (buffer[0] == 'a' || buffer[0] == 'e' || buffer[0] == 'i' ||
       buffer[0] == 'o' || buffer[0] == 'u') ? "n" : "", buffer );
   free_string( obj->short_descr );
   obj->short_descr = str_dup( buf );

   free_string( obj->description );
   sprintf( buf, "%s lies here.", obj->short_descr );
   obj->description = str_dup( buf );

   // Put the item in the mob's inventory
   obj_to_char( obj, mob );
}

// Anything wearable, and trinkets
void wield_random_armor( CHAR_DATA *mob )
{
   int item_type = number_range(0, MAX_WEAR - 1); /* template from LIMBO.ARE */
   OBJ_INDEX_DATA *pObjIndex = get_obj_index( item_type + 30 );
   OBJ_DATA *obj = create_object( pObjIndex, number_fuzzy( mob->level ) );
   int    n_adj1 = number_range(0, nelems(adj1)-1);
   int    n_adj2 = number_range(0, nelems(adj2)-1);
   char *name = "[random]";

   // Armor stuff
   static char *armor_types[] = { "leather", "studded leather", "bronze",
      "chain", "plate", "mithral" };
   static int armor_mul[] = { 1, 3, 2, 5, 10, 10 };
   static int armor_div[] = { 1, 2, 1, 1,  1,  3 };

   // Weapon stuff
   static char *weapon_types[] = { "sword", "sword", "sword", "sword", "sword",
      "short sword", "dagger", "dagger", "hammer", "mace", "mace", "whip" };
   static int weapon_dam[] = { 3, 3, 3, 3, 3, 11, 11, 11, 0, 7, 7, 4 };

   // Trinket stuff
   static char *noun[] = { "pebble", "bauble", "stone", "charm", "fetish",
      "bone", "trinket" };
   char buffer[64];

   char buf[MAX_STRING_LENGTH];

   if (obj->item_type == ITEM_ARMOR)
      {
      int ac_type = URANGE(0, mob->level/5, nelems(armor_types)-1);
      name = armor_types[ac_type];
      obj->weight *= armor_mul[ac_type];
      obj->weight /= armor_div[ac_type];

      if (number_percent() < mob->level / 3)
         random_apply(obj, mob);
      }
   else if (obj->item_type == ITEM_WEAPON)
      {
      int wea_type = number_range(0, nelems(weapon_types)-1);
      name = weapon_types[wea_type];
      obj->value[3] = weapon_dam[wea_type];
      }
   else if (obj->item_type == ITEM_TREASURE)
      {
      if (number_percent() < mob->level)
         {
         random_apply(obj, mob);

         if (number_percent() < mob->level / 3)
            random_apply(obj, mob);
         }

      if (obj->wear_flags & ITEM_HOLD) /* trinket? */
         sprintf(buffer, "%s %s %s", adj1[n_adj1],
            adj2[n_adj2],
            noun[number_range(0, nelems(noun)-1)]);
      else /* no, necklace or something */
         sprintf(buffer, "%s %s", adj1[n_adj1],
            adj2[n_adj2]);
      name = buffer;
      }

   sprintf( buf, obj->short_descr, name );
   free_string( obj->short_descr );
   obj->short_descr = str_dup( buf );

   obj_to_char( obj, mob );
   equip_char( mob, obj, item_type );
}

/*
 * Create an instance of a mobile.
 */
CHAR_DATA *create_mobile( MOB_INDEX_DATA *pMobIndex )
{
    CHAR_DATA *mob;

    if ( pMobIndex == NULL )
    {
	bug( "Create_mobile: NULL pMobIndex.", 0 );
	exit( 1 );
    }

    if ( char_free == NULL )
    {
	mob		= alloc_perm( sizeof(*mob) );
    }
    else
    {
	mob		= char_free;
	char_free	= char_free->next;
    }

    clear_char( mob );
    mob->pIndexData	= pMobIndex;

    mob->name		= pMobIndex->player_name;
    mob->short_descr	= pMobIndex->short_descr;
    mob->long_descr	= pMobIndex->long_descr;
    mob->description	= pMobIndex->description;
    mob->spec_fun	= pMobIndex->spec_fun;
    mob->prompt         = "<%h %m %v>";

    mob->level		= number_fuzzy( pMobIndex->level );
    mob->act		= pMobIndex->act;
    mob->affected_by	= pMobIndex->affected_by;
    mob->alignment	= pMobIndex->alignment;
    mob->sex		= pMobIndex->sex;

/*
 * IMPORTANT !!! to add gold as defined in areas to your MUD, all you
 *               need to do is remove the coments from the following 
 *               line.
 */

 mob->gold           = pMobIndex->gold; 

    mob->armor		= interpolate( mob->level, 100, -100 );

    mob->max_hit	= mob->level * 8 + number_range(
				mob->level * mob->level / 4,
				mob->level * mob->level );
    mob->hit		= mob->max_hit;

#if 0
//    if (number_percent() <= 10)
       wield_random_magic( mob );
//    else
//       wield_random_armor( mob );
#endif

    /*
     * Insert in list.
     */
    mob->next		= char_list;
    char_list		= mob;
    pMobIndex->count++;
    return mob;
}



/*
 * Create an instance of an object.
 */
OBJ_DATA *create_object( OBJ_INDEX_DATA *pObjIndex, int level )
{
    static OBJ_DATA obj_zero;
    OBJ_DATA *obj;

    if ( pObjIndex == NULL )
    {
	bug( "Create_object: NULL pObjIndex.", 0 );
	exit( 1 );
    }

    if ( obj_free == NULL )
    {
	obj		= alloc_perm( sizeof(*obj) );
    }
    else
    {
	obj		= obj_free;
	obj_free	= obj_free->next;
    }

    *obj		= obj_zero;
    obj->pIndexData	= pObjIndex;
    obj->in_room	= NULL;
    obj->level		= level;
    obj->wear_loc	= -1;

    obj->name		= pObjIndex->name;
    obj->short_descr	= pObjIndex->short_descr;
    obj->description	= pObjIndex->description;
    obj->item_type	= pObjIndex->item_type;
    obj->extra_flags	= pObjIndex->extra_flags;
    obj->wear_flags	= pObjIndex->wear_flags;
    obj->value[0]	= pObjIndex->value[0];
    obj->value[1]	= pObjIndex->value[1];
    obj->value[2]	= pObjIndex->value[2];
    obj->value[3]	= pObjIndex->value[3];
    obj->weight		= pObjIndex->weight;
    obj->cost		= number_fuzzy( 10 )
			* number_fuzzy( level ) * number_fuzzy( level );

    /*
     * Mess with object properties.
     */
    switch ( obj->item_type )
    {
    default:
	bug( "Read_object: vnum %d bad type.", pObjIndex->vnum );
	break;

    case ITEM_LIGHT:
    case ITEM_TREASURE:
    case ITEM_FURNITURE:
    case ITEM_TRASH:
    case ITEM_CONTAINER:
    case ITEM_DRINK_CON:
    case ITEM_KEY:
    case ITEM_FOOD:
    case ITEM_BOAT:
    case ITEM_CORPSE_NPC:
    case ITEM_CORPSE_PC:
    case ITEM_FOUNTAIN:
	break;

    case ITEM_SCROLL:
	obj->value[0]	= number_fuzzy( obj->value[0] );
	break;

    case ITEM_WAND:
    case ITEM_STAFF:
	obj->value[0]	= number_fuzzy( obj->value[0] );
	obj->value[1]	= number_fuzzy( obj->value[1] );
	obj->value[2]	= obj->value[1];
	break;

    case ITEM_WEAPON:
	obj->value[1]	= number_fuzzy( number_fuzzy( 1 * level / 4 + 2 ) );
	obj->value[2]	= number_fuzzy( number_fuzzy( 3 * level / 4 + 6 ) );
	break;

    case ITEM_ARMOR:
	obj->value[0]	= number_fuzzy( level / 4 + 2 );
	break;

    case ITEM_POTION:
    case ITEM_PILL:
	obj->value[0]	= number_fuzzy( number_fuzzy( obj->value[0] ) );
	break;

    case ITEM_MONEY:
	obj->value[0]	= obj->cost;
	break;
    }

    obj->next		= object_list;
    object_list		= obj;
    pObjIndex->count++;

    return obj;
}



/*
 * Clear a new character.
 */
void clear_char( CHAR_DATA *ch )
{
    static CHAR_DATA ch_zero;

    *ch				= ch_zero;
    ch->name			= &str_empty[0];
    ch->short_descr		= &str_empty[0];
    ch->long_descr		= &str_empty[0];
    ch->description		= &str_empty[0];
    ch->prompt                  = &str_empty[0];
    ch->last_note               = 0;
    ch->logon			= current_time;
    ch->armor			= 100;
    ch->position		= POS_STANDING;
    ch->practice		= 21;
    ch->hit			= 20;
    ch->max_hit			= 20;
    ch->mana			= 100;
    ch->max_mana		= 100;
    ch->move			= 100;
    ch->max_move		= 100;
    return;
}



/*
 * Free a character.
 */
void free_char( CHAR_DATA *ch )
{
    OBJ_DATA *obj;
    OBJ_DATA *obj_next;
    AFFECT_DATA *paf;
    AFFECT_DATA *paf_next;

    for ( obj = ch->carrying; obj != NULL; obj = obj_next )
    {
	obj_next = obj->next_content;
	extract_obj( obj );
    }

    for ( paf = ch->affected; paf != NULL; paf = paf_next )
    {
	paf_next = paf->next;
	affect_remove( ch, paf );
    }

    free_string( ch->name		);
    free_string( ch->short_descr	);
    free_string( ch->long_descr		);
    free_string( ch->description	);

    if ( ch->pcdata != NULL )
    {
	free_string( ch->pcdata->pwd		);
	free_string( ch->pcdata->bamfin		);
	free_string( ch->pcdata->bamfout	);
	free_string( ch->pcdata->title		);
	ch->pcdata->next = pcdata_free;
	pcdata_free      = ch->pcdata;
    }

    ch->next	     = char_free;
    char_free	     = ch;
    return;
}



/*
 * Get an extra description from a list.
 */
char *get_extra_descr( const char *name, EXTRA_DESCR_DATA *ed )
{
    for ( ; ed != NULL; ed = ed->next )
    {
	if ( is_name( name, ed->keyword ) )
	    return ed->description;
    }
    return NULL;
}



/*
 * Translates mob virtual number to its mob index struct.
 * Hash table lookup.
 */
MOB_INDEX_DATA *get_mob_index( int vnum )
{
    MOB_INDEX_DATA *pMobIndex;

    for ( pMobIndex  = mob_index_hash[vnum % MAX_KEY_HASH];
	  pMobIndex != NULL;
	  pMobIndex  = pMobIndex->next )
    {
	if ( pMobIndex->vnum == vnum )
	    return pMobIndex;
    }

    if ( fBootDb )
    {
	bug( "Get_mob_index: bad vnum %d.", vnum );
	exit( 1 );
    }

    return NULL;
}



/*
 * Translates mob virtual number to its obj index struct.
 * Hash table lookup.
 */
OBJ_INDEX_DATA *get_obj_index( int vnum )
{
    OBJ_INDEX_DATA *pObjIndex;

    for ( pObjIndex  = obj_index_hash[vnum % MAX_KEY_HASH];
	  pObjIndex != NULL;
	  pObjIndex  = pObjIndex->next )
    {
	if ( pObjIndex->vnum == vnum )
	    return pObjIndex;
    }

    if ( fBootDb )
    {
	bug( "Get_obj_index: bad vnum %d.", vnum );
	exit( 1 );
    }

    return NULL;
}



/*
 * Translates mob virtual number to its room index struct.
 * Hash table lookup.
 */
ROOM_INDEX_DATA *get_room_index( int vnum )
{
    ROOM_INDEX_DATA *pRoomIndex;

    for ( pRoomIndex  = room_index_hash[vnum % MAX_KEY_HASH];
	  pRoomIndex != NULL;
	  pRoomIndex  = pRoomIndex->next )
    {
	if ( pRoomIndex->vnum == vnum )
	    return pRoomIndex;
    }

    if ( fBootDb && !fIgnoreUnconnected)
    {
	bug( "Get_room_index: bad vnum %d.", vnum );
	exit( 1 );
    }

    return NULL;
}



/*
 * Read a letter from a file.
 */
char fread_letter( FILE *fp )
{
    char c;

    do
    {
	c = getc( fp );
    }
    while ( isspace(c) );

    return c;
}



/*
 * Read a number from a file.
 */
int fread_number( FILE *fp )
{
    int number;
    bool sign;
    char c;

    do
    {
	c = getc( fp );
    }
    while ( isspace(c) );

    number = 0;

    sign   = FALSE;
    if ( c == '+' )
    {
	c = getc( fp );
    }
    else if ( c == '-' )
    {
	sign = TRUE;
	c = getc( fp );
    }

    if ( !isdigit(c) )
    {
	bug( "Fread_number: bad format.", 0 );
	exit( 1 );
    }

    while ( isdigit(c) )
    {
	number = number * 10 + c - '0';
	c      = getc( fp );
    }

    if ( sign )
	number = 0 - number;

    if ( c == '|' )
	number += fread_number( fp );
    else if ( c != ' ' )
	ungetc( c, fp );

    return number;
}



/*
 * Read and allocate space for a string from a file.
 * These strings are read-only and shared.
 * Strings are hashed:
 *   each string prepended with hash pointer to prev string,
 *   hash code is simply the string length.
 * This function takes 40% to 50% of boot-up time.
 */
char *fread_string( FILE *fp )
{
    char *plast;
    char c;

    plast = top_string + sizeof(char *);
    if ( plast > &string_space[MAX_STRING - MAX_STRING_LENGTH] )
    {
	bug( "Fread_string: MAX_STRING %d exceeded.", MAX_STRING );
	exit( 1 );
	 }

	 /*
	  * Skip blanks.
	  * Read first char.
	  */
	 do
	 {
	c = getc( fp );
	 }
	 while ( isspace(c) );

	 if ( ( *plast++ = c ) == '~' )
	return &str_empty[0];

	 for ( ;; )
	 {
	/*
	 * Back off the char type lookup,
	 *   it was too dirty for portability.
	 *   -- Furey
	 */
	switch ( *plast = getc( fp ) )
	{
	default:
		 plast++;
		 break;

	case EOF:
		 bug( "Fread_string: EOF", 0 );
		 exit( 1 );
		 break;

	case '\n':
		 plast++;
		 *plast++ = '\r';
		 break;

	case '\r':
		 break;

	case '~':
		 plast++;
		 {
		union
		{
			 char *	pc;
			 char	rgc[sizeof(char *)];
		} u1;
		int ic;
		int iHash;
		char *pHash;
		char *pHashPrev;
		char *pString;

		plast[-1] = '\0';
		iHash     = UMIN( MAX_KEY_HASH - 1, plast - 1 - top_string );
		for ( pHash = string_hash[iHash]; pHash; pHash = pHashPrev )
		{
			 for ( ic = 0; ic < sizeof(char *); ic++ )
			u1.rgc[ic] = pHash[ic];
			 pHashPrev = u1.pc;
			 pHash    += sizeof(char *);

			 if ( top_string[sizeof(char *)] == pHash[0]
			 &&   !strcmp( top_string+sizeof(char *)+1, pHash+1 ) )
			return pHash;
		}

		if ( fBootDb )
		{
			 pString		= top_string;
			 top_string		= plast;
			 u1.pc		= string_hash[iHash];
			 for ( ic = 0; ic < sizeof(char *); ic++ )
			pString[ic] = u1.rgc[ic];
			 string_hash[iHash]	= pString;

			 nAllocString += 1;
			 sAllocString += top_string - pString;
			 return pString + sizeof(char *);
		}
		else
		{
			 return str_dup( top_string + sizeof(char *) );
		}
		 }
	}
	 }
}



/*
 * Read to end of line (for comments).
 */
void fread_to_eol( FILE *fp )
{
    char c;

    do
    {
	c = getc( fp );
    }
    while ( c != '\n' && c != '\r' );

    do
    {
	c = getc( fp );
    }
    while ( c == '\n' || c == '\r' );

    ungetc( c, fp );
    return;
}



/*
 * Read one word (into static buffer).
 */
char *fread_word( FILE *fp )
{
    static char word[MAX_INPUT_LENGTH];
    char *pword;
    char cEnd;

    do
    {
	cEnd = getc( fp );
    }
    while ( isspace( cEnd ) );

    if ( cEnd == '\'' || cEnd == '"' )
    {
	pword   = word;
    }
    else
    {
	word[0] = cEnd;
	pword   = word+1;
	cEnd    = ' ';
    }

    for ( ; pword < word + MAX_INPUT_LENGTH; pword++ )
    {
	*pword = getc( fp );
	if ( cEnd == ' ' ? isspace(*pword) : *pword == cEnd )
	{
	    if ( cEnd == ' ' )
		ungetc( *pword, fp );
	    *pword = '\0';
	    return word;
	}
    }

    bug( "Fread_word: word too long.", 0 );
    exit( 1 );
    return NULL;
}



/*
 * Allocate some ordinary memory,
 *   with the expectation of freeing it someday.
 */
void *alloc_mem( int sMem )
{
#if 1 // %%%
    return malloc(sMem);
#else
    void *pMem;
    int iList;

    sMem += 2 * sizeof (int); // @@@

    for ( iList = 0; iList < MAX_MEM_LIST; iList++ )
    {
	if ( sMem <= rgSizeList[iList] )
	    break;
    }

    if ( iList == MAX_MEM_LIST )
    {
#if 1 // @@@
	bug( "Alloc_mem: size %d too large.", sMem );
	exit( 1 );
#else
   char szBuffer[256];
   wsprintf(szBuffer, "alloc_mem(%d) to large.  Continue?", sMem);
   if (MessageBox(0, szBuffer, "Merc22", MB_ICONQUESTION|MB_YESNO) == IDYES)
      return 0;
   else
      {
      DebugBreak();
      exit( 1 );
      }
#endif
    }

    if ( rgFreeList[iList] == NULL )
    {
	pMem		  = alloc_perm( rgSizeList[iList] );
    }
    else
    {
	pMem              = rgFreeList[iList];
	rgFreeList[iList] = * ((void **) rgFreeList[iList]);
    }

    ((int *) pMem)[0] = 0x55555555; // @@@
    ((int *) pMem)[1] = sMem; // @@@
    return ((int *) pMem) + 2; // @@@
//    return pMem; // @@@
#endif
}



/*
 * Free some memory.
 * Recycle it back onto the free list for blocks of that size.
 */
void free_mem( void *pMem, int sMem )
{
#if 1 // %%%
    free(pMem);
#else
    int iList;

#if 1 // @@@
    ((int *)pMem) -= 2;
    sMem += 2 * sizeof (int);
    if (((int *) pMem)[0] == 0xaaaaaaaa)
    {
	bug( "Free_mem: this block was already freed.", 0 );
	exit( 1 );
    }
    if (((int *) pMem)[0] != 0x55555555)
    {
	bug( "Free_mem: invalid key.", 0 );
	exit( 1 );
    }
    if (((int *) pMem)[1] != sMem)
    {
	bug( "Free_mem: invalid size.", 0 );
	exit( 1 );
    }
    ((int *) pMem)[0] = 0xaaaaaaaa;
#endif

    for ( iList = 0; iList < MAX_MEM_LIST; iList++ )
    {
	if ( sMem <= rgSizeList[iList] )
	    break;
    }

    if ( iList == MAX_MEM_LIST )
    {
	bug( "Free_mem: size %d too large.", sMem );
	exit( 1 );
    }

    pMem = memset( pMem, 0, sMem );
    * ((void **) pMem) = rgFreeList[iList];
    rgFreeList[iList]  = pMem;

    return;
#endif
}



/*
 * Allocate some permanent memory.
 * Permanent memory is never freed,
 *   pointers into it may be copied safely.
 */
void *alloc_perm( int sMem )
{
    static char *pMemPerm;
    static int iMemPerm;
    void *pMem;

    while ( sMem % sizeof(long) != 0 )
	sMem++;
    if ( sMem > MAX_PERM_BLOCK )
    {
	bug( "Alloc_perm: %d too large.", sMem );
	exit( 1 );
    }

    if ( pMemPerm == NULL || iMemPerm + sMem > MAX_PERM_BLOCK )
    {
	iMemPerm = 0;
	if ( ( pMemPerm = calloc( 1, MAX_PERM_BLOCK ) ) == NULL )
	{
	    perror( "Alloc_perm" );
	    exit( 1 );
	}
    }

    pMem        = pMemPerm + iMemPerm;
    iMemPerm   += sMem;
    nAllocPerm += 1;
    sAllocPerm += sMem;
    return pMem;
}



/*
 * Duplicate a string into dynamic memory.
 * Fread_strings are read-only and shared.
 */
char *str_dup( const char *str )
{
    char *str_new;
#if 0 // %%%
    if ( str[0] == '\0' )
	return &str_empty[0];

    if ( str >= string_space && str < top_string )
	return (char *) str;
#endif
    str_new = alloc_mem( strlen(str) + 1 );
    strcpy( str_new, str );
    return str_new;
}



/*
 * Free a string.
 * Null is legal here to simplify callers.
 * Read-only shared strings are not touched.
 */
void free_string( char *pstr )
{
    if ( pstr == NULL
    ||   pstr == &str_empty[0]
    || ( pstr >= string_space && pstr < top_string ) )
	return;

    free_mem( pstr, strlen(pstr) + 1 );
    return;
}


//#if 0
void do_areas( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    AREA_DATA *pArea1;
    AREA_DATA *pArea2;
    int iArea;
	 int iAreaHalf;

	 iAreaHalf = (top_area + 1) / 2;
	 pArea1    = area_first;
	 pArea2    = area_first;
	 for ( iArea = 0; iArea < iAreaHalf; iArea++ )
	pArea2 = pArea2->next;

	 for ( iArea = 0; iArea < iAreaHalf; iArea++ )
	 {
	sprintf( buf, "%-39s%-39s\n\r",
		 pArea1->name, (pArea2 != NULL) ? pArea2->name : "" );
	send_to_char( buf, ch );
	pArea1 = pArea1->next;
	if ( pArea2 != NULL )
		 pArea2 = pArea2->next;
	 }

	 return;
}


void do_memory( CHAR_DATA *ch, char *argument )
{
	 char buf[MAX_STRING_LENGTH];

	 sprintf( buf, "Affects %5d\n\r", top_affect    ); send_to_char( buf, ch );
	 sprintf( buf, "Areas   %5d\n\r", top_area      ); send_to_char( buf, ch );
	 sprintf( buf, "ExDes   %5d\n\r", top_ed        ); send_to_char( buf, ch );
	 sprintf( buf, "Exits   %5d\n\r", top_exit      ); send_to_char( buf, ch );
	 sprintf( buf, "Helps   %5d\n\r", top_help      ); send_to_char( buf, ch );
	 sprintf( buf, "Mobs    %5d\n\r", top_mob_index ); send_to_char( buf, ch );
	 sprintf( buf, "Objs    %5d\n\r", top_obj_index ); send_to_char( buf, ch );
	 sprintf( buf, "Resets  %5d\n\r", top_reset     ); send_to_char( buf, ch );
	 sprintf( buf, "Rooms   %5d\n\r", top_room      ); send_to_char( buf, ch );
	 sprintf( buf, "Shops   %5d\n\r", top_shop      ); send_to_char( buf, ch );

	 sprintf( buf, "Strings %5d strings of %7d bytes (max %d).\n\r",
	nAllocString, sAllocString, MAX_STRING );
	 send_to_char( buf, ch );

	 sprintf( buf, "Perms   %5d blocks  of %7d bytes.\n\r",
	nAllocPerm, sAllocPerm );
	 send_to_char( buf, ch );

	 return;
}
//#endif


/*
 * Stick a little fuzz on a number.
 */
int number_fuzzy( int number )
{
    switch ( number_bits( 2 ) )
    {
    case 0:  number -= 1; break;
    case 3:  number += 1; break;
    }

    return UMAX( 1, number );
}



/*
 * Generate a random number.
 */
int number_range( int from, int to )
{
    int power;
    int number;

    if ( ( to = to - from + 1 ) <= 1 )
	return from;

    for ( power = 2; power < to; power <<= 1 )
	;

    while ( ( number = number_mm( ) & (power - 1) ) >= to )
	;

    return from + number;
}



/*
 * Generate a percentile roll.
 */
int number_percent( void )
{
    int percent;

    while ( ( percent = number_mm( ) & (128-1) ) > 99 )
	;

    return 1 + percent;
}



/*
 * Generate a random door.
 */
int number_door( void )
{
    int door;

    while ( ( door = number_mm( ) & (8-1) ) > 5 )
	;

    return door;
}



int number_bits( int width )
{
    return number_mm( ) & ( ( 1 << width ) - 1 );
}



/*
 * I've gotten too many bad reports on OS-supplied random number generators.
 * This is the Mitchell-Moore algorithm from Knuth Volume II.
 * Best to leave the constants alone unless you've read Knuth.
 * -- Furey
 */
static	int	rgiState[2+55];

void init_mm( )
{
    int *piState;
    int iState;

    piState	= &rgiState[2];

    piState[-2]	= 55 - 55;
    piState[-1]	= 55 - 24;

    piState[0]	= ((int) current_time) & ((1 << 30) - 1);
    piState[1]	= 1;
    for ( iState = 2; iState < 55; iState++ )
    {
	piState[iState] = (piState[iState-1] + piState[iState-2])
			& ((1 << 30) - 1);
    }
    return;
}



int number_mm( void )
{
    int *piState;
    int iState1;
    int iState2;
    int iRand;

    piState		= &rgiState[2];
    iState1	 	= piState[-2];
    iState2	 	= piState[-1];
    iRand	 	= (piState[iState1] + piState[iState2])
			& ((1 << 30) - 1);
    piState[iState1]	= iRand;
    if ( ++iState1 == 55 )
	iState1 = 0;
    if ( ++iState2 == 55 )
	iState2 = 0;
    piState[-2]		= iState1;
    piState[-1]		= iState2;
    return iRand >> 6;
}



/*
 * Roll some dice.
 */
int dice( int number, int size )
{
    int idice;
    int sum;

    switch ( size )
    {
    case 0: return 0;
    case 1: return number;
    }

    for ( idice = 0, sum = 0; idice < number; idice++ )
	sum += number_range( 1, size );

    return sum;
}



/*
 * Simple linear interpolation.
 */
int interpolate( int level, int value_00, int value_32 )
{
    return value_00 + level * (value_32 - value_00) / 32;
}



/*
 * Removes the tildes from a string.
 * Used for player-entered strings that go into disk files.
 */
void smash_tilde( char *str )
{
    for ( ; *str != '\0'; str++ )
    {
	if ( *str == '~' )
	    *str = '-';
    }

    return;
}



/*
 * Compare strings, case insensitive.
 * Return TRUE if different
 *   (compatibility with historical functions).
 */
bool str_cmp( const char *astr, const char *bstr )
{
    if ( astr == NULL )
    {
	bug( "Str_cmp: null astr.", 0 );
	return TRUE;
    }

    if ( bstr == NULL )
    {
	bug( "Str_cmp: null bstr.", 0 );
	return TRUE;
    }

    for ( ; *astr || *bstr; astr++, bstr++ )
    {
	if ( LOWER(*astr) != LOWER(*bstr) )
	    return TRUE;
    }

    return FALSE;
}



/*
 * Compare strings, case insensitive, for prefix matching.
 * Return TRUE if astr not a prefix of bstr
 *   (compatibility with historical functions).
 */
bool str_prefix( const char *astr, const char *bstr )
{
    if ( astr == NULL )
    {
	bug( "Strn_cmp: null astr.", 0 );
	return TRUE;
    }

    if ( bstr == NULL )
    {
	bug( "Strn_cmp: null bstr.", 0 );
	return TRUE;
    }

    for ( ; *astr; astr++, bstr++ )
    {
	if ( LOWER(*astr) != LOWER(*bstr) )
	    return TRUE;
    }

    return FALSE;
}



/*
 * Compare strings, case insensitive, for match anywhere.
 * Returns TRUE is astr not part of bstr.
 *   (compatibility with historical functions).
 */
bool str_infix( const char *astr, const char *bstr )
{
    int sstr1;
    int sstr2;
    int ichar;
    char c0;

    if ( ( c0 = LOWER(astr[0]) ) == '\0' )
	return FALSE;

    sstr1 = strlen(astr);
    sstr2 = strlen(bstr);

    for ( ichar = 0; ichar <= sstr2 - sstr1; ichar++ )
    {
	if ( c0 == LOWER(bstr[ichar]) && !str_prefix( astr, bstr + ichar ) )
	    return FALSE;
    }

    return TRUE;
}



/*
 * Compare strings, case insensitive, for suffix matching.
 * Return TRUE if astr not a suffix of bstr
 *   (compatibility with historical functions).
 */
bool str_suffix( const char *astr, const char *bstr )
{
    int sstr1;
    int sstr2;

    sstr1 = strlen(astr);
    sstr2 = strlen(bstr);
    if ( sstr1 <= sstr2 && !str_cmp( astr, bstr + sstr2 - sstr1 ) )
	return FALSE;
    else
	return TRUE;
}



/*
 * Returns an initial-capped string.
 */
char *capitalize( const char *str )
{
    static char strcap[MAX_STRING_LENGTH];
    int i;

    for ( i = 0; str[i] != '\0'; i++ )
	strcap[i] = LOWER(str[i]);
    strcap[i] = '\0';
    strcap[0] = UPPER(strcap[0]);
    return strcap;
}



/*
 * Append a string to a file.
 */
void append_file( CHAR_DATA *ch, char *file, char *str )
{
    FILE *fp;

    if ( IS_NPC(ch) || str[0] == '\0' )
	return;

    fclose( fpReserve );
    if ( ( fp = fopen( file, "a" ) ) == NULL )
    {
	perror( file );
	send_to_char( "Could not open the file!\n\r", ch );
    }
    else
    {
	fprintf( fp, "[%5d] %s: %s\n",
	    ch->in_room ? ch->in_room->vnum : 0, ch->name, str );
	fclose( fp );
    }

    fpReserve = fopen( NULL_FILE, "r" );
    return;
}

/*
 * Returns a lowercase string.
 */
char *strlower( const char *str )
{
    static char strlow[MAX_STRING_LENGTH];
    int i;

    for ( i = 0; str[i] != '\0'; i++ )
	strlow[i] = LOWER(str[i]);
    strlow[i] = '\0';
    return strlow;
}


#if 0
/*
 * Reports a bug.
 */
void bug( const char *str, int param )
{
    char buf[MAX_STRING_LENGTH];
    FILE *fp;

    if ( fpArea != NULL )
    {
	int iLine;
	int iChar;

	if ( fpArea == stdin )
	{
	    iLine = 0;
	}
	else
	{
	    iChar = ftell( fpArea );
	    fseek( fpArea, 0, 0 );
	    for ( iLine = 0; ftell( fpArea ) < iChar; iLine++ )
	    {
		while ( getc( fpArea ) != '\n' )
		    ;
	    }
	    fseek( fpArea, iChar, 0 );
	}

	sprintf( buf, "Log: [*****] FILE: %s LINE: %d", strArea, iLine );
	log_string( buf );
	 NOTIFY( buf, LEVEL_IMMORTAL, WIZ_NOTIFY_LOG );

	if ( ( fp = fopen( "shutdown.txt", "a" ) ) != NULL )
	{
		 fprintf( fp, "[*****] %s\n", buf );
		 fclose( fp );
	}
	 }

	 strcpy( buf, "Log: [*****] BUG: " );
	 sprintf( buf + strlen(buf), str, param );
	 log_string( buf );
	 NOTIFY( buf, LEVEL_IMMORTAL, WIZ_NOTIFY_LOG );

	 fclose( fpReserve );
    if ( ( fp = fopen( BUG_FILE, "a" ) ) != NULL )
    {
	fprintf( fp, "%s\n", buf );
	fclose( fp );
    }
    fpReserve = fopen( NULL_FILE, "r" );

    MessageBox(0, buf, "Merc2.2 server", MB_ICONHAND|MB_OK);
    DebugBreak();

    return;
}
#endif

#ifdef SINGLEUSER
/*
 * Writes a string to the log.
 */
void log_string( const char *str )
{
	 char *strtime;

	 strtime                    = ctime( &current_time );
	 strtime[strlen(strtime)-1] = '\0';
	 fprintf( stderr, "%s :: %s\n", strtime, str );
	 return;
}
#endif


/*
 * This function is here to aid in debugging.
 * If the last expression in a function is another function call,
 *   gcc likes to generate a JMP instead of a CALL.
 * This is called "tail chaining."
 * It hoses the debugger call stack for that call.
 * So I make this the last call in certain critical functions,
 *   where I really need the call stack to be right for debugging!
 *
 * If you don't understand this, then LEAVE IT ALONE.
 * Don't remove any calls to tail_chain anywhere.
 *
 * -- Furey
 */
void tail_chain( void )
{
    return;
}

/*
 * MOBprogram code block
*/
/* the functions */

/* This routine transfers between alpha and numeric forms of the
 *  mob_prog bitvector types. This allows the use of the words in the
 *  mob/script files.
 */
int mprog_name_to_type (name) char* name;
{
   if (!str_cmp(name,"in_file_prog"))	return IN_FILE_PROG;
   if (!str_cmp(name,"act_prog"))	return ACT_PROG;
   if (!str_cmp(name,"speech_prog"))	return SPEECH_PROG;
   if (!str_cmp(name,"rand_prog"))	return RAND_PROG;
   if (!str_cmp(name,"fight_prog"))	return FIGHT_PROG;
   if (!str_cmp(name,"hitprcnt_prog"))	return HITPRCNT_PROG;
   if (!str_cmp(name,"death_prog"))	return DEATH_PROG;
   if (!str_cmp(name,"entry_prog"))	return ENTRY_PROG;
   if (!str_cmp(name,"greet_prog"))	return GREET_PROG;
   if (!str_cmp(name,"all_greet_prog"))	return ALL_GREET_PROG;
   if (!str_cmp(name,"give_prog"))	return GIVE_PROG;
   if (!str_cmp(name,"bribe_prog"))	return BRIBE_PROG;
   return(ERROR_PROG);
}

/* This routine reads in scripts of MOBprograms from a file */
MPROG_DATA* mprog_file_read(f, mprg, pMobIndex)
char* f;
MPROG_DATA* mprg;
MOB_INDEX_DATA* pMobIndex;
{
  MPROG_DATA* mprg2;
  FILE * progfile;
  char letter;
  bool done=FALSE;
  char MOBProgfile[MAX_INPUT_LENGTH];

  sprintf( MOBProgfile, "%s%s", MOB_DIR, f );
  progfile=fopen(MOBProgfile,"r");
  if (!progfile)
  {
     bug("Mob:%d couldnt open mobprog file",pMobIndex->vnum);
     exit( 1 );
  }
  mprg2=mprg;
  switch (letter=fread_letter(progfile))
  {
    case '>':
     break;
    case '|':
       bug("empty mobprog file.", 0);
       exit( 1 );
     break;
    default:
       bug("in mobprog file syntax error.", 0);
       exit( 1 );
     break;
  }
  while (!done)
  {
  mprg2->type=mprog_name_to_type(fread_word(progfile));
    switch (mprg2->type)
    {
     case ERROR_PROG:
        bug( "mobprog file type error", 0 );
        exit( 1 );
      break;
     case IN_FILE_PROG:
        bug( "mprog file contains a call to file.", 0 );
        exit( 1 );
      break;
     default:
        pMobIndex->progtypes = pMobIndex->progtypes | mprg2->type;
        mprg2->arglist=fread_string(progfile);
        mprg2->comlist=fread_string(progfile);
        switch (letter=fread_letter(progfile))
        {
          case '>':
             mprg2->next=(MPROG_DATA *)alloc_perm(sizeof(MPROG_DATA));
             mprg2=mprg2->next;
             mprg2->next=NULL;
           break;
          case '|':
             done=TRUE;
           break;
          default:
             bug( "in mobprog file syntax error.",0);
             exit( 1 );
           break;
        }
      break;
    }
  }
  fclose(progfile);
  return mprg2;
}


/* Snarf a MOBprogram section from the area file.
 */
void load_mobprogs(fp) FILE * fp;
{
char letter;
MOB_INDEX_DATA * iMob;
int value;
MPROG_DATA* original;
MPROG_DATA* working;

for (;;) switch (letter=fread_letter( fp))
  {
  default:
     bug( "Load_mobprogs: bad command '%c'.",letter);
     exit(1);
   break;
  case 'S':
  case 's':
     fread_to_eol(fp); 
     return;
  case '*':
     fread_to_eol(fp); 
     break;
  case 'M':
  case 'm':
   value=fread_number(fp);
   if ((iMob=get_mob_index(value))==NULL)
     {
      bug( "Load_mobprogs: vnum %d doesnt exist", value );
      exit( 1 );
     }
    
     if (original=iMob->mobprogs)
       for (;original->next!=NULL; original=original->next);
     working=(MPROG_DATA *)alloc_perm(sizeof(MPROG_DATA));
     if (original) original->next=working;
     else iMob->mobprogs=working;
     working=mprog_file_read(fread_word(fp),working,iMob);
     working->next=NULL;
     fread_to_eol(fp);
     break;
   }
} 

/* This procedure is responsible for reading any in_file MOBprograms.
 */
void mprog_read_programs(fp,pMobIndex) FILE* fp; MOB_INDEX_DATA * pMobIndex;
{
MPROG_DATA * mprg;
bool done=FALSE;
char letter;
  if ( (letter=fread_letter(fp)) != '>' )
  {
      bug( "Load_mobiles: vnum %d MOBPROG char", pMobIndex->vnum );
      exit( 1 );
  }
  pMobIndex->mobprogs=(MPROG_DATA *)alloc_perm(sizeof(MPROG_DATA));
  mprg=pMobIndex->mobprogs;
  while (!done)
  {
    mprg->type=mprog_name_to_type(fread_word(fp));
    switch (mprg->type)
    {
     case ERROR_PROG:
        bug( "Load_mobiles: vnum %d MOBPROG type.", pMobIndex->vnum );
        exit( 1 );
      break;
     case IN_FILE_PROG:
        mprg=mprog_file_read(fread_string(fp),mprg,pMobIndex);
        fread_to_eol(fp);
        switch (letter=fread_letter(fp))
        {
          case '>':
             mprg->next=(MPROG_DATA *)alloc_perm(sizeof(MPROG_DATA));
             mprg=mprg->next;
             mprg->next=NULL;
           break;
          case '|':
             mprg->next=NULL;
             fread_to_eol(fp);
             done=TRUE;
           break;
          default:
             bug( "Load_mobiles: vnum %d bad MOBPROG.", pMobIndex->vnum );
             exit( 1 );
           break;
        }
      break;
     default:
        pMobIndex->progtypes = pMobIndex->progtypes | mprg->type;
        mprg->arglist=fread_string(fp);
        fread_to_eol(fp);
        mprg->comlist=fread_string(fp);
        fread_to_eol(fp);
        switch (letter=fread_letter(fp))
        {
          case '>':
             mprg->next=(MPROG_DATA *)alloc_perm(sizeof(MPROG_DATA));
             mprg=mprg->next;
             mprg->next=NULL;
           break;
          case '|':
             mprg->next=NULL;
             fread_to_eol(fp);
             done=TRUE;
           break;
          default:
             bug( "Load_mobiles: vnum %d bad MOBPROG.", pMobIndex->vnum );
             exit( 1 );
           break;
        }
      break;
    }
  }
}
