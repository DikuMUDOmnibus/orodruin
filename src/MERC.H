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



/*
 * Accommodate old non-Ansi compilers.
 */
#if defined(TRADITIONAL)
#define const
#define args( list )			( )
#define DECLARE_DO_FUN( fun )		void fun( )
#define DECLARE_SPEC_FUN( fun )		bool fun( )
#define DECLARE_SPELL_FUN( fun )	void fun( )
#else
#define args( list )			list
#define DECLARE_DO_FUN( fun )		DO_FUN    fun
#define DECLARE_SPEC_FUN( fun )		SPEC_FUN  fun
#define DECLARE_SPELL_FUN( fun )	SPELL_FUN fun
#endif

/*
 * Short scalar types.
 * Diavolo reports AIX compiler has bugs with short types.
 */
#if	!defined(FALSE)
#define FALSE	 0
#endif

#if	!defined(TRUE)
#define TRUE	 1
#endif

#if	defined(_AIX)
#if	!defined(const)
#define const
#endif
typedef int				sh_int;
typedef int				bool;
#define unix
#else
typedef short    int			sh_int;
typedef unsigned char			bool;
#endif

#ifdef WIN32
struct host_and_name_lookup *host_and_name;
typedef unsigned int    u_int;
typedef u_int           SOCKET;
#endif



/*
 * Structure types.
 */
typedef struct	affect_data		AFFECT_DATA;
typedef struct	area_data		AREA_DATA;
typedef struct	ban_data		BAN_DATA;
typedef struct	char_data		CHAR_DATA;
typedef struct	descriptor_data		DESCRIPTOR_DATA;
typedef struct	exit_data		EXIT_DATA;
typedef struct	extra_descr_data	EXTRA_DESCR_DATA;
typedef struct	help_data		HELP_DATA;
typedef struct	kill_data		KILL_DATA;
typedef struct	mob_index_data		MOB_INDEX_DATA;
typedef struct	note_data		NOTE_DATA;
typedef struct	board_data		BOARD_DATA;
typedef struct	obj_data		OBJ_DATA;
typedef struct	obj_index_data		OBJ_INDEX_DATA;
typedef struct	pc_data			PC_DATA;
typedef	struct	clan_data		CLAN_DATA;
typedef struct	reset_data		RESET_DATA;
typedef struct	room_index_data		ROOM_INDEX_DATA;
typedef struct	shop_data		SHOP_DATA;
typedef struct	time_info_data		TIME_INFO_DATA;
typedef struct	weather_data		WEATHER_DATA;
typedef struct  mob_prog_data           MPROG_DATA;         /* MOBprogram */
typedef struct  mob_prog_act_list       MPROG_ACT_LIST;     /* MOBprogram */
typedef struct tourney_data      TOURNEY_DATA;     /* arena */


/*
 * Function types.
 */
typedef	void DO_FUN	args( ( CHAR_DATA *ch, char *argument ) );
typedef bool SPEC_FUN	args( ( CHAR_DATA *ch ) );
typedef void SPELL_FUN	args( ( int sn, int level, CHAR_DATA *ch, void *vo ) );



/*
 * String and memory management parameters.
 */
#define	MAX_KEY_HASH		 1024
#define MAX_STRING_LENGTH	 4096
#define MAX_INPUT_LENGTH	  160



/*
 * Game parameters.
 * Increase the max'es if you add more of something.
 * Adjust the pulse numbers to suit yourself.
 */
#define MAX_SKILL		  190
#define MAX_CLASS		    6
#define MAX_RACE		    12
#define MAX_CLAN		   50
#define MAX_COLORS                 10
#define MAX_LEVEL		   54
#define LEVEL_HERO		   (MAX_LEVEL - 4)
#define LEVEL_IMMORTAL		   (MAX_LEVEL - 3)

#define PULSE_PER_SECOND	    4
#define PULSE_VIOLENCE		  ( 3 * PULSE_PER_SECOND)
#define PULSE_MOBILE		  ( 4 * PULSE_PER_SECOND)
#define PULSE_TICK		  (30 * PULSE_PER_SECOND)
#define PULSE_AREA		  (60 * PULSE_PER_SECOND)



/*
 * Site ban structure.
 */
struct	ban_data
{
	 BAN_DATA *	next;
	 char *	name;
};



/*
 * Time and weather stuff.
 */
#define SUN_DARK		    0
#define SUN_RISE		    1
#define SUN_LIGHT		    2
#define SUN_SET			    3

#define SKY_CLOUDLESS		    0
#define SKY_CLOUDY		    1
#define SKY_RAINING		    2
#define SKY_LIGHTNING		    3

struct	time_info_data
{
    int		hour;
    int		day;
    int		month;
    int		year;
};

struct	weather_data
{
    int		mmhg;
    int		change;
    int		sky;
    int		sunlight;
};



/*
 * Connected state for a channel.
 */
#define CON_PLAYING			 0
#define CON_GET_NAME			 1
#define CON_GET_OLD_PASSWORD		 2
#define CON_CONFIRM_NEW_NAME		 3
#define CON_GET_NEW_PASSWORD		 4
#define CON_CONFIRM_NEW_PASSWORD	 5
#define CON_GET_NEW_SEX			 6
#define CON_GET_NEW_CLASS		 7
#define CON_READ_MOTD			 8
#define CON_GET_NEW_RACE       9
#define CON_GET_NEW_COLOR		10
#define CON_BREAK_CONNECT     11
/*
 * Descriptor (channel) structure.
 */
struct	descriptor_data
{
    DESCRIPTOR_DATA *	next;
    DESCRIPTOR_DATA *	snoop_by;
    CHAR_DATA *		character;
    CHAR_DATA *		original;
	 char *		host;
#ifdef WIN32
    SOCKET		descriptor;
    // This gets players site and user name
	 struct     host_and_name_lookup *host_and_name;
#else
    sh_int		descriptor;
#endif
    sh_int		connected;
    bool		fcommand;
    char		inbuf		[4 * MAX_INPUT_LENGTH];
    char		incomm		[MAX_INPUT_LENGTH];
    char		inlast		[MAX_INPUT_LENGTH];
    int			repeat;
    char *              showstr_head;
    char *              showstr_point;
    char *		outbuf;
	 int			outsize;
    int			outtop;
    char **             pString;	/* OLC */
};

/*
	descriptor_data(SOCKET desc) {
      descriptor	= desc;
      connected	= CON_GET_NAME;
      showstr_head  = (char *) 0;
      showstr_point = (char *) 0;
      outsize	= 2000;
      outbuf	= new char[outsize];

      next = (descriptor_data *) 0;
      snoop_by = (descriptor_data *) 0;
      host = 0;
      fcommand = FALSE;
      repeat = 0;
      outtop = 0;

      incomm[0] = '\0';
      }
   ~descriptor_data() {
      delete[] outbuf;
      delete[] host;
	 }
*/

/*
 * Attribute bonus structures.
 */
struct	str_app_type
{
    sh_int	tohit;
    sh_int	todam;
    sh_int	carry;
    sh_int	wield;
};

struct	int_app_type
{
    sh_int	learn;
};

struct	wis_app_type
{
    sh_int	practice;
};

struct	dex_app_type
{
	 sh_int	defensive;
};

struct	con_app_type
{
    sh_int	hitp;
    sh_int	shock;
};



/*
 * TO types for act.
 */
#define TO_ROOM		    0
#define TO_NOTVICT	    1
#define TO_VICT		    2
#define TO_CHAR		    3



/*
 * Help table types.
 */
struct	help_data
{
	 HELP_DATA *	next;
    sh_int	level;
    char *	keyword;
    char *	text;
};



/*
 * Shop types.
 */
#define MAX_TRADE	 5

struct	shop_data
{
    SHOP_DATA *	next;			/* Next shop in list		*/
	 sh_int	keeper;			/* Vnum of shop keeper mob	*/
    sh_int	buy_type [MAX_TRADE];	/* Item types shop will buy	*/
    sh_int	profit_buy;		/* Cost multiplier for buying	*/
    sh_int	profit_sell;		/* Cost multiplier for selling	*/
    sh_int	open_hour;		/* First opening hour		*/
    sh_int	close_hour;		/* First closing hour		*/
};



/*
 * Per-class stuff.
 */
struct	class_type
{
    char 	who_name	[4];	/* Three-letter name for 'who'	*/
    sh_int	attr_prime;		/* Prime attribute		*/
    sh_int	weapon;			/* First weapon			*/
    sh_int	guild;			/* Vnum of guild room		*/
    sh_int	skill_adept;		/* Maximum skill level		*/
    sh_int	thac0_00;		/* Thac0 for level  0		*/
    sh_int	thac0_32;		/* Thac0 for level 32		*/
	 sh_int	hp_min;			/* Min hp gained on leveling	*/
	 sh_int	hp_max;			/* Max hp gained on leveling	*/
    bool	fMana;			/* Class gains mana on level	*/
};

/*
 * Per-race stuff.
 */
struct	race_type
{
    char 	who_name	[4];	/* Three-letter name for 'who'	*/
    char    long_name[20];
    sh_int	mod_str;
    sh_int	mod_int;
	 sh_int	mod_wis;
    sh_int	mod_dex;
    sh_int	mod_con;
    sh_int  city_vnum;     /* Racial city */
    sh_int  city_temple;   /* Racial temple */
    int     class_flags;   /* Classes allowed */
    int		bitvector;     /* Permanent magical ability */
};

/*
 * Un colour type.
 */
struct color_data
{
   char         code[10];
   char         act_code[5];
   char         name[15];
   int          number;
};

#define CLAN_PLAIN	 0
#define CLAN_VAMPIRE	 1
#define CLAN_WARRIOR	 2
#define CLAN_DRUID	 3
#define CLAN_MAGE	 4
#define CLAN_CELTIC	 5
#define CLAN_THIEF	 6
#define CLAN_CLERIC	 7
#define CLAN_UNDEAD	 8
#define CLAN_CHAOTIC	 9
#define CLAN_NEUTRAL	10
#define CLAN_LAWFUL	11
#define CLAN_NOKILL	12

struct	clan_data
{
    CLAN_DATA * next;		/* next clan in list			*/
    CLAN_DATA * prev;		/* previous clan in list		*/
    char *	filename;	/* Clan filename			*/
    char *	name;		/* Clan name				*/
    char *	motto;		/* Clan motto				*/
    char *	description;	/* A brief description of the clan	*/
    char *	deity;		/* Clan's deity				*/
    char *	leader;		/* Head clan leader			*/
    char *	number1;	/* First officer			*/
    char *	number2;	/* Second officer			*/
    int		pkills;		/* Number of pkills on behalf of clan	*/
    int		pdeaths;	/* Number of pkills against clan	*/
    int		mkills;		/* Number of mkills on behalf of clan	*/
    int		mdeaths;	/* Number of clan deaths due to mobs	*/
    int		illegal_pk;	/* Number of illegal pk's by clan	*/
    int		score;		/* Overall score			*/
    sh_int	clan_type;	/* See clan type defines		*/
    sh_int	favour;		/* Deities favour upon the clan		*/
    sh_int	strikes;	/* Number of strikes against the clan	*/
    sh_int	members;	/* Number of clan members		*/
	 sh_int	alignment;	/* Clan's general alignment		*/
    sh_int	board;		/* Vnum of clan board			*/
    sh_int	clanobj1;	/* Vnum of first clan obj (ring)	*/
    sh_int	clanobj2;	/* Vnum of second clan obj (shield)	*/
    sh_int	clanobj3;	/* Vnum of third clan obj (weapon)	*/
    sh_int	recall;		/* Vnum of clan's recall room		*/
    sh_int	storeroom;	/* Vnum of clan's store room		*/
    sh_int	guard1;		/* Vnum of clan guard type 1		*/
    sh_int	guard2;		/* Vnum of clan guard type 2		*/
};




/*
 * Data structure for notes.
 */
struct	note_data
{
    NOTE_DATA *	next;
    char *	sender;
    char *	date;
    char *	to_list;
    char *	subject;
    char *	text;
    time_t      date_stamp;
};


struct	board_data
{
    BOARD_DATA * next;			/* Next board in list		   */
	 BOARD_DATA * prev;			/* Previous board in list	   */
    NOTE_DATA *  first_note;		/* First note on board		   */
    NOTE_DATA *  last_note;		/* Last note on board		   */
    char *	 note_file;		/* Filename to save notes to	   */
    sh_int	 board_obj;		/* Vnum of board object		   */
    sh_int	 num_posts;		/* Number of notes on this board   */
    sh_int	 min_read_level;	/* Minimum level to read a note	   */
    sh_int	 min_post_level;	/* Minimum level to post a note    */
    sh_int	 min_remove_level;	/* Minimum level to remove a note  */
    sh_int	 max_posts;		/* Maximum amount of notes allowed */
};



/*
 * An affect.
 */
struct	affect_data
{
    AFFECT_DATA *	next;
    sh_int		type;
    sh_int		duration;
    sh_int		location;
    sh_int		modifier;
    int			bitvector;
};



/*
 * A kill structure (indexed by level).
 */
struct	kill_data
{
    sh_int		number;
    sh_int		killed;
};



/***************************************************************************
 *                                                                         *
 *                   VALUES OF INTEREST TO AREA BUILDERS                   *
 *                   (Start of section ... start here)                     *
 *                                                                         *
 ***************************************************************************/
/*
 * Ansi colors and VT100 codes
 * Used in #PLAYER
 *
 * On most machines that use ANSI, namely the IBM PCs, the decimal value for
 * the escape character is 27 (1B hex).     Change this value when needed.
 */

#define ESC        '\x1b'

#define BLACK      "\x1b[30m"    /* These are foreground color codes */
#define RED        "\x1b[31m"
#define GREEN      "\x1b[32m"
#define YELLOW     "\x1b[33m"
#define BLUE       "\x1b[34m"
#define PURPLE     "\x1b[35m"
#define CYAN       "\x1b[36m"
#define GREY       "\x1b[37m"

#define B_BLACK    "\x1b[40m"    /* These are background color codes */
#define B_RED      "\x1b[41m"
#define B_GREEN    "\x1b[42m"
#define B_YELLOW   "\x1b[43m"
#define B_BLUE     "\x1b[44m"
#define B_PURPLE   "\x1b[45m"
#define B_CYAN     "\x1b[46m"
#define B_GREY     "\x1b[47m"

/* Below are VT100 and ANSI codes (above are ANSI exclusively)       */

#define EEEE       "\x1b#8"                 /* Turns screen to EEEEs */
#define CLRSCR     "\x1b[2j"                /* Clear screen          */
#define CLREOL     "\x1b[K"                 /* Clear to end of line  */

#define UPARR      "\x1b[A"                 /* Up one line           */
#define DOWNARR    "\x1b[B"                 /* Down one line         */
#define RIGHTARR   "\x1b[C"                 /* Right one column      */
#define LEFTARR    "\x1b[D"                 /* Left one column       */
#define HOMEPOS    "\x1b[H"                 /* Home (upper left)     */

#define BOLD       "\x1b[1m"                /* High intensity        */
#define FLASH      "\x1b[5m"                /* Flashing text         */
#define INVERSE    "\x1b[7m"                /* XORed back and fore   */
#define NTEXT      "\x1b[0m"                /* Normal text  (grey)   */

/*
 * Other codes of note for future ANSI development:
 * The <esc>[y;xH code works nicely for positioning the cursor.
 */

/*
 * Color codings for
 * channels+says
 */
#define COLOR_AUCTION                 0
#define COLOR_CHAT                    1
#define COLOR_SHOUT                   2
#define COLOR_IMMTALK                 3
#define COLOR_SAY                     4
#define COLOR_TELL                    5
#define COLOR_UNUSED6                 6
#define COLOR_UNUSED7                 7
#define COLOR_UNUSED8                 8
#define COLOR_UNUSED9                 9


/*
 * Well known mob virtual numbers.
 * Defined in #MOBILES.
 */
#define MOB_VNUM_CITYGUARD	   3060
#define MOB_VNUM_VAMPIRE	   3404
#define MOB_VNUM_WALKING_DEAD 20

#define AT_BLACK	    0
#define AT_BLOOD	    1
#define AT_DGREEN           2
#define AT_ORANGE	    3
#define AT_DBLUE	    4
#define AT_PURPLE	    5
#define AT_CYAN	  	    6
#define AT_GREY		    7
#define AT_DGREY	    8
#define AT_RED		    9
#define AT_GREEN	   10
#define AT_YELLOW	   11
#define AT_BLUE		   12
#define AT_PINK		   13
#define AT_LBLUE	   14
#define AT_WHITE	   15
#define AT_BLINK	   16
#define AT_PLAIN	   AT_GREY
#define AT_ACTION	   AT_GREY
#define AT_SAY		   AT_LBLUE
#define AT_GOSSIP	   AT_LBLUE
#define AT_YELL	           AT_WHITE
#define AT_TELL		   AT_WHITE
#define AT_HIT		   AT_WHITE
#define AT_HITME	   AT_YELLOW
#define AT_IMMORT	   AT_YELLOW
#define AT_HURT		   AT_RED
#define AT_FALLING	   AT_WHITE + AT_BLINK
#define AT_DANGER	   AT_RED + AT_BLINK
#define AT_MAGIC	   AT_BLUE
#define AT_CONSIDER	   AT_GREY
#define AT_REPORT	   AT_GREY
#define AT_POISON	   AT_GREEN
#define AT_SOCIAL	   AT_CYAN
#define AT_DYING	   AT_YELLOW
#define AT_DEAD		   AT_RED
#define AT_SKILL	   AT_GREEN
#define AT_CARNAGE	   AT_BLOOD
#define AT_DAMAGE	   AT_WHITE
#define AT_FLEE		   AT_YELLOW
#define AT_RMNAME	   AT_WHITE
#define AT_RMDESC	   AT_YELLOW
#define AT_OBJECT	   AT_GREEN
#define AT_PERSON	   AT_PINK
#define AT_LIST		   AT_BLUE
#define AT_BYE		   AT_GREEN
#define AT_GOLD		   AT_YELLOW
#define AT_GTELL	   AT_BLUE
#define AT_NOTE		   AT_GREEN
#define AT_HUNGRY	   AT_ORANGE
#define AT_THIRSTY	   AT_BLUE
#define	AT_FIRE		   AT_RED
#define AT_SOBER	   AT_WHITE
#define AT_WEAROFF	   AT_YELLOW
#define AT_EXITS	   AT_WHITE
#define AT_SCORE	   AT_LBLUE
#define AT_RESET	   AT_DGREEN
#define AT_LOG		   AT_PURPLE
#define AT_DIEMSG	   AT_WHITE


/*
 * ACT bits for mobs.
 * Used in #MOBILES.
 */
#define ACT_IS_NPC		      1		/* Auto set for mobs	*/
#define ACT_SENTINEL		      2		/* Stays in one room	*/
#define ACT_SCAVENGER		      4		/* Picks up objects	*/
#define ACT_AGGRESSIVE		     32		/* Attacks PC's		*/
#define ACT_STAY_AREA		     64		/* Won't leave area	*/
#define ACT_WIMPY		    128		/* Flees when hurt	*/
#define ACT_PET			    256		/* Auto set for pets	*/
#define ACT_TRAIN		    512		/* Can train PC's	*/
#define ACT_PRACTICE		   1024		/* Can practice PC's	*/



/*
 * Bits for 'affected_by'.
 * Used in #MOBILES.
 */
#define AFF_BLIND		      1
#define AFF_INVISIBLE		      2
#define AFF_DETECT_EVIL		      4
#define AFF_DETECT_INVIS	      8
#define AFF_DETECT_MAGIC	     16
#define AFF_DETECT_HIDDEN	     32
#define AFF_HOLD		     64		/* Unused	*/
#define AFF_SANCTUARY		    128
#define AFF_FAERIE_FIRE		    256
#define AFF_INFRARED		    512
#define AFF_CURSE		   1024
#define AFF_POISON		   4096
#define AFF_PROTECT		   8192
#define AFF_PARALYSIS		  16384		/* Unused	*/
#define AFF_SNEAK		  32768
#define AFF_HIDE		  65536
#define AFF_SLEEP		 131072
#define AFF_CHARM		 262144
#define AFF_FLYING		 524288
#define AFF_PASS_DOOR		1048576
#define AFF_BERSERK		2048



/*
 * Sex.
 * Used in #MOBILES.
 */
#define SEX_NEUTRAL		      0
#define SEX_MALE		      1
#define SEX_FEMALE		      2



/*
 * Well known object virtual numbers.
 * Defined in #OBJECTS.
 */
#define OBJ_VNUM_MONEY_ONE	      2
#define OBJ_VNUM_MONEY_SOME	      3

#define OBJ_VNUM_CORPSE_NPC	     10
#define OBJ_VNUM_CORPSE_PC	     11
#define OBJ_VNUM_SEVERED_HEAD	     12
#define OBJ_VNUM_TORN_HEART	     13
#define OBJ_VNUM_SLICED_ARM	     14
#define OBJ_VNUM_SLICED_LEG	     15
#define OBJ_VNUM_FINAL_TURD	     16

#define OBJ_VNUM_MUSHROOM	     20
#define OBJ_VNUM_LIGHT_BALL	     21
#define OBJ_VNUM_SPRING		     22

#define OBJ_VNUM_SCHOOL_MACE	   3700
#define OBJ_VNUM_SCHOOL_DAGGER	   3701
#define OBJ_VNUM_SCHOOL_SWORD	   3702
#define OBJ_VNUM_SCHOOL_VEST	   3703
#define OBJ_VNUM_SCHOOL_SHIELD	   3704
#define OBJ_VNUM_SCHOOL_BANNER     3716
#define OBJ_VNUM_BLACK_POWDER      8903

// The error object is used when a player file has an item that is no longer
// part of the world--Slash
#define OBJ_VNUM_ERROR  4

/*
 * Item types.
 * Used in #OBJECTS.
 */
#define ITEM_LIGHT		      1
#define ITEM_SCROLL		      2
#define ITEM_WAND		      3
#define ITEM_STAFF		      4
#define ITEM_WEAPON		      5
#define ITEM_TREASURE		      8
#define ITEM_ARMOR		      9
#define ITEM_POTION		     10
#define ITEM_FURNITURE		     12
#define ITEM_TRASH		     13
#define ITEM_CONTAINER		     15
#define ITEM_DRINK_CON		     17
#define ITEM_KEY		     18
#define ITEM_FOOD		     19
#define ITEM_MONEY		     20
#define ITEM_BOAT		     22
#define ITEM_CORPSE_NPC		     23
#define ITEM_CORPSE_PC		     24
#define ITEM_FOUNTAIN		     25
#define ITEM_PILL		     26
#define ITEM_BOARD                 27 /* v0: index of board */
#define ITEM_CLANOBJECT		28
#define ITEM_CLANCORPSE		29


/*
 * Extra flags.
 * Used in #OBJECTS.
 */
#define ITEM_GLOW		      1
#define ITEM_HUM		      2
#define ITEM_DARK		      4
#define ITEM_LOCK		      8
#define ITEM_EVIL		     16
#define ITEM_INVIS		     32
#define ITEM_MAGIC		     64
#define ITEM_NODROP		    128
#define ITEM_BLESS		    256
#define ITEM_ANTI_GOOD		    512
#define ITEM_ANTI_EVIL		   1024
#define ITEM_ANTI_NEUTRAL	   2048
#define ITEM_NOREMOVE		   4096
#define ITEM_INVENTORY		   8192
#define ITEM_POISONED             16384



/*
 * Wear flags.
 * Used in #OBJECTS.
 */
#define ITEM_TAKE		      1
#define ITEM_WEAR_FINGER	      2
#define ITEM_WEAR_NECK		      4
#define ITEM_WEAR_BODY		      8
#define ITEM_WEAR_HEAD		     16
#define ITEM_WEAR_LEGS		     32
#define ITEM_WEAR_FEET		     64
#define ITEM_WEAR_HANDS		    128 
#define ITEM_WEAR_ARMS		    256
#define ITEM_WEAR_SHIELD	    512
#define ITEM_WEAR_ABOUT		   1024 
#define ITEM_WEAR_WAIST		   2048
#define ITEM_WEAR_WRIST		   4096
#define ITEM_WIELD		   8192
#define ITEM_HOLD		  16384



/*
 * Apply types (for affects).
 * Used in #OBJECTS.
 */
#define APPLY_NONE		      0
#define APPLY_STR		      1
#define APPLY_DEX		      2
#define APPLY_INT		      3
#define APPLY_WIS		      4
#define APPLY_CON		      5
#define APPLY_SEX		      6
#define APPLY_CLASS		      7
#define APPLY_LEVEL		      8
#define APPLY_AGE		      9
#define APPLY_HEIGHT		     10
#define APPLY_WEIGHT		     11
#define APPLY_MANA		     12
#define APPLY_HIT		     13
#define APPLY_MOVE		     14
#define APPLY_GOLD		     15
#define APPLY_EXP		     16
#define APPLY_AC		     17
#define APPLY_HITROLL		     18
#define APPLY_DAMROLL		     19
#define APPLY_SAVING_PARA	     20
#define APPLY_SAVING_ROD	     21
#define APPLY_SAVING_PETRI	     22
#define APPLY_SAVING_BREATH	     23
#define APPLY_SAVING_SPELL	     24

// TFC
#define APPLY_CHARISMA           25
#define APPLY_LUC               26 /* Luck */
#define APPLY_NOSTEAL           27
#define APPLY_NOSLEEP           28
#define APPLY_NOSUMMON          29 /* Also called blocking */
#define APPLY_NOCHARM           30 /* Also called steadfastness? */
#define APPLY_NOSEXCHANGE       31 /* Also called familiarity */
#define APPLY_TRUESEE           32
#define APPLY_NOINFO            33 /* Also called Obscurement */


/*
 * Values for containers (value[1]).
 * Used in #OBJECTS.
 */
#define CONT_CLOSEABLE		      1
#define CONT_PICKPROOF		      2
#define CONT_CLOSED		      4
#define CONT_LOCKED		      8



/*
 * Well known room virtual numbers.
 * Defined in #ROOMS.
 */
#define ROOM_VNUM_LIMBO		      2
#define ROOM_VNUM_CHAT		   1200
#define ROOM_VNUM_TEMPLE	   3001
#define ROOM_VNUM_ALTAR		   3054
#define ROOM_VNUM_SCHOOL	   3700
#define ROOM_VNUM_GRAVEYARD_A       427
#define ROOM_VNUM_PURGATORY_A       401



/*
 * Room flags.
 * Used in #ROOMS.
 */
#define ROOM_DARK		      1
#define ROOM_NO_MOB		      4
#define ROOM_INDOORS		      8
#define ROOM_PRIVATE		    512
#define ROOM_SAFE		   1024
#define ROOM_SOLITARY		   2048
#define ROOM_PET_SHOP		   4096
#define ROOM_NO_RECALL		   8192



/*
 * Directions.
 * Used in #ROOMS.
 */
#define DIR_NORTH		      0
#define DIR_EAST		      1
#define DIR_SOUTH		      2
#define DIR_WEST		      3
#define DIR_UP			      4
#define DIR_DOWN		      5



/*
 * Exit flags.
 * Used in #ROOMS.
 */
#define EX_ISDOOR		      1
#define EX_CLOSED		      2
#define EX_LOCKED		      4
#define EX_BASHED                     8
#define EX_BASHPROOF                 16
#define EX_PICKPROOF		     32
#define EX_PASSPROOF                 64



/*
 * Sector types.
 * Used in #ROOMS.
 */
#define SECT_INSIDE		      0
#define SECT_CITY		      1
#define SECT_FIELD		      2
#define SECT_FOREST		      3
#define SECT_HILLS		      4
#define SECT_MOUNTAIN		      5
#define SECT_WATER_SWIM		      6
#define SECT_WATER_NOSWIM	      7
#define SECT_UNUSED		      8
#define SECT_AIR		      9
#define SECT_DESERT		     10
#define SECT_MAX		     11



/*
 * Equpiment wear locations.
 * Used in #RESETS.
 */
#define WEAR_NONE		     -1
#define WEAR_LIGHT		      0
#define WEAR_FINGER_L		      1
#define WEAR_FINGER_R		      2
#define WEAR_NECK_1		      3
#define WEAR_NECK_2		      4
#define WEAR_BODY		      5
#define WEAR_HEAD		      6
#define WEAR_LEGS		      7
#define WEAR_FEET		      8
#define WEAR_HANDS		      9
#define WEAR_ARMS		     10
#define WEAR_SHIELD		     11
#define WEAR_ABOUT		     12
#define WEAR_WAIST		     13
#define WEAR_WRIST_L		     14
#define WEAR_WRIST_R		     15
#define WEAR_WIELD		     16
#define WEAR_HOLD		     17
#define MAX_WEAR		     18



/***************************************************************************
 *                                                                         *
 *                   VALUES OF INTEREST TO AREA BUILDERS                   *
 *                   (End of this section ... stop here)                   *
 *                                                                         *
 ***************************************************************************/

/*
 * Conditions.
 */
#define COND_DRUNK		      0
#define COND_FULL		      1
#define COND_THIRST		      2



/*
 * Positions.
 */
#define POS_DEAD		      0
#define POS_MORTAL		      1
#define POS_INCAP		      2
#define POS_STUNNED		      3
#define POS_SLEEPING		      4
#define POS_RESTING		      5
#define POS_FIGHTING		      6
#define POS_STANDING		      7



/*
 * ACT bits for players.
 */
#define PLR_IS_NPC		      1		/* Don't EVER set.	*/
#define PLR_BOUGHT_PET		      2

#define PLR_AUTOEXIT		      8
#define PLR_AUTOLOOT		     16
#define PLR_AUTOSAC                  32
#define PLR_BLANK		     64
#define PLR_BRIEF		    128
#define PLR_COMBINE		    512
#define PLR_PROMPT		   1024
#define PLR_TELNET_GA		   2048

#define PLR_HOLYLIGHT		   4096
#define PLR_WIZINVIS		   8192

#define	PLR_SILENCE		  32768
#define PLR_NO_EMOTE		  65536
#define PLR_NO_TELL		 262144
#define PLR_LOG			 524288
#define PLR_DENY		1048576
#define PLR_FREEZE		2097152
#define PLR_THIEF		4194304
#define PLR_KILLER		8388608


#define PLR_ANSI		            4
#define PLR_VT100		          256
#define WIZ_NOTIFY           16384
#define WIZ_NOTIFY_MONITOR  131072
#define WIZ_NOTIFY_LOG    16777216



/*
 * Channel bits.
 */
#define	CHANNEL_AUCTION		   1
#define	CHANNEL_CHAT		      2
#define	CHANNEL_HACKER		      4
#define	CHANNEL_IMMTALK		   8
#define	CHANNEL_MUSIC		     16
#define	CHANNEL_QUEST	        32
#define	CHANNEL_SHOUT		     64
#define	CHANNEL_YELL		    128
#define  CHANNEL_SPEW          256
#define  CHANNEL_MUSE          512

/*
 * Prototype for a mob.
 * This is the in-memory version of #MOBILES.
 */
struct	mob_index_data
{
	 MOB_INDEX_DATA *	next;
    SPEC_FUN *		spec_fun;
    SHOP_DATA *		pShop;
    char *		player_name;
    char *		short_descr;
    char *		long_descr;
	 char *		description;
    sh_int		vnum;
    sh_int		count;
    sh_int		killed;
    sh_int		sex;
    sh_int		level;
	 int			act;
    int			affected_by;
    sh_int		alignment;
    sh_int		hitroll;		/* Unused */
    sh_int		ac;			/* Unused */
    sh_int		hitnodice;		/* Unused */
    sh_int		hitsizedice;		/* Unused */
    sh_int		hitplus;		/* Unused */
    sh_int		damnodice;		/* Unused */
    sh_int		damsizedice;		/* Unused */
    sh_int		damplus;		/* Unused */
    int			gold;			/* Unused */
    MPROG_DATA *        mobprogs;               /* Used by MOBprogram */
	 int                 progtypes;              /* Used by MOBprogram */
    void       *cargo; // @@@ ###
};



/*
 * One character (PC or NPC).
 */
struct	char_data
{
	 CHAR_DATA *		next;               /* Global chain of all char/mob */
	 CHAR_DATA *		next_in_room;       /* Chain set by char_to_room() */
	 CHAR_DATA *		master;             /* Who char follows or is charmed by */
    CHAR_DATA *		leader;             /* Leader of a group */
    CHAR_DATA *		fighting;           /* Who char is fighting */
    CHAR_DATA *		reply;              /* Last person who talked to char */
	 CHAR_DATA *      hunting;            /* Used by hunting routine */
	 SPEC_FUN *		spec_fun;           /* Mobile special function*/
	 MOB_INDEX_DATA *	pIndexData;     /* Mobile class info */
	 DESCRIPTOR_DATA *	desc;           /* Player socket info */
	 AFFECT_DATA *	affected;           /* List of stat affects on char */
	 NOTE_DATA *		pnote;              /* Note writing stuff */
	 OBJ_DATA *		carrying;           /* Chain of char's inventory/wielded */
	 ROOM_INDEX_DATA *	in_room;        /* Room char is in */
	 ROOM_INDEX_DATA *	was_in_room;    /* Room returned to after 'idle void' */
	 PC_DATA *		pcdata;             /* Data only PCs have */
    OBJ_DATA *		first_carrying;
    OBJ_DATA *		last_carrying;
	 char *		name;
	 char *		short_descr;
	 char *		long_descr;
	 char *		description;
    char *              prompt;
    sh_int		sex;
    sh_int		class;
    sh_int		race;
    sh_int		level;
    sh_int		trust;
    bool                wizbit;
    int			played;
	 time_t		logon;
    time_t		save_time;
	 time_t              last_note;
    sh_int		timer;
    sh_int		wait;
    sh_int		hit;
    sh_int		max_hit;
    sh_int		mana;
    sh_int		max_mana;
	 sh_int		move;
    sh_int		max_move;
    sh_int     invis_level;
	 int			gold;
	 int			exp;
    int			act;
    int                 act2;
    int			affected_by;
    sh_int		position;
    sh_int		practice;
    sh_int		carry_weight;
    sh_int		carry_number;
    sh_int		saving_throw;
    sh_int		alignment;
    sh_int		hitroll;
    sh_int		damroll;
    sh_int		armor;
	 sh_int		wimpy;
    sh_int		deaf;
    sh_int              colors [MAX_COLORS];
	 MPROG_ACT_LIST *    mpact;                  /* Used by MOBprogram */
    int                 mpactnum;               /* Used by MOBprogram */
	 bool        openedtourney;          /*arena*/
	 void       *cargo; // @@@ ###
};



/*
 * Data which only PC's have.
 */
struct	pc_data
{
    CLAN_DATA *		clan;
	 char *     clan_name;
	 PC_DATA *		next;
	 char *		pwd;
	 char *		bamfin;
	 char *		bamfout;
	 char *		title;
    int			pkills;	     /* Number of pkills on behalf of clan */
    int			pdeaths;     /* Number of times pkilled (legally)  */
    int			mkills;	     /* Number of mobs killed		   */
    int			mdeaths;     /* Number of deaths due to mobs       */
    int			illegal_pk;  /* Number of illegal pk's committed   */
	 sh_int		perm_str;
	 sh_int		perm_int;
	 sh_int		perm_wis;
	 sh_int		perm_dex;
	 sh_int		perm_con;
	 sh_int		mod_str;
	 sh_int		mod_int;
	 sh_int		mod_wis;
	 sh_int		mod_dex;
	 sh_int		mod_con;
	 sh_int		wizinvis;    /* wizinvis level */
	 sh_int		condition	[3];
	 sh_int              pagelen;
	 sh_int		learned		[MAX_SKILL];
};

struct tourney_data         /* arena */
{
	 int      open;
	 int      low_level;
    int      hi_level;
};

/*
 * MOBprogram block
*/

struct  mob_prog_act_list
{
    MPROG_ACT_LIST * next;
    char *           buf;
    CHAR_DATA *      ch;
    OBJ_DATA *       obj;
    void *           vo;
};

struct  mob_prog_data
{
    MPROG_DATA *next;
    int         type;
	 char *      arglist;
    char *      comlist;
};

bool    MOBtrigger;

#define ERROR_PROG        -1
#define IN_FILE_PROG       0
#define ACT_PROG           1
#define SPEECH_PROG        2
#define RAND_PROG          4
#define FIGHT_PROG         8
#define DEATH_PROG        16
#define HITPRCNT_PROG     32
#define ENTRY_PROG        64
#define GREET_PROG       128
#define ALL_GREET_PROG   256
#define GIVE_PROG        512
#define BRIBE_PROG      1024


/*
 * Liquids.
 */
#define LIQ_WATER        0
#define LIQ_MAX		16

struct	liq_type
{
    char *	liq_name;
    char *	liq_color;
    sh_int	liq_affect[3];
};



/*
 * Extra description data for a room or object.
 */
struct	extra_descr_data
{
    EXTRA_DESCR_DATA *next;	/* Next in list                     */
    char *keyword;              /* Keyword in look/examine          */
    char *description;          /* What to see                      */
};



/*
 * Prototype for an object.
 */
struct	obj_index_data
{
    OBJ_INDEX_DATA *	next;
    EXTRA_DESCR_DATA *	extra_descr;
    AFFECT_DATA *	affected;
    char *		name;
    char *		short_descr;
    char *		description;
    sh_int		vnum;
    sh_int		item_type;
    sh_int		extra_flags;
    sh_int		wear_flags;
    sh_int		count;
    sh_int		weight;
	 int			cost;			/* Unused */
	 int			value	[4];
};



/*
 * One object.
 */
struct	obj_data
{
    OBJ_DATA *		next;
    OBJ_DATA *		next_content;
	 OBJ_DATA *		contains;
    OBJ_DATA *		in_obj;
    CHAR_DATA *		carried_by;
    EXTRA_DESCR_DATA *	extra_descr;
    AFFECT_DATA *	affected;
    OBJ_INDEX_DATA *	pIndexData;
    ROOM_INDEX_DATA *	in_room;
	 char *		name;
    char *		short_descr;
    char *		description;
	 sh_int		item_type;
    sh_int		extra_flags;
    sh_int		wear_flags;
	 sh_int		wear_loc;
    sh_int		weight;
    int			cost;
    sh_int		level;
    sh_int		timer;
    int			value	[4];
};



/*
 * Exit data.
 */
struct	exit_data
{
    ROOM_INDEX_DATA *	to_room;
    sh_int		vnum;
    sh_int		exit_info;
    sh_int		key;
	 char *		keyword;
	 char *		description;
};



/*
 * Reset commands:
 *   '*': comment
 *   'M': read a mobile 
 *   'O': read an object
 *   'P': put object in object
 *   'G': give object to mobile
 *   'E': equip object to mobile
 *   'D': set state of door
 *   'R': randomize room exits
 *   'S': stop (end of list)
 */

/*
 * Area-reset definition.
 */
struct	reset_data
{
    RESET_DATA *	next;
    char		command;
    sh_int		arg1;
	 sh_int		arg2;
    sh_int		arg3;
};



/*
 * Area definition.
 */
struct	area_data
{
    AREA_DATA *		next;
	 RESET_DATA *	reset_first;
	 RESET_DATA *	reset_last;
	 char *		name;
	 sh_int		age;
	 sh_int		nplayer;
    int                 lvnum;
    int                 uvnum;
	 int                 vnum;

	 };



/*
 * Room type.
 */
struct	room_index_data
{
    ROOM_INDEX_DATA *	next;
    CHAR_DATA *		people;
	 OBJ_DATA *		contents;
    EXTRA_DESCR_DATA *	extra_descr;
    AREA_DATA *		area;
    EXIT_DATA *		exit	[6];
    char *		name;
    char *		description;
    sh_int		vnum;
	 sh_int		room_flags;
    sh_int		light;
    sh_int		sector_type;
};



/*
 * Types of attacks.
 * Must be non-overlapping with spell/skill types,
 * but may be arbitrary beyond that.
 */
#define TYPE_UNDEFINED               -1
#define TYPE_HIT                     1000



/*
 *  Target types.
 */
#define TAR_IGNORE		    0
#define TAR_CHAR_OFFENSIVE	    1
#define TAR_CHAR_DEFENSIVE	    2
#define TAR_CHAR_SELF		    3
#define TAR_OBJ_INV		    4



/*
 * Skills include spells as a particular case.
 */
struct	skill_type
{
	 char *	name;			/* Name of skill		*/
	 sh_int	skill_level[MAX_CLASS];	/* Level needed by class	*/
	 SPELL_FUN *	spell_fun;		/* Spell pointer (for spells)	*/
	 sh_int	target;			/* Legal targets		*/
	 sh_int	minimum_position;	/* Position for caster / user	*/
	 sh_int *	pgsn;			/* Pointer to associated gsn	*/
	 sh_int	slot;			/* Slot for #OBJECT loading	*/
	 sh_int	min_mana;		/* Minimum mana used		*/
	 sh_int	beats;			/* Waiting time after use	*/
	 char *	noun_damage;		/* Damage message		*/
	 char *	msg_off;		/* Wear off message		*/
};

struct	spell_type
{
	 char *	name;			/* Name of skill		*/
	 sh_int	skill_level[MAX_CLASS];	/* Level needed by class	*/
	 SPELL_FUN *	spell_fun;		/* Spell pointer (for spells)	*/
	 sh_int	target;			/* Legal targets		*/
	 sh_int	minimum_position;	/* Position for caster / user	*/
	 sh_int *	pgsn;			/* Pointer to associated gsn	*/
	 sh_int	slot;			/* Slot for #OBJECT loading	*/
	 sh_int	min_mana;		/* Minimum mana used		*/
	 sh_int	beats;			/* Waiting time after use	*/
	 char *	noun_damage;		/* Damage message		*/
	 char *	msg_off;		/* Wear off message		*/
};


/*
 * These are skill_lookup return values for common skills and spells.
 */
extern	sh_int	gsn_backstab;
extern	sh_int	gsn_dodge;
extern	sh_int	gsn_hide;
extern   sh_int   gsn_hunt;
extern	sh_int	gsn_peek;
extern	sh_int	gsn_pick_lock;
extern	sh_int	gsn_sneak;
extern	sh_int	gsn_steal;
extern  int       gsn_poison_weapon;

extern  int       gsn_bash;
extern   sh_int   gsn_berserk;
extern	sh_int	gsn_disarm;
extern	sh_int	gsn_enhanced_damage;
extern	sh_int	gsn_kick;
extern	sh_int	gsn_parry;
extern	sh_int	gsn_rescue;
extern	sh_int	gsn_second_attack;
extern	sh_int	gsn_third_attack;

extern	sh_int	gsn_blindness;
extern	sh_int	gsn_charm_person;
extern	sh_int	gsn_curse;
extern	sh_int	gsn_invis;
extern	sh_int	gsn_mass_invis;
extern	sh_int	gsn_poison;
extern	sh_int	gsn_sleep;
extern   int      gsn_sstime;

/*
 * Psionicist gsn's.
 */
extern  int     gsn_chameleon;
extern  int     gsn_domination;
extern  int     gsn_heighten;
extern  int     gsn_shadow;


/*
 * Utility macros.
 */
#define UMIN(a, b)		((a) < (b) ? (a) : (b))
#define UMAX(a, b)		((a) > (b) ? (a) : (b))
#define URANGE(a, b, c)		((b) < (a) ? (a) : ((b) > (c) ? (c) : (b)))
#define LOWER(c)		((c) >= 'A' && (c) <= 'Z' ? (c)+'a'-'A' : (c))
#define UPPER(c)		((c) >= 'a' && (c) <= 'z' ? (c)+'A'-'a' : (c))
#define IS_SET(flag, bit)	((flag) & (bit))
#define SET_BIT(var, bit)	((var) |= (bit))
#define REMOVE_BIT(var, bit)	((var) &= ~(bit))


/*
 * Memory allocation macros.
 */

#define CREATE(result, type, number) \
do {\
   if (!((result) = (type *) calloc ((number), sizeof(type))))\
	{ perror("malloc failure"); abort(); } \
} while(0)

#define RECREATE(result,type,number) \
do {\
  if (!((result) = (type *) realloc ((result), sizeof(type) * (number))))\
	{ perror("realloc failure"); abort(); } \
} while(0)


#define DISPOSE(point) \
do {\
  if (!(point)) bug( "Freeing null pointer", (int) point ); else free(point); \
  point = NULL; \
} while(0)





/*
 * Character macros.
 */
#define IS_NPC(ch)		(IS_SET((ch)->act, ACT_IS_NPC))
#define IS_IMMORTAL(ch)		(get_trust(ch) >= LEVEL_IMMORTAL)
#define IS_HERO(ch)		(get_trust(ch) >= LEVEL_HERO)
#define IS_GOD(ch)      (get_trust(ch) = MAX_LEVEL)
#define IS_AFFECTED(ch, sn)	(IS_SET((ch)->affected_by, (sn)))

#define IS_GOOD(ch)		(ch->alignment >= 350)
#define IS_EVIL(ch)		(ch->alignment <= -350)
#define IS_NEUTRAL(ch)		(!IS_GOOD(ch) && !IS_EVIL(ch))

#define IS_AWAKE(ch)		(ch->position > POS_SLEEPING)
#define GET_AC(ch)		((ch)->armor				    \
					 + ( IS_AWAKE(ch)			    \
					 ? dex_app[get_curr_dex(ch)].defensive   \
					 : 0 ))
#define GET_HITROLL(ch)		((ch)->hitroll+str_app[get_curr_str(ch)].tohit)
#define GET_DAMROLL(ch)		((ch)->damroll+str_app[get_curr_str(ch)].todam)

#define IS_OUTSIDE(ch)		(!IS_SET(				    \
					 (ch)->in_room->room_flags,		    \
					 ROOM_INDOORS))

#define WAIT_STATE(ch, npulse)	((ch)->wait = UMAX((ch)->wait, (npulse)))

#define MANA_COST(ch, sn)       (IS_NPC(ch) ? 0 : UMAX (                    \
				skill_table[sn].min_mana,                   \
				100 / (2 + ch->level -                      \
				skill_table[sn].skill_level[ch->class] ) ) )

/*
 * Object macros.
 */
#define CAN_WEAR(obj, part)	(IS_SET((obj)->wear_flags,  (part)))
#define IS_OBJ_STAT(obj, stat)	(IS_SET((obj)->extra_flags, (stat)))



/*
 * Description macros.
 */
#define PERS(ch, looker)	( can_see( looker, (ch) ) ?		\
				( IS_NPC(ch) ? (ch)->short_descr	\
				: (ch)->name ) : "someone" )

#define NOTIFY(buf, lvl, tog)       ( global( buf, lvl, tog, WIZ_NOTIFY ) )


/*
 * Structure for a command in the command lookup table.
 */
struct	cmd_type
{
	 char * const	name;
	 DO_FUN *		do_fun;
	 sh_int		position;
	 sh_int		level;
	 sh_int		log;
    int			num_uses;
};



/*
 * Structure for a social in the socials table.
 */
struct	social_type
{
	 char * const	name;
	 char * const	char_no_arg;
	 char * const	others_no_arg;
	 char * const	char_found;
	 char * const	others_found;
	 char * const	vict_found;
	 char * const	char_auto;
	 char * const	others_auto;
};



/*
 * Global constants.
 */
extern	const	struct	str_app_type	str_app		[26];
extern	const	struct	int_app_type	int_app		[26];
extern	const	struct	wis_app_type	wis_app		[26];
extern	const	struct	dex_app_type	dex_app		[26];
extern	const	struct	con_app_type	con_app		[26];

extern	const	struct	class_type	class_table	[MAX_CLASS];
extern  const   struct  color_data      color_table     [];
extern	const	struct	race_type   race_table	[MAX_RACE];
extern	struct	cmd_type	cmd_table	[];
extern	const	struct	liq_type	liq_table	[LIQ_MAX];
extern	const	struct	skill_type	skill_table	[MAX_SKILL];
extern	const	struct	social_type	social_table	[];
extern	char *	const			title_table	[MAX_CLASS]
							[MAX_LEVEL+1]
							[2];
#define MAX_COLOR_LIST             18
extern  char *  const                   color_list      [MAX_COLOR_LIST];
extern		CLAN_DATA	  *	first_clan;
extern		CLAN_DATA	  *	last_clan;



/*
 * Global variables.
 */
extern		HELP_DATA	  *	help_first;
extern		SHOP_DATA	  *	shop_first;

extern		BAN_DATA	  *	ban_list;
extern		CHAR_DATA	  *	char_list;
extern		DESCRIPTOR_DATA   *	descriptor_list;
extern		NOTE_DATA	  *	note_list;
extern		OBJ_DATA	  *	object_list;

extern		AFFECT_DATA	  *	affect_free;
extern		BAN_DATA	  *	ban_free;
extern		CHAR_DATA	  *	char_free;
extern		DESCRIPTOR_DATA	  *	descriptor_free;
extern		EXTRA_DESCR_DATA  *	extra_descr_free;
extern		NOTE_DATA	  *	note_free;
extern		OBJ_DATA	  *	obj_free;
extern		PC_DATA		  *	pcdata_free;

extern		char			bug_buf		[];
extern		time_t			current_time;
extern		bool			fLogAll;
extern		FILE *			fpReserve;
extern		KILL_DATA		kill_table	[];
extern		char			log_buf		[];
extern		TIME_INFO_DATA		time_info;
extern		WEATHER_DATA		weather_info;
extern          char              *     down_time;
extern          char              *     warning1;
extern          char              *     warning2;
extern		BOARD_DATA	  *	first_board;
extern		BOARD_DATA	  *	last_board;



/*
 * Command functions.
 * Defined in act_*.c (mostly).
 */
DECLARE_DO_FUN(	do_advance	);
DECLARE_DO_FUN(	do_allow	);
DECLARE_DO_FUN(   do_ansi     );     /* NC */
DECLARE_DO_FUN(	do_answer	);
DECLARE_DO_FUN(	do_areas	);
DECLARE_DO_FUN(	do_aset	);
DECLARE_DO_FUN(	do_at		);
DECLARE_DO_FUN(	do_auction	);
DECLARE_DO_FUN(	do_authorize  );
DECLARE_DO_FUN( do_auto         );
DECLARE_DO_FUN( do_autoexit     );
DECLARE_DO_FUN( do_autoloot     );
DECLARE_DO_FUN( do_autosac      );
DECLARE_DO_FUN(	do_backstab	);
DECLARE_DO_FUN(	do_balzhur	);
DECLARE_DO_FUN(	do_bamfin	);
DECLARE_DO_FUN(	do_bamfout	);
DECLARE_DO_FUN(	do_ban		);
DECLARE_DO_FUN(	do_bash 	);
DECLARE_DO_FUN(	do_berserk 	);
DECLARE_DO_FUN( do_blank        );
DECLARE_DO_FUN(	do_brandish	);
DECLARE_DO_FUN( do_brief        );
DECLARE_DO_FUN( do_boards	);
DECLARE_DO_FUN( do_bset		);
DECLARE_DO_FUN( do_bstat	);
DECLARE_DO_FUN(	do_bug		);
DECLARE_DO_FUN(	do_buy		);
DECLARE_DO_FUN(	do_cast		);
DECLARE_DO_FUN(	do_channels	);
DECLARE_DO_FUN(	do_chat		);
DECLARE_DO_FUN(	do_close	);
DECLARE_DO_FUN(	do_cmdtable	);
DECLARE_DO_FUN( do_combine      );
DECLARE_DO_FUN(	do_commands	);
DECLARE_DO_FUN(	do_compare	);
DECLARE_DO_FUN(	do_config	);
DECLARE_DO_FUN(	do_consider	);
DECLARE_DO_FUN(	do_credits	);
DECLARE_DO_FUN(	do_deny		);
DECLARE_DO_FUN(	do_description	);
DECLARE_DO_FUN(	do_disarm	);
DECLARE_DO_FUN(	do_disconnect	);
DECLARE_DO_FUN(	do_down		);
DECLARE_DO_FUN(	do_drink	);
DECLARE_DO_FUN(	do_drop		);
DECLARE_DO_FUN(	do_east		);
DECLARE_DO_FUN(	do_eat		);
DECLARE_DO_FUN(	do_echo		);
DECLARE_DO_FUN(	do_emote	);
DECLARE_DO_FUN(	do_equipment	);
DECLARE_DO_FUN(	do_examine	);
DECLARE_DO_FUN(	do_exits	);
DECLARE_DO_FUN(	do_exlist	);
DECLARE_DO_FUN(	do_fill		);
DECLARE_DO_FUN(	do_flee		);
DECLARE_DO_FUN(	do_follow	);
DECLARE_DO_FUN(	do_for	);
DECLARE_DO_FUN(	do_force	);
DECLARE_DO_FUN(	do_freeze	);
DECLARE_DO_FUN(	do_get		);
DECLARE_DO_FUN(	do_give		);
DECLARE_DO_FUN(	do_goto		);
DECLARE_DO_FUN(	do_group	);
DECLARE_DO_FUN(	do_gtell	);
DECLARE_DO_FUN(	do_help		);
DECLARE_DO_FUN(	do_hide		);
DECLARE_DO_FUN(	do_holylight	);
DECLARE_DO_FUN(   do_hunt     );
DECLARE_DO_FUN(	do_idea		);
DECLARE_DO_FUN(	do_identify	); // @@@ ###
DECLARE_DO_FUN(	do_immtalk	);
DECLARE_DO_FUN(	do_inventory	);
DECLARE_DO_FUN(	do_invis	);
DECLARE_DO_FUN(	do_kick		);
DECLARE_DO_FUN(	do_kill		);
DECLARE_DO_FUN(	do_list		);
DECLARE_DO_FUN(	do_lock		);
DECLARE_DO_FUN(	do_log		);
DECLARE_DO_FUN(	do_logcha	);
DECLARE_DO_FUN(	do_look		);
DECLARE_DO_FUN(	do_makeboard	);
DECLARE_DO_FUN(	do_memory	);
DECLARE_DO_FUN(	do_mfind 	);
DECLARE_DO_FUN(	do_mload  	);
DECLARE_DO_FUN(	do_monitor	);
DECLARE_DO_FUN( do_mpasound     );
DECLARE_DO_FUN( do_mpat         );
DECLARE_DO_FUN( do_mpecho       );
DECLARE_DO_FUN( do_mpechoaround );
DECLARE_DO_FUN( do_mpechoat     );
DECLARE_DO_FUN( do_mpforce      );
DECLARE_DO_FUN( do_mpgoto       );
DECLARE_DO_FUN( do_mpjunk       );
DECLARE_DO_FUN( do_mpkill       );
DECLARE_DO_FUN( do_mpmload      );
DECLARE_DO_FUN( do_mpoload      );
DECLARE_DO_FUN( do_mppurge      );
DECLARE_DO_FUN( do_mpstat       );
DECLARE_DO_FUN( do_mptransfer   );
DECLARE_DO_FUN(	do_mset		);
DECLARE_DO_FUN(	do_mstat	);
DECLARE_DO_FUN(	do_mwhere	);
DECLARE_DO_FUN(	do_murde	);
DECLARE_DO_FUN(	do_murder	);
DECLARE_DO_FUN(	do_muse	);
DECLARE_DO_FUN(	do_music	);
DECLARE_DO_FUN(	do_noemote	);
DECLARE_DO_FUN(   do_notify   );     /* NC */
DECLARE_DO_FUN(	do_north	);
DECLARE_DO_FUN(	do_note		);
DECLARE_DO_FUN(	do_notell	);
DECLARE_DO_FUN(	do_ofind	);
DECLARE_DO_FUN(	do_oload	);
DECLARE_DO_FUN(	do_open		);
DECLARE_DO_FUN(	do_opentournament		);
DECLARE_DO_FUN(	do_order	);
DECLARE_DO_FUN(	do_orphans	);
DECLARE_DO_FUN(	do_oset		);
DECLARE_DO_FUN(	do_ostat	);
DECLARE_DO_FUN(	do_outfit	);
DECLARE_DO_FUN(	do_owhere	);
DECLARE_DO_FUN(   do_pagelen  );
DECLARE_DO_FUN(	do_pardon	);
DECLARE_DO_FUN(	do_password	);
DECLARE_DO_FUN(	do_peace	   );
DECLARE_DO_FUN(   do_pecho    );
DECLARE_DO_FUN(	do_pick		);
DECLARE_DO_FUN(	do_poison_weapon);
DECLARE_DO_FUN(	do_pose		);
DECLARE_DO_FUN(	do_practice	);
DECLARE_DO_FUN(   do_prompt   );
DECLARE_DO_FUN(	do_purge	   );
DECLARE_DO_FUN(	do_put	 	);
DECLARE_DO_FUN(	do_make	 	);
DECLARE_DO_FUN(	do_clans	 	);
DECLARE_DO_FUN(	do_induct	 	);
DECLARE_DO_FUN(	do_outcast	 	);
DECLARE_DO_FUN(	do_makeclan	 	);
DECLARE_DO_FUN(	do_setclan	 	);
DECLARE_DO_FUN(	do_showclan	 	);
DECLARE_DO_FUN(	do_quaff	   );
DECLARE_DO_FUN(	do_quest	);
DECLARE_DO_FUN(	do_qui		);
DECLARE_DO_FUN(	do_quit		);
DECLARE_DO_FUN(	do_reboo	);
DECLARE_DO_FUN(	do_reboot	);
DECLARE_DO_FUN(	do_recall	);
DECLARE_DO_FUN(	do_recho	);
DECLARE_DO_FUN(	do_recite	);
DECLARE_DO_FUN(	do_remove	);
DECLARE_DO_FUN(	do_rename		);
DECLARE_DO_FUN(	do_rent		);
DECLARE_DO_FUN(	do_reply	);
DECLARE_DO_FUN(	do_report	);
DECLARE_DO_FUN(	do_rescue	);
DECLARE_DO_FUN(	do_rest		);
DECLARE_DO_FUN(	do_restore	);
DECLARE_DO_FUN(	do_restrict	);
DECLARE_DO_FUN(	do_return	);
DECLARE_DO_FUN(	do_rset		);
DECLARE_DO_FUN(	do_rstat	);
DECLARE_DO_FUN(	do_sacrifice	);
DECLARE_DO_FUN(	do_save		);
DECLARE_DO_FUN(	do_say		);
DECLARE_DO_FUN(	do_score	);
DECLARE_DO_FUN(	do_sell		);
DECLARE_DO_FUN(	do_shoplist	);
DECLARE_DO_FUN(	do_shout	);
DECLARE_DO_FUN(	do_shutdow	);
DECLARE_DO_FUN(	do_shutdown	);
DECLARE_DO_FUN(	do_silence	);
DECLARE_DO_FUN(	do_sla		);
DECLARE_DO_FUN(	do_slay		);
DECLARE_DO_FUN(	do_sleep	);
DECLARE_DO_FUN( do_slist        );
DECLARE_DO_FUN(	do_slookup	);
DECLARE_DO_FUN(	do_sneak	);
DECLARE_DO_FUN(	do_snoop	);
DECLARE_DO_FUN(	do_socials	);
DECLARE_DO_FUN(	do_south	);
DECLARE_DO_FUN(   do_spells );
DECLARE_DO_FUN(   do_spew  );
DECLARE_DO_FUN(	do_split	);
DECLARE_DO_FUN(	do_sset		);
DECLARE_DO_FUN(	do_sstime	);
DECLARE_DO_FUN(	do_stand	);
DECLARE_DO_FUN(	do_starttournament	);
DECLARE_DO_FUN(	do_steal	);
DECLARE_DO_FUN(	do_switch	);
DECLARE_DO_FUN(	do_tell		);
DECLARE_DO_FUN(	do_time		);
DECLARE_DO_FUN(	do_title	);
DECLARE_DO_FUN(	do_train	);
DECLARE_DO_FUN(	do_transfer	);
DECLARE_DO_FUN(	do_trust	);
DECLARE_DO_FUN(	do_typo		);
DECLARE_DO_FUN(	do_unlock	);
DECLARE_DO_FUN(	do_up		);
DECLARE_DO_FUN(	do_users	);
DECLARE_DO_FUN(	do_value	);
DECLARE_DO_FUN(	do_visible	);
DECLARE_DO_FUN(	do_vlist	);
DECLARE_DO_FUN(	do_wake		);
DECLARE_DO_FUN(	do_wear		);
DECLARE_DO_FUN(	do_weather	);
DECLARE_DO_FUN(	do_west		);
DECLARE_DO_FUN(	do_where	);
DECLARE_DO_FUN(	do_who		);
DECLARE_DO_FUN(	do_wimpy	);
DECLARE_DO_FUN(	do_wizhelp	);
DECLARE_DO_FUN( do_wizify       );
DECLARE_DO_FUN( do_wizlist      );
DECLARE_DO_FUN(	do_wizlock	);
DECLARE_DO_FUN(	do_yell		);
DECLARE_DO_FUN(	do_zap		);
DECLARE_DO_FUN(	do_zones		);



/*
 * Spell functions.
 * Defined in magic.c.
 */
DECLARE_SPELL_FUN(	spell_null		);
DECLARE_SPELL_FUN(	spell_acid_blast	);
DECLARE_SPELL_FUN(	spell_armor		);
DECLARE_SPELL_FUN(	spell_bless		);
DECLARE_SPELL_FUN(	spell_blindness		);
DECLARE_SPELL_FUN(	spell_burning_hands	);
DECLARE_SPELL_FUN(	spell_call_lightning	);
DECLARE_SPELL_FUN(	spell_cause_critical	);
DECLARE_SPELL_FUN(	spell_cause_light	);
DECLARE_SPELL_FUN(	spell_cause_serious	);
DECLARE_SPELL_FUN(	spell_change_sex	);
DECLARE_SPELL_FUN(	spell_charm_person	);
DECLARE_SPELL_FUN(	spell_chill_touch	);
DECLARE_SPELL_FUN(	spell_colour_spray	);
DECLARE_SPELL_FUN(	spell_continual_light	);
DECLARE_SPELL_FUN(	spell_control_weather	);
DECLARE_SPELL_FUN(	spell_create_food	);
DECLARE_SPELL_FUN(	spell_create_spring	);
DECLARE_SPELL_FUN(	spell_create_water	);
DECLARE_SPELL_FUN(	spell_cure_blindness	);
DECLARE_SPELL_FUN(	spell_cure_critical	);
DECLARE_SPELL_FUN(	spell_cure_light	);
DECLARE_SPELL_FUN(	spell_cure_poison	);
DECLARE_SPELL_FUN(	spell_cure_serious	);
DECLARE_SPELL_FUN(	spell_curse		);
DECLARE_SPELL_FUN(	spell_detect_evil	);
DECLARE_SPELL_FUN(	spell_detect_hidden	);
DECLARE_SPELL_FUN(	spell_detect_invis	);
DECLARE_SPELL_FUN(	spell_detect_magic	);
DECLARE_SPELL_FUN(	spell_detect_poison	);
DECLARE_SPELL_FUN(	spell_dispel_evil	);
DECLARE_SPELL_FUN(	spell_dispel_magic	);
DECLARE_SPELL_FUN(	spell_earthquake	);
DECLARE_SPELL_FUN(	spell_enchant_weapon	);
DECLARE_SPELL_FUN(	spell_energy_drain	);
DECLARE_SPELL_FUN(	spell_faerie_fire	);
DECLARE_SPELL_FUN(	spell_faerie_fog	);
DECLARE_SPELL_FUN(	spell_fireball		);
DECLARE_SPELL_FUN(	spell_flamestrike	);
DECLARE_SPELL_FUN(	spell_fly		);
DECLARE_SPELL_FUN(	spell_gate		);
DECLARE_SPELL_FUN(	spell_general_purpose	);
DECLARE_SPELL_FUN(	spell_giant_strength	);
DECLARE_SPELL_FUN(	spell_harm		);
DECLARE_SPELL_FUN(	spell_heal		);
DECLARE_SPELL_FUN(	spell_high_explosive	);
DECLARE_SPELL_FUN(	spell_identify		);
DECLARE_SPELL_FUN(	spell_infravision	);
DECLARE_SPELL_FUN(	spell_invis		);
DECLARE_SPELL_FUN(	spell_know_alignment	);
DECLARE_SPELL_FUN(	spell_lightning_bolt	);
DECLARE_SPELL_FUN(	spell_locate_object	);
DECLARE_SPELL_FUN(	spell_magic_missile	);
DECLARE_SPELL_FUN(	spell_make_purse	);      // @@@
DECLARE_SPELL_FUN(	spell_mass_invis	);
DECLARE_SPELL_FUN(	spell_pass_door		);
DECLARE_SPELL_FUN(	spell_poison		);
DECLARE_SPELL_FUN(	spell_protection	);
DECLARE_SPELL_FUN(	spell_refresh		);
DECLARE_SPELL_FUN(	spell_remove_curse	);
DECLARE_SPELL_FUN(	spell_sanctuary		);
DECLARE_SPELL_FUN(	spell_shocking_grasp	);
DECLARE_SPELL_FUN(	spell_shield		);
DECLARE_SPELL_FUN(	spell_sleep		);
DECLARE_SPELL_FUN(	spell_stone_skin	);
DECLARE_SPELL_FUN(	spell_summon		);
DECLARE_SPELL_FUN(	spell_teleport		);
DECLARE_SPELL_FUN(	spell_ventriloquate	);
DECLARE_SPELL_FUN(	spell_weaken		);
DECLARE_SPELL_FUN(	spell_word_of_recall	);
DECLARE_SPELL_FUN(	spell_acid_breath	);
DECLARE_SPELL_FUN(	spell_fire_breath	);
DECLARE_SPELL_FUN(	spell_frost_breath	);
DECLARE_SPELL_FUN(	spell_gas_breath	);
DECLARE_SPELL_FUN(	spell_lightning_breath	);

// TFC spell functions
DECLARE_SPELL_FUN(	spell_animate_dead	);

/*
 * Psi spell_functions, in magic.c.
 */
DECLARE_SPELL_FUN(      spell_adrenaline_control);
DECLARE_SPELL_FUN(      spell_agitation         );
DECLARE_SPELL_FUN(      spell_aura_sight        );
DECLARE_SPELL_FUN(      spell_awe               );
DECLARE_SPELL_FUN(      spell_ballistic_attack  );
DECLARE_SPELL_FUN(      spell_biofeedback       );
DECLARE_SPELL_FUN(      spell_cell_adjustment   );
DECLARE_SPELL_FUN(      spell_combat_mind       );
DECLARE_SPELL_FUN(      spell_complete_healing  );
DECLARE_SPELL_FUN(      spell_control_flames    );
DECLARE_SPELL_FUN(      spell_create_sound      );
DECLARE_SPELL_FUN(      spell_death_field       );
DECLARE_SPELL_FUN(      spell_detonate          );
DECLARE_SPELL_FUN(      spell_disintegrate      );
DECLARE_SPELL_FUN(      spell_displacement      );
DECLARE_SPELL_FUN(      spell_domination        );
DECLARE_SPELL_FUN(      spell_ectoplasmic_form  );
DECLARE_SPELL_FUN(      spell_ego_whip          );
DECLARE_SPELL_FUN(      spell_energy_containment);
DECLARE_SPELL_FUN(      spell_enhance_armor     );
DECLARE_SPELL_FUN(      spell_enhanced_strength );
DECLARE_SPELL_FUN(      spell_flesh_armor       );
DECLARE_SPELL_FUN(      spell_inertial_barrier  );
DECLARE_SPELL_FUN(      spell_inflict_pain      );
DECLARE_SPELL_FUN(      spell_intellect_fortress);
DECLARE_SPELL_FUN(      spell_lend_health       );
DECLARE_SPELL_FUN(      spell_levitation        );
DECLARE_SPELL_FUN(      spell_mental_barrier    );
DECLARE_SPELL_FUN(      spell_mind_thrust       );
DECLARE_SPELL_FUN(      spell_project_force     );
DECLARE_SPELL_FUN(      spell_psionic_blast     );
DECLARE_SPELL_FUN(      spell_psychic_crush     );
DECLARE_SPELL_FUN(      spell_psychic_drain     );
DECLARE_SPELL_FUN(      spell_psychic_healing   );
DECLARE_SPELL_FUN(      spell_share_strength    );
DECLARE_SPELL_FUN(      spell_thought_shield    );
DECLARE_SPELL_FUN(      spell_ultrablast        );



/*
 * OS-dependent declarations.
 * These are all very standard library functions,
 *   but some systems have incomplete or non-ansi header files.
 */
#if	defined(_AIX)
char *	crypt		args( ( const char *key, const char *salt ) );
#endif
int	atoi		args( ( const char *string ) );

#if	defined(apollo)
int	atoi		args( ( const char *string ) );
void *	calloc		args( ( unsigned nelem, size_t size ) );
char *	crypt		args( ( const char *key, const char *salt ) );
#endif

#if	defined(hpux)
char *	crypt		args( ( const char *key, const char *salt ) );
#endif

#if	defined(linux)
char *	crypt		args( ( const char *key, const char *salt ) );
#endif

#if	defined(macintosh)
#define NOCRYPT
#if	defined(unix)
#undef	unix
#endif
#endif

#if	defined(MIPS_OS)
char *	crypt		args( ( const char *key, const char *salt ) );
#endif

#if	defined(MSDOS)
#define NOCRYPT
#if	defined(unix)
#undef	unix
#endif
#endif

#if	defined(NeXT)
char *	crypt		args( ( const char *key, const char *salt ) );
#endif

#if	defined(sequent)
char *	crypt		args( ( const char *key, const char *salt ) );
int	fclose		args( ( FILE *stream ) );
int	fprintf		args( ( FILE *stream, const char *format, ... ) );
int	fread		args( ( void *ptr, int size, int n, FILE *stream ) );
int	fseek		args( ( FILE *stream, long offset, int ptrname ) );
void	perror		args( ( const char *s ) );
int	ungetc		args( ( int c, FILE *stream ) );
#endif

#if	defined(sun)
char *	crypt		args( ( const char *key, const char *salt ) );
int	fclose		args( ( FILE *stream ) );
int	fprintf		args( ( FILE *stream, const char *format, ... ) );
int	fread		args( ( void *ptr, int size, int n, FILE *stream ) );
int	fseek		args( ( FILE *stream, long offset, int ptrname ) );
void	perror		args( ( const char *s ) );
int	ungetc		args( ( int c, FILE *stream ) );
#endif

#if	defined(ultrix)
char *	crypt		args( ( const char *key, const char *salt ) );
#endif



/*
 * The crypt(3) function is not available on some operating systems.
 * In particular, the U.S. Government prohibits its export from the
 *   United States to foreign countries.
 * Turn on NOCRYPT to keep passwords in plain text.
 */
#if	defined(NOCRYPT)
#define crypt(s1, s2)	(s1)
#endif



/*
 * Data files used by the server.
 *
 * AREA_LIST contains a list of areas to boot.
 * All files are read in completely at bootup.
 * Most output files (bug, idea, typo, shutdown) are append-only.
 *
 * The NULL_FILE is held open so that we have a stream handle in reserve,
 *   so players can go ahead and telnet to all the other descriptors.
 * Then we close it whenever we need to open a file (e.g. a save file).
 */
//#if defined(macintosh)
//#define PLAYER_DIR	""		/* Player files			*/
//#define NULL_FILE	"proto.are"	/* To reserve one stream	*/
//#define MOB_DIR         ""              /* MOBProg files                */
//#endif

//#if defined(MSDOS)
#define BOARD_DIR	""	/* Board data dir		*/
#define CLAN_DIR	""	/* Clan data dir		*/
#define PLAYER_DIR	""		/* Player files                 */
#define NULL_FILE	"nul"		/* To reserve one stream	*/
#define MOB_DIR         ""              /* MOBProg files                */
//#endif

//#if defined(unix)
//#define PLAYER_DIR	"../player/"	/* Player files			*/
//#define NULL_FILE	"/dev/null"	/* To reserve one stream	*/
//#define MOB_DIR         "MOBProgs/"     /* MOBProg files                */
//#endif

//#if defined(linux)
//#define PLAYER_DIR	"../player/"	/* Player files			*/
//#define NULL_FILE	"/dev/null"	/* To reserve one stream	*/
//#define MOB_DIR         "MOBProgs/"     /* MOBProg files                */
//#endif

#define AREA_LIST	"area.lst"	/* List of areas		*/
#define CLAN_LIST	"clan.lst"	/* List of clans		*/

#define BUG_FILE	"bugs.txt"      /* For 'bug' and bug( )		*/
#define BOARD_FILE	"boards.txt"	/* For bulletin boards		*/
#define IDEA_FILE	"ideas.txt"	/* For 'idea'			*/
#define TYPO_FILE	"typos.txt"     /* For 'typo'			*/
#define NOTE_FILE	"notes.txt"	/* For 'notes'			*/
#define SHUTDOWN_FILE	"shutdown.txt"	/* For 'shutdown'		*/
#define DOWN_TIME_FILE  "time.txt"      /* For automatic shutdown       */



/*
 * Our function prototypes.
 * One big lump ... this is every function in Merc.
 */
#define CD	CHAR_DATA
#define CL	CLAN_DATA
#define MID	MOB_INDEX_DATA
#define OD	OBJ_DATA
#define OID	OBJ_INDEX_DATA
#define RID	ROOM_INDEX_DATA
#define SF	SPEC_FUN
#define BD	BOARD_DATA

/* clans.c */
CL *	get_clan	args( ( char *name ) );
void	load_clans	args( ( void ) );
void	save_clan	args( ( CLAN_DATA *clan ) );

/* act_comm.c */
void	add_follower	args( ( CHAR_DATA *ch, CHAR_DATA *master ) );
void	stop_follower	args( ( CHAR_DATA *ch ) );
void	die_follower	args( ( CHAR_DATA *ch ) );
bool	is_same_group	args( ( CHAR_DATA *ach, CHAR_DATA *bch ) );
bool	is_note_to	args( ( CHAR_DATA *ch, NOTE_DATA *pnote ) );
void   ansi_color      args( ( const char *txt, CHAR_DATA *ch ) );

/* act_info.c */
void	set_title	args( ( CHAR_DATA *ch, char *title ) );

/* act_move.c */
void	move_char	args( ( CHAR_DATA *ch, int door ) );

/* act_obj.c */

/* act_wiz.c */
ROOM_INDEX_DATA *	find_location	args( ( CHAR_DATA *ch, char *arg ) );

/* boards.c */
void	load_boards	args( ( void ) );
BD *	get_board	args( ( OBJ_DATA *obj ) );

/* wsckcomm.c */
void	close_socket	args( ( DESCRIPTOR_DATA *dclose ) );
void	write_to_buffer	args( ( DESCRIPTOR_DATA *d, const char *txt,
				 int length ) );
void	set_char_color  args( ( const sh_int AType, CHAR_DATA *ch ) );
void	send_to_char	args( ( const char *txt, CHAR_DATA *ch ) );
void    show_string     args( ( DESCRIPTOR_DATA *d, char *input ) );
void    global          args( ( char *buf, int level, int toggler,
										  int toggler2 ) );
void act(sh_int AType, const char *format, CHAR_DATA *ch, const void *arg1, const void *arg2, int type );

/* db.c */
void	boot_db		args( ( void ) );
void	area_update	args( ( void ) );
CD *	create_mobile	args( ( MOB_INDEX_DATA *pMobIndex ) );
OD *	create_object	args( ( OBJ_INDEX_DATA *pObjIndex, int level ) );
void	clear_char	args( ( CHAR_DATA *ch ) );
void	free_char	args( ( CHAR_DATA *ch ) );
char *	get_extra_descr	args( ( const char *name, EXTRA_DESCR_DATA *ed ) );
MID *	get_mob_index	args( ( int vnum ) );
OID *	get_obj_index	args( ( int vnum ) );
RID *	get_room_index	args( ( int vnum ) );
char	fread_letter	args( ( FILE *fp ) );
int	fread_number	args( ( FILE *fp ) );
char *	fread_string	args( ( FILE *fp ) );
void	fread_to_eol	args( ( FILE *fp ) );
char *	fread_word	args( ( FILE *fp ) );
void *	alloc_mem	args( ( int sMem ) );
void *	alloc_perm	args( ( int sMem ) );
void	free_mem	args( ( void *pMem, int sMem ) );
char *	str_dup		args( ( const char *str ) );
void	free_string	args( ( char *pstr ) );
int	number_fuzzy	args( ( int number ) );
int	number_range	args( ( int from, int to ) );
int	number_percent	args( ( void ) );
int	number_door	args( ( void ) );
int	number_bits	args( ( int width ) );
int	number_mm	args( ( void ) );
int	dice		args( ( int number, int size ) );
int	interpolate	args( ( int level, int value_00, int value_32 ) );
void	smash_tilde	args( ( char *str ) );
bool	str_cmp		args( ( const char *astr, const char *bstr ) );
bool	str_prefix	args( ( const char *astr, const char *bstr ) );
bool	str_infix	args( ( const char *astr, const char *bstr ) );
bool	str_suffix	args( ( const char *astr, const char *bstr ) );
char *	capitalize	args( ( const char *str ) );
void	append_file	args( ( CHAR_DATA *ch, char *file, char *str ) );
void	bug		args( ( const char *str, int param ) );
void	log_string	args( ( const char *str ) );
void	tail_chain	args( ( void ) );

/* fight.c */
void	violence_update	args( ( void ) );
void	multi_hit	args( ( CHAR_DATA *ch, CHAR_DATA *victim, int dt ) );
void	damage		args( ( CHAR_DATA *ch, CHAR_DATA *victim, int dam,
			    int dt ) );
void	update_pos	args( ( CHAR_DATA *victim ) );
void	stop_fighting	args( ( CHAR_DATA *ch, bool fBoth ) );
void	death_cry	args( ( CHAR_DATA *ch ) );
void	raw_kill	args( ( CHAR_DATA *victim ) );

/* handler.c */
int	get_trust	args( ( CHAR_DATA *ch ) );
int	get_age		args( ( CHAR_DATA *ch ) );
int	get_curr_str	args( ( CHAR_DATA *ch ) );
int	get_curr_int	args( ( CHAR_DATA *ch ) );
int	get_curr_wis	args( ( CHAR_DATA *ch ) );
int	get_curr_dex	args( ( CHAR_DATA *ch ) );
int	get_curr_con	args( ( CHAR_DATA *ch ) );
int	can_carry_n	args( ( CHAR_DATA *ch ) );
int	can_carry_w	args( ( CHAR_DATA *ch ) );
bool	is_name		args( ( const char *str, char *namelist ) );
void	affect_to_char	args( ( CHAR_DATA *ch, AFFECT_DATA *paf ) );
void	affect_remove	args( ( CHAR_DATA *ch, AFFECT_DATA *paf ) );
void	affect_strip	args( ( CHAR_DATA *ch, int sn ) );
bool	is_affected	args( ( CHAR_DATA *ch, int sn ) );
void	affect_join	args( ( CHAR_DATA *ch, AFFECT_DATA *paf ) );
void	char_from_room	args( ( CHAR_DATA *ch ) );
void	char_to_room	args( ( CHAR_DATA *ch, ROOM_INDEX_DATA *pRoomIndex ) );
void	obj_to_char	args( ( OBJ_DATA *obj, CHAR_DATA *ch ) );
void	obj_from_char	args( ( OBJ_DATA *obj ) );
int	apply_ac	args( ( OBJ_DATA *obj, int iWear ) );
OD *	get_eq_char	args( ( CHAR_DATA *ch, int iWear ) );
void	equip_char	args( ( CHAR_DATA *ch, OBJ_DATA *obj, int iWear ) );
void	unequip_char	args( ( CHAR_DATA *ch, OBJ_DATA *obj ) );
int	count_obj_list	args( ( OBJ_INDEX_DATA *obj, OBJ_DATA *list ) );
void	obj_from_room	args( ( OBJ_DATA *obj ) );
void	obj_to_room	args( ( OBJ_DATA *obj, ROOM_INDEX_DATA *pRoomIndex ) );
void	obj_to_obj	args( ( OBJ_DATA *obj, OBJ_DATA *obj_to ) );
void	obj_from_obj	args( ( OBJ_DATA *obj ) );
void	extract_obj	args( ( OBJ_DATA *obj ) );
void	extract_char	args( ( CHAR_DATA *ch, bool fPull ) );
CD *	get_char_room	args( ( CHAR_DATA *ch, char *argument ) );
CD *	get_char_world	args( ( CHAR_DATA *ch, char *argument ) );
OD *	get_obj_type	args( ( OBJ_INDEX_DATA *pObjIndexData ) );
OD *	get_obj_list	args( ( CHAR_DATA *ch, char *argument,
			    OBJ_DATA *list ) );
OD *	get_obj_carry	args( ( CHAR_DATA *ch, char *argument ) );
OD *	get_obj_wear	args( ( CHAR_DATA *ch, char *argument ) );
OD *	get_obj_here	args( ( CHAR_DATA *ch, char *argument ) );
OD *	get_obj_world	args( ( CHAR_DATA *ch, char *argument ) );
OD *	create_money	args( ( int amount ) );
int	get_obj_number	args( ( OBJ_DATA *obj ) );
int	get_obj_weight	args( ( OBJ_DATA *obj ) );
bool	room_is_dark	args( ( ROOM_INDEX_DATA *pRoomIndex ) );
bool	room_is_private	args( ( ROOM_INDEX_DATA *pRoomIndex ) );
bool	can_see		args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
bool	can_see_obj	args( ( CHAR_DATA *ch, OBJ_DATA *obj ) );
bool	can_drop_obj	args( ( CHAR_DATA *ch, OBJ_DATA *obj ) );
char *	item_type_name	args( ( OBJ_DATA *obj ) );
char *	affect_loc_name	args( ( int location ) );
char *	affect_bit_name	args( ( int vector ) );
char *	extra_bit_name	args( ( int extra_flags ) );
CD   *  get_char        args( ( CHAR_DATA *ch ) );
bool    authorized      args( ( CHAR_DATA *ch, int gsn ) );
CHAR_DATA *get_char_area( CHAR_DATA *ch, char *argument );

/* interp.c */
void	interpret	args( ( CHAR_DATA *ch, char *argument ) );
bool	is_number	args( ( char *arg ) );
int	number_argument	args( ( char *argument, char *arg ) );
char *	one_argument	args( ( char *argument, char *arg_first ) );
bool    IS_SWITCHED     args( ( CHAR_DATA *ch ) );

/* magic.c */
int	skill_lookup	args( ( const char *name ) );
int	slot_lookup	args( ( int slot ) );
bool	saves_spell	args( ( int level, CHAR_DATA *victim ) );
void	obj_cast_spell	args( ( int sn, int level, CHAR_DATA *ch,
				    CHAR_DATA *victim, OBJ_DATA *obj ) );

/* mob_prog.c */
#ifdef DUNNO_STRSTR
char *  strstr                  args( (const char *s1, const char *s2 ) );
#endif

void    mprog_wordlist_check    args( ( char * arg, CHAR_DATA *mob,
                CHAR_DATA* actor, OBJ_DATA* object, void* vo, int type ) );
void    mprog_percent_check     args( ( CHAR_DATA *mob,
                CHAR_DATA* actor, OBJ_DATA* object, void* vo, int type ) );
void    mprog_act_trigger       args( ( char* buf, CHAR_DATA* mob,
                        CHAR_DATA* ch, OBJ_DATA* obj, void* vo ) );
void    mprog_bribe_trigger     args( ( CHAR_DATA* mob, CHAR_DATA* ch,
                        int amount ) );
void    mprog_entry_trigger     args( ( CHAR_DATA* mob ) );
void    mprog_give_trigger      args( ( CHAR_DATA* mob, CHAR_DATA* ch,
                        OBJ_DATA* obj ) );
void    mprog_greet_trigger     args( ( CHAR_DATA* mob ) );
void    mprog_fight_trigger     args( ( CHAR_DATA* mob, CHAR_DATA* ch ) );
void    mprog_hitprcnt_trigger  args( ( CHAR_DATA* mob, CHAR_DATA* ch ) );
void    mprog_death_trigger     args( ( CHAR_DATA* mob ) );
void    mprog_random_trigger    args( ( CHAR_DATA* mob ) );
void    mprog_speech_trigger    args( ( char* txt, CHAR_DATA* mob ) );

/* save.c */
void	save_char_obj	args( ( CHAR_DATA *ch ) );
bool	load_char_obj	args( ( DESCRIPTOR_DATA *d, char *name ) );

/* special.c */
SF *	spec_lookup	args( ( const char *name ) );

/* update.c */
void	advance_level	args( ( CHAR_DATA *ch ) );
void	gain_exp	args( ( CHAR_DATA *ch, int gain ) );
void	gain_condition	args( ( CHAR_DATA *ch, int iCond, int value ) );
void	update_handler	args( ( void ) );

/* hunt.c */
void    hunt_victim     args( ( CHAR_DATA *ch ) );

/* string.c */
void	string_edit	args( ( CHAR_DATA *ch, char **pString ) );
void	string_append	args( ( CHAR_DATA *ch, char **pString ) );
void	string_add	args( ( CHAR_DATA *ch, char *argument ) );

#undef	CD
#undef	CL
#undef	MID
#undef	OD
#undef	OID
#undef	RID
#undef	SF

// @@@
int SpinTheMessageLoop();

